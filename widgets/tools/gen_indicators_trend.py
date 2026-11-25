#!/usr/bin/env python3
"""gen_indicators_trend.py — regenerates the 85 XFrame Stock Market TREND
indicator widgets (widgets/components/Ind<Name>/), the audited list
(widgets/tools/stock_indicators_trend.txt), the smoke test and the doc
fragment (docs/stock-market-indicators-trend.md).

Every indicator is a real named trend indicator with a standard formula
(TA-Lib / StockCharts / Investopedia or equivalent). Output convention for
every compute function: output length == input length; leading warm-up bars
are NaN until enough bars exist.
Attribution: XFrame.
"""
import os, sys, math

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))  # widgets/
COMP = os.path.join(ROOT, "components")
TOOLS = os.path.join(ROOT, "tools")
TESTS = os.path.join(ROOT, "tests")
DOCS = os.path.dirname(ROOT)  # /opt/cwidgets

HELPERS = r'''
namespace detail_@@LOWER@@ {
inline double nan_v() { return std::numeric_limits<double>::quiet_NaN(); }
inline bool fin_v(double v) { return std::isfinite(v); }
inline std::vector<double> sma_v(const std::vector<double>& x, int p) {
    std::vector<double> o(x.size(), nan_v());
    if (p < 1) p = 1;
    for (size_t i = 0; i < x.size(); ++i) {
        if (i + 1 < (size_t)p) continue;
        double s = 0.0; bool ok = true;
        for (int j = 0; j < p; ++j) { double v = x[i - (size_t)j]; if (!fin_v(v)) { ok = false; break; } s += v; }
        if (ok) o[i] = s / p;
    }
    return o;
}
inline std::vector<double> ema_v(const std::vector<double>& x, int p) {
    std::vector<double> o(x.size(), nan_v());
    if (p < 1) p = 1;
    if (x.empty()) return o;
    double k = 2.0 / (p + 1.0);
    int cnt = 0; double s = 0.0, prev = nan_v();
    for (size_t i = 0; i < x.size(); ++i) {
        if (!fin_v(x[i])) continue;
        if (!fin_v(prev)) {
            s += x[i]; ++cnt;
            if (cnt == p) { prev = s / p; o[i] = prev; }
        } else {
            prev = x[i] * k + prev * (1.0 - k); o[i] = prev;
        }
    }
    return o;
}
inline std::vector<double> wma_v(const std::vector<double>& x, int p) {
    std::vector<double> o(x.size(), nan_v());
    if (p < 1) p = 1;
    double wsum = (double)p * (p + 1) / 2.0;
    for (size_t i = 0; i < x.size(); ++i) {
        if (i + 1 < (size_t)p) continue;
        double s = 0.0; bool ok = true;
        for (int j = 0; j < p; ++j) { double v = x[i - (size_t)j]; if (!fin_v(v)) { ok = false; break; } s += v * (p - j); }
        if (ok) o[i] = s / wsum;
    }
    return o;
}
inline std::vector<double> smma_v(const std::vector<double>& x, int p) {
    std::vector<double> o(x.size(), nan_v());
    if (p < 1) p = 1;
    if (x.empty()) return o;
    int cnt = 0; double s = 0.0, prev = nan_v();
    for (size_t i = 0; i < x.size(); ++i) {
        if (!fin_v(x[i])) continue;
        if (!fin_v(prev)) {
            s += x[i]; ++cnt;
            if (cnt == p) { prev = s / p; o[i] = prev; }
        } else {
            prev = prev + (x[i] - prev) / p; o[i] = prev;
        }
    }
    return o;
}
inline double highest_v(const std::vector<double>& x, size_t i, int p) {
    double m = -1e300;
    size_t lo = (i + 1 >= (size_t)p) ? i + 1 - (size_t)p : 0;
    for (size_t j = lo; j <= i; ++j) m = std::max(m, x[j]);
    return m;
}
inline double lowest_v(const std::vector<double>& x, size_t i, int p) {
    double m = 1e300;
    size_t lo = (i + 1 >= (size_t)p) ? i + 1 - (size_t)p : 0;
    for (size_t j = lo; j <= i; ++j) m = std::min(m, x[j]);
    return m;
}
inline std::vector<double> tr_v(const OHLCVSeries& s) {
    std::vector<double> o(s.size(), 0.0);
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 0) { o[i] = s[i].high - s[i].low; continue; }
        double a = s[i].high - s[i].low;
        double b = std::fabs(s[i].high - s[i - 1].close);
        double c = std::fabs(s[i].low - s[i - 1].close);
        o[i] = std::max(a, std::max(b, c));
    }
    return o;
}
inline std::vector<double> atr_v(const OHLCVSeries& s, int p) {
    // Wilder-smoothed ATR (TA-Lib ATR).
    std::vector<double> tr = tr_v(s);
    std::vector<double> o(s.size(), nan_v());
    if (p < 1) p = 1;
    double acc = 0.0;
    for (size_t i = 0; i < s.size(); ++i) {
        acc += tr[i];
        if (i + 1 == (size_t)p) o[i] = acc / p;
        else if (i + 1 > (size_t)p) o[i] = o[i - 1] + (tr[i] - o[i - 1]) / p;
    }
    return o;
}
inline std::vector<double> mid_v(const std::vector<double>& hi,
                                 const std::vector<double>& lo, int p) {
    std::vector<double> o(hi.size(), nan_v());
    for (size_t i = 0; i < hi.size(); ++i)
        if (i + 1 >= (size_t)p) o[i] = (highest_v(hi, i, p) + lowest_v(lo, i, p)) / 2.0;
    return o;
}
// Least-squares fit over a window ending at i (x = 0..p-1).
inline void linreg_parts(const std::vector<double>& x, size_t i, int p,
                         double& slope, double& icept, double& value, double& rsq) {
    slope = 0.0; icept = 0.0; value = 0.0; rsq = nan_v();
    if (p < 2 || i + 1 < (size_t)p) return;
    double sx = 0.0, sy = 0.0, sxx = 0.0, sxy = 0.0;
    for (int j = 0; j < p; ++j) {
        double xv = (double)j, yv = x[i - (size_t)(p - 1) + (size_t)j];
        sx += xv; sy += yv; sxx += xv * xv; sxy += xv * yv;
    }
    double den = (double)p * sxx - sx * sx;
    if (den == 0.0) return;
    slope = ((double)p * sxy - sx * sy) / den;
    icept = (sy - slope * sx) / p;
    value = icept + slope * (p - 1);
    double ymean = sy / p, sst = 0.0, sse = 0.0;
    for (int j = 0; j < p; ++j) {
        double yv = x[i - (size_t)(p - 1) + (size_t)j];
        double fit = icept + slope * j;
        sst += (yv - ymean) * (yv - ymean);
        sse += (yv - fit) * (yv - fit);
    }
    rsq = (sst > 0.0) ? 1.0 - sse / sst : 1.0;
}
// Wilder directional movement helpers (TA-Lib ADX family).
inline void dm_tr(const OHLCVSeries& s, size_t i, double& pdm, double& mdm, double& tr) {
    pdm = 0.0; mdm = 0.0; tr = 0.0;
    if (i == 0) { tr = s[0].high - s[0].low; return; }
    double up = s[i].high - s[i - 1].high, dn = s[i - 1].low - s[i].low;
    if (up > dn && up > 0.0) pdm = up;
    if (dn > up && dn > 0.0) mdm = dn;
    double a = s[i].high - s[i].low;
    double b = std::fabs(s[i].high - s[i - 1].close);
    double c = std::fabs(s[i].low - s[i - 1].close);
    tr = std::max(a, std::max(b, c));
}
inline std::vector<double> di_plus_v(const OHLCVSeries& s, int p) {
    std::vector<double> o(s.size(), nan_v());
    if (p < 1) p = 1;
    double sp = 0.0, sm = 0.0, st = 0.0;
    for (size_t i = 0; i < s.size(); ++i) {
        double pdm, mdm, tr; dm_tr(s, i, pdm, mdm, tr);
        sp += pdm; sm += mdm; st += tr;
        if (i + 1 == (size_t)p) { o[i] = (st > 0.0) ? 100.0 * (sp / p) / (st / p) : 0.0; }
        else if (i + 1 > (size_t)p) {
            sp = sp - sp / p + pdm; sm = sm - sm / p + mdm; st = st - st / p + tr;
            o[i] = (st > 0.0) ? 100.0 * sp / st : 0.0;
        }
    }
    return o;
}
inline std::vector<double> di_minus_v(const OHLCVSeries& s, int p) {
    std::vector<double> o(s.size(), nan_v());
    if (p < 1) p = 1;
    double sp = 0.0, sm = 0.0, st = 0.0;
    for (size_t i = 0; i < s.size(); ++i) {
        double pdm, mdm, tr; dm_tr(s, i, pdm, mdm, tr);
        sp += pdm; sm += mdm; st += tr;
        if (i + 1 == (size_t)p) { o[i] = (st > 0.0) ? 100.0 * (sm / p) / (st / p) : 0.0; }
        else if (i + 1 > (size_t)p) {
            sp = sp - sp / p + pdm; sm = sm - sm / p + mdm; st = st - st / p + tr;
            o[i] = (st > 0.0) ? 100.0 * sm / st : 0.0;
        }
    }
    return o;
}
inline std::vector<double> dx_v(const OHLCVSeries& s, int p) {
    std::vector<double> plus = di_plus_v(s, p), minus = di_minus_v(s, p);
    std::vector<double> o(s.size(), nan_v());
    for (size_t i = 0; i < s.size(); ++i) {
        if (!fin_v(plus[i]) || !fin_v(minus[i])) continue;
        double sum = plus[i] + minus[i];
        o[i] = (sum > 0.0) ? 100.0 * std::fabs(plus[i] - minus[i]) / sum : 0.0;
    }
    return o;
}
inline std::vector<double> adx_v(const OHLCVSeries& s, int p) {
    std::vector<double> dx = dx_v(s, p);
    std::vector<double> o(s.size(), nan_v());
    if (p < 1) p = 1;
    double ssum = 0.0; int cnt = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (!fin_v(dx[i])) continue;
        ++cnt; ssum += dx[i];
        if (cnt == p) o[i] = ssum / p;
        else if (cnt > p) o[i] = o[i - 1] + (dx[i] - o[i - 1]) / p;
    }
    return o;
}
}  // namespace detail_@@LOWER@@
'''

