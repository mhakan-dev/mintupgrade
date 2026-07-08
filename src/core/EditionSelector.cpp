#include "EditionSelector.h"
#include "Log.h"
#include <format>

int recommendedEditionIndex(const SystemInfo& si) {
    if (si.ramMB >= cfg::RAM_FOR_CINNAMON && si.logicalCores >= 2)
        return 0;                                       // Cinnamon
    if (si.ramMB >= cfg::RAM_FOR_MATE)
        return 1;                                       // MATE
    return 2;                                           // Xfce
}

const cfg::Edition* pickEdition(const SystemInfo& si) {
    int idx = recommendedEditionIndex(si);
    const cfg::Edition* e = &cfg::EDITIONS[idx];
    logx::write(std::format(L"Recommendation: {} (RAM={} MB, {} cores)",
                            e->displayName, si.ramMB, si.logicalCores));
    return e;
}

bool editionTooHeavy(const SystemInfo& si, int editionIndex) {
    if (editionIndex < 0 || editionIndex >= (int)(sizeof(cfg::EDITIONS)/sizeof(cfg::EDITIONS[0])))
        return false;
    // Consider it "heavy" if we're below the RAM this edition wants to run well.
    // We leave a small tolerance margin (e.g. allow 3.5 GB instead of 3.8).
    uint64_t need = cfg::EDITIONS[editionIndex].minRamMB;
    uint64_t tol  = (need > 512) ? need - 512 : need;
    return si.ramMB < tol;
}
