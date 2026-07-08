#pragma once
// ============================================================================
//  BcdBoot.h — Adds a GRUB entry to the Windows Boot Manager (NVRAM/BCD)
//  Same approach as Grub2Win: copy {bootmgr} -> set path -> move to first
// ============================================================================
#include <string>

// Creates the firmware boot entry and makes it the DEFAULT.
// efiPath: "\EFI\MintAssistant\grubx64.efi"
bool addFirmwareBootEntry(const std::wstring& efiPath,
                          std::wstring& outGuid,
                          std::wstring& err);

// Writes the C:\MintInstall\geri_al.cmd script (removes the entry + ESP files)
bool writeRevertScript(const std::wstring& guid);