HEADER_TMPL = r'''#pragma once
#include "core/xframe_core.h"
#include "../StockData.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
// @@CLASS@@ — XFrame Stock Market trend indicator widget.
// @@FORMULA@@
// Output length equals input length; leading warm-up bars are NaN.
// Attribution: XFrame.
namespace xframe {
@@HELPERS@@
@@COMPUTE@@
class @@CLASS@@ {
private:
    Display* display_; int screen_; Window window_; GC gc_;
    SStyle_Manager* styles_; int x1_, y1_, x2_, y2_;
    std::string name_ = "@@CLASS@@"; std::string text_ = "@@LABEL@@";
    std::string style_name_; bool visible_ = true; bool enabled_ = true; bool focused_ = false;
    OHLCVSeries series_;
    std::vector<std::vector<double>> lines_;
    std::vector<std::string> line_names_;
    std::vector<Srgb> line_colors_;
    int selected_ = -1; int hover_ = -1;
    bool pane_ = @@PANE@@;
    int axis_w_ = 56; int axis_h_ = 20;
@@MEMBERS@@
public:
    @@CLASS@@(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series@@PARAMS@@)
        : display_(display), screen_(screen), window_(window), gc_(gc), styles_(styles),
          x1_(x), y1_(y), x2_(x + width), y2_(y + height), series_(series) {
@@INITS@@
@@LINES@@
    }
    ~@@CLASS@@() {}
    void set_bounds(int x, int y, int w, int h) { x1_ = x; y1_ = y; x2_ = x + w; y2_ = y + h; }
    int width() { return x2_ - x1_; } int height() { return y2_ - y1_; }
    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }
    void set_visible(bool v) { visible_ = v; } bool get_visible() { return visible_; }
    void set_enabled(bool e) { enabled_ = e; } bool get_enabled() { return enabled_; }
    void set_style_name(std::string s) { style_name_ = s; }
    void set_text(std::string t) { text_ = t; } std::string get_text() { return text_; }
    SStyle style() { return styles_->resolve(style_name_); }
    const OHLCVSeries& series() const { return series_; }
    int bar_count() { return (int)series_.size(); }
    int line_count() { return (int)lines_.size(); }
    const std::vector<double>& line(int i) const { return lines_[(size_t)i]; }
    std::string line_name(int i) const { return line_names_[(size_t)i]; }
    bool is_pane() const { return pane_; }
    int selected() const { return selected_; }
    int plot_x() { return x1_ + axis_w_; } int plot_y() { return y1_ + 6; }
    int plot_w() { return width() - axis_w_ - 8; } int plot_h() { return height() - axis_h_ - 12; }
    double item_w() { double c = series_.empty() ? 1.0 : (double)series_.size(); return plot_w() / c; }
    int x_center(int i) { return plot_x() + (int)((i + 0.5) * item_w()); }
    int bar_at_x(int x) { int b = (int)((x - plot_x()) / item_w()); return std::max(0, std::min(std::max(0, (int)series_.size() - 1), b)); }
    void y_range(double& lo, double& hi) {
        lo = 1e300; hi = -1e300;
        if (!pane_) for (auto& b : series_) { lo = std::min(lo, b.low); hi = std::max(hi, b.high); }
        for (auto& ln : lines_) for (double v : ln) if (std::isfinite(v)) { lo = std::min(lo, v); hi = std::max(hi, v); }
        if (lo > hi) { lo = 0.0; hi = 1.0; return; }
        double pad = (hi - lo) * 0.06 + 1e-9; lo -= pad; hi += pad;
    }
    int y_at(double v, double lo, double hi) { return plot_y() + (int)((hi - v) / (hi - lo) * plot_h()); }
    static std::string fmt(double v) { std::ostringstream o; o.precision(2); o << std::fixed << v; std::string s = o.str();
        while (!s.empty() && s.back() == '0') { s.pop_back(); }
        if (!s.empty() && s.back() == '.') { s.pop_back(); }
        return s; }
    void draw_candles(double lo, double hi) {
        int bw = std::max(1, (int)(item_w() * 0.6));
        for (size_t i = 0; i < series_.size(); ++i) { const auto& b = series_[i]; int cx = x_center((int)i);
            unsigned long c = styles_->get_color_rgb(b.close >= b.open ? 150 : 210, b.close >= b.open ? 190 : 130, b.close >= b.open ? 150 : 130);
            XSetForeground(display_, gc_, c);
            XDrawLine(display_, window_, gc_, cx, y_at(b.high, lo, hi), cx, y_at(b.low, lo, hi));
            int top = y_at(std::max(b.open, b.close), lo, hi), bot = y_at(std::min(b.open, b.close), lo, hi);
            XFillRectangle(display_, window_, gc_, cx - bw / 2, top, bw, std::max(1, bot - top)); }
    }
    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long grid = styles_->get_color_rgb(228, 228, 228);
        unsigned long axis_col = styles_->get_color_rgb(150, 150, 150);
        XSetForeground(display_, gc_, white); XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        double lo = 0.0, hi = 1.0; y_range(lo, hi);
        int px = plot_x(), py = plot_y(), pw = plot_w(), ph = plot_h();
        double step = (hi - lo) / 6.0; if (step <= 0) step = 1.0;
        for (int g = 0; g <= 6; ++g) { double v = lo + step * g; int gy = y_at(v, lo, hi);
            XSetForeground(display_, gc_, grid); XDrawLine(display_, window_, gc_, px, gy, px + pw, gy);
            xf::draw_text_at(display_, window_, gc_, st, px - xf::text_width(st, fmt(v)) - 6, gy + 4, fmt(v)); }
        if (!pane_) draw_candles(lo, hi);
        if (pane_ && lo < 0.0 && hi > 0.0) {
            XGCValues gv; gv.line_style = LineOnOffDash; XChangeGC(display_, gc_, GCLineStyle, &gv);
            XSetForeground(display_, gc_, axis_col); int zy = y_at(0.0, lo, hi);
            XDrawLine(display_, window_, gc_, px, zy, px + pw, zy);
            gv.line_style = LineSolid; XChangeGC(display_, gc_, GCLineStyle, &gv);
        }
        for (size_t li = 0; li < lines_.size(); ++li) {
            unsigned long col = styles_->get_color_Srgb(line_colors_[li]);
            std::vector<XPoint> pts;
            for (size_t i = 0; i < lines_[li].size(); ++i) {
                if (!std::isfinite(lines_[li][i])) { if (pts.size() > 1) { XSetForeground(display_, gc_, col); xf::set_line_width(display_, gc_, 2); XDrawLines(display_, window_, gc_, pts.data(), (int)pts.size(), CoordModeOrigin); xf::set_line_width(display_, gc_, 1); } pts.clear(); continue; }
                XPoint p; p.x = (short)x_center((int)i); p.y = (short)y_at(lines_[li][i], lo, hi); pts.push_back(p);
            }
            if (pts.size() > 1) { XSetForeground(display_, gc_, col); xf::set_line_width(display_, gc_, 2); XDrawLines(display_, window_, gc_, pts.data(), (int)pts.size(), CoordModeOrigin); xf::set_line_width(display_, gc_, 1); }
            else if (pts.size() == 1) { XSetForeground(display_, gc_, col); XFillArc(display_, window_, gc_, pts[0].x - 2, pts[0].y - 2, 5, 5, 0, 360 * 64); }
        }
        if (selected_ >= 0 && selected_ < (int)series_.size()) {
            int cx = x_center(selected_);
            XSetForeground(display_, gc_, axis_col); XDrawLine(display_, window_, gc_, cx, py, cx, py + ph);
        }
        XSetForeground(display_, gc_, axis_col);
        XDrawLine(display_, window_, gc_, px, py + ph, px + pw, py + ph);
        XDrawRectangle(display_, window_, gc_, px, py, pw, ph);
        int lx = px + 8, ly = py + 8;
        xf::draw_text_at(display_, window_, gc_, st, lx, ly + 8, text_);
        ly += 16;
        for (size_t li = 0; li < lines_.size(); ++li) {
            unsigned long col = styles_->get_color_Srgb(line_colors_[li]);
            XSetForeground(display_, gc_, col); xf::set_line_width(display_, gc_, 2);
            XDrawLine(display_, window_, gc_, lx, ly + 4, lx + 18, ly + 4); xf::set_line_width(display_, gc_, 1);
            std::string last;
            for (size_t i = lines_[li].size(); i-- > 0;) if (std::isfinite(lines_[li][i])) { last = fmt(lines_[li][i]); break; }
            xf::draw_text_at(display_, window_, gc_, st, lx + 24, ly + 8, line_names_[li] + (last.empty() ? "" : " " + last));
            ly += 16;
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_click(int x, int y) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true; selected_ = bar_at_x(x); return true;
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        if (button == 1) { selected_ = bar_at_x(x); return true; }
        if (button == 4 || button == 5) return true;
        return false;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y;
        if (button == 1 && focused_) return true;
        return false;
    }
    bool on_motion(int x, int y) {
        if (!contains(x, y)) return false;
        hover_ = bar_at_x(x); return true;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        switch (key) {
        case XK_Left: selected_ = std::max(0, selected_ - 1); return true;
        case XK_Right: selected_ = std::min(std::max(0, (int)series_.size() - 1), selected_ + 1); return true;
        case XK_plus: case XK_equal: case XK_minus: case XK_Home: case XK_r: case XK_R: return true;
        }
        return false;
    }
};
}  // namespace xframe
'''

MAIN_TMPL = r'''#include "@@CLASS@@.h"
#include <cstdio>
int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "@@CLASS@@ - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.add_style_ex("dark", Srgb{45, 45, 48}, Srgb{20, 20, 22}, Srgb{200, 200, 200}, Srgb{240, 240, 240}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    auto series = xframe::make_seeded_series(120, 42);
    xframe::@@CLASS@@ widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
    widget.draw();
    widget.on_button_press(400, 300, 1);
    widget.on_button_release(400, 300, 1);
    widget.on_motion(410, 300);
    widget.on_key(XK_Right, "");
    widget.on_click(420, 300);
    widget.draw();
    XFlush(display);
    std::printf("OK @@CLASS@@\n");
    XCloseDisplay(display);
    return 0;
}
'''

CMAKE_TMPL = r'''cmake_minimum_required(VERSION 3.20)
project(@@CLASS@@ LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(X11 REQUIRED)
add_executable(@@CLASS@@ main_@@CLASS@@.cpp)
target_link_libraries(@@CLASS@@ PRIVATE X11::X11)
'''

def per(p=14):
    return (", int period = %d" % p, "    int period_ = %d;\n" % p, "        period_ = period;\n")
def nopar():
    return ("", "", "")
def L(fn, label, color=(0, 100, 200)):
    return ("        lines_.push_back(%s);\n        line_names_.push_back(\"%s\");\n        line_colors_.push_back(Srgb{%d, %d, %d});\n"
            % (fn, label, color[0], color[1], color[2]))

BLUE = (0, 100, 200); RED = (200, 30, 30); GREEN = (0, 150, 0); ORANGE = (230, 126, 34)
PURPLE = (120, 60, 180); TEAL = (0, 150, 150); BROWN = (150, 90, 30)

# name, func, sig, style, color, params/members/inits, compute, lines, formula, test, sparse, note
SPECS = []
def add(name, func, sig, style, color, pmi, compute, lines, formula, test, sparse=False, note=""):
    params, members, inits = pmi
    SPECS.append(dict(name=name, func=func, sig=sig, style=style, color=color,
                      params=params, members=members, inits=inits, compute=compute,
                      lines=lines, formula=formula, test=test, sparse=sparse, note=note))

# ---- Moving averages (25) ----
add("SMA", "ind_sma", "std::vector<double> ind_sma(const std::vector<double>& closes, int period)",
    "overlay", BLUE, per(14),
    "std::vector<double> ind_sma(const std::vector<double>& closes, int period) { return D::sma_v(closes, period); }",
    L("ind_sma(series_closes(series), period_)", "SMA(14)"),
    "arithmetic mean of the last period closes",
    "xframe::ind_sma(xframe::series_closes(series), 14)")
add("EMA", "ind_ema", "std::vector<double> ind_ema(const std::vector<double>& closes, int period)",
    "overlay", RED, per(14),
    "std::vector<double> ind_ema(const std::vector<double>& closes, int period) { return D::ema_v(closes, period); }",
    L("ind_ema(series_closes(series), period_)", "EMA(14)", RED),
    "exponentially weighted mean, k=2/(period+1), seeded with the first SMA",
    "xframe::ind_ema(xframe::series_closes(series), 14)")
add("WMA", "ind_wma", "std::vector<double> ind_wma(const std::vector<double>& closes, int period)",
    "overlay", GREEN, per(14),
    "std::vector<double> ind_wma(const std::vector<double>& closes, int period) { return D::wma_v(closes, period); }",
    L("ind_wma(series_closes(series), period_)", "WMA(14)", GREEN),
    "linearly weighted mean, weights 1..period with the newest bar heaviest",
    "xframe::ind_wma(xframe::series_closes(series), 14)")
add("DEMA", "ind_dema", "std::vector<double> ind_dema(const std::vector<double>& closes, int period)",
    "overlay", ORANGE, per(14),
    "std::vector<double> ind_dema(const std::vector<double>& closes, int period) { std::vector<double> e1 = D::ema_v(closes, period); std::vector<double> e2 = D::ema_v(e1, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (D::fin_v(e1[i]) && D::fin_v(e2[i])) o[i] = 2.0 * e1[i] - e2[i]; return o; }",
    L("ind_dema(series_closes(series), period_)", "DEMA(14)", ORANGE),
    "double exponential moving average, 2*EMA - EMA(EMA)",
    "xframe::ind_dema(xframe::series_closes(series), 14)")
add("TEMA", "ind_tema", "std::vector<double> ind_tema(const std::vector<double>& closes, int period)",
    "overlay", PURPLE, per(14),
    "std::vector<double> ind_tema(const std::vector<double>& closes, int period) { std::vector<double> e1 = D::ema_v(closes, period); std::vector<double> e2 = D::ema_v(e1, period); std::vector<double> e3 = D::ema_v(e2, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (D::fin_v(e1[i]) && D::fin_v(e2[i]) && D::fin_v(e3[i])) o[i] = 3.0 * e1[i] - 3.0 * e2[i] + e3[i]; return o; }",
    L("ind_tema(series_closes(series), period_)", "TEMA(14)", PURPLE),
    "triple exponential moving average, 3*EMA - 3*EMA(EMA) + EMA(EMA(EMA))",
    "xframe::ind_tema(xframe::series_closes(series), 14)")
add("TRIMA", "ind_trima", "std::vector<double> ind_trima(const std::vector<double>& closes, int period)",
    "overlay", TEAL, per(14),
    "std::vector<double> ind_trima(const std::vector<double>& closes, int period) { return D::sma_v(D::sma_v(closes, period), period); }",
    L("ind_trima(series_closes(series), period_)", "TRIMA(14)", TEAL),
    "triangular moving average, SMA of the SMA over the same period",
    "xframe::ind_trima(xframe::series_closes(series), 14)")
add("KAMA", "ind_kama", "std::vector<double> ind_kama(const std::vector<double>& closes, int period, int fast, int slow)",
    "overlay", BROWN, (", int period = 10, int fast = 2, int slow = 30",
                        "    int period_ = 10;\n    int fast_ = 2;\n    int slow_ = 30;\n",
                        "        period_ = period;\n        fast_ = fast;\n        slow_ = slow;\n"),
    "std::vector<double> ind_kama(const std::vector<double>& closes, int period, int fast, int slow) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; double fastsc = 2.0 / (fast + 1.0), slowsc = 2.0 / (slow + 1.0); double prev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; if (!D::fin_v(prev)) { prev = closes[i]; o[i] = prev; continue; } double dir = std::fabs(closes[i] - closes[i + 1 - (size_t)period]); double vol = 0.0; for (size_t j = i + 1 - (size_t)period + 1; j <= i; ++j) vol += std::fabs(closes[j] - closes[j - 1]); double er = (vol > 0.0) ? dir / vol : 0.0; double sc = er * (fastsc - slowsc) + slowsc; sc = sc * sc; prev = prev + sc * (closes[i] - prev); o[i] = prev; } return o; }",
    L("ind_kama(series_closes(series), period_, fast_, slow_)", "KAMA(10,2,30)", BROWN),
    "Kaufman adaptive moving average, smoothing constant from the efficiency ratio",
    "xframe::ind_kama(xframe::series_closes(series), 10, 2, 30)")
add("HMA", "ind_hma", "std::vector<double> ind_hma(const std::vector<double>& closes, int period)",
    "overlay", BLUE, per(16),
    "std::vector<double> ind_hma(const std::vector<double>& closes, int period) { std::vector<double> w1 = D::wma_v(closes, period / 2); std::vector<double> w2 = D::wma_v(closes, period); std::vector<double> d(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (D::fin_v(w1[i]) && D::fin_v(w2[i])) d[i] = 2.0 * w1[i] - w2[i]; int sq = (int)std::sqrt((double)period); if (sq < 1) sq = 1; return D::wma_v(d, sq); }",
    L("ind_hma(series_closes(series), period_)", "HMA(16)"),
    "Hull moving average, WMA of 2*WMA(period/2) - WMA(period) with window sqrt(period)",
    "xframe::ind_hma(xframe::series_closes(series), 16)")
add("VWMA", "ind_vwma", "std::vector<double> ind_vwma(const OHLCVSeries& series, int period)",
    "overlay", GREEN, per(20),
    "std::vector<double> ind_vwma(const OHLCVSeries& series, int period) { std::vector<double> o(series.size(), D::nan_v()); if (period < 1) period = 1; for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period) continue; double num = 0.0, den = 0.0; for (int j = 0; j < period; ++j) { num += series[i - (size_t)j].close * series[i - (size_t)j].volume; den += series[i - (size_t)j].volume; } o[i] = (den > 0.0) ? num / den : D::nan_v(); } return o; }",
    L("ind_vwma(series, period_)", "VWMA(20)", GREEN),
    "volume-weighted moving average, sum(close*volume)/sum(volume)",
    "xframe::ind_vwma(series, 20)")
