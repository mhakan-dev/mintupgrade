# ============================================================================
#  MintUpgradeAssistant — CMake derleme tanımı
#  Visual Studio 2022: "Open a local folder" ile bu klasörü açmanız yeterli.
# ============================================================================
cmake_minimum_required(VERSION 3.20)
project(MintUpgradeAssistant LANGUAGES CXX)

# CRT statik bağlanır (/MT, /MTd): başka bilgisayarda vcruntime140*.dll,
# msvcp140*.dll gibi VC++ Runtime DLL'leri GEREKMEZ — exe tek başına çalışır.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(WIN32)
    enable_language(RC)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MintAssistant WIN32
    src/main.cpp
    src/Wizard.cpp
    src/ui/Ui.cpp
    src/ui/Image.cpp
    src/core/Log.cpp
    src/core/Util.cpp
    src/core/SystemInfo.cpp
    src/core/EditionSelector.cpp
    src/core/AppCompat.cpp
    src/core/Sha256.cpp
    src/core/Downloader.cpp
    src/core/IsoMount.cpp
    src/core/EspSetup.cpp
    src/core/BcdBoot.cpp
    src/core/Installer.cpp
    src/core/VersionCheck.cpp
    resources/app.rc
)

target_compile_definitions(MintAssistant PRIVATE
    UNICODE _UNICODE _WIN32_WINNT=0x0A00 NOMINMAX)

if(MSVC)
    # Kaynaklar UTF-8 (Türkçe metinler için /utf-8 şart)
    target_compile_options(MintAssistant PRIVATE /utf-8 /W3 /EHsc)
    # bcdedit + mountvol + ESP erişimi için uygulama YÖNETİCİ olarak başlamalı
    set_target_properties(MintAssistant PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
endif()

target_link_libraries(MintAssistant PRIVATE
    user32 gdi32 gdiplus shlwapi shell32 advapi32 winhttp virtdisk bcrypt)

# NOT: Logo (logo.png) ve simge (mint.ico) app.rc üzerinden EXE İÇİNE gömülür.
# Exe tek başına taşınabilir; yanında resources klasörü GEREKMEZ.
