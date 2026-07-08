#pragma once
// ============================================================================
//  Util.h — Shared helpers (process execution, drive letter, restart)
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace util {

// Runs a command and captures stdout+stderr. exitCode is optional.
// systemTool=true prepends the System32 path to the command (bcdedit, mountvol...).
std::wstring runCapture(const std::wstring& commandLine, DWORD* exitCode = nullptr);
std::wstring sys32(const wchar_t* exeName);           // "C:\Windows\System32\<exe>"

wchar_t findFreeDriveLetter();                         // First free letter from Z downward, else 0
bool    writeTextFileUtf8(const std::wstring& path, const std::wstring& text, bool lfOnly);
bool    enableShutdownPrivilege();
void    rebootNow();

std::wstring trim(std::wstring s);
std::wstring toLower(std::wstring s);
std::wstring fmtGB(uint64_t mb);                       // 8123 MB -> "7,9 GB"

} // namespace util
