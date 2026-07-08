#pragma once
// ============================================================================
//  IsoMount.h — Mounts an ISO using Windows' built-in virtual DVD feature
//  (virtdisk.h — Windows 8 and later, no extra software needed)
// ============================================================================
#include <string>

class IsoMount {
public:
    ~IsoMount();
    bool    mount(const std::wstring& isoPath, std::wstring& err);
    void    unmount();
    wchar_t driveLetter() const { return m_letter; }   // e.g. L'E' -> "E:\"
private:
    void*   m_handle = nullptr;                        // HANDLE
    wchar_t m_letter = 0;
};
