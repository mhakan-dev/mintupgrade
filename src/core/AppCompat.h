#pragma once
// ============================================================================
//  AppCompat.h — Scans installed apps, assesses Linux Mint compatibility
//  and produces C:\MintInstall\uyumluluk_raporu.html.
// ============================================================================
#include <string>
#include <vector>

enum class Compat { Native, Alternative, Caution, Unknown };
// Native      : Runs directly on Linux (Steam, Discord, Firefox…)
// Alternative : Doesn't run; a suggested alternative exists (Photoshop -> GIMP…)
// Caution     : Runs partially/conditionally (anti-cheat games, Wine-dependent ones)
// Unknown     : Not in the database — listed as "not evaluated" in the report

struct AppItem {
    std::wstring name;        // DisplayName from the registry
    Compat       status;
    std::wstring linuxNote;   // Alternative name or note
};

struct CompatResult {
    std::vector<AppItem> items;        // All items (sorted by status)
    int nativeCount   = 0;
    int reviewCount   = 0;             // Alternative + Caution
    int unknownCount  = 0;
    std::wstring reportPath;           // Path to the generated HTML report
};

CompatResult scanInstalledApps();      // Scans the registry Uninstall keys
