#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "Installer.h"
#include "../Context.h"
#include "../Strings.h"
#include "Downloader.h"
#include "Sha256.h"
#include "IsoMount.h"
#include "EspSetup.h"
#include "BcdBoot.h"
#include "Util.h"
#include "Log.h"
#include <thread>
#include <format>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// Mirror order to try: the one that responded during version discovery first, then the rest
static std::vector<std::wstring> mirrorOrder() {
    std::vector<std::wstring> v;
    v.push_back(g_ctx.mirrorBase);
    for (const wchar_t* m : cfg::MIRRORS)
        if (g_ctx.mirrorBase != m) v.push_back(m);
    return v;
}

// ============================================================================
//  DOWNLOAD THREAD  (sha256sum.txt -> expected digest, then the ISO)
//  At each step mirrors are tried IN ORDER; if one fails we move to the next.
// ============================================================================
static void downloadProc() {
    g_ctx.dlState = 1;
    const std::wstring isoName = g_ctx.isoFile();
    const std::wstring ver     = g_ctx.mintVersion;
    std::vector<std::wstring> bases = mirrorOrder();

    // --- 1) sha256sum.txt: get the expected digest from the first working mirror -
    std::wstring expected, okBase;
    for (const auto& b : bases) {
        std::wstring url = b + ver + L"/" + cfg::SHA_FILE;
        std::wstring txt, e;
        if (!dl::httpGetText(url, txt, e, /*quick*/ true)) {
            logx::write(std::format(L"Checksum fetch failed [{}]: {}", url, e));
            continue;
        }
        size_t pos = 0;                                  // scan line by line
        while (pos < txt.size()) {
            size_t eol = txt.find(L'\n', pos);
            if (eol == std::wstring::npos) eol = txt.size();
            std::wstring line = util::trim(txt.substr(pos, eol - pos));
            if (line.size() > 64 && line.find(isoName) != std::wstring::npos) {
                expected = util::toLower(line.substr(0, 64));
                break;
            }
            pos = eol + 1;
        }
        if (expected.size() == 64) { okBase = b; break; }
        expected.clear();
        logx::write(std::format(L"No line for {} in sha256sum.txt [{}]", isoName, b));
    }
    if (expected.empty()) {
        g_ctx.setError(str::ERR_SHA_FMT);
        g_ctx.dlState = 3;
        return;
    }
    { std::lock_guard lk(g_ctx.mtx); g_ctx.expectedSha = expected; }
    logx::write(L"Expected SHA-256: " + expected);

    // Move the working mirror to the front (so the ISO is tried there first too)
    if (okBase != bases.front()) {
        std::vector<std::wstring> r{ okBase };
        for (const auto& b : bases) if (b != okBase) r.push_back(b);
        bases = std::move(r);
    }

    // --- 2) If the ISO is already fully downloaded, skip it (digest verified at install) -
    const std::wstring dest = g_ctx.isoPath();
    std::error_code fec;
    if (fs::exists(dest, fec)) {
        uint64_t sz = (uint64_t)fs::file_size(dest, fec);
        g_ctx.dlDone  = sz;
        g_ctx.dlTotal = sz;
        g_ctx.dlState = 2;
        logx::write(L"ISO already present, download skipped: " + dest);
        return;
    }

    // 3) Download the ISO: mirrors in order, resuming via .part + HTTP Range
    g_ctx.dlTotal = g_ctx.edition->approxMB * 1024ull * 1024ull;   // ilk tahmin
    bool ok = false;
    std::wstring lastErr;
    for (const auto& b : bases) {
        if (g_ctx.cancel) { lastErr = L"Cancelled"; break; }
        std::wstring url = b + ver + L"/" + isoName;
        logx::write(L"Trying download: " + url);
        std::wstring e;
        if (dl::downloadFile(url, dest,
                [](uint64_t d, uint64_t t) {
                    g_ctx.dlDone = d;
                    if (t) g_ctx.dlTotal = t;
                },
                g_ctx.cancel, e)) {
            ok = true;
            break;
        }
        lastErr = e;
        logx::write(std::format(L"Mirror failed [{}]: {}", b, e));
        if (e.find(L"Cancelled") != std::wstring::npos) break;   // if cancelled, don't try the next
    }
    if (!ok) {
        // If cancelled, show the localized "Cancelled."; otherwise append the last error.
        if (g_ctx.cancel) {
            g_ctx.setError(str::ERR_CANCELLED);
        } else {
            wchar_t buf[512];
            swprintf_s(buf, str::ERR_ISO_FMT, lastErr.c_str());
            g_ctx.setError(buf);
        }
        g_ctx.dlState = 3;
        return;
    }
    g_ctx.dlState = 2;
}

