#pragma once
// ============================================================================
//  Image.h — PNG loading and drawing via GDI+ (with transparency)
//  Loads small images like the logo from the resources/ folder next to the exe.
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace img {

void startup();     // Start GDI+ (at the top of WinMain)
void shutdown();    // Shut down GDI+ (on exit)

// Loads the PNG (once) and draws it at the given top-left corner, scaled to the
// target height. Aspect ratio is preserved. If the file is missing it silently
// does nothing and returns false (the caller can fall back to text).
// key: cache key (so the same file isn't loaded repeatedly).
bool draw(HDC dc, const std::wstring& fileName, int x, int y, int targetHeight);

// Returns the image's width at the target height if loadable (else 0).
int  width(const std::wstring& fileName, int targetHeight);


// Anti-aliased small drawings via GDI+ (Ui delegates to these)
// Radio button: outer ring + inner fill if selected (smooth edges).
void radio(HDC dc, int x, int y, int diameter, bool selected, COLORREF color);
// Modern loading ring: a rotating arc whose tail fades with real alpha.
void spinner(HDC dc, int cx, int cy, int tick, COLORREF color);

} // namespace img
