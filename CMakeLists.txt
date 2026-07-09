# ============================================================================
#  MintUpgradeAssistant — CMake build definition
# Visual Studio 2022: Simply open this folder using "Open a local folder".
# ============================================================================
cmake_minimum_required(VERSION 3.20)
project(MintUpgradeAssistant LANGUAGES CXX)

# CRT is statically linked (/MT, /MTd): VC++ Runtime DLLs such as vcruntime140*.dll
# and msvcp140*.dll are NOT required on other computers — the exe runs standalone.
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
    # Sources: UTF-8 (UTF-8 is mandatory for Turkish texts)
    target_compile_options(MintAssistant PRIVATE /utf-8 /W3 /EHsc)
    set_target_properties(MintAssistant PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
endif()

target_link_libraries(MintAssistant PRIVATE
    user32 gdi32 gdiplus shlwapi shell32 advapi32 winhttp virtdisk bcrypt)
