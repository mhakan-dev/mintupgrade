// ============================================================================
//  Wizard.cpp — Page flow and painting (Windows 8 Upgrade Assistant look)
//  Flow: Check -> Compatibility scan -> Result -> What to keep -> Ready
//        -> Install ("Preparing %..") -> Restart  (+ Error page)
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <shellapi.h>
#include "Wizard.h"
#include "Context.h"
#include "Theme.h"
#include "Strings.h"
#include "ui/Ui.h"
#include "ui/Image.h"
#include "core/EditionSelector.h"
#include "core/VersionCheck.h"
#include "core/Installer.h"
#include "core/Util.h"
#include "core/Log.h"
#include <thread>
#include <format>
#include <algorithm>
#include <cstdio>

enum Page { PG_SPLASH, PG_CHECK, PG_SCAN, PG_RESULT, PG_KEEP, PG_READY,
            PG_INSTALL, PG_RESTART, PG_ERROR };

enum Ids  { ID_NEXT = 1, ID_BACK, ID_INSTALL, ID_CANCEL,
            ID_RESTART, ID_LATER, ID_CLOSE };

// Page state
static struct State {
    Page page        = PG_SPLASH;
    int  splashPct   = 0;         // splash screen percentage
    int  splashStage = 0;         // 0: centered logo, 1: top-left logo + %
    int  splashTick  = 0;
    bool busy        = true;      // check/scan in progress
    int  tick        = 0;         // spinner animasyonu
    int  hover       = -1;        // index of button under the mouse
    bool keepNothing = true;
    bool dlStarted   = false;
    int  pct         = 0;         // install percentage
    int  phase       = PH_PREP;
    bool later       = false;     // "Later" was chosen
    RECT linkRect{};              // report link
    RECT radioNothing{};          // "Nothing" click area
    int  edIndex     = -1;        // selected edition (-1: not set yet -> recommendation)
    int  edRecommend = 0;         // recommended edition index
    RECT edRect[3]{};             // edition selection click areas
    bool edExpand    = false;     // whether the selector was opened via "Choose a different edition"
    RECT edLink{};                // link click area
    RECT logLink{};               // log-file link on the error page
    std::vector<ui::Btn> btns;
} S;

// Layout constants (close to the proportions in screenshot 3)
static const int MX      = theme::MARGIN_X;
static const int NAV_Y   = 26;
static const int TITLE_Y = 62;
static const int SUB_Y   = 126;
static const int BODY_Y  = 176;
static const int BTN_W   = 124;
static const int BTN_H   = 34;
static const int BTN_Y   = theme::CLIENT_H - 76;

static void repaint(HWND w) { InvalidateRect(w, nullptr, FALSE); }

// Linux Mint logo: tries the PNG (resources/logo.png) first; falls back to text.
static void drawLogo(HDC dc, int x, int y, int height = 26) {
    if (img::draw(dc, L"logo.png", x, y, height))
        return;
    // Fallback: text logo (if the PNG isn't found)
    ui::text(dc, x, y, theme::fonts.logo, theme::MINT_GREEN, str::LOGO);
}

// Button layout (arranged right to left)
static int g_btnRight = 0;
static void pushBtn(int id, const wchar_t* label, bool enabled = true, int w = BTN_W) {
    ui::Btn b;
    b.id = id;
    b.label = label;
    b.enabled = enabled;
    b.r = { g_btnRight - w, BTN_Y, g_btnRight, BTN_Y + BTN_H };
    g_btnRight -= (w + 14);
    S.btns.push_back(std::move(b));
}

