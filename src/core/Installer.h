#pragma once
// ============================================================================
//  Installer.h — Download and install threads (orchestration)
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Same order as the INS_PHASES array in Strings.h
enum InstallPhase {
    PH_PREP = 0,     // Preparing
    PH_DOWNLOAD,     // ISO indiriliyor        ( %0  - %70 )
    PH_VERIFY,       // Verifying SHA-256      ( 70% - 80% )
    PH_MOUNT,        // Mounting the ISO       ( 80%        )
    PH_COPY,         // Copying to the ESP     ( 82% - 93% )
    PH_GRUB,         // Writing grub.cfg       ( 94%        )
    PH_BCD,          // BCD girdisi            ( %96        )
    PH_DONE          // Completed              ( 100%       )
};

// Downloads sha256sum.txt + ISO in the background (starts after the system check)
void startDownloadThread(HWND notify);

// Waits for download -> verifies -> mounts ISO -> ESP -> BCD. Result reported via WM_APP_DONE.
void startInstallThread(HWND notify);