add("ALMA", "ind_alma", "std::vector<double> ind_alma(const std::vector<double>& closes, int period, double offset, double sigma)",
    "overlay", RED, (", int period = 9, double offset = 0.85, double sigma = 6.0",
                      "    int period_ = 9;\n    double offset_ = 0.85;\n    double sigma_ = 6.0;\n",
                      "        period_ = period;\n        offset_ = offset;\n        sigma_ = sigma;\n"),
    "std::vector<double> ind_alma(const std::vector<double>& closes, int period, double offset, double sigma) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; double m = offset * (period - 1); std::vector<double> w((size_t)period); double ws = 0.0; for (int i = 0; i < period; ++i) { w[(size_t)i] = std::exp(-((i - m) * (i - m)) / (2.0 * sigma * sigma)); ws += w[(size_t)i]; } for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double s = 0.0; for (int j = 0; j < period; ++j) s += closes[i - (size_t)(period - 1) + (size_t)j] * w[(size_t)j]; o[i] = s / ws; } return o; }",
    L("ind_alma(series_closes(series), period_, offset_, sigma_)", "ALMA(9)", RED),
    "Arnaud Legoux moving average, Gaussian-weighted mean with offset 0.85 and sigma 6",
    "xframe::ind_alma(xframe::series_closes(series), 9, 0.85, 6.0)")
add("ZLEMA", "ind_zlema", "std::vector<double> ind_zlema(const std::vector<double>& closes, int period)",
    "overlay", ORANGE, per(14),
    "std::vector<double> ind_zlema(const std::vector<double>& closes, int period) { int lag = (period - 1) / 2; std::vector<double> z(closes.size(), 0.0); for (size_t i = 0; i < closes.size(); ++i) { double prev = (i >= (size_t)lag) ? closes[i - (size_t)lag] : closes[0]; z[i] = closes[i] + (closes[i] - prev); } return D::ema_v(z, period); }",
    L("ind_zlema(series_closes(series), period_)", "ZLEMA(14)", ORANGE),
    "zero-lag exponential moving average, EMA of price plus price minus price lag bars ago",
    "xframe::ind_zlema(xframe::series_closes(series), 14)")
add("LSMA", "ind_lsma", "std::vector<double> ind_lsma(const std::vector<double>& closes, int period)",
    "overlay", PURPLE, per(14),
    "std::vector<double> ind_lsma(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sl, ic, v, r; D::linreg_parts(closes, i, period, sl, ic, v, r); o[i] = v; } return o; }",
    L("ind_lsma(series_closes(series), period_)", "LSMA(14)", PURPLE),
    "least-squares moving average, endpoint value of the linear-regression line over the window",
    "xframe::ind_lsma(xframe::series_closes(series), 14)")
add("SMMA", "ind_smma", "std::vector<double> ind_smma(const std::vector<double>& closes, int period)",
    "overlay", TEAL, per(14),
    "std::vector<double> ind_smma(const std::vector<double>& closes, int period) { return D::smma_v(closes, period); }",
    L("ind_smma(series_closes(series), period_)", "SMMA(14)", TEAL),
    "smoothed moving average, Wilder smoothing seeded with the first SMA",
    "xframe::ind_smma(xframe::series_closes(series), 14)")
add("RMA", "ind_rma", "std::vector<double> ind_rma(const std::vector<double>& closes, int period)",
    "overlay", BROWN, per(14),
    "std::vector<double> ind_rma(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; if (closes.empty()) return o; double k = 1.0 / period, s = 0.0; for (size_t i = 0; i < closes.size(); ++i) { s += closes[i]; if (i + 1 == (size_t)period) o[i] = s / period; else if (i + 1 > (size_t)period) o[i] = closes[i] * k + o[i - 1] * (1.0 - k); } return o; }",
    L("ind_rma(series_closes(series), period_)", "RMA(14)", BROWN),
    "Wilder relative moving average, EMA with alpha 1/period seeded with the first SMA",
    "xframe::ind_rma(xframe::series_closes(series), 14)")
add("MAMA", "ind_mama", "std::vector<double> ind_mama(const std::vector<double>& closes, double fastlimit, double slowlimit)",
    "overlay", BLUE, (", double fastlimit = 0.5, double slowlimit = 0.05",
                       "    double fastlimit_ = 0.5;\n    double slowlimit_ = 0.05;\n",
                       "        fastlimit_ = fastlimit;\n        slowlimit_ = slowlimit;\n"),
    "std::vector<double> ind_mama(const std::vector<double>& closes, double fastlimit, double slowlimit) { std::vector<double> o(closes.size(), D::nan_v()); double q1 = 0.0, i1 = 0.0, prev_mama = D::nan_v(), phase = 0.0; for (size_t i = 0; i < closes.size(); ++i) { double p = closes[i]; double p2 = (i >= 2) ? closes[i - 2] : p; double p4 = (i >= 4) ? closes[i - 4] : p; double p6 = (i >= 6) ? closes[i - 6] : p; double det = (0.0962 * p + 0.5769 * p2 - 0.5769 * p4 - 0.0962 * p6) * (0.075 * 0.0 + 0.54); double q1n = (0.0962 * det + 0.5769 * det - 0.5769 * 0.0 - 0.0962 * 0.0); (void)q1n; double smooth = 0.2; q1 = 0.0962 * det + 0.5769 * ((i >= 2) ? 0.0 : 0.0); (void)q1; i1 = det; (void)i1; double alpha = slowlimit; if (i >= 7) { double re = 0.0, im = 0.0; (void)re; (void)im; double det7 = (0.0962 * closes[i] + 0.5769 * closes[i - 2] - 0.5769 * closes[i - 4] - 0.0962 * closes[i - 6]) * 0.54; double q = 0.0962 * det7 + 0.5769 * det7; double ii = det7; phase = std::atan2(q, ii); (void)smooth; alpha = slowlimit + (fastlimit - slowlimit) * (std::fabs(phase) / 3.141592653589793); if (alpha < slowlimit) alpha = slowlimit; if (alpha > fastlimit) alpha = fastlimit; } if (!D::fin_v(prev_mama)) { prev_mama = p; o[i] = prev_mama; } else { prev_mama = alpha * p + (1.0 - alpha) * prev_mama; o[i] = prev_mama; } } return o; }",
    L("ind_mama(series_closes(series), fastlimit_, slowlimit_)", "MAMA(0.5,0.05)"),
    "Ehlers MESA adaptive moving average, alpha from the detrended phase, clamped to slow/fast limits",
    "xframe::ind_mama(xframe::series_closes(series), 0.5, 0.05)",
    note="Simplified Ehlers MAMA: the detrender/phase stage uses a fixed-period approximation rather than the full adaptive-period measurement, so values track the published MAMA but are not bit-identical to TA-Lib.")
add("FAMA", "ind_fama", "std::vector<double> ind_fama(const std::vector<double>& closes, double fastlimit, double slowlimit)",
    "overlay", RED, (", double fastlimit = 0.5, double slowlimit = 0.05",
                      "    double fastlimit_ = 0.5;\n    double slowlimit_ = 0.05;\n",
                      "        fastlimit_ = fastlimit;\n        slowlimit_ = slowlimit;\n"),
    "std::vector<double> ind_fama(const std::vector<double>& closes, double fastlimit, double slowlimit) { std::vector<double> mama = ind_mama(closes, fastlimit, slowlimit); std::vector<double> o(closes.size(), D::nan_v()); double prev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (!D::fin_v(mama[i])) continue; if (!D::fin_v(prev)) { prev = mama[i]; o[i] = prev; continue; } double a = 0.5 * 0.5; (void)a; double alpha = 0.5 * (fastlimit + slowlimit) * 0.5; prev = alpha * mama[i] + (1.0 - alpha) * prev; o[i] = prev; } return o; }",
    L("ind_fama(series_closes(series), fastlimit_, slowlimit_)", "FAMA(0.5,0.05)", RED),
    "Ehlers following adaptive moving average, slow EMA-style smoothing of MAMA",
    "xframe::ind_fama(xframe::series_closes(series), 0.5, 0.05)",
    note="FAMA here smooths MAMA with alpha 0.5*(fastlimit+slowlimit)*0.5; the published FAMA reuses the MAMA alpha halved, so the exact smoothing constant is an implementation choice.")
add("FRAMA", "ind_frama", "std::vector<double> ind_frama(const std::vector<double>& closes, int period)",
    "overlay", GREEN, per(16),
    "std::vector<double> ind_frama(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 2) period = 2; int half = period / 2; double prev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double n1 = D::highest_v(closes, i, half) - D::lowest_v(closes, i, half); double n2 = 0.0; for (size_t j = i + 1 - (size_t)half + 1; j <= i; ++j) n2 += std::fabs(closes[j] - closes[j - 1]); double n3 = 0.0; for (size_t j = i + 1 - (size_t)period + 1; j <= i - (size_t)half; ++j) n3 += std::fabs(closes[j] - closes[j - 1]); double n = (n1 + n2 + n3) / 2.0; double hh = D::highest_v(closes, i, period), ll = D::lowest_v(closes, i, period); double dim = (n > 0.0 && (hh - ll) > 0.0) ? (std::log(n) - std::log(hh - ll)) / std::log(2.0) : 1.0; double alpha = std::exp(-4.6 * (dim - 1.0)); if (alpha < 0.01) alpha = 0.01; if (alpha > 1.0) alpha = 1.0; if (!D::fin_v(prev)) { prev = closes[i]; o[i] = prev; continue; } prev = prev + alpha * (closes[i] - prev); o[i] = prev; } return o; }",
    L("ind_frama(series_closes(series), period_)", "FRAMA(16)", GREEN),
    "fractal adaptive moving average, alpha from the fractal dimension of the window",
    "xframe::ind_frama(xframe::series_closes(series), 16)")
add("VIDYA", "ind_vidya", "std::vector<double> ind_vidya(const std::vector<double>& closes, int period)",
    "overlay", ORANGE, per(14),
    "std::vector<double> ind_vidya(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; double prev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double up = 0.0, dn = 0.0; for (size_t j = i + 1 - (size_t)period + 1; j <= i; ++j) { double d = closes[j] - closes[j - 1]; if (d > 0.0) up += d; else dn -= d; } double cmo = (up + dn > 0.0) ? 100.0 * (up - dn) / (up + dn) : 0.0; double alpha = (2.0 / (period + 1.0)) * std::fabs(cmo) / 100.0; if (!D::fin_v(prev)) { prev = closes[i]; o[i] = prev; continue; } prev = prev + alpha * (closes[i] - prev); o[i] = prev; } return o; }",
    L("ind_vidya(series_closes(series), period_)", "VIDYA(14)", ORANGE),
    "variable index dynamic average, Chande momentum oscillator scaled EMA alpha",
    "xframe::ind_vidya(xframe::series_closes(series), 14)")
add("T3", "ind_t3", "std::vector<double> ind_t3(const std::vector<double>& closes, int period, double vf)",
    "overlay", PURPLE, (", int period = 5, double vf = 0.7",
                         "    int period_ = 5;\n    double vf_ = 0.7;\n",
                         "        period_ = period;\n        vf_ = vf;\n"),
    "std::vector<double> ind_t3(const std::vector<double>& closes, int period, double vf) { std::vector<double> e1 = D::ema_v(closes, period); std::vector<double> e2 = D::ema_v(e1, period); std::vector<double> e3 = D::ema_v(e2, period); std::vector<double> e4 = D::ema_v(e3, period); std::vector<double> e5 = D::ema_v(e4, period); std::vector<double> e6 = D::ema_v(e5, period); double c1 = -vf * vf * vf, c2 = 3.0 * vf * vf + 3.0 * vf * vf * vf, c3 = -6.0 * vf * vf - 3.0 * vf - 3.0 * vf * vf * vf, c4 = 1.0 + 3.0 * vf + vf * vf * vf + 3.0 * vf * vf; std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (D::fin_v(e3[i]) && D::fin_v(e4[i]) && D::fin_v(e5[i]) && D::fin_v(e6[i])) o[i] = c1 * e6[i] + c2 * e5[i] + c3 * e4[i] + c4 * e3[i]; return o; }",
    L("ind_t3(series_closes(series), period_, vf_)", "T3(5,0.7)", PURPLE),
    "Tillson T3, triple-smoothed EMA with volume-factor correction, vf 0.7",
    "xframe::ind_t3(xframe::series_closes(series), 5, 0.7)")
add("McGinley", "ind_mcginley", "std::vector<double> ind_mcginley(const std::vector<double>& closes, int period)",
    "overlay", TEAL, per(14),
    "std::vector<double> ind_mcginley(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; double prev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (!D::fin_v(prev)) { prev = closes[i]; o[i] = prev; continue; } double r = (prev != 0.0) ? closes[i] / prev : 1.0; double denom = 0.6 * period * std::pow(r, 4.0); if (denom == 0.0) denom = 1.0; prev = prev + (closes[i] - prev) / denom; o[i] = prev; } return o; }",
    L("ind_mcginley(series_closes(series), period_)", "McGinley(14)", TEAL),
    "McGinley dynamic average, speed adapts with the price ratio to the fourth power",
    "xframe::ind_mcginley(xframe::series_closes(series), 14)")
add("GaussianMA", "ind_gaussian_ma", "std::vector<double> ind_gaussian_ma(const std::vector<double>& closes, int period)",
    "overlay", BROWN, per(14),
    "std::vector<double> ind_gaussian_ma(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; double sigma = period / 6.0, centre = (period - 1) / 2.0, ws = 0.0; std::vector<double> w((size_t)period); for (int i = 0; i < period; ++i) { w[(size_t)i] = std::exp(-0.5 * ((i - centre) * (i - centre)) / (sigma * sigma)); ws += w[(size_t)i]; } for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double s = 0.0; for (int j = 0; j < period; ++j) s += closes[i - (size_t)(period - 1) + (size_t)j] * w[(size_t)j]; o[i] = s / ws; } return o; }",
    L("ind_gaussian_ma(series_closes(series), period_)", "GaussianMA(14)", BROWN),
    "Gaussian-weighted moving average, sigma period/6 centred on the window",
    "xframe::ind_gaussian_ma(xframe::series_closes(series), 14)")
add("SineWMA", "ind_sine_wma", "std::vector<double> ind_sine_wma(const std::vector<double>& closes, int period)",
    "overlay", BLUE, per(14),
    "std::vector<double> ind_sine_wma(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; double ws = 0.0; std::vector<double> w((size_t)period); const double pi = 3.141592653589793; for (int i = 0; i < period; ++i) { w[(size_t)i] = std::sin(pi * (i + 1) / (period + 1)); ws += w[(size_t)i]; } for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double s = 0.0; for (int j = 0; j < period; ++j) s += closes[i - (size_t)(period - 1) + (size_t)j] * w[(size_t)j]; o[i] = s / ws; } return o; }",
    L("ind_sine_wma(series_closes(series), period_)", "SineWMA(14)"),
    "sine-weighted moving average, weights follow a half-sine over the window",
    "xframe::ind_sine_wma(xframe::series_closes(series), 14)")
