#pragma once
// ============================================================================
//  Log.h — Thread-safe logging to C:\MintInstall\assistant.log
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace logx {
    void init();                              // Prepares the folder and file
    void write(const std::wstring& line);     // Appends a timestamped line
    std::wstring lastError(DWORD err = 0);    // GetLastError -> readable text
}
