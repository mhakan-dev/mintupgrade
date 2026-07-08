#pragma once
// ============================================================================
//  EspSetup.h — EFI System Partition (ESP) preparation:
//   1) Mounts the ESP to a temporary letter via mountvol /S
//   2) Copies grubx64.efi + bootx64.efi + GRUB modules from the ISO to the ESP
//   3) Writes grub.cfg for loopback ISO boot (the Grub2Win method)
// ============================================================================
#include <string>
#include <functional>

struct EspResult {
    std::wstring grubEfiPathBcd;   // BCD'ye verilecek yol: \EFI\MintAssistant\grubx64.efi
    std::wstring kernelPath;       // Detected inside the ISO: /casper/vmlinuz
    std::wstring initrdPath;       // Detected inside the ISO: /casper/initrd.lz (or initrd)
};

// isoDriveRoot: mounted ISO root, e.g. "E:\\"
// isoGrubPath : the ISO's path in GRUB notation, e.g. "/MintInstall/linuxmint-22.3-cinnamon-64bit.iso"
// progress    : in-phase progress from 0 to 100
bool setupEsp(const std::wstring& isoDriveRoot,
              const std::wstring& isoGrubPath,
              const std::function<void(int)>& progress,
              EspResult& out,
              std::wstring& err);
