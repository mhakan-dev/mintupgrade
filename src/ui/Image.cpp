// ============================================================================
//  Image.cpp — PNG drawing via GDI+ (embedded resource + optional disk).
//
//  IMPORTANT: GDI+ headers require OLE types (IStream, PROPID, byte).
//  These are EXCLUDED from windows.h under WIN32_LEAN_AND_MEAN, causing
//  hundreds of compile errors. So the macro is NOT defined in THIS FILE ONLY;
//  windows.h tam haliyle + objidl.h ile dahil edilir.
// ============================================================================
#include <windows.h>
#include <objidl.h>                  // IStream (required by GDI+)
#include <shlwapi.h>                 // SHCreateMemStream (embedded PNG -> IStream)
#include <algorithm>
using std::min;                      // NOMINMAX is defined; gdiplus expects min/max
using std::max;
#include <gdiplus.h>
#include "Image.h"
#include "../../resources/resource.h"
#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <cmath>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")

namespace fs = std::filesystem;

namespace img {

static ULONG_PTR g_token = 0;
static std::map<std::wstring, std::shared_ptr<Gdiplus::Bitmap>> g_cache;

void startup() {
    Gdiplus::GdiplusStartupInput in;
    Gdiplus::GdiplusStartup(&g_token, &in, nullptr);
}
void shutdown() {
    g_cache.clear();
    if (g_token) { Gdiplus::GdiplusShutdown(g_token); g_token = 0; }
}

// Known file name -> embedded resource id inside the exe
static int resourceIdFor(const std::wstring& name) {
    if (name == L"logo.png") return IDR_LOGO;
    return 0;
}

// Load a PNG from an embedded RCDATA resource (the exe alone suffices, NO extra file)
static Gdiplus::Bitmap* fromResource(int id) {
    HMODULE mod = GetModuleHandleW(nullptr);
    HRSRC   hr  = FindResourceW(mod, MAKEINTRESOURCEW(id), (LPCWSTR)RT_RCDATA);
    if (!hr) return nullptr;
    HGLOBAL hg  = LoadResource(mod, hr);
    DWORD   sz  = SizeofResource(mod, hr);
    const void* data = hg ? LockResource(hg) : nullptr;
    if (!data || !sz) return nullptr;

    IStream* st = SHCreateMemStream((const BYTE*)data, sz);
    if (!st) return nullptr;
    Gdiplus::Bitmap* b = Gdiplus::Bitmap::FromStream(st, FALSE);
    st->Release();
    if (b && b->GetLastStatus() != Gdiplus::Ok) { delete b; b = nullptr; }
    return b;
}

// Optional customization: if logo.png (or resources\logo.png) exists next to the
// exe, it's used instead of the embedded one. Otherwise nothing is needed.
static std::wstring diskPath(const std::wstring& fileName) {
    wchar_t exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    fs::path base = fs::path(exe).parent_path();
    std::error_code ec;
    fs::path p1 = base / fileName;
    if (fs::exists(p1, ec)) return p1.wstring();
    fs::path p2 = base / L"resources" / fileName;
    if (fs::exists(p2, ec)) return p2.wstring();
    return L"";
}

static Gdiplus::Bitmap* load(const std::wstring& fileName) {
    auto it = g_cache.find(fileName);
    if (it != g_cache.end()) return it->second.get();

    std::shared_ptr<Gdiplus::Bitmap> bmp;

    // 1) Disk (a user image takes priority if present — NOT required)
    std::wstring path = diskPath(fileName);
    if (!path.empty()) {
        bmp.reset(Gdiplus::Bitmap::FromFile(path.c_str(), FALSE));
        if (bmp && bmp->GetLastStatus() != Gdiplus::Ok) bmp.reset();
    }
    // 2) Embedded resource — the normal path: from inside the exe
    if (!bmp) {
        if (int id = resourceIdFor(fileName))
            bmp.reset(fromResource(id));
    }
    g_cache[fileName] = bmp;              // cache nullptr too if not found
    return bmp.get();
}

int width(const std::wstring& fileName, int targetHeight) {
    Gdiplus::Bitmap* b = load(fileName);
    if (!b) return 0;
    double ar = (double)b->GetWidth() / (double)b->GetHeight();
    return (int)(targetHeight * ar + 0.5);
}

bool draw(HDC dc, const std::wstring& fileName, int x, int y, int targetHeight) {
    Gdiplus::Bitmap* b = load(fileName);
    if (!b) return false;
    double ar = (double)b->GetWidth() / (double)b->GetHeight();
    int w = (int)(targetHeight * ar + 0.5);

    Gdiplus::Graphics g(dc);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.DrawImage(b, x, y, w, targetHeight);
    return true;
}


// ============================================================================
//  Anti-aliased small drawings — GDI+ is used instead of GDI's pixelated
//  Ellipse/Line output; real alpha is supported.
// ============================================================================
void radio(HDC dc, int x, int y, int diameter, bool selected, COLORREF color) {
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    Gdiplus::Color c(255, GetRValue(color), GetGValue(color), GetBValue(color));
    Gdiplus::REAL  d = (Gdiplus::REAL)diameter;

    Gdiplus::Pen ring(c, 2.0f);
    g.DrawEllipse(&ring, (Gdiplus::REAL)x + 1.0f, (Gdiplus::REAL)y + 1.0f,
                  d - 2.0f, d - 2.0f);

    if (selected) {
        Gdiplus::SolidBrush br(c);
        Gdiplus::REAL in  = d * 0.46f;
        Gdiplus::REAL off = (d - in) / 2.0f;
        g.FillEllipse(&br, (Gdiplus::REAL)x + off, (Gdiplus::REAL)y + off, in, in);
    }
}

void spinner(HDC dc, int cx, int cy, int tick, COLORREF color) {
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    const float PI    = 3.14159265f;
    const float R     = 14.0f;                        // radius
    const int   SEG   = 30;                           // arc segment count
    const float SWEEP = 300.0f * PI / 180.0f;         // tail length (300°)
    const float head  = tick * 0.16f;                 // rotation speed

    const int r = GetRValue(color), gr = GetGValue(color), b = GetBValue(color);

    float px = cx + R * cosf(head);
    float py = cy + R * sinf(head);
    for (int k = 1; k <= SEG; ++k) {
        float a  = head - SWEEP * (float)k / SEG;
        float x2 = cx + R * cosf(a);
        float y2 = cy + R * sinf(a);
        int alpha = (int)(235.0f * (1.0f - (float)k / SEG));   // head is bright
        Gdiplus::Pen pen(Gdiplus::Color((BYTE)alpha, (BYTE)r, (BYTE)gr, (BYTE)b), 3.4f);
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        g.DrawLine(&pen, px, py, x2, y2);
        px = x2;
        py = y2;
    }
}

} // namespace img