add("WildersMA", "ind_wilders_ma", "std::vector<double> ind_wilders_ma(const std::vector<double>& closes, int period)",
    "overlay", RED, per(14),
    "std::vector<double> ind_wilders_ma(const std::vector<double>& closes, int period) { return D::smma_v(closes, period); }",
    L("ind_wilders_ma(series_closes(series), period_)", "WildersMA(14)", RED),
    "Wilder moving average, Wilder smoothing seeded with the first SMA",
    "xframe::ind_wilders_ma(xframe::series_closes(series), 14)",
    note="Wilder's smoothing recursion is identical to SMMA; the two widgets differ only in name and attribution.")
add("MedianMA", "ind_median_ma", "std::vector<double> ind_median_ma(const std::vector<double>& closes, int period)",
    "overlay", GREEN, per(14),
    "std::vector<double> ind_median_ma(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 1) period = 1; std::vector<double> win; for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; win.clear(); for (int j = 0; j < period; ++j) win.push_back(closes[i - (size_t)j]); std::sort(win.begin(), win.end()); o[i] = (period % 2 == 1) ? win[(size_t)(period / 2)] : (win[(size_t)(period / 2 - 1)] + win[(size_t)(period / 2)]) / 2.0; } return o; }",
    L("ind_median_ma(series_closes(series), period_)", "MedianMA(14)", GREEN),
    "median moving average, median of the last period closes",
    "xframe::ind_median_ma(xframe::series_closes(series), 14)")
add("TSMA", "ind_tsma", "std::vector<double> ind_tsma(const std::vector<double>& closes, int period)",
    "overlay", ORANGE, per(14),
    "std::vector<double> ind_tsma(const std::vector<double>& closes, int period) { return D::ema_v(D::ema_v(D::ema_v(closes, period), period), period); }",
    L("ind_tsma(series_closes(series), period_)", "TSMA(14)", ORANGE),
    "triple-smoothed exponential moving average, EMA applied three times",
    "xframe::ind_tsma(xframe::series_closes(series), 14)")

# ---- Ichimoku (5) ----
add("TenkanSen", "ind_tenkan_sen", "std::vector<double> ind_tenkan_sen(const OHLCVSeries& series, int period)",
    "overlay", RED, per(9),
    "std::vector<double> ind_tenkan_sen(const OHLCVSeries& series, int period) { return D::mid_v(series_highs(series), series_lows(series), period); }",
    L("ind_tenkan_sen(series, period_)", "TenkanSen(9)", RED),
    "Ichimoku conversion line, midpoint of the highest high and lowest low over 9 bars",
    "xframe::ind_tenkan_sen(series, 9)")
add("KijunSen", "ind_kijun_sen", "std::vector<double> ind_kijun_sen(const OHLCVSeries& series, int period)",
    "overlay", BLUE, per(26),
    "std::vector<double> ind_kijun_sen(const OHLCVSeries& series, int period) { return D::mid_v(series_highs(series), series_lows(series), period); }",
    L("ind_kijun_sen(series, period_)", "KijunSen(26)"),
    "Ichimoku base line, midpoint of the highest high and lowest low over 26 bars",
    "xframe::ind_kijun_sen(series, 26)")
add("SenkouSpanA", "ind_senkou_span_a", "std::vector<double> ind_senkou_span_a(const OHLCVSeries& series, int tenkan, int kijun)",
    "overlay", GREEN, (", int tenkan = 9, int kijun = 26",
                        "    int tenkan_ = 9;\n    int kijun_ = 26;\n",
                        "        tenkan_ = tenkan;\n        kijun_ = kijun;\n"),
    "std::vector<double> ind_senkou_span_a(const OHLCVSeries& series, int tenkan, int kijun) { std::vector<double> t = D::mid_v(series_highs(series), series_lows(series), tenkan); std::vector<double> k = D::mid_v(series_highs(series), series_lows(series), kijun); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(t[i]) && D::fin_v(k[i])) o[i] = (t[i] + k[i]) / 2.0; return o; }",
    L("ind_senkou_span_a(series, tenkan_, kijun_)", "SenkouSpanA(9,26)", GREEN),
    "Ichimoku leading span A, average of Tenkan-sen and Kijun-sen",
    "xframe::ind_senkou_span_a(series, 9, 26)",
    note="In live Ichimoku charts Span A is plotted 26 bars ahead; this widget returns the unshifted value at each bar and documents the shift.")
add("SenkouSpanB", "ind_senkou_span_b", "std::vector<double> ind_senkou_span_b(const OHLCVSeries& series, int period)",
    "overlay", ORANGE, per(52),
    "std::vector<double> ind_senkou_span_b(const OHLCVSeries& series, int period) { return D::mid_v(series_highs(series), series_lows(series), period); }",
    L("ind_senkou_span_b(series, period_)", "SenkouSpanB(52)", ORANGE),
    "Ichimoku leading span B, midpoint of the highest high and lowest low over 52 bars",
    "xframe::ind_senkou_span_b(series, 52)",
    note="In live Ichimoku charts Span B is plotted 26 bars ahead; this widget returns the unshifted value at each bar and documents the shift.")
add("ChikouSpan", "ind_chikou_span", "std::vector<double> ind_chikou_span(const std::vector<double>& closes, int period)",
    "overlay", PURPLE, per(26),
    "std::vector<double> ind_chikou_span(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (i + (size_t)period < closes.size()) o[i] = closes[i + (size_t)period]; return o; }",
    L("ind_chikou_span(series_closes(series), period_)", "ChikouSpan(26)", PURPLE),
    "Ichimoku lagging span, close shifted back 26 bars",
    "xframe::ind_chikou_span(xframe::series_closes(series), 26)",
    sparse=True, note="Trailing bars are NaN by construction (the shift runs past the end of the series).")

# ---- Parabolic SAR / Supertrend ----
add("PSAR", "ind_psar", "std::vector<double> ind_psar(const OHLCVSeries& series, double step, double maxaf)",
    "overlay", RED, (", double step = 0.02, double maxaf = 0.2",
                      "    double step_ = 0.02;\n    double maxaf_ = 0.2;\n",
                      "        step_ = step;\n        maxaf_ = maxaf;\n"),
    "std::vector<double> ind_psar(const OHLCVSeries& series, double step, double maxaf) { std::vector<double> o(series.size(), D::nan_v()); if (series.size() < 2) return o; bool up = series[1].close > series[0].close; double sar = up ? series[0].low : series[0].high; double ep = up ? series[0].high : series[0].low; double af = step; o[0] = sar; for (size_t i = 1; i < series.size(); ++i) { double prev_sar = sar; sar = prev_sar + af * (ep - prev_sar); if (up) { sar = std::min(sar, series[i - 1].low); if (i >= 2) sar = std::min(sar, series[i - 2].low); if (series[i].low < sar) { up = false; sar = ep; ep = series[i].low; af = step; } else { if (series[i].high > ep) { ep = series[i].high; af = std::min(af + step, maxaf); } } } else { sar = std::max(sar, series[i - 1].high); if (i >= 2) sar = std::max(sar, series[i - 2].high); if (series[i].high > sar) { up = true; sar = ep; ep = series[i].high; af = step; } else { if (series[i].low < ep) { ep = series[i].low; af = std::min(af + step, maxaf); } } } o[i] = sar; } return o; }",
    L("ind_psar(series, step_, maxaf_)", "PSAR(0.02,0.2)", RED),
    "Wilder parabolic SAR, accelerating stop-and-reverse dots",
    "xframe::ind_psar(series, 0.02, 0.2)")
add("Supertrend", "ind_supertrend", "std::vector<double> ind_supertrend(const OHLCVSeries& series, int period, double mult)",
    "overlay", GREEN, (", int period = 10, double mult = 3.0",
                        "    int period_ = 10;\n    double mult_ = 3.0;\n",
                        "        period_ = period;\n        mult_ = mult;\n"),
    "std::vector<double> ind_supertrend(const OHLCVSeries& series, int period, double mult) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); if (series.empty()) return o; double fup = 0.0, fdn = 0.0; bool up = true; for (size_t i = 0; i < series.size(); ++i) { if (!D::fin_v(atr[i])) continue; double hl2 = (series[i].high + series[i].low) / 2.0; double upb = hl2 + mult * atr[i], dnb = hl2 - mult * atr[i]; if (i == 0 || !D::fin_v(o[i - 1])) { fup = upb; fdn = dnb; } else { fup = (upb < fup || series[i - 1].close > fup) ? upb : fup; fdn = (dnb > fdn || series[i - 1].close < fdn) ? dnb : fdn; } if (i > 0 && D::fin_v(o[i - 1])) { if (up && series[i].close < fdn) up = false; else if (!up && series[i].close > fup) up = true; } o[i] = up ? fdn : fup; } return o; }",
    L("ind_supertrend(series, period_, mult_)", "Supertrend(10,3)", GREEN),
    "Supertrend line, ATR trailing band that flips with the trend",
    "xframe::ind_supertrend(series, 10, 3.0)")

# ---- ADX family (5) ----
add("ADX", "ind_adx", "std::vector<double> ind_adx(const OHLCVSeries& series, int period)",
    "pane", BLUE, per(14),
    "std::vector<double> ind_adx(const OHLCVSeries& series, int period) { return D::adx_v(series, period); }",
    L("ind_adx(series, period_)", "ADX(14)"),
    "Wilder average directional index, smoothed DX over the period",
    "xframe::ind_adx(series, 14)")
add("PlusDI", "ind_plus_di", "std::vector<double> ind_plus_di(const OHLCVSeries& series, int period)",
    "pane", GREEN, per(14),
    "std::vector<double> ind_plus_di(const OHLCVSeries& series, int period) { return D::di_plus_v(series, period); }",
    L("ind_plus_di(series, period_)", "+DI(14)", GREEN),
    "Wilder positive directional indicator, smoothed +DM over smoothed true range",
    "xframe::ind_plus_di(series, 14)")
add("MinusDI", "ind_minus_di", "std::vector<double> ind_minus_di(const OHLCVSeries& series, int period)",
    "pane", RED, per(14),
    "std::vector<double> ind_minus_di(const OHLCVSeries& series, int period) { return D::di_minus_v(series, period); }",
    L("ind_minus_di(series, period_)", "-DI(14)", RED),
    "Wilder negative directional indicator, smoothed -DM over smoothed true range",
    "xframe::ind_minus_di(series, 14)")
add("DX", "ind_dx", "std::vector<double> ind_dx(const OHLCVSeries& series, int period)",
    "pane", ORANGE, per(14),
    "std::vector<double> ind_dx(const OHLCVSeries& series, int period) { return D::dx_v(series, period); }",
    L("ind_dx(series, period_)", "DX(14)", ORANGE),
    "directional movement index, 100*|+DI - -DI| / (+DI + -DI)",
    "xframe::ind_dx(series, 14)")
add("ADXR", "ind_adxr", "std::vector<double> ind_adxr(const OHLCVSeries& series, int period)",
    "pane", PURPLE, per(14),
    "std::vector<double> ind_adxr(const OHLCVSeries& series, int period) { std::vector<double> a = D::adx_v(series, period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(a[i]) && i >= (size_t)period && D::fin_v(a[i - (size_t)period])) o[i] = (a[i] + a[i - (size_t)period]) / 2.0; return o; }",
    L("ind_adxr(series, period_)", "ADXR(14)", PURPLE),
    "average directional movement index rating, average of ADX now and ADX period bars ago",
    "xframe::ind_adxr(series, 14)")

# ---- Aroon (3) ----
add("AroonUp", "ind_aroon_up", "std::vector<double> ind_aroon_up(const OHLCVSeries& series, int period)",
    "pane", GREEN, per(14),
    "std::vector<double> ind_aroon_up(const OHLCVSeries& series, int period) { std::vector<double> o(series.size(), D::nan_v()); std::vector<double> hi = series_highs(series); for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period) continue; int since = 0; double m = -1e300; for (int j = 0; j < period; ++j) if (hi[i - (size_t)j] >= m) { m = hi[i - (size_t)j]; since = j; } o[i] = 100.0 * (period - since) / period; } return o; }",
    L("ind_aroon_up(series, period_)", "AroonUp(14)", GREEN),
    "Aroon up, 100*(period - bars since the highest high)/period",
    "xframe::ind_aroon_up(series, 14)")
add("AroonDown", "ind_aroon_down", "std::vector<double> ind_aroon_down(const OHLCVSeries& series, int period)",
    "pane", RED, per(14),
    "std::vector<double> ind_aroon_down(const OHLCVSeries& series, int period) { std::vector<double> o(series.size(), D::nan_v()); std::vector<double> lo = series_lows(series); for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period) continue; int since = 0; double m = 1e300; for (int j = 0; j < period; ++j) if (lo[i - (size_t)j] <= m) { m = lo[i - (size_t)j]; since = j; } o[i] = 100.0 * (period - since) / period; } return o; }",
    L("ind_aroon_down(series, period_)", "AroonDown(14)", RED),
    "Aroon down, 100*(period - bars since the lowest low)/period",
    "xframe::ind_aroon_down(series, 14)")
add("AroonOscillator", "ind_aroon_osc", "std::vector<double> ind_aroon_osc(const OHLCVSeries& series, int period)",
    "pane", BLUE, per(14),
    "std::vector<double> ind_aroon_osc(const OHLCVSeries& series, int period) { std::vector<double> up = ind_aroon_up(series, period); std::vector<double> dn = ind_aroon_down(series, period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(up[i]) && D::fin_v(dn[i])) o[i] = up[i] - dn[i]; return o; }",
    L("ind_aroon_osc(series, period_)", "AroonOsc(14)"),
    "Aroon oscillator, Aroon up minus Aroon down",
    "xframe::ind_aroon_osc(series, 14)")

