#pragma once
// ============================================================================
//  Theme.h — Windows 8 Upgrade Assistant look (flat dark teal background,
//  white Segoe UI Light headings, light-gray buttons, green progress bar)
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <initializer_list>

namespace theme {

// Renkler
inline const COLORREF BG          = RGB(0x0B, 0x48, 0x52);  // Zemin (koyu turkuaz)
inline const COLORREF BG_PANEL    = RGB(0xFF, 0xFF, 0xFF);  // Beyaz bilgi paneli
inline const COLORREF TXT         = RGB(0xFF, 0xFF, 0xFF);  // Ana metin
inline const COLORREF TXT_DIM     = RGB(0xA8, 0xC9, 0xCE);  // Secondary text
inline const COLORREF TXT_PANEL   = RGB(0x1A, 0x1A, 0x1A);  // Text inside panels
inline const COLORREF LINK        = RGB(0x6F, 0xD6, 0xE8);  // Link
inline const COLORREF OK_GREEN    = RGB(0x7A, 0xE0, 0x8C);  // ✓
inline const COLORREF BAD_RED     = RGB(0xFF, 0x7A, 0x6E);  // ✗
inline const COLORREF WARN_YEL    = RGB(0xFF, 0xD5, 0x66);  // ⚠
inline const COLORREF MINT_GREEN  = RGB(0x87, 0xCF, 0x3E);  // "Linux Mint" text logo

inline const COLORREF BTN_BG      = RGB(0xE6, 0xE6, 0xE6);
inline const COLORREF BTN_BG_HOT  = RGB(0xFF, 0xFF, 0xFF);
inline const COLORREF BTN_BG_DIS  = RGB(0x2E, 0x62, 0x6B);
inline const COLORREF BTN_TXT     = RGB(0x10, 0x10, 0x10);
inline const COLORREF BTN_TXT_DIS = RGB(0x7F, 0xA3, 0xA9);
inline const COLORREF BTN_BORDER  = RGB(0xFF, 0xFF, 0xFF);

inline const COLORREF BAR_TRACK   = RGB(0xD6, 0xD6, 0xD6);  // Klasik ilerleme zemini
inline const COLORREF BAR_FILL    = RGB(0x21, 0xB3, 0x3C);  // Green fill
inline const COLORREF BAR_BORDER  = RGB(0xFF, 0xFF, 0xFF);
inline const COLORREF BAR_THIN_BG = RGB(0x1F, 0x5C, 0x66);  // "Preparing" thin-line track

// Window dimensions
inline const int CLIENT_W = 980;
inline const int CLIENT_H = 620;
inline const int MARGIN_X = 60;

// Splash window dimensions (small frameless window)
inline const int SPLASH_W = 560;
inline const int SPLASH_H = 280;

// Fonts (created in WM_CREATE)
struct Fonts {
    HFONT title  = nullptr;   // Segoe UI Light 40 — page titles
    HFONT huge   = nullptr;   // Segoe UI Light 48 — "Preparing 96%"
    HFONT logo   = nullptr;   // Segoe UI Light 24 — "Linux Mint" text logo
    HFONT mid    = nullptr;   // Segoe UI Light 28 — splash "Preparing" text
    HFONT nav    = nullptr;   // Segoe UI Semibold 15
    HFONT body   = nullptr;   // Segoe UI 16
    HFONT small_ = nullptr;   // Segoe UI 14
    HFONT btn    = nullptr;   // Segoe UI 15
    HFONT sym    = nullptr;   // Segoe UI Symbol 16 (✓ ✗ ⚠)

    void create() {
        title  = CreateFontW(-40, 0, 0, 0, FW_LIGHT,    0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Light");
        huge   = CreateFontW(-48, 0, 0, 0, FW_LIGHT,    0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Light");
        logo   = CreateFontW(-24, 0, 0, 0, FW_LIGHT,    0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Light");
        mid    = CreateFontW(-28, 0, 0, 0, FW_LIGHT,    0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Light");
        nav    = CreateFontW(-15, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        body   = CreateFontW(-16, 0, 0, 0, FW_NORMAL,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        small_ = CreateFontW(-14, 0, 0, 0, FW_NORMAL,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        btn    = CreateFontW(-15, 0, 0, 0, FW_NORMAL,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        sym    = CreateFontW(-16, 0, 0, 0, FW_NORMAL,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Symbol");
    }
    void destroy() {
        for (HFONT f : { title, huge, logo, mid, nav, body, small_, btn, sym })
            if (f) DeleteObject(f);
    }
};

inline Fonts fonts;

} // namespace theme
