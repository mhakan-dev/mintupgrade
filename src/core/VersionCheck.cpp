#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "VersionCheck.h"
#include "Downloader.h"
#include "Log.h"
#include "../Config.h"
#include <regex>
#include <vector>
#include <format>
#include <cstdlib>

// "22.3" -> {22,3,0}  (for numeric comparison)
static std::vector<int> verParts(const std::wstring& v) {
    std::vector<int> p;
    size_t i = 0;
    while (i < v.size()) {
        size_t j = v.find(L'.', i);
        if (j == std::wstring::npos) j = v.size();
        p.push_back(_wtoi(v.substr(i, j - i).c_str()));
        i = j + 1;
    }
    while (p.size() < 3) p.push_back(0);
    return p;
}
static bool newer(const std::wstring& a, const std::wstring& b) {   // a > b ?
    return verParts(a) > verParts(b);
}

MintRelease discoverLatestMint() {
    // Match version folders like "22.3/" in the directory listing.
    // The leading [^0-9.] condition prevents years like "2026/" being caught as "26/".
    // Version folders can appear in two forms in directory listings:
    //   • Relative links (kernel.org):        href="22.3/"        -> ">22.3/<"
    //   • Mutlak linkler (pub.linuxmint.io):  href=".../stable/22.3/"
    // We scan both patterns separately and merge; this way dots/numbers inside
    // the host name (e.g. "linuxmint.io") don't cause false matches.
    //   rxSlash : "NN[.N]/" following a '>' or '/' (relative links)
    //   rxStable: "stable/NN[.N]/" (absolute links — the 'stable/' anchor is decisive)
    static const std::wregex rxSlash (LR"([>/](\d{1,2}(?:\.\d{1,2})?)/)");
    static const std::wregex rxStable(LR"([Ss]table/(\d{1,2}(?:\.\d{1,2})?)/)");
    // Last resort: "NN.N/" in any context (with no digit/dot before it)
    static const std::wregex rxLoose (LR"((?:^|[^0-9.])(\d{1,2}\.\d{1,2})/)");

    auto scanBest = [](const std::wstring& html, const std::wregex& rx,
                       std::wstring& best) {
        for (auto it = std::wsregex_iterator(html.begin(), html.end(), rx);
             it != std::wsregex_iterator(); ++it) {
            std::wstring v = (*it)[1].str();
            if (verParts(v)[0] < 17) continue;          // drop old/irrelevant numbers
            if (best.empty() || newer(v, best)) best = v;
        }
    };

    for (const wchar_t* base : cfg::MIRRORS) {
        std::wstring html, err;
        if (!dl::httpGetText(base, html, err, /*quick*/ true)) {
            logx::write(std::format(L"Version query failed [{}]: {}", base, err));
            continue;
        }
        std::wstring best;
        scanBest(html, rxStable, best);                 // the decisive 'stable/' anchor first
        scanBest(html, rxSlash,  best);                 // then relative links
        scanBest(html, rxLoose,  best);                 // last-resort pattern
        if (!best.empty()) {
            logx::write(std::format(L"Latest Mint version: {}  (source: {})", best, base));
            return { best, base, true };
        }
        // Diagnostics: what exactly did the server return? A printable sample of the
        // first 160 chars is logged — this instantly reveals compressed/corrupt
        // content or a redirect page.
        std::wstring sample = html.substr(0, 160);
        for (auto& ch : sample)
            if (ch < 32 || ch == 127) ch = L'.';
        logx::write(std::format(L"Could not parse directory listing: {}  [content sample: {}]",
                                base, sample));
    }

    logx::write(std::format(L"No online version found; using fallback: {}",
                            cfg::FALLBACK_VERSION));
    return { cfg::FALLBACK_VERSION, cfg::MIRRORS[0], false };
}
