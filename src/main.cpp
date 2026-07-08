// ============================================================================
//  main.cpp — WinMain, window class and message loop
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include "Theme.h"
#include "Strings.h"
#include "Context.h"
#include "Wizard.h"
#include "ui/Image.h"
#include "core/Log.h"
#include <filesystem>

static LRESULT CALLBACK WndProc(HWND w, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
        case WM_CREATE:
            theme::fonts.create();
            wiz::onCreate(w);
            return 0;
        case WM_PAINT:
            wiz::onPaint(w);
            return 0;
        case WM_ERASEBKGND:
            return 1;                                    // paints double-buffered
        case WM_MOUSEMOVE: {
            POINT p{ GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            wiz::onMouseMove(w, p);
            return 0;
        }
        case WM_SETCURSOR:
            // onMouseMove manages the client-area cursor; prevent the window
            // class's default arrow from overriding it. The border/title bar
            // keeps its default behavior.
            if (LOWORD(lp) == HTCLIENT) return TRUE;
            break;
        case WM_LBUTTONUP: {
            POINT p{ GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            wiz::onLButtonUp(w, p);
            return 0;
        }
        case WM_TIMER:
            wiz::onTimer(w);
            return 0;
        case WM_KEYDOWN:
            wiz::onKey(w, wp);
            return 0;
        case WM_APP_CHECKDONE:
        case WM_APP_SCANDONE:
        case WM_APP_PROGRESS:
        case WM_APP_DONE:
            return wiz::onAppMsg(w, m, wp, lp);
        case WM_CLOSE: {
            // On error/finish screens, exit directly without confirmation (work is
            // done or cancelled). On all other pages X/Alt+F4/Close ask to confirm;
            // "No" is the default button, so an accidental Enter won't exit.
            if (!wiz::needExitConfirm()) {
                DestroyWindow(w);
                return 0;
            }
            int r = MessageBoxW(w, str::EXIT_CONFIRM, str::APP_TITLE,
                                MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
            if (r == IDYES)
                DestroyWindow(w);
            return 0;
        }
        case WM_DESTROY:
            theme::fonts.destroy();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(w, m, wp, lp);
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int) {
    // DPI: SetProcessDpiAwarenessContext is a Win10 1703+ only API. If linked
    // statically the exe won't even OPEN on Win7/8.1 ("entry point not found").
    // So it's loaded dynamically; on older systems we fall back to the classic call.
    {
        typedef BOOL (WINAPI* PFN_SPDAC)(DPI_AWARENESS_CONTEXT);
        HMODULE u32 = GetModuleHandleW(L"user32.dll");
        PFN_SPDAC pDpi = u32
            ? (PFN_SPDAC)GetProcAddress(u32, "SetProcessDpiAwarenessContext")
            : nullptr;
        if (pDpi) pDpi(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
        else      SetProcessDPIAware();               // Vista+ (Win7/8.1 yolu)
    }

    // Working folder + log + GDI+ (for the PNG logo)
    std::error_code ec;
    std::filesystem::create_directories(cfg::INSTALL_DIR, ec);
    logx::init();
    img::startup();

    WNDCLASSW wc{};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = inst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIconW(inst, MAKEINTRESOURCEW(1));   // resources/app.rc (embedded)
    wc.hbrBackground = CreateSolidBrush(theme::BG);
    wc.lpszClassName = L"MintUpgradeAssistant";
    // If no embedded icon, try loading from resources\mint.ico
    if (!wc.hIcon) {
        wchar_t exe[MAX_PATH];
        GetModuleFileNameW(nullptr, exe, MAX_PATH);
        std::filesystem::path ico = std::filesystem::path(exe).parent_path()
                                     / L"resources" / L"mint.ico";
        wc.hIcon = (HICON)LoadImageW(nullptr, ico.c_str(), IMAGE_ICON, 0, 0,
                                     LR_LOADFROMFILE | LR_DEFAULTSIZE);
    }
    if (!wc.hIcon) wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    RegisterClassW(&wc);

    // Startup: begins SMALL and frameless like the Windows 8 boot screen
    // (first a centered logo, then top-left logo + "Preparing %.."),
    // once the splash ends Wizard switches the window to normal size/frame.
    int W = theme::SPLASH_W;
    int H = theme::SPLASH_H;
    int x = (GetSystemMetrics(SM_CXSCREEN) - W) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - H) / 2;

    HWND w = CreateWindowExW(0, wc.lpszClassName, str::APP_TITLE, WS_POPUP,
                             x, y, W, H, nullptr, nullptr, inst, nullptr);
    ShowWindow(w, SW_SHOW);
    UpdateWindow(w);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    img::shutdown();
    return 0;
}
