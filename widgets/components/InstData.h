// InstData.h — shared data types and deterministic demo values for the
// XFrame Instrumentation widget section (tank level, switches, lamps, LEDs,
// gauges, panel furniture).
//
// No hardware I/O, no network: demos and tests run on deterministic values
// defined here, so results are reproducible.
// Attribution: XFrame.
#pragma once

#include <cstdint>

#include <X11/Xlib.h>

namespace xframe {

// Tank / channel level, percent full, clamped to [0, 100].
inline double inst_clamp_level(double pct) {
    if (pct < 0.0) return 0.0;
    if (pct > 100.0) return 100.0;
    return pct;
}

// Alarm bands for a level, in percent. hi_hi >= hi >= lo >= lo_lo.
struct InstAlarmBands {
    double hi_hi = 95.0;
    double hi = 80.0;
    double lo = 20.0;
    double lo_lo = 5.0;
};

// Switch positions. Centre-off rockers use Off as the middle position;
// breakers use Tripped for the fault position.
enum class InstSwitchState { Off = 0, On = 1, Tripped = 2 };

// One illuminator: on/off plus brightness 0-100 (100 = full).
struct InstLamp {
    bool on = false;
    int brightness = 100;
};

// One RGB status LED: three independent channels, each an independent
// illuminator. Any of the 8 colour combinations plus off.
struct InstRgb {
    bool red = false;
    bool green = false;
    bool blue = false;
};

// Deterministic demo level for a widget, in [0, 100]. Same seed gives the
// same level on every run and every platform.
inline double inst_demo_level(unsigned seed = 42) {
    std::uint64_t x = 0x9E3779B97F4A7C15ULL ^
        (static_cast<std::uint64_t>(seed) * 0xBF58476D1CE4E5B9ULL);
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    const std::uint64_t v = x * 0x2545F4914F6CDD1DULL;
    return static_cast<double>(v % 9001) / 100.0 + 5.0;  // 5.00 .. 95.00
}

// Deterministic demo value for a gauge, in [min, max].
inline double inst_demo_value(double min_v, double max_v, unsigned seed = 42) {
    if (max_v <= min_v) return min_v;
    const double f = inst_demo_level(seed) / 100.0;
    return min_v + f * (max_v - min_v);
}


// Bold instrumentation font: the 13 px medium core font grown by 33%
// (13 * 1.33 = 17.29) and made bold. The exact 17 px size does not exist
// in the server fixed-font set (probed Nov 26, 2026: bold sizes available
// are 13, 14, 15, 18), so 18 px is the target and the others are
// fallbacks in probe order. Lazy static, loaded once; never XFreeFont'd:
// a process-lifetime resource, the display closes before any destructor
// runs (same rationale as the SStyle_Manager comment in
// core/xframe_core.h). Returns nullptr if no size loads, in which case
// callers keep the style's own font.
inline XFontStruct* inst_bold_font(Display* display) {
    if (!display) return nullptr;
    static XFontStruct* cached = nullptr;
    static bool tried = false;
    if (tried) return cached;
    tried = true;
    static const char* kNames[] = {
        "-misc-fixed-bold-r-normal--18-*",
        "-misc-fixed-bold-r-normal--17-*",
        "-misc-fixed-bold-r-normal--16-*",
        "-misc-fixed-bold-r-normal--20-*",
    };
    for (const char* name : kNames) {
        cached = XLoadQueryFont(display, name);
        if (cached) break;
    }
    return cached;
}

}  // namespace xframe