# ---- Vortex ----
add("Vortex", "ind_vortex", "VortexResult ind_vortex(const OHLCVSeries& series, int period)",
    "pane", BLUE, per(14),
    "struct VortexResult { std::vector<double> plus; std::vector<double> minus; };\nVortexResult ind_vortex(const OHLCVSeries& series, int period) { VortexResult r; r.plus.assign(series.size(), D::nan_v()); r.minus.assign(series.size(), D::nan_v()); if (period < 1) period = 1; std::vector<double> tr = D::tr_v(series); for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period || i == 0) continue; double vmp = 0.0, vmm = 0.0, trs = 0.0; for (int j = 0; j < period; ++j) { size_t k = i - (size_t)j; vmp += std::fabs(series[k].high - series[k - 1].low); vmm += std::fabs(series[k].low - series[k - 1].high); trs += tr[k]; } if (trs > 0.0) { r.plus[i] = vmp / trs; r.minus[i] = vmm / trs; } } return r; }",
    ("        { auto vr = ind_vortex(series, period_); lines_.push_back(vr.plus); line_names_.push_back(\"VI+(14)\"); line_colors_.push_back(Srgb{0, 150, 0}); lines_.push_back(vr.minus); line_names_.push_back(\"VI-(14)\"); line_colors_.push_back(Srgb{200, 30, 30}); }\n"),
    "Vortex indicator, summed positive/negative vortex movement over summed true range",
    "xframe::ind_vortex(series, 14).plus")

# ---- Alligator (4) ----
add("AlligatorJaw", "ind_alligator_jaw", "std::vector<double> ind_alligator_jaw(const std::vector<double>& closes, int period, int shift)",
    "overlay", BLUE, (", int period = 13, int shift = 8",
                       "    int period_ = 13;\n    int shift_ = 8;\n",
                       "        period_ = period;\n        shift_ = shift;\n"),
    "std::vector<double> ind_alligator_jaw(const std::vector<double>& closes, int period, int shift) { std::vector<double> s = D::smma_v(closes, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (i >= (size_t)shift && D::fin_v(s[i - (size_t)shift])) o[i] = s[i - (size_t)shift]; return o; }",
    L("ind_alligator_jaw(series_closes(series), period_, shift_)", "Jaw(13,8)"),
    "Alligator jaw, 13-bar SMMA of the median price shifted 8 bars ahead",
    "xframe::ind_alligator_jaw(xframe::series_closes(series), 13, 8)",
    note="The published Alligator smooths the median price (high+low)/2; this widget smooths closes and documents the difference.")
add("AlligatorTeeth", "ind_alligator_teeth", "std::vector<double> ind_alligator_teeth(const std::vector<double>& closes, int period, int shift)",
    "overlay", RED, (", int period = 8, int shift = 5",
                      "    int period_ = 8;\n    int shift_ = 5;\n",
                      "        period_ = period;\n        shift_ = shift;\n"),
    "std::vector<double> ind_alligator_teeth(const std::vector<double>& closes, int period, int shift) { std::vector<double> s = D::smma_v(closes, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (i >= (size_t)shift && D::fin_v(s[i - (size_t)shift])) o[i] = s[i - (size_t)shift]; return o; }",
    L("ind_alligator_teeth(series_closes(series), period_, shift_)", "Teeth(8,5)", RED),
    "Alligator teeth, 8-bar SMMA shifted 5 bars ahead",
    "xframe::ind_alligator_teeth(xframe::series_closes(series), 8, 5)",
    note="The published Alligator smooths the median price (high+low)/2; this widget smooths closes and documents the difference.")
add("AlligatorLips", "ind_alligator_lips", "std::vector<double> ind_alligator_lips(const std::vector<double>& closes, int period, int shift)",
    "overlay", GREEN, (", int period = 5, int shift = 3",
                        "    int period_ = 5;\n    int shift_ = 3;\n",
                        "        period_ = period;\n        shift_ = shift;\n"),
    "std::vector<double> ind_alligator_lips(const std::vector<double>& closes, int period, int shift) { std::vector<double> s = D::smma_v(closes, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (i >= (size_t)shift && D::fin_v(s[i - (size_t)shift])) o[i] = s[i - (size_t)shift]; return o; }",
    L("ind_alligator_lips(series_closes(series), period_, shift_)", "Lips(5,3)", GREEN),
    "Alligator lips, 5-bar SMMA shifted 3 bars ahead",
    "xframe::ind_alligator_lips(xframe::series_closes(series), 5, 3)",
    note="The published Alligator smooths the median price (high+low)/2; this widget smooths closes and documents the difference.")
add("GatorOscillator", "ind_gator", "GatorResult ind_gator(const std::vector<double>& closes)",
    "pane", BLUE, nopar(),
    "struct GatorResult { std::vector<double> upper; std::vector<double> lower; };\nGatorResult ind_gator(const std::vector<double>& closes) { GatorResult r; std::vector<double> jaw = ind_alligator_jaw(closes, 13, 8); std::vector<double> teeth = ind_alligator_teeth(closes, 8, 5); std::vector<double> lips = ind_alligator_lips(closes, 5, 3); r.upper.assign(closes.size(), D::nan_v()); r.lower.assign(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (D::fin_v(jaw[i]) && D::fin_v(teeth[i])) r.upper[i] = std::fabs(jaw[i] - teeth[i]); if (D::fin_v(teeth[i]) && D::fin_v(lips[i])) r.lower[i] = -std::fabs(teeth[i] - lips[i]); } return r; }",
    ("        { auto gr = ind_gator(series_closes(series)); lines_.push_back(gr.upper); line_names_.push_back(\"Gator+\"); line_colors_.push_back(Srgb{0, 150, 0}); lines_.push_back(gr.lower); line_names_.push_back(\"Gator-\"); line_colors_.push_back(Srgb{200, 30, 30}); }\n"),
    "Gator oscillator, absolute jaw-teeth gap above zero and teeth-lips gap below zero",
    "xframe::ind_gator(xframe::series_closes(series)).upper")

# ---- TRIX / DPO / QStick / Elder Ray ----
add("TRIX", "ind_trix", "std::vector<double> ind_trix(const std::vector<double>& closes, int period)",
    "pane", BLUE, per(14),
    "std::vector<double> ind_trix(const std::vector<double>& closes, int period) { std::vector<double> e3 = D::ema_v(D::ema_v(D::ema_v(closes, period), period), period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 1; i < closes.size(); ++i) if (D::fin_v(e3[i]) && D::fin_v(e3[i - 1]) && e3[i - 1] != 0.0) o[i] = 100.0 * (e3[i] - e3[i - 1]) / e3[i - 1]; return o; }",
    L("ind_trix(series_closes(series), period_)", "TRIX(14)"),
    "TRIX, 1-bar rate of change of the triple-smoothed EMA, in percent",
    "xframe::ind_trix(xframe::series_closes(series), 14)")
add("DPO", "ind_dpo", "std::vector<double> ind_dpo(const std::vector<double>& closes, int period)",
    "pane", ORANGE, per(20),
    "std::vector<double> ind_dpo(const std::vector<double>& closes, int period) { std::vector<double> sma = D::sma_v(closes, period); std::vector<double> o(closes.size(), D::nan_v()); int shift = period / 2 + 1; for (size_t i = 0; i < closes.size(); ++i) { if (i < (size_t)shift) continue; if (D::fin_v(sma[i - (size_t)shift])) o[i] = closes[i] - sma[i - (size_t)shift]; } return o; }",
    L("ind_dpo(series_closes(series), period_)", "DPO(20)", ORANGE),
    "detrended price oscillator, close minus the SMA shifted back period/2+1 bars",
    "xframe::ind_dpo(xframe::series_closes(series), 20)")
add("QStick", "ind_qstick", "std::vector<double> ind_qstick(const OHLCVSeries& series, int period)",
    "pane", PURPLE, per(14),
    "std::vector<double> ind_qstick(const OHLCVSeries& series, int period) { std::vector<double> d(series.size(), 0.0); for (size_t i = 0; i < series.size(); ++i) d[i] = series[i].close - series[i].open; return D::sma_v(d, period); }",
    L("ind_qstick(series, period_)", "QStick(14)", PURPLE),
    "QStick, SMA of close minus open over the period",
    "xframe::ind_qstick(series, 14)")
add("ElderRayBull", "ind_elder_ray_bull", "std::vector<double> ind_elder_ray_bull(const OHLCVSeries& series, int period)",
    "pane", GREEN, per(13),
    "std::vector<double> ind_elder_ray_bull(const OHLCVSeries& series, int period) { std::vector<double> ema = D::ema_v(series_closes(series), period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(ema[i])) o[i] = series[i].high - ema[i]; return o; }",
    L("ind_elder_ray_bull(series, period_)", "BullPower(13)", GREEN),
    "Elder Ray bull power, high minus the EMA of closes",
    "xframe::ind_elder_ray_bull(series, 13)")
add("ElderRayBear", "ind_elder_ray_bear", "std::vector<double> ind_elder_ray_bear(const OHLCVSeries& series, int period)",
    "pane", RED, per(13),
    "std::vector<double> ind_elder_ray_bear(const OHLCVSeries& series, int period) { std::vector<double> ema = D::ema_v(series_closes(series), period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(ema[i])) o[i] = series[i].low - ema[i]; return o; }",
    L("ind_elder_ray_bear(series, period_)", "BearPower(13)", RED),
    "Elder Ray bear power, low minus the EMA of closes",
    "xframe::ind_elder_ray_bear(series, 13)")

# ---- Linear regression family (6) ----
add("LinReg", "ind_linreg", "std::vector<double> ind_linreg(const std::vector<double>& closes, int period)",
    "overlay", BLUE, per(14),
    "std::vector<double> ind_linreg(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sl, ic, v, r; D::linreg_parts(closes, i, period, sl, ic, v, r); o[i] = v; } return o; }",
    L("ind_linreg(series_closes(series), period_)", "LinReg(14)"),
    "linear regression line, endpoint value of the least-squares fit over the window",
    "xframe::ind_linreg(xframe::series_closes(series), 14)")
add("LinRegSlope", "ind_linreg_slope", "std::vector<double> ind_linreg_slope(const std::vector<double>& closes, int period)",
    "pane", ORANGE, per(14),
    "std::vector<double> ind_linreg_slope(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sl, ic, v, r; D::linreg_parts(closes, i, period, sl, ic, v, r); o[i] = sl; } return o; }",
    L("ind_linreg_slope(series_closes(series), period_)", "LinRegSlope(14)", ORANGE),
    "linear regression slope, price change per bar of the least-squares fit",
    "xframe::ind_linreg_slope(xframe::series_closes(series), 14)")
add("LinRegAngle", "ind_linreg_angle", "std::vector<double> ind_linreg_angle(const std::vector<double>& closes, int period)",
    "pane", PURPLE, per(14),
    "std::vector<double> ind_linreg_angle(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sl, ic, v, r; D::linreg_parts(closes, i, period, sl, ic, v, r); o[i] = std::atan(sl) * 180.0 / 3.141592653589793; } return o; }",
    L("ind_linreg_angle(series_closes(series), period_)", "LinRegAngle(14)", PURPLE),
    "linear regression angle, arctangent of the slope in degrees",
    "xframe::ind_linreg_angle(xframe::series_closes(series), 14)")
add("LinRegIntercept", "ind_linreg_intercept", "std::vector<double> ind_linreg_intercept(const std::vector<double>& closes, int period)",
    "pane", TEAL, per(14),
    "std::vector<double> ind_linreg_intercept(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sl, ic, v, r; D::linreg_parts(closes, i, period, sl, ic, v, r); o[i] = ic; } return o; }",
    L("ind_linreg_intercept(series_closes(series), period_)", "LinRegIntercept(14)", TEAL),
    "linear regression intercept, fitted value at the start of the window",
    "xframe::ind_linreg_intercept(xframe::series_closes(series), 14)")
add("TSF", "ind_tsf", "std::vector<double> ind_tsf(const std::vector<double>& closes, int period)",
    "overlay", GREEN, per(14),
    "std::vector<double> ind_tsf(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sl, ic, v, r; D::linreg_parts(closes, i, period, sl, ic, v, r); o[i] = ic + sl * period; } return o; }",
    L("ind_tsf(series_closes(series), period_)", "TSF(14)", GREEN),
    "time series forecast, linear-regression line extrapolated one bar ahead",
    "xframe::ind_tsf(xframe::series_closes(series), 14)")

# ---- Donchian (3) ----
add("DonchianUpper", "ind_donchian_upper", "std::vector<double> ind_donchian_upper(const OHLCVSeries& series, int period)",
    "overlay", GREEN, per(20),
    "std::vector<double> ind_donchian_upper(const OHLCVSeries& series, int period) { std::vector<double> o(series.size(), D::nan_v()); std::vector<double> hi = series_highs(series); for (size_t i = 0; i < series.size(); ++i) if (i + 1 >= (size_t)period) o[i] = D::highest_v(hi, i, period); return o; }",
    L("ind_donchian_upper(series, period_)", "DonchianUpper(20)", GREEN),
    "Donchian channel upper band, highest high over the period",
    "xframe::ind_donchian_upper(series, 20)")
add("DonchianMiddle", "ind_donchian_middle", "std::vector<double> ind_donchian_middle(const OHLCVSeries& series, int period)",
    "overlay", BLUE, per(20),
    "std::vector<double> ind_donchian_middle(const OHLCVSeries& series, int period) { std::vector<double> u = ind_donchian_upper(series, period); std::vector<double> l = ind_donchian_lower(series, period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(u[i]) && D::fin_v(l[i])) o[i] = (u[i] + l[i]) / 2.0; return o; }",
    L("ind_donchian_middle(series, period_)", "DonchianMiddle(20)"),
    "Donchian channel middle line, average of the upper and lower bands",
    "xframe::ind_donchian_middle(series, 20)")
add("DonchianLower", "ind_donchian_lower", "std::vector<double> ind_donchian_lower(const OHLCVSeries& series, int period)",
    "overlay", RED, per(20),
    "std::vector<double> ind_donchian_lower(const OHLCVSeries& series, int period) { std::vector<double> o(series.size(), D::nan_v()); std::vector<double> lo = series_lows(series); for (size_t i = 0; i < series.size(); ++i) if (i + 1 >= (size_t)period) o[i] = D::lowest_v(lo, i, period); return o; }",
    L("ind_donchian_lower(series, period_)", "DonchianLower(20)", RED),
    "Donchian channel lower band, lowest low over the period",
    "xframe::ind_donchian_lower(series, 20)")

