#include "Ui.h"
#include "Image.h"
#include "../Theme.h"
#include <string>
#include <vector>
#include <cmath>

namespace ui {

void text(HDC dc, int x, int y, HFONT f, COLORREF c, const std::wstring& s) {
    HFONT old = (HFONT)SelectObject(dc, f);
    SetTextColor(dc, c);
    SetBkMode(dc, TRANSPARENT);
    TextOutW(dc, x, y, s.c_str(), (int)s.size());
    SelectObject(dc, old);
}

void textClip(HDC dc, RECT rc, HFONT f, COLORREF c, const std::wstring& s) {
    HFONT old = (HFONT)SelectObject(dc, f);
    SetTextColor(dc, c);
    SetBkMode(dc, TRANSPARENT);
    DrawTextW(dc, s.c_str(), -1, &rc,
              DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    SelectObject(dc, old);
}

int textWrap(HDC dc, RECT rc, HFONT f, COLORREF c, const std::wstring& s) {
    HFONT old = (HFONT)SelectObject(dc, f);
    SetTextColor(dc, c);
    SetBkMode(dc, TRANSPARENT);
    RECT calc = rc;
    DrawTextW(dc, s.c_str(), -1, &calc, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
    DrawTextW(dc, s.c_str(), -1, &rc,   DT_WORDBREAK | DT_NOPREFIX);
    SelectObject(dc, old);
    return calc.bottom - calc.top;
}

SIZE measure(HDC dc, HFONT f, const std::wstring& s) {
    HFONT old = (HFONT)SelectObject(dc, f);
    SIZE sz{};
    GetTextExtentPoint32W(dc, s.c_str(), (int)s.size(), &sz);
    SelectObject(dc, old);
    return sz;
}

void button(HDC dc, const Btn& b, bool hover) {
    COLORREF bg = !b.enabled ? theme::BTN_BG_DIS
                             : (hover ? theme::BTN_BG_HOT : theme::BTN_BG);
    COLORREF tx = b.enabled ? theme::BTN_TXT : theme::BTN_TXT_DIS;

    HBRUSH br = CreateSolidBrush(bg);
    FillRect(dc, &b.r, br);
    DeleteObject(br);

    HPEN pen = CreatePen(PS_SOLID, 1, theme::BTN_BORDER);
    HPEN oldP = (HPEN)SelectObject(dc, pen);
    HBRUSH oldB = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, b.r.left, b.r.top, b.r.right, b.r.bottom);
    SelectObject(dc, oldB);
    SelectObject(dc, oldP);
    DeleteObject(pen);

    RECT rc = b.r;
    HFONT old = (HFONT)SelectObject(dc, theme::fonts.btn);
    SetTextColor(dc, tx);
    SetBkMode(dc, TRANSPARENT);
    DrawTextW(dc, b.label.c_str(), -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, old);
}

void progressBar(HDC dc, RECT rc, int pct) {
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;

    HBRUSH tr = CreateSolidBrush(theme::BAR_TRACK);
    FillRect(dc, &rc, tr);
    DeleteObject(tr);

    RECT fill = rc;
    fill.right = rc.left + (int)((int64_t)(rc.right - rc.left) * pct / 100);
    HBRUSH fl = CreateSolidBrush(theme::BAR_FILL);
    FillRect(dc, &fill, fl);
    DeleteObject(fl);

    HPEN pen = CreatePen(PS_SOLID, 1, theme::BAR_BORDER);
    HPEN oldP = (HPEN)SelectObject(dc, pen);
    HBRUSH oldB = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(dc, oldB);
    SelectObject(dc, oldP);
    DeleteObject(pen);
}

void radio(HDC dc, int x, int y, bool selected, bool enabled) {
    // Anti-aliased draw via GDI+ (Image.cpp).
    img::radio(dc, x, y, 19, selected, enabled ? theme::TXT : theme::TXT_DIM);
}

void spinner(HDC dc, int cx, int cy, int tick) {
    // Modern spinner ring with a real-alpha fading tail via GDI+.
    img::spinner(dc, cx, cy, tick, theme::TXT);
}

void mark(HDC dc, int x, int y, bool ok) {
    text(dc, x, y, theme::fonts.sym,
         ok ? theme::OK_GREEN : theme::BAD_RED,
         ok ? L"\u2713" : L"\u2717");
}

void warn(HDC dc, int x, int y) {
    text(dc, x, y, theme::fonts.sym, theme::WARN_YEL, L"\u26A0");
}

int hitTest(const std::vector<Btn>& v, POINT p) {
    for (size_t i = 0; i < v.size(); ++i)
        if (PtInRect(&v[i].r, p)) return (int)i;
    return -1;
}

} // namespace ui
