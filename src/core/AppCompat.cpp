#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include "AppCompat.h"
#include "Util.h"
#include "Log.h"
#include "../Config.h"
#include "../Context.h"
#include "../Strings.h"
#include <algorithm>
#include <set>
#include <format>

// 
// Compatibility database — lowercase substring matching.
// Order matters: more specific patterns first ("visual studio code" < "visual studio").
// 
struct DbEntry { const wchar_t* match; Compat st; const wchar_t* note; };

static const DbEntry DB[] = {
    // Priority special matches (must be tried BEFORE the generic keys)
    { L"github desktop",      Compat::Alternative, L"GitHub (web) / GitKraken / gitg" },
    { L"gitkraken",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"sourcetree",          Compat::Alternative, L"GitKraken / gitg" },

    // Runs natively
    { L"steam",               Compat::Native, L"Yerel Linux istemcisi var; binlerce oyun Proton ile çalışır" },
    { L"discord",             Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"spotify",             Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"mozilla firefox",     Compat::Native, L"Mint ile birlikte gelir" },
    { L"google chrome",       Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"microsoft edge",      Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"brave",               Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"opera",               Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"vlc",                 Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"obs studio",          Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"telegram",            Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"zoom",                Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"visual studio code",  Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"gimp",                Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"blender",             Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"libreoffice",         Compat::Native, L"Mint ile birlikte gelir" },
    { L"inkscape",            Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"audacity",            Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"krita",               Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"thunderbird",         Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"qbittorrent",         Compat::Native, L"Yerel Linux sürümü mevcut" },
    { L"python",              Compat::Native, L"Linux'ta standarttır" },
    { L"git",                 Compat::Native, L"Linux'ta standarttır" },
    { L"heroic",              Compat::Native, L"Yerel Linux sürümü mevcut" },

    // Needs an alternative
    { L"adobe photoshop",     Compat::Alternative, L"GIMP" },
    { L"adobe illustrator",   Compat::Alternative, L"Inkscape" },
    { L"adobe premiere",      Compat::Alternative, L"Kdenlive / DaVinci Resolve" },
    { L"adobe after effects", Compat::Alternative, L"Natron / Blender" },
    { L"adobe lightroom",     Compat::Alternative, L"Darktable" },
    { L"adobe acrobat",       Compat::Alternative, L"Belge Görüntüleyici (Xreader) / Okular" },
    { L"microsoft 365",       Compat::Alternative, L"LibreOffice / OnlyOffice" },
    { L"microsoft office",    Compat::Alternative, L"LibreOffice / OnlyOffice" },
    { L"office 16",           Compat::Alternative, L"LibreOffice / OnlyOffice" },
    { L"outlook",             Compat::Alternative, L"Thunderbird" },
    { L"onedrive",            Compat::Alternative, L"rclone / Insync" },
    { L"itunes",              Compat::Alternative, L"Rhythmbox" },
    { L"paint.net",           Compat::Alternative, L"Pinta" },
    { L"winrar",              Compat::Alternative, L"Arşiv Yöneticisi / PeaZip" },
    { L"7-zip",               Compat::Alternative, L"Arşiv Yöneticisi (yerleşik)" },
    { L"coreldraw",           Compat::Alternative, L"Inkscape / Krita" },
    { L"autocad",             Compat::Alternative, L"FreeCAD / QCAD" },
    { L"solidworks",          Compat::Alternative, L"FreeCAD / Onshape (web)" },
    { L"sql server",          Compat::Alternative, L"PostgreSQL / MariaDB" },
    { L"visual studio",       Compat::Alternative, L"VS Code / Qt Creator / CLion" },
    { L"notepad++",           Compat::Alternative, L"Xed (yerleşik) / Kate — Wine ile NP++ da çalışır" },
    { L"epic games launcher", Compat::Alternative, L"Heroic Games Launcher" },
    { L"whatsapp",            Compat::Alternative, L"WhatsApp Web / WasIstLos" },
    { L"internet explorer",   Compat::Alternative, L"Firefox" },

    { L"microsoft teams",     Compat::Alternative, L"Teams (web/PWA) / yerel istemci sınırlı" },
    { L"skype",               Compat::Alternative, L"Skype (web) / Element / Jitsi" },
    { L"foobar2000",          Compat::Alternative, L"DeaDBeeF / Strawberry" },
    { L"winamp",              Compat::Alternative, L"Audacious / Qmmp" },
    { L"media player classic",Compat::Alternative, L"mpv / Celluloid / VLC" },
    { L"snagit",              Compat::Alternative, L"Flameshot / Shutter" },
    { L"camtasia",            Compat::Alternative, L"Kdenlive / OpenShot" },
    { L"sketchup",            Compat::Alternative, L"FreeCAD / Blender" },
    { L"fusion 360",          Compat::Alternative, L"FreeCAD / Onshape (web)" },
    { L"microsoft visio",     Compat::Alternative, L"draw.io / LibreOffice Draw" },
    { L"microsoft project",   Compat::Alternative, L"ProjectLibre / GanttProject" },
    { L"microsoft access",    Compat::Alternative, L"LibreOffice Base / DBeaver" },
    { L"microsoft publisher", Compat::Alternative, L"Scribus / LibreOffice Draw" },
    { L"quicktime",           Compat::Alternative, L"mpv / VLC" },
    { L"nero",                Compat::Alternative, L"Brasero / K3b / Xfburn" },
    { L"imgburn",             Compat::Alternative, L"Brasero / K3b" },
    { L"cyberduck",           Compat::Alternative, L"FileZilla / Nautilus" },
    { L"winscp",              Compat::Alternative, L"FileZilla / Nautilus (SFTP)" },
    { L"beyond compare",      Compat::Alternative, L"Meld / Kompare" },
    { L"treesize",            Compat::Alternative, L"Baobab (Disk Kullanımı) / QDirStat" },
    { L"ccleaner",            Compat::Alternative, L"BleachBit" },
    { L"malwarebytes",        Compat::Alternative, L"Linux'ta gerekmez (ClamAV opsiyonel)" },
    { L"logitech g hub",      Compat::Alternative, L"Piper / Solaar" },
    { L"logitech options",    Compat::Alternative, L"Solaar / Piper" },
    { L"razer synapse",       Compat::Alternative, L"OpenRazer / Polychromatic" },
    { L"msi afterburner",     Compat::Alternative, L"GreenWithEnvy / CoreCtrl" },
    { L"rufus",               Compat::Alternative, L"Popsicle / balenaEtcher / Disk Yazıcı" },
    { L"balenaetcher",        Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"cpu-z",               Compat::Alternative, L"CPU-X / hardinfo / inxi" },
    { L"gpu-z",               Compat::Alternative, L"GPU-Viewer / nvidia-smi" },
    { L"hwmonitor",           Compat::Alternative, L"Psensor / lm-sensors" },
    { L"crystaldiskinfo",     Compat::Alternative, L"GSmartControl / smartctl" },
    { L"putty",               Compat::Alternative, L"Yerleşik terminal (ssh) / GNOME Terminal" },
    { L"filezilla",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"docker desktop",      Compat::Native,      L"Docker Engine Linux'ta yereldir (daha hızlı)" },
    { L"virtualbox",          Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"vmware workstation",  Compat::Native,      L"VMware Workstation / VirtualBox / GNOME Boxes" },
    { L"android studio",      Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"intellij",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"pycharm",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"webstorm",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"rider",               Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"clion",               Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"datagrip",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"sublime text",        Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"atom",                Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"eclipse",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"netbeans",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"node",                Compat::Native,      L"Linux'ta standarttır" },
    { L"docker",              Compat::Native,      L"Linux'ta yereldir" },
    { L"postman",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"insomnia",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"dbeaver",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"mysql workbench",     Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"wireshark",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"slack",               Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"signal",              Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"element",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"thunderbird",         Compat::Native,      L"Mint ile birlikte gelir" },
    { L"handbrake",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"davinci resolve",     Compat::Native,      L"Yerel Linux sürümü mevcut (ücretsiz)" },
    { L"kdenlive",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"shotcut",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"darktable",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"scribus",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"calibre",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"anydesk",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"teamviewer",          Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"epic games",          Compat::Alternative, L"Heroic Games Launcher / Lutris" },
    { L"gog galaxy",          Compat::Alternative, L"Heroic Games Launcher / Lutris" },
    { L"ubisoft connect",     Compat::Caution,     L"Lutris/Proton ile kurulabilir; oyun bazında değişir" },
    { L"ea app",              Compat::Caution,     L"Lutris/Proton ile kısmi; anti-cheat'li oyunlar sorunlu" },
    { L"battle.net",          Compat::Caution,     L"Lutris ile çalışır; bazı oyunlar Proton gerektirir" },
    { L"roblox",              Compat::Caution,     L"Anti-cheat nedeniyle Linux'ta ÇALIŞMAZ" },
    { L"obs",                 Compat::Native,      L"Yerel Linux sürümü mevcut" },

    { L"minecraft",           Compat::Native,      L"Resmî Linux başlatıcısı mevcut" },
    { L"prism launcher",      Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"curseforge",          Compat::Alternative, L"Prism Launcher / Modrinth App" },
    { L"retroarch",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"dolphin",             Compat::Native,      L"Yerel Linux sürümü mevcut (emülatör)" },
    { L"kodi",                Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"plex",                Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"jellyfin",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"stremio",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"mpv",                 Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"aimp",                Compat::Alternative, L"Strawberry / Audacious" },
    { L"musicbee",            Compat::Alternative, L"Strawberry / Quod Libet" },
    { L"obsidian",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"joplin",              Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"notion",              Compat::Alternative, L"Notion (web) / AppFlowy" },
    { L"evernote",            Compat::Alternative, L"Joplin / Obsidian" },
    { L"onenote",             Compat::Alternative, L"Joplin / Obsidian / P3X OneNote" },
    { L"grammarly",           Compat::Alternative, L"LanguageTool" },
    { L"foxit",               Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"keepassxc",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"keepass",             Compat::Alternative, L"KeePassXC" },
    { L"bitwarden",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"lastpass",            Compat::Alternative, L"Bitwarden / KeePassXC" },
    { L"veracrypt",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"syncthing",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"dropbox",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"megasync",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"nextcloud",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"google drive",        Compat::Alternative, L"rclone / Insync / Celeste" },
    { L"icloud",              Compat::Alternative, L"iCloud (web) / rclone" },
    { L"sharex",              Compat::Alternative, L"Flameshot / Ksnip" },
    { L"greenshot",           Compat::Alternative, L"Flameshot" },
    { L"everything",          Compat::Alternative, L"FSearch / Catfish" },
    { L"powertoys",           Compat::Alternative, L"Ulauncher / Cinnamon eklentileri" },
    { L"autohotkey",          Compat::Alternative, L"AutoKey / espanso" },
    { L"f.lux",               Compat::Alternative, L"Gece Işığı (yerleşik) / Redshift" },
    { L"rainmeter",           Compat::Alternative, L"Conky" },
    { L"speccy",              Compat::Alternative, L"hardinfo / inxi" },
    { L"geforce experience",  Compat::Alternative, L"NVIDIA Ayarları / GreenWithEnvy" },
    { L"vivaldi",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"chromium",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"tor browser",         Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"yandex",              Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"viber",               Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"teamspeak",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"mumble",              Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"webex",               Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"reaper",              Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"lmms",                Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"fl studio",           Compat::Caution,     L"Wine/Bottles ile çalışabilir; alternatif: LMMS / Reaper" },
    { L"ableton",             Compat::Alternative, L"Bitwig Studio / Reaper" },
    { L"cubase",              Compat::Alternative, L"Reaper / Ardour" },
    { L"vegas pro",           Compat::Alternative, L"Kdenlive / DaVinci Resolve" },
    { L"bandicam",            Compat::Alternative, L"OBS Studio" },
    { L"fraps",               Compat::Alternative, L"OBS Studio / MangoHud" },
    { L"xsplit",              Compat::Alternative, L"OBS Studio" },
    { L"streamlabs",          Compat::Alternative, L"OBS Studio" },
    { L"figma",               Compat::Alternative, L"Figma (web) / Penpot" },
    { L"canva",               Compat::Alternative, L"Canva (web)" },
    { L"cura",                Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"prusaslicer",         Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"anki",                Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"godot",               Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"unity hub",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"unreal engine",       Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"arduino",             Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"ollama",              Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"lm studio",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"rustdesk",            Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"parsec",              Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"moonlight",           Compat::Native,      L"Yerel Linux sürümü mevcut" },
    { L"nordvpn",             Compat::Native,      L"Yerel Linux istemcisi mevcut" },
    { L"expressvpn",          Compat::Native,      L"Yerel Linux istemcisi mevcut" },
    { L"protonvpn",           Compat::Native,      L"Yerel Linux istemcisi mevcut" },
    { L"openvpn",             Compat::Native,      L"Linux'ta standarttır" },
    { L"wireguard",           Compat::Native,      L"Linux'ta standarttır" },
    { L"tailscale",           Compat::Native,      L"Yerel Linux istemcisi mevcut" },
    { L"rockstar games",      Compat::Caution,     L"Lutris/Proton ile kısmi; GTA Online anti-cheat sorunlu" },
    { L"xbox",                Compat::Caution,     L"Xbox uygulaması yok; Cloud Gaming tarayıcıdan çalışır" },

    // Needs caution (anti-cheat / partial)
    { L"riot vanguard",       Compat::Caution, L"Vanguard anti-cheat Linux'ta ÇALIŞMAZ (Valorant oynanamaz)" },
    { L"valorant",            Compat::Caution, L"Vanguard anti-cheat nedeniyle Linux'ta çalışmaz" },
    { L"league of legends",   Compat::Caution, L"Vanguard sonrası Linux'ta çalışmaz" },
    { L"fortnite",            Compat::Caution, L"Easy Anti-Cheat Linux'a kapalı; çalışmaz" },
    { L"easyanticheat",       Compat::Caution, L"Oyun bazında değişir; bazıları Proton'da çalışır" },
    { L"battleye",            Compat::Caution, L"Oyun bazında; bazıları Proton'da etkin, bazıları kapalı" },
    { L"pubg",                Compat::Caution, L"BattlEye nedeniyle çoğunlukla çalışmaz" },
    { L"apex legends",        Compat::Caution, L"EAC değişkenliği; genelde Linux'ta çalışmaz" },
    { L"call of duty",        Compat::Caution, L"Anti-cheat nedeniyle çoğu sürüm Linux'ta çalışmaz" },
    { L"destiny 2",           Compat::Caution, L"Anti-cheat Linux'a kapalı; çalışmaz" },
    { L"rainbow six",         Compat::Caution, L"BattlEye/anti-cheat; genelde çalışmaz" },
    { L"genshin impact",      Compat::Caution, L"Anti-cheat nedeniyle Linux'ta risklidir" },
    { L"wallpaper engine",    Compat::Caution, L"Resmî Linux desteği sınırlı; Linux Hyprland/KDE alternatifleri var" },
    { L"fivem",               Compat::Caution, L"GTA V mod istemcisi; Proton ile kısmi" },
    { L"itunes",              Compat::Caution, L"" }, // the Alternative above matches first
};