void startDownloadThread(HWND) {
    std::thread(downloadProc).detach();
}

// ============================================================================
//  INSTALL THREAD
// ============================================================================
static void post(HWND w, int pct, InstallPhase ph) {
    PostMessageW(w, WM_APP_PROGRESS, (WPARAM)pct, (LPARAM)ph);
}
static void fail(HWND w, const std::wstring& msg) {
    g_ctx.setError(msg);
    logx::write(L"ERROR: " + msg);
    PostMessageW(w, WM_APP_DONE, 1, 0);
}

static void installProc(HWND w) {
    post(w, 0, PH_PREP);

    // If the download errored earlier, try once more (resumes from .part)
    if (g_ctx.dlState.load() == 3) {
        logx::write(L"Retrying download…");
        g_ctx.dlState = 0;
        startDownloadThread(w);
    }

    // 0-70%: wait for the download
    for (;;) {
        int st = g_ctx.dlState.load();
        if (st == 2) break;
        if (st == 3) { fail(w, g_ctx.getError()); return; }
        if (g_ctx.cancel) { fail(w, str::ERR_CANCELLED); return; }
        uint64_t d = g_ctx.dlDone, t = g_ctx.dlTotal;
        post(w, t ? (int)(d * 70 / t) : 0, PH_DOWNLOAD);
        Sleep(250);
    }

    // 70-80%: SHA-256 verification
    post(w, 70, PH_VERIFY);
    std::wstring err;
    std::wstring got = sha256File(g_ctx.isoPath(),
        [w](uint64_t d, uint64_t t) {
            post(w, 70 + (t ? (int)(d * 10 / t) : 0), PH_VERIFY);
        },
        g_ctx.cancel, err);
    if (got.empty()) {
        wchar_t buf[512];
        swprintf_s(buf, str::ERR_SHA_COMPUTE_FMT, err.c_str());
        fail(w, buf);
        return;
    }

    std::wstring expected;
    { std::lock_guard lk(g_ctx.mtx); expected = g_ctx.expectedSha; }
    if (util::toLower(got) != expected) {
        std::error_code fec;
        fs::remove(g_ctx.isoPath(), fec);              // clean up the corrupt file
        fail(w, str::ERR_SHA_MISMATCH);
        return;
    }
    logx::write(L"SHA-256 verified");
    if (g_ctx.cancel) { fail(w, str::ERR_CANCELLED); return; }

    // 80%: mount the ISO as a virtual DVD
    post(w, 80, PH_MOUNT);
    IsoMount iso;
    if (!iso.mount(g_ctx.isoPath(), err)) { fail(w, err); return; }
    std::wstring isoRoot = std::format(L"{}:\\", iso.driveLetter());

    // 82-95%: boot files + grub.cfg to the ESP
    post(w, 82, PH_COPY);
    std::wstring isoGrubPath =
        std::wstring(cfg::INSTALL_DIR_GRUB) + L"/" + g_ctx.isoFile();
    EspResult esp;
    bool espOk = setupEsp(isoRoot, isoGrubPath,
        [w](int p) {
            post(w, 82 + p * 13 / 100, (p >= 80) ? PH_GRUB : PH_COPY);
        },
        esp, err);
    iso.unmount();
    if (!espOk) { fail(w, err); return; }
    if (g_ctx.cancel) { fail(w, str::ERR_CANCELLED); return; }

    // 96%: entry into the Windows Boot Manager
    post(w, 96, PH_BCD);
    std::wstring guid;
    if (!addFirmwareBootEntry(esp.grubEfiPathBcd, guid, err)) { fail(w, err); return; }
    { std::lock_guard lk(g_ctx.mtx); g_ctx.bootGuid = guid; }
    writeRevertScript(guid);

    post(w, 100, PH_DONE);
    logx::write(L"INSTALLATION COMPLETE — ready to restart");
    PostMessageW(w, WM_APP_DONE, 0, 0);
}

void startInstallThread(HWND notify) {
    std::thread(installProc, notify).detach();
}