static void layout() {
    S.btns.clear();
    g_btnRight = theme::CLIENT_W - MX;
    switch (S.page) {
        case PG_CHECK:
            pushBtn(ID_NEXT, str::BTN_NEXT, !S.busy && g_ctx.sys.canProceed());
            break;
        case PG_SCAN:
            break;                                       // no button
        case PG_RESULT:
            pushBtn(ID_NEXT, str::BTN_NEXT);
            break;
        case PG_KEEP:
            pushBtn(ID_NEXT, str::BTN_NEXT);
            pushBtn(ID_BACK, str::BTN_BACK);
            break;
        case PG_READY:
            pushBtn(ID_INSTALL, str::BTN_INSTALL);
            pushBtn(ID_BACK, str::BTN_BACK);
            break;
        case PG_INSTALL:
            pushBtn(ID_CANCEL,
                    g_ctx.cancel ? str::BTN_CANCELLING : str::BTN_CANCEL,
                    !g_ctx.cancel, 160);
            break;
        case PG_RESTART:
            if (!S.later) {
                pushBtn(ID_RESTART, str::BTN_RESTART, true, 210);
                pushBtn(ID_LATER, str::BTN_LATER);
            } else {
                pushBtn(ID_CLOSE, str::BTN_CLOSE);
            }
            break;
        case PG_ERROR:
            pushBtn(ID_CLOSE, str::BTN_CLOSE);
            break;
    }
}

// Background work
static void startCheck(HWND w) {
    S.busy = true;
    std::thread([w] {
        ULONGLONG t0 = GetTickCount64();

        g_ctx.sys = collectSystemInfo();

        // Find the newest Mint version online (fallback in Config stays if no network)
        MintRelease rel = discoverLatestMint();
        g_ctx.mintVersion       = rel.version;
        g_ctx.mirrorBase        = rel.base;
        g_ctx.versionDiscovered = rel.discovered;
        g_ctx.mintCodename      = (rel.version == cfg::FALLBACK_VERSION)
                                  ? cfg::FALLBACK_CODENAME : L"";

        S.edRecommend = recommendedEditionIndex(g_ctx.sys);
        if (S.edIndex < 0) S.edIndex = S.edRecommend;    // pick the recommendation on first run
        g_ctx.edition = &cfg::EDITIONS[S.edIndex];

        ULONGLONG el = GetTickCount64() - t0;            // a brief "checking" moment
        if (el < 1400) Sleep((DWORD)(1400 - el));
        PostMessageW(w, WM_APP_CHECKDONE, 0, 0);
    }).detach();
}

static void startScan(HWND w) {
    S.busy = true;
    std::thread([w] {
        ULONGLONG t0 = GetTickCount64();
        g_ctx.compat = scanInstalledApps();
        ULONGLONG el = GetTickCount64() - t0;
        if (el < 1800) Sleep((DWORD)(1800 - el));
        PostMessageW(w, WM_APP_SCANDONE, 0, 0);
    }).detach();
}

static void gotoPage(HWND w, Page p) {
    S.page = p;
    S.hover = -1;
    switch (p) {
        case PG_CHECK:   startCheck(w); break;
        case PG_SCAN:    startScan(w);  break;
        case PG_INSTALL:
            S.pct = 0;
            S.phase = PH_PREP;
            startInstallThread(w);
            break;
        default: break;
    }
    repaint(w);
}

// Splash complete: switch the window to the normal framed wizard size.
static void endSplash(HWND w) {
    KillTimer(w, 2);
    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    SetWindowLongPtrW(w, GWL_STYLE, (LONG_PTR)style);
    RECT rc{ 0, 0, theme::CLIENT_W, theme::CLIENT_H };
    AdjustWindowRect(&rc, style, FALSE);
    int W = rc.right - rc.left, H = rc.bottom - rc.top;
    int x = (GetSystemMetrics(SM_CXSCREEN) - W) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - H) / 2;
    SetWindowPos(w, nullptr, x, y, W, H,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    gotoPage(w, PG_CHECK);                               // the wizard starts normally
}

void wiz::onCreate(HWND w) {
    SetTimer(w, 1, 90, nullptr);        // general animation timer (spinner etc.)
    SetTimer(w, 2, 45, nullptr);        // fast timer for splash percentage increase
    S.page = PG_SPLASH;
    S.splashPct = 0;
    repaint(w);
}

// ============================================================================
//  PAGE PAINTING
// ============================================================================
static void drawNav(HDC dc) {
    int active;
    switch (S.page) {
        case PG_CHECK:                              active = 0;  break;
        case PG_SCAN: case PG_RESULT: case PG_KEEP: active = 1;  break;
        case PG_READY: case PG_INSTALL:             active = 2;  break;
        case PG_RESTART:                            active = 3;  break;
        default:                                    active = -1; break;
    }
    int x = MX;
    for (int i = 0; i < 4; ++i) {
        ui::text(dc, x, NAV_Y, theme::fonts.nav,
                 (i == active) ? theme::TXT : theme::TXT_DIM, str::NAV[i]);
        x += ui::measure(dc, theme::fonts.nav, str::NAV[i]).cx + 46;
    }
}

