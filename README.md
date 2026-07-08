# MintUpgradeAssistant

A native C++ Win32 application that helps users transition from Windows to Linux Mint without requiring a USB drive.

The application performs hardware and software compatibility analysis, downloads the latest Linux Mint ISO, and configures the system to boot directly into the Linux Mint installer through UEFI.

---

## Features

- Hardware compatibility analysis
  - CPU
  - RAM
  - Available disk space
  - UEFI detection
  - Secure Boot detection

- Automatic Linux Mint edition recommendation
  - Cinnamon
  - MATE
  - Xfce

- Windows application compatibility scan
  - Detects installed applications
  - Identifies software that works on Linux
  - Suggests alternatives when needed
  - Generates an HTML compatibility report

- Automatic ISO management
  - Downloads the latest Linux Mint release
  - Multiple mirror support
  - Resume interrupted downloads
  - SHA-256 integrity verification

- USB-free installation
  - Creates the required EFI boot entry
  - Boots directly into the Linux Mint installer
  - No bootable USB drive required

- Automatic rollback
  - Restores the original Windows boot configuration
  - Removes temporary boot entries

- Portable single executable
  - No additional runtime installation required

- Multi-language support
  - 14 interface languages

---

## Requirements

- Windows 8.1 or newer
- Windows 10/11 recommended
- UEFI firmware
- Secure Boot disabled
- Administrator privileges

---

## Technologies

- C++20
- Win32 API
- CMake
- Windows Boot Configuration (BCD)
- WinHTTP
- BCrypt (SHA-256)

---

## Build

```bash
cmake -B build -A x64
cmake --build build --config Release
```

Output:

```text
build/Release/MintAssistant.exe
```

---

## How it Works

1. Analyze the system hardware.
2. Scan installed Windows applications.
3. Recommend the most suitable Linux Mint edition.
4. Download the latest official Linux Mint ISO.
5. Verify the ISO integrity.
6. Configure a temporary UEFI boot entry.
7. Reboot directly into the Linux Mint installer.
8. Restore the original Windows boot configuration when requested.

---

## Project Goals

- Make migrating from Windows to Linux Mint easier.
- Eliminate the need for creating bootable USB drives.
- Help users understand software compatibility before switching.
- Provide a simple and familiar installation experience.

---

## Screenshots

<img width="979" height="650" alt="image" src="https://github.com/user-attachments/assets/b5e956ef-4446-4119-b176-948d9e898f66" />
<img width="978" height="647" alt="image" src="https://github.com/user-attachments/assets/f77259be-f824-4f3a-b413-f0ff036446e2" />

---

## License

This project is licensed under the GNU General Public License v3.0 (GPLv3).

If you use this project or parts of its source code, please provide attribution to the original author.
