#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <initguid.h>      // Must come BEFORE virtdisk.h to define the VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT GUID
#include <virtdisk.h>
#include "IsoMount.h"
#include "Log.h"
#include <format>
#include <cstdlib>

#pragma comment(lib, "virtdisk.lib")

IsoMount::~IsoMount() { unmount(); }

// "\\.\CDROM3" -> 3
static int cdIdxFromPhysical(const std::wstring& s) {
    size_t p = s.rfind(L"CDROM");
    return (p == std::wstring::npos) ? -1 : _wtoi(s.c_str() + p + 5);
}
// "\Device\CdRom3" -> 3
static int cdIdxFromNt(const std::wstring& s) {
    size_t p = s.rfind(L"CdRom");
    return (p == std::wstring::npos) ? -1 : _wtoi(s.c_str() + p + 5);
}

bool IsoMount::mount(const std::wstring& isoPath, std::wstring& err) {
    VIRTUAL_STORAGE_TYPE vst{ VIRTUAL_STORAGE_TYPE_DEVICE_ISO,
                              VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT };
    HANDLE h = nullptr;
    DWORD rc = OpenVirtualDisk(&vst, isoPath.c_str(),
        (VIRTUAL_DISK_ACCESS_MASK)(VIRTUAL_DISK_ACCESS_ATTACH_RO |
                                   VIRTUAL_DISK_ACCESS_READ |
                                   VIRTUAL_DISK_ACCESS_DETACH |
                                   VIRTUAL_DISK_ACCESS_GET_INFO),
        OPEN_VIRTUAL_DISK_FLAG_NONE, nullptr, &h);
    if (rc != ERROR_SUCCESS) {
        err = L"ISO açılamadı: " + logx::lastError(rc);
        return false;
    }
    rc = AttachVirtualDisk(h, nullptr, ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY,
                           0, nullptr, nullptr);
    if (rc != ERROR_SUCCESS) {
        err = L"ISO bağlanamadı: " + logx::lastError(rc);
        CloseHandle(h);
        return false;
    }
    m_handle = h;

    // Physical device path: "\\.\CDROMn"
    wchar_t phys[MAX_PATH]{};
    ULONG psz = sizeof(phys);
    if (GetVirtualDiskPhysicalPath(h, &psz, phys) != ERROR_SUCCESS) {
        err = L"ISO aygıt yolu alınamadı";
        unmount();
        return false;
    }
    int idx = cdIdxFromPhysical(phys);

    // Wait for Windows to assign a letter (~10 s) and match the CdRom index to a letter
    for (int t = 0; t < 100 && !m_letter; ++t) {
        for (wchar_t ltr = L'D'; ltr <= L'Z'; ++ltr) {
            wchar_t dev[3] = { ltr, L':', 0 };
            wchar_t target[512];
            if (QueryDosDeviceW(dev, target, 512) && cdIdxFromNt(target) == idx) {
                m_letter = ltr;
                break;
            }
        }
        if (!m_letter) Sleep(100);
    }
    if (!m_letter) {
        err = L"Bağlanan ISO'ya sürücü harfi atanmadı";
        unmount();
        return false;
    }
    logx::write(std::format(L"ISO mounted: {} -> {}:  ({})", isoPath, m_letter, phys));
    return true;
}

void IsoMount::unmount() {
    if (m_handle) {
        DetachVirtualDisk((HANDLE)m_handle, DETACH_VIRTUAL_DISK_FLAG_NONE, 0);
        CloseHandle((HANDLE)m_handle);
        m_handle = nullptr;
        m_letter = 0;
        logx::write(L"ISO unmounted");
    }
}