static void drawCheck(HDC dc) {
    if (S.busy) {
        ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::CHK_TITLE_BUSY);
        ui::text(dc, MX, SUB_Y, theme::fonts.body, theme::TXT_DIM, str::CHK_SUB_BUSY);
        ui::text(dc, MX, SUB_Y + 26, theme::fonts.small_, theme::TXT_DIM, str::CHK_BUSY_NET);
        ui::spinner(dc, MX + 14, BODY_Y + 40, S.tick);
        return;
    }
    const SystemInfo& si = g_ctx.sys;
    ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::CHK_TITLE_DONE);

    // 0 = ok (✓), 1 = warning (⚠, not a blocker), 2 = blocker (✗)
    int y = BODY_Y - 28;
    auto row = [&](int state, const wchar_t* name, const std::wstring& val) {
        if (state == 1)      ui::warn(dc, MX, y);
        else                 ui::mark(dc, MX, y, state == 0);
        COLORREF c = (state == 0) ? theme::TXT
                   : (state == 1) ? theme::WARN_YEL : theme::BAD_RED;
        ui::text(dc, MX + 32, y, theme::fonts.body, theme::TXT, name);
        ui::textClip(dc, { MX + 320, y, theme::CLIENT_W - MX, y + 24 },
                     theme::fonts.body, c, val);
        y += 33;
    };
    row(si.ramOK() ? 0 : 1, str::CHK_ITEM_RAM,  util::fmtGB(si.ramMB));   // RAM: NOT a blocker
    row(si.is64 ? 0 : 2,    str::CHK_ITEM_CPU,
        std::format(L"{} ({} {})", si.cpuName, si.logicalCores,
                    str::WORD_CORES));
    row(si.diskOK() ? 0 : 2, str::CHK_ITEM_DISK, util::fmtGB(si.freeDiskMB_C));
    row(si.uefi ? 0 : 2,     str::CHK_ITEM_FW,   si.uefi ? L"UEFI" : L"Legacy/BIOS");
    row(si.secureBoot ? 2 : 0, str::CHK_ITEM_SB,
        si.secureBoot ? str::VAL_ON : str::VAL_OFF);

    for (int i = 0; i < 3; ++i) S.edRect[i] = RECT{};   // reset on each paint
    S.edLink = RECT{};

    y += 20;
    if (si.canProceed()) {
        if (!S.edExpand) {
            // --- Default view: RECOMMENDED edition selected, selector hidden --
            ui::text(dc, MX, y, theme::fonts.body, theme::TXT,
                     std::format(L"{}:  {} — {}",
                                 str::CHK_RECOMMEND, g_ctx.verName(),
                                 cfg::EDITIONS[S.edIndex].displayName));
            y += 28;
            ui::textClip(dc, { MX, y, theme::CLIENT_W - MX, y + 22 },
                         theme::fonts.small_, theme::TXT_DIM,
                         str::ED_BLURB[S.edIndex]);
            y += 28;

            // "Choose a different edition" link (underlined) — opens the selector
            SIZE ls = ui::measure(dc, theme::fonts.small_, str::CHK_LINK_OTHER);
            ui::text(dc, MX, y, theme::fonts.small_, theme::LINK, str::CHK_LINK_OTHER);
            HPEN lp = CreatePen(PS_SOLID, 1, theme::LINK);
            HPEN op = (HPEN)SelectObject(dc, lp);
            MoveToEx(dc, MX, y + ls.cy + 1, nullptr);
            LineTo(dc, MX + ls.cx, y + ls.cy + 1);
            SelectObject(dc, op);
            DeleteObject(lp);
            S.edLink = { MX, y, MX + ls.cx, y + ls.cy + 4 };
            y += 32;
        } else {
            // Selector open: three desktops (compact rows)
            for (int i = 0; i < 3; ++i) {
                const cfg::Edition& e = cfg::EDITIONS[i];
                bool heavy = editionTooHeavy(si, i);

                ui::radio(dc, MX, y + 1, i == S.edIndex, true);
                ui::text(dc, MX + 32, y, theme::fonts.body, theme::TXT, e.displayName);
                int lx = MX + 32 +
                         ui::measure(dc, theme::fonts.body, e.displayName).cx + 12;

                if (i == S.edRecommend) {                // "recommended" badge
                    std::wstring tag = std::format(L"({})", str::CHK_ED_RECOMMEND);
                    ui::text(dc, lx, y + 2, theme::fonts.small_, theme::OK_GREEN, tag);
                    lx += ui::measure(dc, theme::fonts.small_, tag).cx + 10;
                }
                if (heavy) {                             // heavy-for-hardware warning
                    ui::warn(dc, lx, y);
                    ui::text(dc, lx + 20, y + 2, theme::fonts.small_,
                             theme::WARN_YEL, str::CHK_ED_WARN_RAM);
                }
                ui::textClip(dc, { MX + 32, y + 22, theme::CLIENT_W - MX, y + 42 },
                             theme::fonts.small_, theme::TXT_DIM, str::ED_BLURB[i]);
                S.edRect[i] = { MX - 4, y - 4, theme::CLIENT_W - MX, y + 44 };
                y += 50;
            }
            y += 2;
        }

        // Download note — clipped to ONE LINE; no longer overlaps the logo below
        ui::textClip(dc, { MX, y, theme::CLIENT_W - MX, y + 22 },
                     theme::fonts.small_, theme::TXT_DIM,
                     std::format(L"{}  {} — {} {}",
                                 str::CHK_DL_NOTE, g_ctx.verName(), str::WORD_APPROX,
                                 util::fmtGB(cfg::EDITIONS[S.edIndex].approxMB)));
        y += 26;

        if (!S.edExpand && si.ramWarn()) {               // genel RAM notu (kompakt)
            RECT r3{ MX, y, theme::CLIENT_W - MX, y + 40 };
            ui::textWrap(dc, r3, theme::fonts.small_, theme::WARN_YEL, str::CHK_WARN_RAM);
        }
    } else {
        const wchar_t* msg =
            !si.uefi      ? str::CHK_ERR_UEFI :
            si.secureBoot ? str::CHK_ERR_SB   :
            !si.is64      ? str::CHK_ERR_64   : str::CHK_ERR_DISK;
        RECT rc{ MX, y, theme::CLIENT_W - MX, y + 96 };
        ui::textWrap(dc, rc, theme::fonts.body, theme::WARN_YEL, msg);
    }
}

