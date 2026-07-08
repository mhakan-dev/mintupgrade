#pragma once
// ============================================================================
//  Downloader.h — WinHTTP-based downloader
//   • Large ISO download: resume support (HTTP Range), progress feedback
//   • Small text download: sha256sum.txt
// ============================================================================
#include <string>
#include <functional>
#include <atomic>

namespace dl {

// url'yi destPath'e indirir. destPath varsa boyutundan devam etmeyi dener.
// progress(done,total): total 0 olabilir (sunucu bildirmezse).
bool downloadFile(const std::wstring& url,
                  const std::wstring& destPath,
                  const std::function<void(uint64_t done, uint64_t total)>& progress,
                  std::atomic<bool>& cancel,
                  std::wstring& err);

// Downloads a small text file (e.g. sha256sum.txt) into memory.
bool httpGetText(const std::wstring& url, std::wstring& outText, std::wstring& err,
                 bool quickTimeout = false);   // true: short timeout (version query)

} // namespace dl
