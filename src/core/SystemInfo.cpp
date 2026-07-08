#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "SystemInfo.h"
#include "Util.h"
#include "Log.h"
#include "../Config.h"
#include <format>

static std::wstring regStr(HKEY root, const wchar_t* key, const wchar_t* val) {
    wchar_t buf[512];
    DWORD sz = sizeof(buf);
    if (RegGetValueW(root, key, val, RRF_RT_REG_SZ, nullptr, buf, &sz) == ERROR_SUCCESS)
        return buf;
    return L"";
}

bool SystemInfo::ramOK()   const { return ramMB >= cfg::MIN_RAM_MB; }
bool SystemInfo::ramWarn() const { return !ramOK(); }
bool SystemInfo::diskOK()  const { return freeDiskMB_C >= cfg::MIN_FREE_DISK_MB; }
bool SystemInfo::canProceed() const {
    // Low RAM does NOT block installation; only a warning is shown on the check page.
    return uefi && !secureBoot && is64 && diskOK();
}

SystemInfo collectSystemInfo() {
    SystemInfo si;

    MEMORYSTATUSEX ms{ sizeof(ms) };
    GlobalMemoryStatusEx(&ms);
    si.ramMB = ms.ullTotalPhys / (1024ull * 1024ull);

    SYSTEM_INFO sys{};
    GetNativeSystemInfo(&sys);
    si.logicalCores = (int)sys.dwNumberOfProcessors;
    si.is64 = (sys.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);

    si.cpuName = util::trim(regStr(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString"));
    if (si.cpuName.empty()) si.cpuName = L"Bilinmeyen işlemci";

    ULARGE_INTEGER freeUser{}, total{}, totalFree{};
    if (GetDiskFreeSpaceExW(L"C:\\", &freeUser, &total, &totalFree))
        si.freeDiskMB_C = freeUser.QuadPart / (1024ull * 1024ull);

    // GetFirmwareType is a Windows 8+ API; if linked statically the exe won't even
    // open on Windows 7. Loaded dynamically; on Win7 we fall back to a classic UEFI probe.
    {
        typedef BOOL (WINAPI* PFN_GFT)(PFIRMWARE_TYPE);
        HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
        PFN_GFT pGFT = k32 ? (PFN_GFT)GetProcAddress(k32, "GetFirmwareType")
                           : nullptr;
        if (pGFT) {
            FIRMWARE_TYPE ft = FirmwareTypeUnknown;
            if (pGFT(&ft)) si.uefi = (ft == FirmwareTypeUefi);
        } else {
            // Win7: dummy NVRAM query — returns ERROR_INVALID_FUNCTION on BIOS,
            // and a different error (access/privilege) on UEFI.
            GetFirmwareEnvironmentVariableW(L"",
                L"{00000000-0000-0000-0000-000000000000}", nullptr, 0);
            si.uefi = (GetLastError() != ERROR_INVALID_FUNCTION);
        }
    }

    DWORD sb = 0, cb = sizeof(sb);
    if (RegGetValueW(HKEY_LOCAL_MACHINE,
                     L"SYSTEM\\CurrentControlSet\\Control\\SecureBoot\\State",
                     L"UEFISecureBootEnabled", RRF_RT_REG_DWORD, nullptr, &sb, &cb) == ERROR_SUCCESS)
        si.secureBoot = (sb == 1);
    // If the key is missing (old/BIOS system) secureBoot stays false — the UEFI check already blocks it.

    si.windowsName = regStr(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName");

    logx::write(std::format(L"System: RAM={} MB, CPU=\"{}\" ({} cores), 64bit={}, "
                            L"C: free={} MB, UEFI={}, SecureBoot={}",
                            si.ramMB, si.cpuName, si.logicalCores, si.is64,
                            si.freeDiskMB_C, si.uefi, si.secureBoot));
    return si;
}
