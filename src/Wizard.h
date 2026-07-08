#pragma once
// ============================================================================
//  Wizard.h — Page state machine: painting, mouse, timer, worker-thread messages
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace wiz {
    void    onCreate(HWND w);
    void    onPaint(HWND w);
    void    onMouseMove(HWND w, POINT p);
    void    onLButtonUp(HWND w, POINT p);
    void    onTimer(HWND w);
    void    onKey(HWND w, WPARAM key);
    // Is exit confirmation needed? Unnecessary on error/finish screens (work is
    // done or already cancelled); on those pages Close/X exits directly.
    bool    needExitConfirm();
    LRESULT onAppMsg(HWND w, UINT msg, WPARAM wp, LPARAM lp);
}