static void drawScan(HDC dc) {
    ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::CMP_TITLE);
    ui::text(dc, MX, SUB_Y, theme::fonts.body, theme::TXT_DIM, str::CMP_SUB);
    ui::spinner(dc, MX + 14, BODY_Y + 26, S.tick);
    ui::text(dc, MX + 44, BODY_Y + 16, theme::fonts.body, theme::TXT, str::CMP_SCANNING);
}

static void drawResult(HDC dc) {
    ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::RES_TITLE);
    RECT sub{ MX, SUB_Y, theme::CLIENT_W - MX, SUB_Y + 44 };
    ui::textWrap(dc, sub, theme::fonts.body, theme::TXT_DIM, str::RES_SUB);

    const CompatResult& r = g_ctx.compat;
    wchar_t buf[160];
    int y = BODY_Y + 6;

    ui::mark(dc, MX, y, true);
    swprintf_s(buf, str::RES_OK_FMT, r.nativeCount);
    ui::text(dc, MX + 32, y, theme::fonts.body, theme::TXT, buf);
    y += 34;

    ui::mark(dc, MX, y, r.reviewCount == 0);
    swprintf_s(buf, str::RES_BAD_FMT, r.reviewCount);
    ui::text(dc, MX + 32, y, theme::fonts.body, theme::TXT, buf);
    y += 42;

    // White panel for items to review (at most 6 rows; the full list is in the report)
    if (r.reviewCount > 0) {
        int rows = std::min(6, r.reviewCount);
        RECT panel{ MX, y, theme::CLIENT_W - MX, y + 16 + rows * 27 + 10 };
        HBRUSH pb = CreateSolidBrush(theme::BG_PANEL);
        FillRect(dc, &panel, pb);
        DeleteObject(pb);

        int py = y + 12, shown = 0;
        for (const auto& it : r.items) {
            if (it.status != Compat::Alternative && it.status != Compat::Caution)
                continue;
            std::wstring note = str::trNote(it.linuxNote);
            std::wstring line = (it.status == Compat::Alternative)
                ? std::format(L"{}  —  {}: {}", it.name, str::RES_ALT_LABEL, note)
                : std::format(L"{}  —  {}", it.name, note);
            ui::textClip(dc, { MX + 16, py, theme::CLIENT_W - MX - 16, py + 24 },
                         theme::fonts.small_, theme::TXT_PANEL, line);
            py += 27;
            if (++shown == rows) break;
        }
        y = panel.bottom + 16;
    }

    // Report link (underlined)
    SIZE lsz = ui::measure(dc, theme::fonts.body, str::RES_LINK);
    ui::text(dc, MX, y, theme::fonts.body, theme::LINK, str::RES_LINK);
    HPEN pen = CreatePen(PS_SOLID, 1, theme::LINK);
    HPEN op = (HPEN)SelectObject(dc, pen);
    MoveToEx(dc, MX, y + lsz.cy + 1, nullptr);
    LineTo(dc, MX + lsz.cx, y + lsz.cy + 1);
    SelectObject(dc, op);
    DeleteObject(pen);
    S.linkRect = { MX, y, MX + lsz.cx, y + lsz.cy + 4 };
}