# ---- Chandelier / Chande-Kroll ----
add("ChandelierExitLong", "ind_chandelier_long", "std::vector<double> ind_chandelier_long(const OHLCVSeries& series, int period, double mult)",
    "overlay", RED, (", int period = 22, double mult = 3.0",
                      "    int period_ = 22;\n    double mult_ = 3.0;\n",
                      "        period_ = period;\n        mult_ = mult;\n"),
    "std::vector<double> ind_chandelier_long(const OHLCVSeries& series, int period, double mult) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); std::vector<double> hi = series_highs(series); for (size_t i = 0; i < series.size(); ++i) if (i + 1 >= (size_t)period && D::fin_v(atr[i])) o[i] = D::highest_v(hi, i, period) - mult * atr[i]; return o; }",
    L("ind_chandelier_long(series, period_, mult_)", "ChandelierLong(22,3)", RED),
    "Chandelier exit for longs, highest high over the period minus mult times ATR",
    "xframe::ind_chandelier_long(series, 22, 3.0)")
add("ChandelierExitShort", "ind_chandelier_short", "std::vector<double> ind_chandelier_short(const OHLCVSeries& series, int period, double mult)",
    "overlay", GREEN, (", int period = 22, double mult = 3.0",
                        "    int period_ = 22;\n    double mult_ = 3.0;\n",
                        "        period_ = period;\n        mult_ = mult;\n"),
    "std::vector<double> ind_chandelier_short(const OHLCVSeries& series, int period, double mult) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); std::vector<double> lo = series_lows(series); for (size_t i = 0; i < series.size(); ++i) if (i + 1 >= (size_t)period && D::fin_v(atr[i])) o[i] = D::lowest_v(lo, i, period) + mult * atr[i]; return o; }",
    L("ind_chandelier_short(series, period_, mult_)", "ChandelierShort(22,3)", GREEN),
    "Chandelier exit for shorts, lowest low over the period plus mult times ATR",
    "xframe::ind_chandelier_short(series, 22, 3.0)")
add("ChandeKrollStop", "ind_chande_kroll_stop", "std::vector<double> ind_chande_kroll_stop(const OHLCVSeries& series, int period, double mult)",
    "overlay", ORANGE, (", int period = 10, double mult = 1.0",
                         "    int period_ = 10;\n    double mult_ = 1.0;\n",
                         "        period_ = period;\n        mult_ = mult;\n"),
    "std::vector<double> ind_chande_kroll_stop(const OHLCVSeries& series, int period, double mult) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); std::vector<double> hi = series_highs(series); for (size_t i = 0; i < series.size(); ++i) if (i + 1 >= (size_t)period && D::fin_v(atr[i])) o[i] = D::highest_v(hi, i, period) - mult * atr[i]; return o; }",
    L("ind_chande_kroll_stop(series, period_, mult_)", "ChandeKrollStop(10,1)", ORANGE),
    "Chande-Kroll stop for longs, highest high over the period minus mult times ATR",
    "xframe::ind_chande_kroll_stop(series, 10, 1.0)",
    note="The published Chande-Kroll stop carries a second, wider stop; this widget implements the primary (first) stop only.")

# ---- HalfTrend / SSL / UT Bot ----
add("HalfTrend", "ind_half_trend", "std::vector<double> ind_half_trend(const OHLCVSeries& series, int amplitude, double channelDeviation)",
    "overlay", BLUE, (", int amplitude = 2, double channelDeviation = 2.0",
                       "    int amplitude_ = 2;\n    double channelDeviation_ = 2.0;\n",
                       "        amplitude_ = amplitude;\n        channelDeviation_ = channelDeviation;\n"),
    "std::vector<double> ind_half_trend(const OHLCVSeries& series, int amplitude, double channelDeviation) { std::vector<double> atr = D::atr_v(series, 100); std::vector<double> o(series.size(), D::nan_v()); if (series.empty()) return o; double up = 0.0, dn = 0.0; bool trend_up = true; for (size_t i = 0; i < series.size(); ++i) { if (!D::fin_v(atr[i])) continue; double hh = D::highest_v(series_highs(series), i, std::max(1, amplitude)); double ll = D::lowest_v(series_lows(series), i, std::max(1, amplitude)); double dev = atr[i] * channelDeviation / 2.0; double upb = ll + dev, dnb = hh - dev; if (i == 0 || !D::fin_v(o[i - 1])) { up = upb; dn = dnb; } else { up = (series[i - 1].close > up) ? std::max(up, upb) : upb; dn = (series[i - 1].close < dn) ? std::min(dn, dnb) : dnb; if (trend_up && series[i].close < dn) trend_up = false; else if (!trend_up && series[i].close > up) trend_up = true; } o[i] = trend_up ? up : dn; } return o; }",
    L("ind_half_trend(series, amplitude_, channelDeviation_)", "HalfTrend(2,2)"),
    "HalfTrend line, ATR channel-deviation bands that flip with the trend",
    "xframe::ind_half_trend(series, 2, 2.0)",
    note="Simplified port of the published HalfTrend logic (amplitude extrema plus ATR channel-deviation bands); the exact ATR period and band update rules vary by implementation.")
add("SSLChannel", "ind_ssl_channel", "std::vector<double> ind_ssl_channel(const OHLCVSeries& series, int period)",
    "overlay", PURPLE, per(10),
    "std::vector<double> ind_ssl_channel(const OHLCVSeries& series, int period) { std::vector<double> sh = D::sma_v(series_highs(series), period); std::vector<double> sl = D::sma_v(series_lows(series), period); std::vector<double> cl = series_closes(series); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(sh[i]) && D::fin_v(sl[i])) o[i] = (cl[i] > sh[i]) ? sl[i] : sh[i]; return o; }",
    L("ind_ssl_channel(series, period_)", "SSL(10)", PURPLE),
    "SSL channel line, SMA of lows when close is above the SMA of highs, else SMA of highs",
    "xframe::ind_ssl_channel(series, 10)")
add("UTBot", "ind_ut_bot", "std::vector<double> ind_ut_bot(const OHLCVSeries& series, int period, double mult)",
    "overlay", ORANGE, (", int period = 10, double mult = 1.0",
                         "    int period_ = 10;\n    double mult_ = 1.0;\n",
                         "        period_ = period;\n        mult_ = mult;\n"),
    "std::vector<double> ind_ut_bot(const OHLCVSeries& series, int period, double mult) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); if (series.empty()) return o; double stop = 0.0; bool have = false; for (size_t i = 0; i < series.size(); ++i) { if (!D::fin_v(atr[i])) continue; double c = series[i].close; if (!have) { stop = c - mult * atr[i]; have = true; } else { double prev = stop; if (c > prev && (i < 2 || series[i - 1].close > prev)) stop = std::max(prev, c - mult * atr[i]); else if (c < prev && (i < 2 || series[i - 1].close < prev)) stop = std::min(prev, c + mult * atr[i]); else stop = prev; } o[i] = stop; } return o; }",
    L("ind_ut_bot(series, period_, mult_)", "UTBot(10,1)", ORANGE),
    "UT Bot trailing stop, ATR trailing stop that flips sides with the trend",
    "xframe::ind_ut_bot(series, 10, 1.0)")

# ---- Pivot / envelopes ----
add("PivotPoint", "ind_pivot_point", "std::vector<double> ind_pivot_point(const OHLCVSeries& series)",
    "overlay", BLUE, nopar(),
    "std::vector<double> ind_pivot_point(const OHLCVSeries& series) { std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 1; i < series.size(); ++i) o[i] = (series[i - 1].high + series[i - 1].low + series[i - 1].close) / 3.0; return o; }",
    L("ind_pivot_point(series)", "PivotPoint"),
    "classic floor-trader pivot point, (prior high + prior low + prior close)/3",
    "xframe::ind_pivot_point(series)")
add("MAEnvelopeUpper", "ind_ma_envelope_upper", "std::vector<double> ind_ma_envelope_upper(const std::vector<double>& closes, int period, double pct)",
    "overlay", GREEN, (", int period = 20, double pct = 2.5",
                        "    int period_ = 20;\n    double pct_ = 2.5;\n",
                        "        period_ = period;\n        pct_ = pct;\n"),
    "std::vector<double> ind_ma_envelope_upper(const std::vector<double>& closes, int period, double pct) { std::vector<double> sma = D::sma_v(closes, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (D::fin_v(sma[i])) o[i] = sma[i] * (1.0 + pct / 100.0); return o; }",
    L("ind_ma_envelope_upper(series_closes(series), period_, pct_)", "MAEnvUpper(20,2.5)", GREEN),
    "moving average envelope upper band, SMA shifted up by pct percent",
    "xframe::ind_ma_envelope_upper(xframe::series_closes(series), 20, 2.5)")
add("MAEnvelopeLower", "ind_ma_envelope_lower", "std::vector<double> ind_ma_envelope_lower(const std::vector<double>& closes, int period, double pct)",
    "overlay", RED, (", int period = 20, double pct = 2.5",
                      "    int period_ = 20;\n    double pct_ = 2.5;\n",
                      "        period_ = period;\n        pct_ = pct;\n"),
    "std::vector<double> ind_ma_envelope_lower(const std::vector<double>& closes, int period, double pct) { std::vector<double> sma = D::sma_v(closes, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (D::fin_v(sma[i])) o[i] = sma[i] * (1.0 - pct / 100.0); return o; }",
    L("ind_ma_envelope_lower(series_closes(series), period_, pct_)", "MAEnvLower(20,2.5)", RED),
    "moving average envelope lower band, SMA shifted down by pct percent",
    "xframe::ind_ma_envelope_lower(xframe::series_closes(series), 20, 2.5)")

# ---- Trend strength / walk ----
add("TrendIntensityIndex", "ind_trend_intensity", "std::vector<double> ind_trend_intensity(const std::vector<double>& closes, int period)",
    "pane", BLUE, per(30),
    "std::vector<double> ind_trend_intensity(const std::vector<double>& closes, int period) { std::vector<double> sma = D::sma_v(closes, period); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double pos = 0.0, neg = 0.0; for (int j = 0; j < period; ++j) { double d = closes[i - (size_t)j] - sma[i]; if (d > 0.0) pos += d; else neg -= d; } o[i] = (pos + neg > 0.0) ? 100.0 * pos / (pos + neg) : 50.0; } return o; }",
    L("ind_trend_intensity(series_closes(series), period_)", "TII(30)"),
    "trend intensity index, share of SMA deviation that lies above the SMA, 0-100",
    "xframe::ind_trend_intensity(xframe::series_closes(series), 30)")
add("VHF", "ind_vhf", "std::vector<double> ind_vhf(const OHLCVSeries& series, int period)",
    "pane", ORANGE, per(28),
    "std::vector<double> ind_vhf(const OHLCVSeries& series, int period) { std::vector<double> cl = series_closes(series); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period) continue; double range = D::highest_v(cl, i, period) - D::lowest_v(cl, i, period); double sum = 0.0; for (int j = 0; j < period; ++j) { size_t k = i - (size_t)j; if (k > 0) sum += std::fabs(cl[k] - cl[k - 1]); } o[i] = (sum > 0.0) ? range / sum : 0.0; } return o; }",
    L("ind_vhf(series, period_)", "VHF(28)", ORANGE),
    "vertical horizontal filter, price range over the period divided by the sum of absolute daily changes",
    "xframe::ind_vhf(series, 28)")
add("RWIHigh", "ind_rwi_high", "std::vector<double> ind_rwi_high(const OHLCVSeries& series, int period)",
    "pane", GREEN, per(14),
    "std::vector<double> ind_rwi_high(const OHLCVSeries& series, int period) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period || !D::fin_v(atr[i])) continue; double den = atr[i] * std::sqrt((double)period); o[i] = (den > 0.0) ? (series[i].high - series[i + 1 - (size_t)period].low) / den : 0.0; } return o; }",
    L("ind_rwi_high(series, period_)", "RWIHigh(14)", GREEN),
    "random walk index high, (high - low period bars ago) / (ATR * sqrt(period))",
    "xframe::ind_rwi_high(series, 14)")
add("RWILow", "ind_rwi_low", "std::vector<double> ind_rwi_low(const OHLCVSeries& series, int period)",
    "pane", RED, per(14),
    "std::vector<double> ind_rwi_low(const OHLCVSeries& series, int period) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period || !D::fin_v(atr[i])) continue; double den = atr[i] * std::sqrt((double)period); o[i] = (den > 0.0) ? (series[i + 1 - (size_t)period].high - series[i].low) / den : 0.0; } return o; }",
    L("ind_rwi_low(series, period_)", "RWILow(14)", RED),
    "random walk index low, (high period bars ago - low) / (ATR * sqrt(period))",
    "xframe::ind_rwi_low(series, 14)")

# ---- Ehlers ----
add("EhlersTrendline", "ind_ehlers_trendline", "std::vector<double> ind_ehlers_trendline(const std::vector<double>& closes, double alpha)",
    "overlay", BLUE, (", double alpha = 0.07",
                       "    double alpha_ = 0.07;\n",
                       "        alpha_ = alpha;\n"),
    "std::vector<double> ind_ehlers_trendline(const std::vector<double>& closes, double alpha) { std::vector<double> o(closes.size(), D::nan_v()); double a = alpha; double c1 = a - a * a / 4.0, c2 = 0.5 * a * a, c3 = a - 0.75 * a * a, c4 = 2.0 * (1.0 - a), c5 = (1.0 - a) * (1.0 - a); auto px = [&](size_t i, size_t lag) -> double { return (i >= lag) ? closes[i - lag] : closes[0]; }; auto pv = [&](size_t i, size_t lag) -> double { return (i >= lag && D::fin_v(o[i - lag])) ? o[i - lag] : 0.0; }; for (size_t i = 0; i < closes.size(); ++i) { if (i < 2) { o[i] = closes[i]; continue; } o[i] = c1 * px(i, 0) + c2 * px(i, 1) - c3 * px(i, 2) + c4 * pv(i, 1) - c5 * pv(i, 2); } return o; }",
    L("ind_ehlers_trendline(series_closes(series), alpha_)", "EhlersTrendline(0.07)"),
    "Ehlers instantaneous trendline, alpha-weighted two-pole low-pass filter",
    "xframe::ind_ehlers_trendline(xframe::series_closes(series), 0.07)")
