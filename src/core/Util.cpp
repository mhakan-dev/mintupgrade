#include "Util.h"
#include "Log.h"
#include <string>
#include <format>
#include <fstream>
#include <algorithm>
#include <cwctype>

namespace util {

std::wstring sys32(const wchar_t* exeName) {
    wchar_t dir[MAX_PATH];
    GetSystemDirectoryW(dir, MAX_PATH);
    return std::format(L"{}\\{}", dir, exeName);
}

std::wstring runCapture(const std::wstring& commandLine, DWORD* exitCode) {
    SECURITY_ATTRIBUTES sa{ sizeof(sa), nullptr, TRUE };
    HANDLE rd = nullptr, wr = nullptr;
    if (!CreatePipe(&rd, &wr, &sa, 0)) return L"";
    SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si{ sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = wr;
    si.hStdError  = wr;
    si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi{};
    std::wstring cmd = commandLine;                    // CreateProcess needs a writable buffer
    BOOL ok = CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE,
                             CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    CloseHandle(wr);
    if (!ok) {
        logx::write(L"runCapture: failed to start: " + commandLine + L" " + logx::lastError());
        CloseHandle(rd);
        if (exitCode) *exitCode = (DWORD)-1;
        return L"";
    }

    std::string raw;
    char buf[4096];
    DWORD got = 0;
    while (ReadFile(rd, buf, sizeof(buf), &got, nullptr) && got)
        raw.append(buf, got);
    CloseHandle(rd);

    WaitForSingleObject(pi.hProcess, 120000);
    DWORD ec = 0;
    GetExitCodeProcess(pi.hProcess, &ec);
    if (exitCode) *exitCode = ec;
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Console tools use the OEM code page
    int need = MultiByteToWideChar(CP_OEMCP, 0, raw.data(), (int)raw.size(), nullptr, 0);
    std::wstring out(need, L'\0');
    if (need)
        MultiByteToWideChar(CP_OEMCP, 0, raw.data(), (int)raw.size(), out.data(), need);
    return out;
}

wchar_t findFreeDriveLetter() {
    DWORD mask = GetLogicalDrives();
    for (int i = 25; i >= 3; --i)                      // Z: -> D:
        if (!(mask & (1u << i))) return (wchar_t)(L'A' + i);
    return 0;
}

bool writeTextFileUtf8(const std::wstring& path, const std::wstring& text, bool lfOnly) {
    std::wstring t = text;
    if (lfOnly) {                                      // LF-only for grub.cfg
        std::wstring r;
        r.reserve(t.size());
        for (wchar_t c : t) if (c != L'\r') r += c;
        t = r;
    }
    int need = WideCharToMultiByte(CP_UTF8, 0, t.c_str(), (int)t.size(), nullptr, 0, nullptr, nullptr);
    std::string utf8(need, '\0');
    if (need)
        WideCharToMultiByte(CP_UTF8, 0, t.c_str(), (int)t.size(), utf8.data(), need, nullptr, nullptr);
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f.write(utf8.data(), (std::streamsize)utf8.size());
    return (bool)f;
}

bool enableShutdownPrivilege() {
    HANDLE tok;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tok))
        return false;
    TOKEN_PRIVILEGES tp{};
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
    BOOL ok = AdjustTokenPrivileges(tok, FALSE, &tp, 0, nullptr, nullptr);
    CloseHandle(tok);
    return ok && GetLastError() == ERROR_SUCCESS;
}

void rebootNow() {
    enableShutdownPrivilege();
    ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
                              SHTDN_REASON_MINOR_RECONFIG | SHTDN_REASON_FLAG_PLANNED);
}

std::wstring trim(std::wstring s) {
    auto sp = [](wchar_t c){ return c == L' ' || c == L'\t' || c == L'\r' || c == L'\n'; };
    while (!s.empty() && sp(s.front())) s.erase(s.begin());
    while (!s.empty() && sp(s.back()))  s.pop_back();
    return s;
}

std::wstring toLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c){ return (wchar_t)std::towlower(c); });
    return s;
}

std::wstring fmtGB(uint64_t mb) {
    double gb = mb / 1024.0;
    std::wstring s = std::format(L"{:.1f} GB", gb);
    for (auto& c : s) if (c == L'.') c = L',';         // TR decimal separator
    return s;
}

} // namespace util