static void drawKeep(HDC dc) {
    ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::KEEP_TITLE);
    int y = BODY_Y + 6;

    ui::radio(dc, MX, y, false, false);                  // (coming soon)
    ui::text(dc, MX + 32, y - 2, theme::fonts.body, theme::TXT_DIM,
             std::format(L"{} {}", str::KEEP_FILES, str::KEEP_FILES_SOON));
    y += 46;

    ui::radio(dc, MX, y, S.keepNothing, true);
    ui::text(dc, MX + 32, y - 2, theme::fonts.body, theme::TXT, str::KEEP_NOTHING);
    S.radioNothing = { MX - 4, y - 6, MX + 420, y + 26 };
}

static void drawReady(HDC dc) {
    ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::RDY_TITLE);
    RECT sub{ MX, SUB_Y, theme::CLIENT_W - MX, SUB_Y + 50 };
    ui::textWrap(dc, sub, theme::fonts.body, theme::TXT_DIM, str::RDY_SUB);

    int y = BODY_Y + 14;
    auto row = [&](const std::wstring& s) {
        ui::mark(dc, MX, y, true);
        ui::text(dc, MX + 32, y, theme::fonts.body, theme::TXT, s);
        y += 34;
    };
    row(std::format(L"{} — {}", g_ctx.verName(), g_ctx.edition->displayName));
    row(str::RDY_KEEP_NOTH);
    row(str::RDY_NOUSB);

    y += 6;
    ui::text(dc, MX, y, theme::fonts.small_,
             g_ctx.versionDiscovered ? theme::TXT_DIM : theme::WARN_YEL,
             g_ctx.versionDiscovered ? str::RDY_VER_ONLINE : str::RDY_VER_OFFLINE);
    y += 28;

    int st = g_ctx.dlState.load();
    std::wstring dlLine;
    COLORREF c = theme::TXT_DIM;
    if (st == 1) {
        uint64_t d = g_ctx.dlDone, t = g_ctx.dlTotal;
        wchar_t dlBuf[96];
        swprintf_s(dlBuf, str::RDY_DL_FMT, t ? (int)(d * 100 / t) : 0);
        dlLine = dlBuf;
    } else if (st == 2) {
        dlLine = (const wchar_t*)str::RDY_DL_DONE;   // Txt->wstring atama belirsizligini onler
    } else if (st == 3) {
        dlLine = (const wchar_t*)str::RDY_DL_ERR;
        c = theme::WARN_YEL;
    }
    if (!dlLine.empty())
        ui::text(dc, MX, y, theme::fonts.small_, c, dlLine);
}

