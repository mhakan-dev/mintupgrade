#pragma once
// ============================================================================
//  Ui.h — Win8-Upgrade-Assistant-style custom-drawn widgets (button, progress, radio, spinner)
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>

namespace ui {

struct Btn {
    RECT         r{};
    int          id = 0;
    std::wstring label;
    bool         enabled = true;
};

// Single-line text (top-left aligned)
void text(HDC dc, int x, int y, HFONT f, COLORREF c, const std::wstring& s);
// Single line; truncates with "…" if it doesn't fit the width
void textClip(HDC dc, RECT rc, HFONT f, COLORREF c, const std::wstring& s);
// Word-wrapped text; returns the height used
int  textWrap(HDC dc, RECT rc, HFONT f, COLORREF c, const std::wstring& s);
SIZE measure(HDC dc, HFONT f, const std::wstring& s);

void button(HDC dc, const Btn& b, bool hover);
void progressBar(HDC dc, RECT rc, int pct);
void radio(HDC dc, int x, int y, bool selected, bool enabled);
void spinner(HDC dc, int cx, int cy, int tick);
void mark(HDC dc, int x, int y, bool ok);                 // ✓ green / ✗ red
void warn(HDC dc, int x, int y);                          // ⚠ yellow (not a blocker)

int  hitTest(const std::vector<Btn>& v, POINT p);         // indeks, yoksa -1

} // namespace ui
