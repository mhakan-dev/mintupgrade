#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "EspSetup.h"
#include "Util.h"
#include "Log.h"
#include "../Config.h"
#include <filesystem>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

// Mount the ESP to a free drive letter
static wchar_t mountEsp(std::wstring& err) {
    wchar_t ltr = util::findFreeDriveLetter();
    if (!ltr) { err = L"Boş sürücü harfi bulunamadı"; return 0; }

    DWORD ec = 1;
    util::runCapture(std::format(L"\"{}\" {}: /S", util::sys32(L"mountvol.exe"), ltr), &ec);

    std::error_code fec;
    if (ec != 0 || !fs::exists(std::format(L"{}:\\EFI", ltr), fec)) {
        err = L"EFI Sistem Bölümü bağlanamadı (mountvol /S). "
              L"Uygulamayı yönetici olarak çalıştırdığınızdan emin olun.";
        return 0;
    }
    logx::write(std::format(L"ESP mounted: {}:", ltr));
    return ltr;
}

static void unmountEsp(wchar_t ltr) {
    if (!ltr) return;
    util::runCapture(std::format(L"\"{}\" {}: /D", util::sys32(L"mountvol.exe"), ltr));
    logx::write(std::format(L"ESP unmounted: {}:", ltr));
}

// Detect the casper kernel/initrd names inside the ISO
static bool detectCasper(const std::wstring& isoRoot,
                         std::wstring& kernel, std::wstring& initrd) {
    const wchar_t* kNames[] = { L"vmlinuz", L"vmlinuz.efi" };
    const wchar_t* iNames[] = { L"initrd.lz", L"initrd.gz", L"initrd" };
    std::error_code ec;
    for (auto* k : kNames)
        if (fs::exists(isoRoot + L"casper\\" + k, ec)) {
            kernel = std::wstring(L"/casper/") + k;
            break;
        }
    for (auto* i : iNames)
        if (fs::exists(isoRoot + L"casper\\" + i, ec)) {
            initrd = std::wstring(L"/casper/") + i;
            break;
        }
    return !kernel.empty() && !initrd.empty();
}

// grub.cfg content
// Loopback method: GRUB opens the ISO file on NTFS as a virtual disk and loads
// the kernel from inside it; the casper initrd locates the ISO itself via iso-scan.
static std::wstring grubCfgText(const std::wstring& isoGrubPath,
                                const std::wstring& kernel,
                                const std::wstring& initrd) {
    return std::format(
        L"# MintUpgradeAssistant tarafından oluşturuldu — elle düzenlemeyin\n"
        L"set timeout=8\n"
        L"set default=0\n"
        L"insmod all_video\n"
        L"insmod part_gpt\n"
        L"insmod part_msdos\n"
        L"insmod fat\n"
        L"insmod ntfs\n"
        L"insmod ntfscomp\n"
        L"insmod exfat\n"
        L"insmod iso9660\n"
        L"insmod loopback\n"
        L"insmod search\n"
        L"insmod search_fs_file\n"
        L"\n"
        L"set isofile=\"{0}\"\n"
        L"search --no-floppy --set=isodev --file $isofile\n"
        L"\n"
        L"menuentry \"Linux Mint Kurulumu\" {{\n"
        L"    loopback loop ($isodev)$isofile\n"
        L"    linux (loop){1} boot=casper iso-scan/filename=$isofile quiet splash --\n"
        L"    initrd (loop){2}\n"
        L"}}\n"
        L"\n"
        L"menuentry \"Windows Boot Manager\" {{\n"
        L"    search --no-floppy --set=windev --file /EFI/Microsoft/Boot/bootmgfw.efi\n"
        L"    chainloader ($windev)/EFI/Microsoft/Boot/bootmgfw.efi\n"
        L"}}\n",
        isoGrubPath, kernel, initrd);
}

// Files copied from the ISO (a CD filesystem) come with the READ-ONLY attribute.
// If not cleared, the revert script's "rd /s /q" can't delete them silently and
// GRUB files can stay on the ESP (this was one component of the rare "GRUB stuck"
// case). Attributes are reset to normal as soon as copying finishes.
static void clearReadOnly(const fs::path& root) {
    std::error_code ec;
    if (!fs::exists(root, ec)) return;
    SetFileAttributesW(root.c_str(), FILE_ATTRIBUTE_NORMAL);
    if (fs::is_directory(root, ec)) {
        for (auto& e : fs::recursive_directory_iterator(
                 root, fs::directory_options::skip_permission_denied, ec))
            SetFileAttributesW(e.path().c_str(), FILE_ATTRIBUTE_NORMAL);
    }
}

