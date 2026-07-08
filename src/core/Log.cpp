#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "Log.h"
#include "../Config.h"
#include <mutex>
#include <fstream>
#include <format>
#include <filesystem>

namespace fs = std::filesystem;

namespace logx {

static std::mutex g_mtx;
static std::wstring g_path;

void init() {
    std::error_code ec;
    fs::create_directories(cfg::INSTALL_DIR, ec);
    g_path = std::wstring(cfg::INSTALL_DIR) + L"\\assistant.log";
    write(L"================ Session started ================");
}

void write(const std::wstring& line) {
    if (g_path.empty()) return;
    std::lock_guard lk(g_mtx);
    SYSTEMTIME st; GetLocalTime(&st);
    std::wstring stamped = std::format(L"[{:02}:{:02}:{:02}] {}\r\n",
                                       st.wHour, st.wMinute, st.wSecond, line);
    // UTF-8 olarak ekle
    int need = WideCharToMultiByte(CP_UTF8, 0, stamped.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(need > 0 ? need - 1 : 0, '\0');
    if (need > 1)
        WideCharToMultiByte(CP_UTF8, 0, stamped.c_str(), -1, utf8.data(), need, nullptr, nullptr);
    std::ofstream f(g_path, std::ios::app | std::ios::binary);
    f.write(utf8.data(), (std::streamsize)utf8.size());
}

std::wstring lastError(DWORD err) {
    if (!err) err = GetLastError();
    wchar_t* buf = nullptr;
    DWORD n = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                             FORMAT_MESSAGE_IGNORE_INSERTS,
                             nullptr, err, 0, (LPWSTR)&buf, 0, nullptr);
    std::wstring s = n && buf ? std::wstring(buf, n) : L"";
    if (buf) LocalFree(buf);
    while (!s.empty() && (s.back() == L'\r' || s.back() == L'\n' || s.back() == L' ')) s.pop_back();
    return std::format(L"(hata {}) {}", err, s);
}

} // namespace logx
