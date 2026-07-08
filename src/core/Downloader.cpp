#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <winhttp.h>
#include "Downloader.h"
#include "Log.h"
#include <vector>
#include <format>
#include <cstdlib>

#pragma comment(lib, "winhttp.lib")

// May be undefined in older SDK headers; guarded definitions:
#ifndef WINHTTP_OPTION_DECOMPRESSION
#define WINHTTP_OPTION_DECOMPRESSION        118
#define WINHTTP_DECOMPRESSION_FLAG_GZIP     0x00000001
#define WINHTTP_DECOMPRESSION_FLAG_DEFLATE  0x00000002
#define WINHTTP_DECOMPRESSION_FLAG_ALL \
        (WINHTTP_DECOMPRESSION_FLAG_GZIP | WINHTTP_DECOMPRESSION_FLAG_DEFLATE)
#endif

namespace dl {

struct Url { std::wstring host, path; INTERNET_PORT port = 443; bool https = true; };

static bool crack(const std::wstring& url, Url& u) {
    URL_COMPONENTS uc{ sizeof(uc) };
    wchar_t host[256], path[1024];
    uc.lpszHostName = host;  uc.dwHostNameLength = 255;
    uc.lpszUrlPath  = path;  uc.dwUrlPathLength  = 1023;
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &uc)) return false;
    u.host.assign(host, uc.dwHostNameLength);
    u.path.assign(path, uc.dwUrlPathLength);
    u.port  = uc.nPort;
    u.https = (uc.nScheme == INTERNET_SCHEME_HTTPS);
    return true;
}

struct Session {
    HINTERNET s = nullptr, c = nullptr, r = nullptr;
    ~Session() {
        if (r) WinHttpCloseHandle(r);
        if (c) WinHttpCloseHandle(c);
        if (s) WinHttpCloseHandle(s);
    }
};

// Common request setup; adds a Range header if resumeFrom>0.
static bool openRequest(Session& ss, const Url& u, uint64_t resumeFrom,
                        bool quick, std::wstring& err) {
    // Some CDNs return different content/blocks for unknown UAs;
    // a standard-looking UA gives the broadest compatibility.
    ss.s = WinHttpOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                       L"MintUpgradeAssistant/1.0",
                       WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                       WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!ss.s) { err = L"WinHttpOpen: " + logx::lastError(); return false; }

    // quick: short for version/checksum queries; loose timeout for the ISO download
    if (quick) WinHttpSetTimeouts(ss.s,  8000,  8000, 10000, 10000);
    else       WinHttpSetTimeouts(ss.s, 20000, 30000, 60000, 60000);

    // Some servers/CDNs send directory pages gzip'd even when not requested;
    // WinHTTP does NOT decompress this by default and the content looks like
    // "garbage" (the regex finds nothing). We enable transparent decompression
    // ONLY for small text queries — the ISO download path is intentionally left
    // untouched (so Range/progress math doesn't change). On unsupported systems
    // the call fails silently and behavior stays as before.
    if (quick) {
        DWORD dec = WINHTTP_DECOMPRESSION_FLAG_ALL;
        WinHttpSetOption(ss.s, WINHTTP_OPTION_DECOMPRESSION, &dec, sizeof(dec));
    }

    ss.c = WinHttpConnect(ss.s, u.host.c_str(), u.port, 0);
    if (!ss.c) { err = L"WinHttpConnect: " + logx::lastError(); return false; }

    ss.r = WinHttpOpenRequest(ss.c, L"GET", u.path.c_str(), nullptr,
                              WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                              u.https ? WINHTTP_FLAG_SECURE : 0);
    if (!ss.r) { err = L"WinHttpOpenRequest: " + logx::lastError(); return false; }

    if (resumeFrom > 0) {
        std::wstring range = std::format(L"Range: bytes={}-", resumeFrom);
        WinHttpAddRequestHeaders(ss.r, range.c_str(), (DWORD)-1,
                                 WINHTTP_ADDREQ_FLAG_ADD);
    }
    if (!WinHttpSendRequest(ss.r, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(ss.r, nullptr)) {
        err = L"HTTP request failed (check your network connection): " + logx::lastError();
        return false;
    }
    return true;
}

static DWORD statusCode(HINTERNET r) {
    DWORD code = 0, sz = sizeof(code);
    WinHttpQueryHeaders(r, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &code, &sz, WINHTTP_NO_HEADER_INDEX);
    return code;
}