// Splash screen — TWO STAGES like the Windows 8 boot flow (screenshots 6 → 5):
//   Stage 0: just the logo centered in the small, frameless window
//   Stage 1: logo top-left + "Preparing  %.." on the left
// When the splash ends, endSplash() switches the window to the normal wizard size.
static void drawSplash(HDC dc, const RECT& rc) {
    if (S.splashStage == 0) {
        int h  = 56;
        int lw = img::width(L"logo.png", h);
        if (lw <= 0) lw = 300;                           // estimate for the text fallback
        drawLogo(dc, (rc.right - lw) / 2, (rc.bottom - h) / 2, h);
        return;
    }
    drawLogo(dc, 26, 20, 28);                            // small logo top-left
    wchar_t big[64];
    swprintf_s(big, str::SPL_BIG_FMT, S.splashPct);
    ui::text(dc, 30, rc.bottom / 2 - 16, theme::fonts.mid, theme::TXT, big);
}

// "Preparing 96%" — the Windows 8 preparation-screen layout from screenshot 2
static void drawInstall(HDC dc) {
    drawLogo(dc, MX, 34, 40);                            // large logo top-left (screenshot 5)

    wchar_t big[64];
    swprintf_s(big, str::INS_BIG_FMT, S.pct);
    ui::text(dc, MX, 214, theme::fonts.huge, theme::TXT, big);

    ui::text(dc, MX, 296, theme::fonts.body, theme::TXT_DIM, str::INS_PHASES[S.phase]);

    // Thin, minimal progress line
    RECT bar{ MX, 340, theme::CLIENT_W - MX, 345 };
    HBRUSH tr = CreateSolidBrush(theme::BAR_THIN_BG);
    FillRect(dc, &bar, tr);
    DeleteObject(tr);
    RECT fill = bar;
    fill.right = bar.left + (int)((int64_t)(bar.right - bar.left) * S.pct / 100);
    HBRUSH fl = CreateSolidBrush(theme::TXT);
    FillRect(dc, &fill, fl);
    DeleteObject(fl);

    if (S.phase == PH_DOWNLOAD) {                        // indirilen / toplam
        uint64_t d = g_ctx.dlDone, t = g_ctx.dlTotal;
        if (t) ui::text(dc, MX, 362, theme::fonts.small_, theme::TXT_DIM,
                        std::format(L"{} / {}",
                                    util::fmtGB(d / (1024ull * 1024ull)),
                                    util::fmtGB(t / (1024ull * 1024ull))));
    }
}

static void drawRestart(HDC dc) {
    ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::FIN_TITLE);
    RECT sub{ MX, SUB_Y, theme::CLIENT_W - MX, SUB_Y + 110 };
    ui::textWrap(dc, sub, theme::fonts.body, theme::TXT_DIM,
                 S.later ? str::FIN_LATER_TXT : str::FIN_SUB);
    ui::text(dc, MX, 320, theme::fonts.small_, theme::TXT_DIM, str::FIN_REVERT);
}

static void drawError(HDC dc) {
    ui::text(dc, MX, TITLE_Y, theme::fonts.title, theme::TXT, str::ERR_TITLE);
    RECT rc{ MX, SUB_Y + 8, theme::CLIENT_W - MX, SUB_Y + 240 };
    ui::textWrap(dc, rc, theme::fonts.body, theme::WARN_YEL, g_ctx.getError());

    // "Details: " (plain) + log path (clickable, underlined link)
    int ly = 430;
    ui::text(dc, MX, ly, theme::fonts.small_, theme::TXT_DIM, str::ERR_LOG_LABEL);
    int px = MX + ui::measure(dc, theme::fonts.small_, str::ERR_LOG_LABEL).cx;
    SIZE ps = ui::measure(dc, theme::fonts.small_, str::ERR_LOG_PATH);
    ui::text(dc, px, ly, theme::fonts.small_, theme::LINK, str::ERR_LOG_PATH);
    HPEN pen = CreatePen(PS_SOLID, 1, theme::LINK);
    HPEN op = (HPEN)SelectObject(dc, pen);
    MoveToEx(dc, px, ly + ps.cy + 1, nullptr);
    LineTo(dc, px + ps.cx, ly + ps.cy + 1);
    SelectObject(dc, op);
    DeleteObject(pen);
    S.logLink = { px, ly, px + ps.cx, ly + ps.cy + 4 };
}