// Main install
bool setupEsp(const std::wstring& isoDriveRoot,
              const std::wstring& isoGrubPath,
              const std::function<void(int)>& progress,
              EspResult& out,
              std::wstring& err) {
    if (!detectCasper(isoDriveRoot, out.kernelPath, out.initrdPath)) {
        err = L"ISO içinde casper çekirdeği bulunamadı (beklenmeyen ISO düzeni)";
        return false;
    }
    logx::write(L"Casper detected: " + out.kernelPath + L" + " + out.initrdPath);

    wchar_t esp = mountEsp(err);
    if (!esp) return false;

    bool ok = false;
    std::error_code ec;
    do {
        std::wstring espRoot = std::format(L"{}:\\", esp);

        // Free-space check (modules + efi ~8 MB; let's require 12 MB)
        ULARGE_INTEGER freeB{};
        GetDiskFreeSpaceExW(espRoot.c_str(), &freeB, nullptr, nullptr);
        if (freeB.QuadPart < 12ull * 1024 * 1024) {
            err = L"EFI bölümünde yeterli boş alan yok (en az 12 MB gerekli)";
            break;
        }
        if (progress) progress(5);

        // 1) EFI loaders: ISO:\EFI\BOOT\*.efi -> ESP:\EFI\MintAssistant\
        //    (bootx64.efi = shim, grubx64.efi = GRUB; ikisi de Mint ISO'sundan)
        fs::path srcEfi = fs::path(isoDriveRoot) / L"EFI" / L"BOOT";
        fs::path dstEfi = fs::path(espRoot) / cfg::ESP_SUBDIR;
        // No old/mixed leftovers: if the folder remains from a previous install
        // it's removed clean first, then written from scratch.
        if (fs::exists(dstEfi, ec)) {
            clearReadOnly(dstEfi);
            fs::remove_all(dstEfi, ec);
        }
        fs::create_directories(dstEfi, ec);
        for (auto& e : fs::directory_iterator(srcEfi, ec)) {
            if (!e.is_regular_file()) continue;
            std::error_code cec;
            fs::copy_file(e.path(), dstEfi / e.path().filename(),
                          fs::copy_options::overwrite_existing, cec);
            if (cec) { err = L"EFI dosyası kopyalanamadı: " + e.path().wstring(); break; }
        }
        if (!err.empty()) break;
        if (!fs::exists(dstEfi / L"grubx64.efi", ec)) {
            err = L"ISO içinde grubx64.efi bulunamadı";
            break;
        }
        clearReadOnly(dstEfi);                       // clear CD-sourced read-only
        if (progress) progress(25);

        // 2) GRUB modules: ISO:\boot\grub\x86_64-efi -> ESP:\boot\grub\x86_64-efi
        //    grubx64.efi's embedded config finds the root via "search --file /.disk/info"
        //    and sets prefix to ($root)/boot/grub; it loads modules from there.
        fs::path srcMod = fs::path(isoDriveRoot) / L"boot" / L"grub" / L"x86_64-efi";
        fs::path dstMod = fs::path(espRoot) / L"boot" / L"grub" / L"x86_64-efi";
        fs::create_directories(dstMod, ec);
        fs::copy(srcMod, dstMod,
                 fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
        if (ec) { err = L"GRUB modülleri kopyalanamadı: " + srcMod.wstring(); break; }
        clearReadOnly(dstMod);

        // Font (for menu appearance, optional)
        fs::path srcFont = fs::path(isoDriveRoot) / L"boot" / L"grub" / L"fonts";
        if (fs::exists(srcFont, ec)) {
            fs::path dstFont = fs::path(espRoot) / L"boot" / L"grub" / L"fonts";
            fs::create_directories(dstFont, ec);
            fs::copy(srcFont, dstFont,
                     fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
            ec.clear();   // continue even if the font can't be copied
            clearReadOnly(dstFont);
        }
        if (progress) progress(70);

        // 3) Marker files (.disk/info + our own marker)
        fs::path dskDir = fs::path(espRoot) / L".disk";
        fs::create_directories(dskDir, ec);
        fs::path srcInfo = fs::path(isoDriveRoot) / L".disk" / L"info";
        std::error_code iec;
        if (fs::exists(srcInfo, iec))
            fs::copy_file(srcInfo, dskDir / L"info", fs::copy_options::overwrite_existing, iec);
        else
            util::writeTextFileUtf8((dskDir / L"info").wstring(), L"MintUpgradeAssistant", true);
        clearReadOnly(dskDir);
        util::writeTextFileUtf8((fs::path(espRoot) / cfg::ESP_MARKER).wstring(),
                                L"MintUpgradeAssistant", true);
        if (progress) progress(80);

        // 4) grub.cfg — the actual config: ESP:\boot\grub\grub.cfg
        //    If a file (from another install) exists at the same path, back it up as .bak.
        fs::path cfgPath = fs::path(espRoot) / L"boot" / L"grub" / L"grub.cfg";
        if (fs::exists(cfgPath, ec)) {
            std::ifstream f(cfgPath, std::ios::binary);
            std::string head(80, '\0');
            f.read(head.data(), 80);
            if (head.find("MintUpgradeAssistant") == std::string::npos) {
                std::error_code bec;
                fs::copy_file(cfgPath, fs::path(cfgPath.wstring() + L".bak"),
                              fs::copy_options::overwrite_existing, bec);
                logx::write(L"Existing grub.cfg backed up: grub.cfg.bak");
            }
        }
        std::wstring cfgTxt = grubCfgText(isoGrubPath, out.kernelPath, out.initrdPath);
        if (!util::writeTextFileUtf8(cfgPath.wstring(), cfgTxt, true)) {   // LF only
            err = L"grub.cfg yazılamadı";
            break;
        }
        // Some GRUB builds look for $cmdpath/grub.cfg; a backup copy:
        util::writeTextFileUtf8((dstEfi / L"grub.cfg").wstring(), cfgTxt, true);
        if (progress) progress(95);

        out.grubEfiPathBcd = std::wstring(L"\\") + cfg::ESP_SUBDIR + L"\\grubx64.efi";
        ok = true;
    } while (false);

    unmountEsp(esp);
    if (ok) {
        logx::write(L"ESP setup complete: " + out.grubEfiPathBcd);
        if (progress) progress(100);
    }
    return ok;
}
