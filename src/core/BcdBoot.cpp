#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "BcdBoot.h"
#include "Util.h"
#include "Log.h"
#include "../Config.h"
#include <format>

// Captures the {xxxxxxxx-xxxx-...} GUID from bcdedit output (requires a '-' to
// filter out aliases like {bootmgr}).
static std::wstring parseGuid(const std::wstring& out) {
    size_t a = out.find(L'{');
    while (a != std::wstring::npos) {
        size_t b = out.find(L'}', a);
        if (b == std::wstring::npos) break;
        std::wstring g = out.substr(a, b - a + 1);
        if (g.find(L'-') != std::wstring::npos) return g;
        a = out.find(L'{', b);
    }
    return L"";
}

// 
//  Removes ALL leftover "Linux Mint" firmware entries from previous installs.
//
//  Why needed: each install creates a NEW entry via "bcdedit /copy {bootmgr}".
//  Across repeated installs entries pile up; since the revert script deletes
//  only the last GUID, an old entry can remain in the boot order and launch
//  GRUB (the rare case the user hit). By sweeping RIGHT BEFORE installing we
//  prevent this accumulation at the root.
//
//  The method is language-independent: instead of searching for keywords in
//  bcdedit output, we walk line by line; when a hyphenated {guid} is seen it's
//  remembered, and if later lines of the block contain "MintAssistant" (path)
//  or the entry description, the remembered GUID is deleted.
// 
static void removeStaleMintEntries(const std::wstring& bcd) {
    std::wstring out = util::runCapture(std::format(L"\"{}\" /enum firmware", bcd));
    std::wstring lastGuid;
    int removed = 0;

    size_t pos = 0;
    while (pos < out.size()) {
        size_t eol = out.find(L'\n', pos);
        if (eol == std::wstring::npos) eol = out.size();
        std::wstring line = out.substr(pos, eol - pos);
        pos = eol + 1;

        size_t a = line.find(L'{');
        if (a != std::wstring::npos) {
            size_t b = line.find(L'}', a);
            if (b != std::wstring::npos) {
                std::wstring g = line.substr(a, b - a + 1);
                if (g.find(L'-') != std::wstring::npos)
                    lastGuid = g;                        // last real GUID seen
            }
        }
        if (!lastGuid.empty() &&
            (line.find(L"MintAssistant") != std::wstring::npos ||
             line.find(cfg::BOOT_ENTRY_DESC) != std::wstring::npos)) {
            logx::write(L"Removing stale Mint entry: " + lastGuid);
            util::runCapture(std::format(
                L"\"{}\" /set {{fwbootmgr}} displayorder {} /remove", bcd, lastGuid));
            util::runCapture(std::format(L"\"{}\" /delete {} /f", bcd, lastGuid));
            lastGuid.clear();
            ++removed;
        }
    }
    if (removed)
        logx::write(std::format(L"{} stale Mint entries removed", removed));
}

bool addFirmwareBootEntry(const std::wstring& efiPath,
                          std::wstring& outGuid, std::wstring& err) {
    std::wstring bcd = util::sys32(L"bcdedit.exe");
    DWORD ec = 0;

    // 0) First sweep leftover entries from old installs (so duplicates don't accumulate)
    removeStaleMintEntries(bcd);

    // 1) Copy {bootmgr} -> a new firmware-application entry on the ESP.
    //    The copy automatically inherits the device (device=ESP partition) info.
    std::wstring out = util::runCapture(
        std::format(L"\"{}\" /copy {{bootmgr}} /d \"{}\"", bcd, cfg::BOOT_ENTRY_DESC), &ec);
    std::wstring guid = parseGuid(out);
    if (ec != 0 || guid.empty()) {
        err = L"bcdedit /copy başarısız: " + util::trim(out);
        return false;
    }
    logx::write(L"BCD entry created: " + guid);

    // 2) Girdinin yolu bizim grubx64.efi olsun
    out = util::runCapture(std::format(L"\"{}\" /set {} path {}", bcd, guid, efiPath), &ec);
    if (ec != 0) {
        err = L"bcdedit /set path başarısız: " + util::trim(out);
        util::runCapture(std::format(L"\"{}\" /delete {} /f", bcd, guid));
        return false;
    }

    // 3) Move it to the FRONT of the firmware boot order -> default boot
    out = util::runCapture(
        std::format(L"\"{}\" /set {{fwbootmgr}} displayorder {} /addfirst", bcd, guid), &ec);
    if (ec != 0) {
        err = L"bcdedit displayorder başarısız: " + util::trim(out);
        util::runCapture(std::format(L"\"{}\" /delete {} /f", bcd, guid));
        return false;
    }

    outGuid = guid;
    util::writeTextFileUtf8(std::wstring(cfg::INSTALL_DIR) + L"\\boot_girdisi.txt",
                            guid + L"\r\n" + efiPath + L"\r\n", false);
    logx::write(L"GRUB set first in firmware boot order (default): " + efiPath);
    return true;
}

