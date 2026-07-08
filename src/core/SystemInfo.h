#pragma once
// ============================================================================
//  SystemInfo.h — Collects hardware and boot information
// ============================================================================
#include <string>
#include <cstdint>

struct SystemInfo {
    uint64_t     ramMB        = 0;
    int          logicalCores = 0;
    std::wstring cpuName;
    bool         is64         = false;
    uint64_t     freeDiskMB_C = 0;
    bool         uefi         = false;
    bool         secureBoot   = false;
    std::wstring windowsName;

    bool ramOK()   const;
    bool ramWarn() const;                  // Below 2 GB: NOT a blocker, just a warning
    bool diskOK()  const;
    bool canProceed() const;               // UEFI + SB off + 64-bit + disk
};

SystemInfo collectSystemInfo();