// ============================================================================
//  EVENT HANDLERS
// ============================================================================
void wiz::onPaint(HWND w) {
    PAINTSTRUCT ps;
    HDC dc0 = BeginPaint(w, &ps);
    RECT rc;
    GetClientRect(w, &rc);

    // Double-buffered painting (prevents flicker)
    HDC dc = CreateCompatibleDC(dc0);
    HBITMAP bmp = CreateCompatibleBitmap(dc0, rc.right, rc.bottom);
    HBITMAP ob = (HBITMAP)SelectObject(dc, bmp);

    HBRUSH bg = CreateSolidBrush(theme::BG);
    FillRect(dc, &rc, bg);
    DeleteObject(bg);

    bool bareScreen = (S.page == PG_INSTALL || S.page == PG_SPLASH);
    if (!bareScreen)                                     // sade ekranlarda nav yok
        drawNav(dc);
    layout();
    switch (S.page) {
        case PG_SPLASH:  drawSplash(dc, rc); break;
        case PG_CHECK:   drawCheck(dc);   break;
        case PG_SCAN:    drawScan(dc);    break;
        case PG_RESULT:  drawResult(dc);  break;
        case PG_KEEP:    drawKeep(dc);    break;
        case PG_READY:   drawReady(dc);   break;
        case PG_INSTALL: drawInstall(dc); break;
        case PG_RESTART: drawRestart(dc); break;
        case PG_ERROR:   drawError(dc);   break;
    }
    if (!bareScreen)                                     // sol altta logo
        drawLogo(dc, MX, theme::CLIENT_H - 50, 26);

    for (size_t i = 0; i < S.btns.size(); ++i)
        ui::button(dc, S.btns[i], (int)i == S.hover && S.btns[i].enabled);

    BitBlt(dc0, 0, 0, rc.right, rc.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, ob);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(w, &ps);
}

void wiz::onMouseMove(HWND w, POINT p) {
    int h = ui::hitTest(S.btns, p);
    if (h != S.hover) {
        S.hover = h;
        repaint(w);
    }
    // Show a hand cursor over a clickable link/row
    bool onLink =
        (S.page == PG_RESULT && PtInRect(&S.linkRect, p)) ||
        (S.page == PG_ERROR  && PtInRect(&S.logLink,  p)) ||
        (S.page == PG_CHECK && !S.busy && !S.edExpand && PtInRect(&S.edLink, p));
    if (!onLink && S.page == PG_CHECK && !S.busy && S.edExpand) {
        for (int i = 0; i < 3; ++i)
            if (PtInRect(&S.edRect[i], p)) { onLink = true; break; }
    }
    if (h >= 0) onLink = true;                            // buttons get a hand cursor too
    SetCursor(LoadCursorW(nullptr, onLink ? IDC_HAND : IDC_ARROW));
}