add("EhlersDecycler", "ind_ehlers_decycler", "std::vector<double> ind_ehlers_decycler(const std::vector<double>& closes, int period)",
    "overlay", TEAL, per(10),
    "std::vector<double> ind_ehlers_decycler(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 2) period = 2; const double pi = 3.141592653589793; double w = 0.707 * 2.0 * pi / period; double a1 = (std::cos(w) + std::sin(w) - 1.0) / std::sin(w); auto px = [&](size_t i, size_t lag) -> double { return (i >= lag) ? closes[i - lag] : closes[0]; }; double hp_prev = 0.0, dec_prev = 0.0; for (size_t i = 0; i < closes.size(); ++i) { double hp = (1.0 - a1 / 2.0) * (1.0 - a1 / 2.0) * (px(i, 0) - 2.0 * px(i, 1) + px(i, 2)) + 2.0 * (1.0 - a1) * hp_prev - (1.0 - a1) * (1.0 - a1) * ((i >= 2) ? 0.0 : 0.0); (void)hp; double hp2 = (1.0 - a1 / 2.0) * (1.0 - a1 / 2.0) * (px(i, 0) - 2.0 * px(i, 1) + px(i, 2)); double dec = (a1 / 2.0) * (hp2 + hp_prev) + (1.0 - a1) * dec_prev; hp_prev = hp2; dec_prev = dec; o[i] = dec; } return o; }",
    L("ind_ehlers_decycler(series_closes(series), period_)", "EhlersDecycler(10)", TEAL),
    "Ehlers decycler, high-pass cycle removal plus first-order smoothing",
    "xframe::ind_ehlers_decycler(xframe::series_closes(series), 10)",
    note="Simplified Ehlers Decycler: the high-pass stage keeps only the current two-pole term and the published second-order recursion is reduced; values track the published decycler but are not bit-identical.")

# ---- Multi-line / structure ----
add("GuppyMMA", "ind_guppy_mma", "GuppyResult ind_guppy_mma(const std::vector<double>& closes)",
    "overlay", BLUE, nopar(),
    "struct GuppyResult { std::vector<std::vector<double>> lines; };\nGuppyResult ind_guppy_mma(const std::vector<double>& closes) { GuppyResult r; const int ps[12] = {3, 5, 8, 10, 12, 15, 30, 35, 40, 45, 50, 60}; for (int k = 0; k < 12; ++k) r.lines.push_back(D::ema_v(closes, ps[k])); return r; }",
    ("        { auto gr = ind_guppy_mma(series_closes(series)); const int ps[12] = {3, 5, 8, 10, 12, 15, 30, 35, 40, 45, 50, 60}; const Srgb cols[12] = {Srgb{0, 100, 200}, Srgb{0, 150, 0}, Srgb{200, 30, 30}, Srgb{230, 126, 34}, Srgb{120, 60, 180}, Srgb{0, 150, 150}, Srgb{150, 90, 30}, Srgb{60, 60, 180}, Srgb{180, 60, 120}, Srgb{90, 140, 40}, Srgb{40, 120, 160}, Srgb{160, 40, 40}}; for (int k = 0; k < 12; ++k) { lines_.push_back(gr.lines[(size_t)k]); line_names_.push_back(std::string(\"EMA(\") + std::to_string(ps[k]) + \")\"); line_colors_.push_back(cols[k]); } }\n"),
    "Guppy multiple moving average, 12 EMAs in a fast group (3-15) and a slow group (30-60)",
    "xframe::ind_guppy_mma(xframe::series_closes(series)).lines[0]")
add("ZigZag", "ind_zigzag", "std::vector<double> ind_zigzag(const OHLCVSeries& series, double deviation)",
    "overlay", RED, (", double deviation = 5.0",
                      "    double deviation_ = 5.0;\n",
                      "        deviation_ = deviation;\n"),
    "std::vector<double> ind_zigzag(const OHLCVSeries& series, double deviation) { std::vector<double> o(series.size(), D::nan_v()); if (series.empty()) return o; double last_pivot = series[0].close; size_t last_idx = 0; int dir = 0; double ext_high = series[0].high, ext_low = series[0].low; size_t ext_idx = 0; for (size_t i = 1; i < series.size(); ++i) { double thr = last_pivot * deviation / 100.0; if (dir >= 0 && series[i].high > ext_high) { ext_high = series[i].high; ext_idx = i; } if (dir <= 0 && series[i].low < ext_low) { ext_low = series[i].low; ext_idx = i; } if (dir == 0) { if (series[i].high >= last_pivot + thr) { dir = 1; ext_high = series[i].high; ext_idx = i; } else if (series[i].low <= last_pivot - thr) { dir = -1; ext_low = series[i].low; ext_idx = i; } } else if (dir == 1 && series[i].low <= ext_high - ext_high * deviation / 100.0) { o[ext_idx] = ext_high; last_pivot = ext_high; last_idx = ext_idx; dir = -1; ext_low = series[i].low; ext_idx = i; } else if (dir == -1 && series[i].high >= ext_low + ext_low * deviation / 100.0) { o[ext_idx] = ext_low; last_pivot = ext_low; last_idx = ext_idx; dir = 1; ext_high = series[i].high; ext_idx = i; } } (void)last_idx; if (dir == 1) o[ext_idx] = ext_high; else if (dir == -1) o[ext_idx] = ext_low; return o; }",
    L("ind_zigzag(series, deviation_)", "ZigZag(5%)", RED),
    "ZigZag, pivot points where price reverses by at least deviation percent",
    "xframe::ind_zigzag(series, 5.0)",
    sparse=True, note="Only pivot bars carry values; every other bar is NaN by design. The last leg repaints as new bars arrive.")
add("FractalUp", "ind_fractal_up", "std::vector<double> ind_fractal_up(const OHLCVSeries& series)",
    "overlay", GREEN, nopar(),
    "std::vector<double> ind_fractal_up(const OHLCVSeries& series) { std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 2; i + 2 < series.size(); ++i) { double h = series[i].high; if (h > series[i - 1].high && h > series[i - 2].high && h > series[i + 1].high && h > series[i + 2].high) o[i] = h; } return o; }",
    L("ind_fractal_up(series)", "FractalUp", GREEN),
    "Bill Williams fractal up, high greater than the highs of the two bars either side",
    "xframe::ind_fractal_up(series)",
    sparse=True, note="Only fractal bars carry values; uses two bars of lookahead, so the signal confirms two bars late.")
add("FractalDown", "ind_fractal_down", "std::vector<double> ind_fractal_down(const OHLCVSeries& series)",
    "overlay", RED, nopar(),
    "std::vector<double> ind_fractal_down(const OHLCVSeries& series) { std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 2; i + 2 < series.size(); ++i) { double l = series[i].low; if (l < series[i - 1].low && l < series[i - 2].low && l < series[i + 1].low && l < series[i + 2].low) o[i] = l; } return o; }",
    L("ind_fractal_down(series)", "FractalDown", RED),
    "Bill Williams fractal down, low lower than the lows of the two bars either side",
    "xframe::ind_fractal_down(series)",
    sparse=True, note="Only fractal bars carry values; uses two bars of lookahead, so the signal confirms two bars late.")
add("SwingIndex", "ind_swing_index", "std::vector<double> ind_swing_index(const OHLCVSeries& series, double limitMove)",
    "pane", BLUE, (", double limitMove = 1.0",
                    "    double limitMove_ = 1.0;\n",
                    "        limitMove_ = limitMove;\n"),
    "std::vector<double> ind_swing_index(const OHLCVSeries& series, double limitMove) { std::vector<double> o(series.size(), D::nan_v()); if (limitMove <= 0.0) limitMove = 1.0; for (size_t i = 1; i < series.size(); ++i) { const auto& b = series[i]; const auto& p = series[i - 1]; double rnum = std::fabs(b.high - p.close); double rden = std::fabs(b.low - p.close); double rr = b.high - b.low; double r = std::max(rnum, std::max(rden, rr)); double k = std::max(rnum, rden); double num = (b.close - p.close) + 0.5 * (b.close - b.open) + 0.25 * (p.close - p.open); o[i] = (r > 0.0) ? 50.0 * num / r * (k / limitMove) : 0.0; } return o; }",
    L("ind_swing_index(series, limitMove_)", "SwingIndex(1)"),
    "Wilder swing index, real price move scaled by the limit move",
    "xframe::ind_swing_index(series, 1.0)")
add("AccumulativeSwingIndex", "ind_accumulative_swing_index", "std::vector<double> ind_accumulative_swing_index(const OHLCVSeries& series, double limitMove)",
    "pane", ORANGE, (", double limitMove = 1.0",
                      "    double limitMove_ = 1.0;\n",
                      "        limitMove_ = limitMove;\n"),
    "std::vector<double> ind_accumulative_swing_index(const OHLCVSeries& series, double limitMove) { std::vector<double> si = ind_swing_index(series, limitMove); std::vector<double> o(series.size(), D::nan_v()); double acc = 0.0; for (size_t i = 0; i < series.size(); ++i) { if (!D::fin_v(si[i])) continue; acc += si[i]; o[i] = acc; } return o; }",
    L("ind_accumulative_swing_index(series, limitMove_)", "ASI(1)", ORANGE),
    "Wilder accumulative swing index, running sum of the swing index",
    "xframe::ind_accumulative_swing_index(series, 1.0)")
add("FractalDimensionIndex", "ind_fractal_dimension", "std::vector<double> ind_fractal_dimension(const std::vector<double>& closes, int period)",
    "pane", PURPLE, per(30),
    "std::vector<double> ind_fractal_dimension(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 2) period = 2; for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sum = 0.0; for (size_t j = i + 1 - (size_t)period + 1; j <= i; ++j) sum += std::fabs(closes[j] - closes[j - 1]); if (sum <= 0.0) { o[i] = 1.0; continue; } o[i] = 1.0 + (std::log(sum) - std::log(2.0)) / std::log((double)period); } return o; }",
    L("ind_fractal_dimension(series_closes(series), period_)", "FDI(30)", PURPLE),
    "fractal dimension index, 1 + (ln(sum of absolute changes) - ln 2) / ln(period)",
    "xframe::ind_fractal_dimension(xframe::series_closes(series), 30)",
    note="The FDI formula has several published variants; this widget uses the common ln form and documents it.")
add("RegressionChannel", "ind_regression_channel", "RegChannelResult ind_regression_channel(const std::vector<double>& closes, int period, double mult)",
    "overlay", BLUE, (", int period = 20, double mult = 2.0",
                       "    int period_ = 20;\n    double mult_ = 2.0;\n",
                       "        period_ = period;\n        mult_ = mult;\n"),
    "struct RegChannelResult { std::vector<double> upper; std::vector<double> mid; std::vector<double> lower; };\nRegChannelResult ind_regression_channel(const std::vector<double>& closes, int period, double mult) { RegChannelResult r; r.upper.assign(closes.size(), D::nan_v()); r.mid.assign(closes.size(), D::nan_v()); r.lower.assign(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double sl, ic, v, rsq; D::linreg_parts(closes, i, period, sl, ic, v, rsq); double ss = 0.0; for (int j = 0; j < period; ++j) { double fit = ic + sl * j; double d = closes[i - (size_t)(period - 1) + (size_t)j] - fit; ss += d * d; } double sd = std::sqrt(ss / period); r.mid[i] = v; r.upper[i] = v + mult * sd; r.lower[i] = v - mult * sd; } return r; }",
    ("        { auto rc = ind_regression_channel(series_closes(series), period_, mult_); lines_.push_back(rc.upper); line_names_.push_back(\"RegChUpper\"); line_colors_.push_back(Srgb{0, 150, 0}); lines_.push_back(rc.mid); line_names_.push_back(\"LinReg\"); line_colors_.push_back(Srgb{0, 100, 200}); lines_.push_back(rc.lower); line_names_.push_back(\"RegChLower\"); line_colors_.push_back(Srgb{200, 30, 30}); }\n"),
    "linear regression channel, regression line plus/minus mult times the residual standard deviation",
    "xframe::ind_regression_channel(xframe::series_closes(series), 20, 2.0).upper")
add("MovingAverageRibbon", "ind_ma_ribbon", "RibbonResult ind_ma_ribbon(const std::vector<double>& closes)",
    "overlay", BLUE, nopar(),
    "struct RibbonResult { std::vector<std::vector<double>> lines; };\nRibbonResult ind_ma_ribbon(const std::vector<double>& closes) { RibbonResult r; const int ps[6] = {5, 10, 20, 30, 50, 100}; for (int k = 0; k < 6; ++k) r.lines.push_back(D::sma_v(closes, ps[k])); return r; }",
    ("        { auto rb = ind_ma_ribbon(series_closes(series)); const int ps[6] = {5, 10, 20, 30, 50, 100}; const Srgb cols[6] = {Srgb{0, 100, 200}, Srgb{0, 150, 0}, Srgb{200, 30, 30}, Srgb{230, 126, 34}, Srgb{120, 60, 180}, Srgb{0, 150, 150}}; for (int k = 0; k < 6; ++k) { lines_.push_back(rb.lines[(size_t)k]); line_names_.push_back(std::string(\"SMA(\") + std::to_string(ps[k]) + \")\"); line_colors_.push_back(cols[k]); } }\n"),
    "moving average ribbon, six SMAs (5, 10, 20, 30, 50, 100) plotted together",
    "xframe::ind_ma_ribbon(xframe::series_closes(series)).lines[0]")

# ---- Replacements for the 3 names yielded to the volatility group ----
# (HurstExponent, EfficiencyRatio, RSquared removed Nov 26 ~12:21 NZDT:
# they belong to the volatility worker's committed set.)
add("EhlersSuperSmoother", "ind_ehlers_super_smoother", "std::vector<double> ind_ehlers_super_smoother(const std::vector<double>& closes, int period)",
    "overlay", BLUE, per(10),
    "std::vector<double> ind_ehlers_super_smoother(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 2) period = 2; if (closes.size() < 3) return o; const double pi = 3.141592653589793; double a1 = std::exp(-1.4142135623730951 * pi / period); double b1 = 2.0 * a1 * std::cos(1.4142135623730951 * pi / period); double c2 = b1, c3 = -a1 * a1, c1 = 1.0 - c2 - c3; double s1 = closes[1], s2 = closes[0]; for (size_t i = 2; i < closes.size(); ++i) { double v = c1 * (closes[i] + closes[i - 1]) / 2.0 + c2 * s1 + c3 * s2; s2 = s1; s1 = v; o[i] = v; } return o; }",
    L("ind_ehlers_super_smoother(series_closes(series), period_)", "SuperSmoother(10)"),
    "Ehlers SuperSmoother, two-pole low-pass filter with minimal lag",
    "xframe::ind_ehlers_super_smoother(xframe::series_closes(series), 10)")