static uint64_t contentLength(HINTERNET r) {
    wchar_t buf[32];
    DWORD sz = sizeof(buf);
    if (WinHttpQueryHeaders(r, WINHTTP_QUERY_CONTENT_LENGTH,
                            WINHTTP_HEADER_NAME_BY_INDEX, buf, &sz, WINHTTP_NO_HEADER_INDEX))
        return (uint64_t)_wtoi64(buf);
    return 0;
}

bool downloadFile(const std::wstring& url, const std::wstring& destPath,
                  const std::function<void(uint64_t, uint64_t)>& progress,
                  std::atomic<bool>& cancel, std::wstring& err) {
    Url u;
    if (!crack(url, u)) { err = L"Geçersiz URL: " + url; return false; }

    std::wstring part = destPath + L".part";

    // If a partial file exists, get its size -> resume with Range
    uint64_t resumeFrom = 0;
    {
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (GetFileAttributesExW(part.c_str(), GetFileExInfoStandard, &fad))
            resumeFrom = ((uint64_t)fad.nFileSizeHigh << 32) | fad.nFileSizeLow;
    }

    Session ss;
    if (!openRequest(ss, u, resumeFrom, false, err)) return false;

    DWORD code = statusCode(ss.r);
    uint64_t total = 0, done = 0;
    bool append = false;

    if (code == 206 && resumeFrom > 0) {               // Sunucu devam etmeyi kabul etti
        append = true;
        done   = resumeFrom;
        total  = resumeFrom + contentLength(ss.r);
        logx::write(std::format(L"Resuming download from {} bytes", resumeFrom));
    } else if (code == 200) {                          // Start from scratch
        total = contentLength(ss.r);
        if (resumeFrom > 0) logx::write(L"Server did not support Range; downloading from start");
    } else {
        err = std::format(L"Sunucu {} yanıtı döndürdü ({})", code, url);
        return false;
    }

    HANDLE f = CreateFileW(part.c_str(),
                           append ? FILE_APPEND_DATA : GENERIC_WRITE,
                           0, nullptr,
                           append ? OPEN_ALWAYS : CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        err = L"Dosya yazılamıyor: " + part + L" " + logx::lastError();
        return false;
    }
    if (append) SetFilePointer(f, 0, nullptr, FILE_END);

    std::vector<BYTE> buf(256 * 1024);
    bool ok = true;
    for (;;) {
        if (cancel.load()) { err = L"Cancelled"; ok = false; break; }
        DWORD got = 0;
        if (!WinHttpReadData(ss.r, buf.data(), (DWORD)buf.size(), &got)) {
            err = L"Ağ okuma hatası: " + logx::lastError();
            ok = false;
            break;
        }
        if (got == 0) break;                            // bitti
        DWORD wrote = 0;
        if (!WriteFile(f, buf.data(), got, &wrote, nullptr) || wrote != got) {
            err = L"Disk yazma hatası: " + logx::lastError();
            ok = false;
            break;
        }
        done += got;
        if (progress) progress(done, total);
    }
    CloseHandle(f);
    if (!ok) return false;                              // .part remains -> resume next launch

    if (total && done != total) {
        err = std::format(L"İndirme eksik kaldı ({}/{})", done, total);
        return false;
    }
    if (!MoveFileExW(part.c_str(), destPath.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        err = L"Dosya adı değiştirilemedi: " + logx::lastError();
        return false;
    }
    logx::write(std::format(L"Download complete: {} ({} bytes)", destPath, done));
    return true;
}

bool httpGetText(const std::wstring& url, std::wstring& outText, std::wstring& err,
                 bool quickTimeout) {
    Url u;
    if (!crack(url, u)) { err = L"Geçersiz URL: " + url; return false; }

    Session ss;
    if (!openRequest(ss, u, 0, quickTimeout, err)) return false;
    if (statusCode(ss.r) != 200) {
        err = std::format(L"Sunucu {} yanıtı döndürdü ({})", statusCode(ss.r), url);
        return false;
    }

    std::string raw;
    char buf[8192];
    DWORD got = 0;
    while (WinHttpReadData(ss.r, buf, sizeof(buf), &got) && got)
        raw.append(buf, got);

    int need = MultiByteToWideChar(CP_UTF8, 0, raw.data(), (int)raw.size(), nullptr, 0);
    outText.assign(need, L'\0');
    if (need)
        MultiByteToWideChar(CP_UTF8, 0, raw.data(), (int)raw.size(), outText.data(), need);
    return true;
}

} // namespace dl