void wiz::onLButtonUp(HWND w, POINT p) {
    // Link and radio areas
    if (S.page == PG_RESULT && PtInRect(&S.linkRect, p)) {
        ShellExecuteW(w, L"open", g_ctx.compat.reportPath.c_str(),
                      nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    // Error page: log-path link -> open the assistant.log file
    if (S.page == PG_ERROR && PtInRect(&S.logLink, p)) {
        std::wstring logPath = std::wstring(cfg::INSTALL_DIR) + L"\\assistant.log";
        ShellExecuteW(w, L"open", logPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    if (S.page == PG_KEEP && PtInRect(&S.radioNothing, p)) {
        S.keepNothing = true;
        repaint(w);
        return;
    }
    // Edition selection (check page)
    if (S.page == PG_CHECK && !S.busy) {
        if (!S.edExpand && PtInRect(&S.edLink, p)) {     // "Choose a different edition"
            S.edExpand = true;
            repaint(w);
            return;
        }
        if (S.edExpand) {
            for (int i = 0; i < 3; ++i) {
                if (PtInRect(&S.edRect[i], p)) {
                    if (S.edIndex != i) {
                        S.edIndex = i;
                        g_ctx.edition = &cfg::EDITIONS[i];
                        logx::write(std::format(L"User selected edition: {}",
                                                cfg::EDITIONS[i].displayName));
                    }
                    repaint(w);
                    return;
                }
            }
        }
    }

    int i = ui::hitTest(S.btns, p);
    if (i < 0 || !S.btns[i].enabled) return;

    switch (S.btns[i].id) {
        case ID_NEXT:
            if (S.page == PG_CHECK) {
                if (!S.dlStarted) {                      // ISO download starts in the background
                    S.dlStarted = true;
                    startDownloadThread(w);
                    logx::write(L"Background download started: " + g_ctx.isoUrl());
                }
                gotoPage(w, PG_SCAN);
            } else if (S.page == PG_RESULT) {
                gotoPage(w, PG_KEEP);
            } else if (S.page == PG_KEEP) {
                gotoPage(w, PG_READY);
            }
            break;
        case ID_BACK:
            if (S.page == PG_KEEP)       gotoPage(w, PG_RESULT);  // no re-scan
            else if (S.page == PG_READY) gotoPage(w, PG_KEEP);
            break;
        case ID_INSTALL:
            logx::write(L"Installation started");
            gotoPage(w, PG_INSTALL);
            break;
        case ID_CANCEL: {
            if (g_ctx.cancel) break;                 // zaten iptal ediliyor
            // Ask for confirmation while install/download is running; prevents
            // losing progress by an accidental click. "No" is the default button.
            int r = MessageBoxW(w, str::CANCEL_CONFIRM, str::APP_TITLE,
                                MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
            if (r == IDYES) {
                g_ctx.cancel = true;
                logx::write(L"User requested cancellation");
            }
            repaint(w);
            break;
        }
        case ID_RESTART:
            logx::write(L"Restarting…");
            util::rebootNow();
            break;
        case ID_LATER:
            S.later = true;
            repaint(w);
            break;
        case ID_CLOSE:
            PostMessageW(w, WM_CLOSE, 0, 0);
            break;
    }
}

void wiz::onTimer(HWND w) {
    // Timer 2: splash screen percentage (~45 ms * 100 ≈ 4-5 s)
    // This is the "wait a few seconds with an increasing percentage" behavior requested.
    if (S.page == PG_SPLASH) {
        ++S.splashTick;
        if (S.splashStage == 0) {                        // ortada logo (~1,5 sn)
            if (S.splashTick >= 34) {
                S.splashStage = 1;
                S.splashTick  = 0;
                S.splashPct   = 0;
                repaint(w);
            }
        } else if (S.splashPct < 100) {                  // "Preparing %.." increase
            S.splashPct += 2;
            if (S.splashPct > 100) S.splashPct = 100;
            repaint(w);
        } else {
            endSplash(w);                                // switch to the normal window
        }
        return;
    }

    ++S.tick;
    if ((S.page == PG_CHECK && S.busy) || S.page == PG_SCAN ||
        S.page == PG_INSTALL || S.page == PG_READY)
        repaint(w);
}

void wiz::onKey(HWND w, WPARAM key) {
    if (S.page == PG_SPLASH && key == VK_ESCAPE)         // ESC: skip the splash
        endSplash(w);
}

// No need to ask for exit confirmation on the error or finish screen: work is either
// complete or already cancelled. On all other pages confirmation is requested.
bool wiz::needExitConfirm() {
    return !(S.page == PG_ERROR || S.page == PG_RESTART);
}

LRESULT wiz::onAppMsg(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_APP_CHECKDONE:
            S.busy = false;
            repaint(w);
            break;
        case WM_APP_SCANDONE:
            S.busy = false;
            gotoPage(w, PG_RESULT);
            break;
        case WM_APP_PROGRESS:
            S.pct = (int)wp;
            S.phase = (int)lp;
            if (S.page == PG_INSTALL) repaint(w);
            break;
        case WM_APP_DONE:
            gotoPage(w, wp == 0 ? PG_RESTART : PG_ERROR);
            break;
    }
    return 0;
}