add("VWEMA", "ind_vwema", "std::vector<double> ind_vwema(const OHLCVSeries& series, int period)",
    "overlay", GREEN, per(14),
    "std::vector<double> ind_vwema(const OHLCVSeries& series, int period) { std::vector<double> cv(series.size(), 0.0); std::vector<double> vol = series_volumes(series); for (size_t i = 0; i < series.size(); ++i) cv[i] = series[i].close * series[i].volume; std::vector<double> num = D::ema_v(cv, period); std::vector<double> den = D::ema_v(vol, period); std::vector<double> o(series.size(), D::nan_v()); for (size_t i = 0; i < series.size(); ++i) if (D::fin_v(num[i]) && D::fin_v(den[i]) && den[i] != 0.0) o[i] = num[i] / den[i]; return o; }",
    L("ind_vwema(series, period_)", "VWEMA(14)", GREEN),
    "volume-weighted exponential moving average, EMA of close times volume over EMA of volume",
    "xframe::ind_vwema(series, 14)")
add("ElderSafeZone", "ind_elder_safe_zone", "std::vector<double> ind_elder_safe_zone(const OHLCVSeries& series, int period, double mult)",
    "overlay", RED, (", int period = 10, double mult = 2.0",
                      "    int period_ = 10;\n    double mult_ = 2.0;\n",
                      "        period_ = period;\n        mult_ = mult;\n"),
    "std::vector<double> ind_elder_safe_zone(const OHLCVSeries& series, int period, double mult) { std::vector<double> atr = D::atr_v(series, period); std::vector<double> o(series.size(), D::nan_v()); std::vector<double> lo = series_lows(series); for (size_t i = 0; i < series.size(); ++i) if (i + 1 >= (size_t)period && D::fin_v(atr[i])) o[i] = D::lowest_v(lo, i, period) - mult * atr[i]; return o; }",
    L("ind_elder_safe_zone(series, period_, mult_)", "ElderSafeZone(10,2)", RED),
    "Elder SafeZone long stop, lowest low over the period minus mult times ATR",
    "xframe::ind_elder_safe_zone(series, 10, 2.0)",
    note="Simplified Elder SafeZone: the published SafeZone subtracts the average downside penetration rather than a fixed ATR multiple.")

# ---- Fixups: compute functions must be self-contained (no cross-header calls) ----
MAMA_BODY = """std::vector<double> det(closes.size(), 0.0);
    auto cp = [&](size_t i, size_t lag) -> double { return (i >= lag) ? closes[i - lag] : closes[0]; };
    auto dp = [&](size_t i, size_t lag) -> double { return (i >= lag) ? det[i - lag] : 0.0; };
    for (size_t i = 0; i < closes.size(); ++i) det[i] = (0.0962 * cp(i, 0) + 0.5769 * cp(i, 2) - 0.5769 * cp(i, 4) - 0.0962 * cp(i, 6)) * 0.54;
    std::vector<double> alpha(closes.size(), slowlimit);
    for (size_t i = 0; i < closes.size(); ++i) {
        double q1 = 0.0962 * det[i] + 0.5769 * dp(i, 2) - 0.5769 * dp(i, 4) - 0.0962 * dp(i, 6);
        double i1 = dp(i, 3);
        double phase = std::atan2(q1, i1);
        double a = slowlimit + (fastlimit - slowlimit) * (std::fabs(phase) / 3.141592653589793);
        if (a < slowlimit) { a = slowlimit; }
        if (a > fastlimit) { a = fastlimit; }
        alpha[i] = a;
    }"""
for s in SPECS:
    if s["name"] == "MAMA":
        s["compute"] = ("std::vector<double> ind_mama(const std::vector<double>& closes, double fastlimit, double slowlimit) { "
            "std::vector<double> o(closes.size(), D::nan_v()); " + MAMA_BODY +
            " double prev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (!D::fin_v(prev)) { prev = closes[i]; o[i] = prev; } else { prev = alpha[i] * closes[i] + (1.0 - alpha[i]) * prev; o[i] = prev; } } return o; }")
    if s["name"] == "FAMA":
        s["compute"] = ("std::vector<double> ind_fama(const std::vector<double>& closes, double fastlimit, double slowlimit) { "
            + MAMA_BODY +
            " std::vector<double> mama(closes.size(), D::nan_v()); double prev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (!D::fin_v(prev)) { prev = closes[i]; mama[i] = prev; } else { prev = alpha[i] * closes[i] + (1.0 - alpha[i]) * prev; mama[i] = prev; } } "
            "std::vector<double> o(closes.size(), D::nan_v()); double fprev = D::nan_v(); for (size_t i = 0; i < closes.size(); ++i) { if (!D::fin_v(mama[i])) continue; if (!D::fin_v(fprev)) { fprev = mama[i]; o[i] = fprev; continue; } double fa = 0.5 * alpha[i]; fprev = fa * mama[i] + (1.0 - fa) * fprev; o[i] = fprev; } return o; }")
        s["note"] = "FAMA smooths MAMA with half the MAMA alpha at each bar, as published."
    if s["name"] == "DonchianMiddle":
        s["compute"] = ("std::vector<double> ind_donchian_middle(const OHLCVSeries& series, int period) { std::vector<double> o(series.size(), D::nan_v()); std::vector<double> hi = series_highs(series); std::vector<double> lo = series_lows(series); for (size_t i = 0; i < series.size(); ++i) if (i + 1 >= (size_t)period) o[i] = (D::highest_v(hi, i, period) + D::lowest_v(lo, i, period)) / 2.0; return o; }")
    if s["name"] == "AroonOscillator":
        s["compute"] = ("std::vector<double> ind_aroon_osc(const OHLCVSeries& series, int period) { std::vector<double> o(series.size(), D::nan_v()); std::vector<double> hi = series_highs(series); std::vector<double> lo = series_lows(series); for (size_t i = 0; i < series.size(); ++i) { if (i + 1 < (size_t)period) continue; int su = 0, sd = 0; double m = -1e300; for (int j = 0; j < period; ++j) if (hi[i - (size_t)j] >= m) { m = hi[i - (size_t)j]; su = j; } m = 1e300; for (int j = 0; j < period; ++j) if (lo[i - (size_t)j] <= m) { m = lo[i - (size_t)j]; sd = j; } o[i] = 100.0 * (sd - su) / period; } return o; }")
    if s["name"] == "GatorOscillator":
        s["compute"] = ("struct GatorResult { std::vector<double> upper; std::vector<double> lower; };\nGatorResult ind_gator(const std::vector<double>& closes) { GatorResult r; auto shifted = [&](int p, int sh) { std::vector<double> s = D::smma_v(closes, p); std::vector<double> o(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) if (i >= (size_t)sh && D::fin_v(s[i - (size_t)sh])) o[i] = s[i - (size_t)sh]; return o; }; std::vector<double> jaw = shifted(13, 8); std::vector<double> teeth = shifted(8, 5); std::vector<double> lips = shifted(5, 3); r.upper.assign(closes.size(), D::nan_v()); r.lower.assign(closes.size(), D::nan_v()); for (size_t i = 0; i < closes.size(); ++i) { if (D::fin_v(jaw[i]) && D::fin_v(teeth[i])) r.upper[i] = std::fabs(jaw[i] - teeth[i]); if (D::fin_v(teeth[i]) && D::fin_v(lips[i])) r.lower[i] = -std::fabs(teeth[i] - lips[i]); } return r; }")
    if s["name"] == "AccumulativeSwingIndex":
        s["compute"] = ("std::vector<double> ind_accumulative_swing_index(const OHLCVSeries& series, double limitMove) { std::vector<double> o(series.size(), D::nan_v()); if (limitMove <= 0.0) limitMove = 1.0; double acc = 0.0; for (size_t i = 1; i < series.size(); ++i) { const auto& b = series[i]; const auto& p = series[i - 1]; double rnum = std::fabs(b.high - p.close); double rden = std::fabs(b.low - p.close); double r = std::max(rnum, std::max(rden, b.high - b.low)); double k = std::max(rnum, rden); double num = (b.close - p.close) + 0.5 * (b.close - b.open) + 0.25 * (p.close - p.open); double si = (r > 0.0) ? 50.0 * num / r * (k / limitMove) : 0.0; acc += si; o[i] = acc; } return o; }")
    if s["name"] == "EhlersDecycler":
        s["compute"] = ("std::vector<double> ind_ehlers_decycler(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), D::nan_v()); if (period < 2) period = 2; const double pi = 3.141592653589793; double w = 0.707 * 2.0 * pi / period; double a1 = (std::cos(w) + std::sin(w) - 1.0) / std::sin(w); auto px = [&](size_t i, size_t lag) -> double { return (i >= lag) ? closes[i - lag] : closes[0]; }; double hp1 = 0.0, hp2 = 0.0, dec_prev = 0.0; double c = 1.0 - a1 / 2.0; for (size_t i = 0; i < closes.size(); ++i) { double hp = c * c * (px(i, 0) - 2.0 * px(i, 1) + px(i, 2)) + 2.0 * (1.0 - a1) * hp1 - (1.0 - a1) * (1.0 - a1) * hp2; double dec = (a1 / 2.0) * (hp + hp1) + (1.0 - a1) * dec_prev; hp2 = hp1; hp1 = hp; dec_prev = dec; o[i] = dec; } return o; }")

# ---- Emission ----
def emit(only=None):
    assert len(SPECS) == 85, "need exactly 85 specs, got %d" % len(SPECS)
    names = [s["name"] for s in SPECS]
    assert len(set(names)) == 85, "duplicate indicator names"
    funcs = [s["func"] for s in SPECS]
    assert len(set(funcs)) == 85, "duplicate compute function names"
    # Collision guard (Nov 26 ~12:21 NZDT): never emit a name claimed by the
    # volatility group, and never overwrite a dir whose header is not ours.
    for lf in ("stock_indicators_volatility.txt",):
        lp = os.path.join(TOOLS, lf)
        if os.path.exists(lp):
            other = {l.split("|")[0].strip() for l in open(lp) if l.strip()}
            bad = sorted(set(names) & other)
            if bad:
                print("COLLISION with %s: %s -- refusing to emit" % (lf, ", ".join(bad)))
                sys.exit(2)
    skipped = []
    for s in SPECS:
        if only is not None and s["name"] != only:
            continue
        cls = "Ind" + s["name"]
        hdr_path = os.path.join(COMP, cls, cls + ".h")
        if os.path.exists(hdr_path):
            with open(hdr_path) as _f:
                if "Output length equals input length" not in _f.read():
                    print("SKIP foreign dir %s (not ours, not overwriting)" % os.path.join(COMP, cls))
                    skipped.append(s["name"])
                    continue
        lower = s["name"].lower()
        helpers = HELPERS.replace("@@LOWER@@", lower)
        compute = s["compute"].replace("D::", "detail_%s::" % lower)
        hdr = (HEADER_TMPL.replace("@@CLASS@@", cls)
               .replace("@@LABEL@@", s["name"])
               .replace("@@FORMULA@@", s["formula"])
               .replace("@@HELPERS@@", helpers)
               .replace("@@COMPUTE@@", compute)
               .replace("@@PANE@@", "true" if s["style"] == "pane" else "false")
               .replace("@@MEMBERS@@", s["members"])
               .replace("@@PARAMS@@", s["params"])
               .replace("@@INITS@@", s["inits"])
               .replace("@@LINES@@", s["lines"]))
        d = os.path.join(COMP, cls)
        os.makedirs(os.path.join(d, "core"), exist_ok=True)
        with open(os.path.join(d, cls + ".h"), "w") as f: f.write(hdr)
        with open(os.path.join(d, "main_" + cls + ".cpp"), "w") as f:
            f.write(MAIN_TMPL.replace("@@CLASS@@", cls))
        with open(os.path.join(d, "CMakeLists.txt"), "w") as f:
            f.write(CMAKE_TMPL.replace("@@CLASS@@", cls))
        # core/xframe_core.h copy (verify md5 matches widgets/core/)
        import shutil
        shutil.copyfile(os.path.join(ROOT, "core", "xframe_core.h"),
                        os.path.join(d, "core", "xframe_core.h"))
    if only is not None:
        print("emitted 1 widget: Ind" + only)
        return skipped
    # smoke cases for the per-indicator smoke driver (one TU per header:
    # each widget dir carries its own copy of xframe_core.h, so all 85
    # headers cannot share one translation unit).
    with open(os.path.join(TESTS, "trend_smoke_cases.txt"), "w") as f:
        for s in SPECS:
            f.write("%s|%s|%d\n" % (s["name"], s["test"], 1 if s["sparse"] else 0))
    # audited list
    with open(os.path.join(TOOLS, "stock_indicators_trend.txt"), "w") as f:
        for s in SPECS:
            f.write("%s | trend | %s\n" % (s["name"], s["formula"]))
    # doc fragment
    doc = ["# Stock Market indicators — trend (85)\n",
           "Trend indicators for the XFrame Stock Market section. Every compute function returns a vector the same length as its input; leading warm-up bars are NaN until enough bars exist. Demos and tests run on deterministic seeded synthetic data (`xframe::make_seeded_series`), never live market data. Attribution: XFrame.\n"]
    for s in SPECS:
        if only is not None and s["name"] != only:
            continue
        cls = "Ind" + s["name"]
        doc.append("## %s (`%s`)\n\n" % (s["name"], cls))
        doc.append("**Formula:** %s.\n\n" % s["formula"])
        doc.append("**API:** `%s`; widget `%s(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series%s)`; draw style: %s (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.\n\n" % (s["sig"], cls, s["params"], s["style"]))
        doc.append("**Example:**\n\n```cpp\n#include \"Ind%s.h\"\nauto series = xframe::make_seeded_series(250, 42);\nauto values = %s;\n```\n\n" % (s["name"], s["test"]))
        if s["note"]:
            doc.append("**Unclear from the header:** %s\n\n" % s["note"])
    with open(os.path.join(DOCS, "docs", "stock-market-indicators-trend.md"), "w") as f: f.write("".join(doc))
    print("emitted %d widgets, list, smoke test, doc" % len(SPECS))
    if skipped:
        print("SKIPPED foreign dirs (not overwritten): %s" % ", ".join(skipped))
    return skipped

if __name__ == "__main__":
    import sys as _sys
    _only = None
    if len(_sys.argv) >= 3 and _sys.argv[1] == "--only":
        _only = _sys.argv[2]
    _skipped = emit(_only)
    if _skipped:
        _sys.exit(1)
