#pragma once
// ============================================================================
//  Sha256.h — Streaming SHA-256 via BCrypt (with progress feedback for the 3 GB ISO)
// ============================================================================
#include <string>
#include <functional>
#include <atomic>

// Returns the file's SHA-256 digest as lowercase hex. On error/cancel -> empty string.
std::wstring sha256File(const std::wstring& path,
                        const std::function<void(uint64_t done, uint64_t total)>& progress,
                        std::atomic<bool>& cancel,
                        std::wstring& err);
