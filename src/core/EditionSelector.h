#pragma once
// ============================================================================
//  EditionSelector.h — Recommends a Mint edition based on hardware
// ============================================================================
#include "SystemInfo.h"
#include "../Config.h"

// Returns the index into EDITIONS[] of the best-fit edition for the hardware (0..2).
int recommendedEditionIndex(const SystemInfo& si);

// Backward compatibility: pointer to the recommended edition.
const cfg::Edition* pickEdition(const SystemInfo& si);

// Is the given edition "heavy" for this hardware? (RAM below the edition's minRamMB)
// true -> a small warning is shown next to it on the edition selection screen.
bool editionTooHeavy(const SystemInfo& si, int editionIndex);
