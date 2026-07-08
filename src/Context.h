#pragma once
// ============================================================================
//  Context.h — Application state shared between the UI and worker threads
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "core/SystemInfo.h"
#include "core/AppCompat.h"
#include "Config.h"
#include <atomic>
#include <mutex>
#include <string>
#include <format>

// Worker-thread messages posted to the UI
#define WM_APP_CHECKDONE  (WM_APP + 1)   // system check + version discovery done
#define WM_APP_SCANDONE   (WM_APP + 2)   // app scan done
#define WM_APP_PROGRESS   (WM_APP + 3)   // wParam = percent (0-100), lParam = phase
#define WM_APP_DONE       (WM_APP + 4)   // wParam = 0 success / 1 error

struct AppContext {
    SystemInfo          sys;
    const cfg::Edition* edition = nullptr;
    CompatResult        compat;

    // --- Version info (filled by VersionCheck; fallback stays if no network) --
    std::wstring mintVersion  = cfg::FALLBACK_VERSION;   // e.g. "22.3"
    std::wstring mintCodename = cfg::FALLBACK_CODENAME;  // only for a known version
    std::wstring mirrorBase   = cfg::MIRRORS[0];         // the mirror that responded
    bool versionDiscovered = false;                      // true: verified online

    // Download state: 0 not started, 1 in progress, 2 completed, 3 error
    std::atomic<int>      dlState{ 0 };
    std::atomic<uint64_t> dlDone { 0 };
    std::atomic<uint64_t> dlTotal{ 0 };

    std::atomic<bool> cancel{ false };

    std::mutex   mtx;                    // guards the fields below
    std::wstring errMsg;
    std::wstring expectedSha;            // expected digest from sha256sum.txt
    std::wstring bootGuid;               // GUID of the created BCD entry

    // Derived helpers
    std::wstring isoFile() const {       // linuxmint-22.3-cinnamon-64bit.iso
        return cfg::isoFileName(mintVersion, edition->id);
    }
    std::wstring isoPath() const {       // C:\MintInstall\linuxmint-....iso
        return std::wstring(cfg::INSTALL_DIR) + L"\\" + isoFile();
    }
    std::wstring isoUrl() const {        // full URL via the default mirror
        return mirrorBase + mintVersion + L"/" + isoFile();
    }
    std::wstring verName() const {       // Linux Mint 22.3 "Zena"
        if (mintCodename.empty())
            return std::format(L"Linux Mint {}", mintVersion);
        return std::format(L"Linux Mint {} \"{}\"", mintVersion, mintCodename);
    }

    void setError(const std::wstring& e) { std::lock_guard lk(mtx); errMsg = e; }
    std::wstring getError()              { std::lock_guard lk(mtx); return errMsg; }
};

inline AppContext g_ctx;                 // single instance (C++17 inline variable)