bool writeRevertScript(const std::wstring& guid) {
    // The script is kept ASCII (against console code-page issues).
    //
    // Hardening (against the rare "GRUB stuck" case):
    //  • After deleting the saved GUID, the :supur subroutine finds and deletes
    //    ALL firmware entries whose description or path is Mint, using a
    //    language-independent GUID pattern (including leftovers from old installs).
    //  • attrib now also covers the boot\grub tree: files copied from the ISO can
    //    be READ-ONLY, so if attributes aren't cleared rd fails silently and the
    //    GRUB files stay on the ESP.
    //  • The free-letter search for the ESP tries the whole Z..D range.
    //  • Real verification at the end: is any Mint trace left in the firmware list?
    std::wstring s = std::format(
        L"@echo off\r\n"
        L"setlocal enableextensions\r\n"
        L"REM ==== MintUpgradeAssistant geri alma betigi ====\r\n"
        L"\r\n"
        L"REM --- Yonetici degilse kendini yeniden yukselt ---\r\n"
        L"net session >nul 2>&1\r\n"
        L"if %errorlevel% neq 0 (\r\n"
        L"  echo Yonetici haklari isteniyor...\r\n"
        L"  powershell -NoProfile -Command \"Start-Process -Verb RunAs -FilePath '%~f0'\"\r\n"
        L"  exit /b\r\n"
        L")\r\n"
        L"\r\n"
        L"echo ============================================\r\n"
        L"echo  Linux Mint onyukleme kaldirma\r\n"
        L"echo ============================================\r\n"
        L"echo.\r\n"
        L"\r\n"
        L"echo [1/3] Firmware girdileri siliniyor...\r\n"
        L"bcdedit /set {{fwbootmgr}} displayorder {0} /remove >nul 2>&1\r\n"
        L"bcdedit /delete {0} /f >nul 2>&1\r\n"
        L"call :supur\r\n"
        L"REM Guvenlik agi: Windows Boot Manager her durumda sirada EN BASA alinir;\r\n"
        L"REM boylece temizlik yarim kalsa bile bir sonraki acilis Windows olur.\r\n"
        L"bcdedit /set {{fwbootmgr}} displayorder {{bootmgr}} /addfirst >nul 2>&1\r\n"
        L"echo.\r\n"
        L"\r\n"
        L"echo [2/3] EFI bolumu temizleniyor...\r\n"
        L"set \"EL=\"\r\n"
        L"for %%L in (Z Y X W V U T S R Q P O N M L K J I H G F E D) do (\r\n"
        L"  if not defined EL (\r\n"
        L"    mountvol %%L: /S >nul 2>&1 && set \"EL=%%L\"\r\n"
        L"  )\r\n"
        L")\r\n"
        L"if defined EL (\r\n"
        L"  echo   EFI bolumu %EL%: olarak baglandi.\r\n"
        L"  attrib -r -s -h \"%EL%:\\EFI\\MintAssistant\\*\" /s /d >nul 2>&1\r\n"
        L"  attrib -r -s -h \"%EL%:\\boot\\grub\\*\" /s /d >nul 2>&1\r\n"
        L"  REM ONCE onyukleme ikilileri silinir: kismi hata olsa bile GRUB\r\n"
        L"  REM yuklenemez ve firmware dogrudan Windows girdisine gecer.\r\n"
        L"  del /f /q \"%EL%:\\EFI\\MintAssistant\\*.efi\" >nul 2>&1\r\n"
        L"  del /f /q \"%EL%:\\EFI\\MintAssistant\\*\" >nul 2>&1\r\n"
        L"  rd /s /q \"%EL%:\\EFI\\MintAssistant\" >nul 2>&1\r\n"
        L"  del /f /q \"%EL%:\\boot\\grub\\grub.cfg\" >nul 2>&1\r\n"
        L"  del /f /q \"%EL%:\\boot\\grub\\grub.cfg.bak\" >nul 2>&1\r\n"
        L"  rd /s /q \"%EL%:\\boot\\grub\\x86_64-efi\" >nul 2>&1\r\n"
        L"  rd /s /q \"%EL%:\\boot\\grub\\fonts\" >nul 2>&1\r\n"
        L"  del /f /q \"%EL%:\\.disk\\info\" >nul 2>&1\r\n"
        L"  del /f /q \"%EL%:\\.disk\\mintassistant\" >nul 2>&1\r\n"
        L"  rd \"%EL%:\\.disk\" >nul 2>&1\r\n"
        L"  rd \"%EL%:\\boot\\grub\" >nul 2>&1\r\n"
        L"  rd \"%EL%:\\boot\" >nul 2>&1\r\n"
        L"  mountvol %EL%: /D >nul 2>&1\r\n"
        L"  echo   [TAMAM] EFI bolumu temizlendi.\r\n"
        L") else (\r\n"
        L"  echo   [UYARI] EFI bolumu baglanamadi ^(bos surucu harfi bulunamadi^).\r\n"
        L")\r\n"
        L"echo.\r\n"
        L"\r\n"
        L"echo [3/3] Dogrulama...\r\n"
        L"bcdedit /enum firmware | findstr /i /c:\"MintAssistant\" /c:\"Linux Mint\" >nul 2>&1\r\n"
        L"if %errorlevel% equ 0 (\r\n"
        L"  echo   [UYARI] Firmware listesinde hala Mint izi gorunuyor.\r\n"
        L"  echo   Betigi bir kez daha calistirin ya da su ciktiyi inceleyin:\r\n"
        L"  echo     bcdedit /enum firmware\r\n"
        L") else (\r\n"
        L"  echo   [TAMAM] Firmware temiz - yeniden baslatinca dogrudan Windows acilir.\r\n"
        L")\r\n"
        L"echo.\r\n"
        L"echo C:\\MintInstall klasorunu (ISO dahil) elle silebilirsiniz.\r\n"
        L"pause\r\n"
        L"exit /b\r\n"
        L"\r\n"
        L":supur\r\n"
        L"REM Firmware'daki her nesneyi tek tek acar (GUID bazinda); icinde\r\n"
        L"REM \"MintAssistant\" yolu ya da \"Linux Mint Kurulumu\" aciklamasi\r\n"
        L"REM gecenleri siler. Dil bagimsizdir ve gecikmeli genisletme gerektirmez\r\n"
        L"REM (borulu komutlarda !var! acilmadigi icin eski yontem calismiyordu).\r\n"
        L"for /f \"tokens=2 delims={{}}\" %%G in ('bcdedit /enum firmware ^| findstr /c:\"{{\"') do (\r\n"
        L"  bcdedit /enum {{%%G}} 2>nul | findstr /i /c:\"MintAssistant\" /c:\"Linux Mint Kurulumu\" >nul && (\r\n"
        L"    bcdedit /set {{fwbootmgr}} displayorder {{%%G}} /remove >nul 2>&1\r\n"
        L"    bcdedit /delete {{%%G}} /f >nul 2>&1\r\n"
        L"    echo   temizlendi: {{%%G}}\r\n"
        L"  )\r\n"
        L")\r\n"
        L"goto :eof\r\n",
        guid);
    return util::writeTextFileUtf8(std::wstring(cfg::INSTALL_DIR) + L"\\geri_al.cmd", s, false);
}
