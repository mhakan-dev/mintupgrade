#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <bcrypt.h>
#include "Sha256.h"
#include "Log.h"
#include <vector>
#include <format>

#pragma comment(lib, "bcrypt.lib")

std::wstring sha256File(const std::wstring& path,
                        const std::function<void(uint64_t, uint64_t)>& progress,
                        std::atomic<bool>& cancel,
                        std::wstring& err) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        err = L"Dosya açılamadı: " + path + L" " + logx::lastError();
        return L"";
    }
    LARGE_INTEGER size{};
    GetFileSizeEx(f, &size);

    BCRYPT_ALG_HANDLE  alg  = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    std::wstring result;

    do {
        if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) {
            err = L"BCrypt sağlayıcı açılamadı";
            break;
        }
        if (BCryptCreateHash(alg, &hash, nullptr, 0, nullptr, 0, 0) != 0) {
            err = L"BCrypt hash oluşturulamadı";
            break;
        }

        std::vector<BYTE> buf(1024 * 1024);            // 1 MB chunk
        uint64_t done = 0;
        DWORD got = 0;
        bool fail = false;
        while (ReadFile(f, buf.data(), (DWORD)buf.size(), &got, nullptr) && got) {
            if (cancel.load()) { err = L"Cancelled"; fail = true; break; }
            if (BCryptHashData(hash, buf.data(), got, 0) != 0) {
                err = L"BCryptHashData başarısız";
                fail = true;
                break;
            }
            done += got;
            if (progress) progress(done, (uint64_t)size.QuadPart);
        }
        if (fail) break;

        BYTE digest[32];
        if (BCryptFinishHash(hash, digest, sizeof(digest), 0) != 0) {
            err = L"BCryptFinishHash başarısız";
            break;
        }
        wchar_t hex[65];
        for (int i = 0; i < 32; ++i)
            swprintf_s(hex + i * 2, 3, L"%02x", digest[i]);
        result = hex;
    } while (false);

    if (hash) BCryptDestroyHash(hash);
    if (alg)  BCryptCloseAlgorithmProvider(alg, 0);
    CloseHandle(f);
    return result;
}