// Filter out Windows-component / update noise
static bool isNoise(const std::wstring& low) {
    static const wchar_t* skip[] = {
        L"microsoft visual c++", L"microsoft .net", L"windows sdk", L"windows software development",
        L"update for", L"security update", L"hotfix", L"redistributable", L"vs_", L"vcpp",
        L"microsoft edge webview", L"windows app runtime", L"winrt",
    };
    for (auto* s : skip)
        if (low.find(s) != std::wstring::npos) return true;
    return false;
}

static void scanRoot(HKEY root, const wchar_t* sub, REGSAM wow,
                     std::set<std::wstring>& seen, std::vector<AppItem>& out) {
    HKEY h;
    if (RegOpenKeyExW(root, sub, 0, KEY_READ | wow, &h) != ERROR_SUCCESS) return;

    for (DWORD i = 0;; ++i) {
        wchar_t name[256];
        DWORD nlen = 256;
        if (RegEnumKeyExW(h, i, name, &nlen, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
            break;

        HKEY k;
        if (RegOpenKeyExW(h, name, 0, KEY_READ | wow, &k) != ERROR_SUCCESS) continue;

        wchar_t disp[512]; DWORD sz = sizeof(disp);
        DWORD sysComp = 0;  DWORD scSz = sizeof(sysComp);
        wchar_t parent[64] = L""; DWORD pSz = sizeof(parent);
        RegGetValueW(k, nullptr, L"SystemComponent", RRF_RT_REG_DWORD, nullptr, &sysComp, &scSz);
        RegGetValueW(k, nullptr, L"ParentKeyName",   RRF_RT_REG_SZ,   nullptr, parent, &pSz);
        bool hasName = RegGetValueW(k, nullptr, L"DisplayName", RRF_RT_REG_SZ,
                                    nullptr, disp, &sz) == ERROR_SUCCESS;
        RegCloseKey(k);

        if (!hasName || sysComp == 1 || parent[0]) continue;
        std::wstring dn = util::trim(disp);
        if (dn.empty()) continue;
        std::wstring low = util::toLower(dn);
        if (isNoise(low)) continue;
        if (!seen.insert(low).second) continue;        // yinelenenleri at

        AppItem it{ dn, Compat::Unknown, L"" };
        for (const auto& d : DB) {
            if (low.find(d.match) != std::wstring::npos) {
                it.status = d.st;
                it.linuxNote = d.note;
                break;
            }
        }
        out.push_back(std::move(it));
    }
    RegCloseKey(h);
}

static const wchar_t* stLabel(Compat c) {
    switch (c) {
        case Compat::Native:      return str::CST_NATIVE;
        case Compat::Alternative: return str::CST_ALT;
        case Compat::Caution:     return str::CST_CAUTION;
        default:                  return str::CST_UNKNOWN;
    }
}
static const wchar_t* stColor(Compat c) {
    switch (c) {
        case Compat::Native:      return L"#1d7a34";
        case Compat::Alternative: return L"#b3541e";
        case Compat::Caution:     return L"#a3262d";
        default:                  return L"#666";
    }
}

static void writeHtmlReport(const CompatResult& r) {
    std::wstring h;
    h += L"<!DOCTYPE html><html lang=\"tr\"><head><meta charset=\"utf-8\">"
         L"<title>Linux Mint</title><style>"
         L"body{font-family:'Segoe UI',sans-serif;background:#0B4852;color:#fff;margin:32px}"
         L"h1{font-weight:300}table{border-collapse:collapse;background:#fff;color:#1a1a1a;width:100%}"
         L"th,td{padding:8px 14px;border-bottom:1px solid #ddd;text-align:left;font-size:14px}"
         L"th{background:#eee}.s{font-weight:600}</style></head><body>";
    wchar_t titleBuf[128];
    swprintf_s(titleBuf, str::RPT_TITLE_FMT, g_ctx.mintVersion.c_str());
    h += std::format(L"<h1>{}</h1>", titleBuf);
    wchar_t sumBuf[256];
    swprintf_s(sumBuf, str::RPT_SUMMARY_FMT, r.nativeCount, r.reviewCount, r.unknownCount);
    h += std::format(L"<p>{}</p>", sumBuf);
    h += std::format(L"<table><tr><th>{}</th><th>{}</th><th>{}</th></tr>",
                     (const wchar_t*)str::RPT_COL_APP,
                     (const wchar_t*)str::RPT_COL_STATUS,
                     (const wchar_t*)str::RPT_COL_ONMINT);
    for (const auto& it : r.items) {
        std::wstring note = it.linuxNote.empty() ? L"—" : str::trNote(it.linuxNote);
        h += std::format(L"<tr><td>{}</td><td class=\"s\" style=\"color:{}\">{}</td><td>{}</td></tr>",
                         it.name, stColor(it.status), stLabel(it.status), note);
    }
    h += L"</table></body></html>";
    util::writeTextFileUtf8(std::wstring(cfg::INSTALL_DIR) + L"\\uyumluluk_raporu.html", h, false);
}

CompatResult scanInstalledApps() {
    CompatResult r;
    std::set<std::wstring> seen;
    const wchar_t* SUB = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";

    scanRoot(HKEY_LOCAL_MACHINE, SUB, KEY_WOW64_64KEY, seen, r.items);
    scanRoot(HKEY_LOCAL_MACHINE, SUB, KEY_WOW64_32KEY, seen, r.items);
    scanRoot(HKEY_CURRENT_USER,  SUB, 0,               seen, r.items);

    for (const auto& it : r.items) {
        switch (it.status) {
            case Compat::Native:      r.nativeCount++;  break;
            case Compat::Alternative:
            case Compat::Caution:     r.reviewCount++;  break;
            default:                  r.unknownCount++; break;
        }
    }
    // Problematic first, then compatible, unknowns last
    std::stable_sort(r.items.begin(), r.items.end(), [](const AppItem& a, const AppItem& b) {
        auto rank = [](Compat c) {
            switch (c) {
                case Compat::Alternative: return 0;
                case Compat::Caution:     return 1;
                case Compat::Native:      return 2;
                default:                  return 3;
            }
        };
        return rank(a.status) < rank(b.status);
    });

    r.reportPath = std::wstring(cfg::INSTALL_DIR) + L"\\uyumluluk_raporu.html";
    writeHtmlReport(r);
    logx::write(std::format(L"Compatibility: {} compatible, {} to review, {} unknown — report: {}",
                            r.nativeCount, r.reviewCount, r.unknownCount, r.reportPath));
    return r;
}
