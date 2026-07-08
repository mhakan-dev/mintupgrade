#pragma once
// ============================================================================
//  Config.h — Mirrors, offline fallback version, thresholds and fixed paths
//
//  NOTE: The Mint version is now auto-detected ONLINE (core/VersionCheck).
//        The version here is only the FALLBACK used when the network is
//        unavailable; you don't need to update this file for each new release.
// ============================================================================
#include <string>
#include <format>
#include <cstdint>

namespace cfg {

// Offline fallback (used if online discovery fails)
inline const wchar_t* FALLBACK_VERSION  = L"22.3";
inline const wchar_t* FALLBACK_CODENAME = L"Zena";

// ISO mirrors — tried IN ORDER
//  Each must end with ".../stable/" (version folders live beneath it).
//  To add a mirror, just add one line here.
inline const wchar_t* MIRRORS[] = {
    L"https://pub.linuxmint.io/stable/",                    // Official repo
    L"https://mirrors.edge.kernel.org/linuxmint/stable/",   // kernel.org (global CDN)
};
inline constexpr int MIRROR_COUNT = (int)(sizeof(MIRRORS) / sizeof(MIRRORS[0]));

inline const wchar_t* SHA_FILE = L"sha256sum.txt";

// ISO filename pattern: linuxmint-<version>-<edition-id>-64bit.iso
inline std::wstring isoFileName(const std::wstring& version, const wchar_t* editionId) {
    return std::format(L"linuxmint-{}-{}-64bit.iso", version, editionId);
}

// Editions (desktop environments)
struct Edition {
    const wchar_t* id;          // "cinnamon" / "mate" / "xfce" (used in the URL)
    const wchar_t* displayName; // Name shown to the user
    uint64_t       approxMB;    // Approx. ISO size (progress-bar seed value)
    uint32_t       minRamMB;    // Min RAM this edition runs comfortably on
    const wchar_t* blurb;       // Recommendation rationale
};

inline const Edition EDITIONS[] = {
    { L"cinnamon", L"Cinnamon", 2949, 4096,
      L"Modern ve tam donanımlı masaüstü. Donanımınız bunun için fazlasıyla yeterli." },
    { L"mate",     L"MATE",     2989, 2048,
      L"Klasik ve dengeli masaüstü. Orta seviye donanımlar için ideal." },
    { L"xfce",     L"Xfce",     2894, 1024,
      L"En hafif sürüm. Düşük RAM'li sistemlerde bile akıcı çalışır." },
};

// Hardware thresholds
inline constexpr uint64_t MIN_RAM_MB        = 1800;      // RECOMMENDED floor (not a blocker!)
inline constexpr uint64_t RAM_FOR_CINNAMON  = 3800;      // ~4 GB
inline constexpr uint64_t RAM_FOR_MATE      = 2400;      // ~2.5 GB
inline constexpr uint64_t MIN_FREE_DISK_MB  = 15 * 1024; // ISO + working headroom (blocker)
inline constexpr uint64_t REC_FREE_DISK_MB  = 25 * 1024;

// Local paths
inline const wchar_t* INSTALL_DIR      = L"C:\\MintInstall";     // ISO + report + log
inline const wchar_t* INSTALL_DIR_GRUB = L"/MintInstall";        // Same path, GRUB notation
inline const wchar_t* ESP_SUBDIR       = L"EFI\\MintAssistant";  // Our folder on the ESP
inline const wchar_t* ESP_MARKER       = L".disk\\mintassistant";
inline const wchar_t* BOOT_ENTRY_DESC  = L"Linux Mint Kurulumu"; // BCD/NVRAM entry name

} // namespace cfg
