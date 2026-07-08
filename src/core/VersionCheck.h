#pragma once
// ============================================================================
//  VersionCheck.h — Finds the newest Linux Mint version online from the
//  mirror's stable/ directory listing. Falls back to Config if the network is down.
// ============================================================================
#include <string>

struct MintRelease {
    std::wstring version;      // e.g. "22.3"
    std::wstring base;         // the mirror that responded (".../stable/")
    bool discovered = false;   // false -> offline fallback was used
};

MintRelease discoverLatestMint();
