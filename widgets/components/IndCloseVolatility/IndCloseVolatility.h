#pragma once
// IndCloseVolatility -- CloseVolatility (volatility indicator).
// Formula: Close Volatility = 100 * StdDev(log(close / previous close), 20), unannualised, in percent.
// Output: one value per input bar; leading warm-up bars are NaN until
// enough bars exist (see the docs entry). Attribution: XFrame.
#include "core/xframe_core.h"
#include "../StockData.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace xframe {

namespace detail_close_volatility {
inline double nanv() { return std::numeric_limits<double>::quiet_NaN(); }
inline bool fin(double v) { return std::isfinite(v); }
inline std::vector<double> sma(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (p <= 1) return v;
    double s = 0.0;
    for (size_t i = 0; i < v.size(); ++i) {
        s += v[i];
        if (i >= (size_t)p) s -= v[i - (size_t)p];
        if (i >= (size_t)(p - 1)) o[i] = s / p;
    }
    return o;
}
inline std::vector<double> ema(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (v.empty()) return o;
    double k = 2.0 / (p + 1), e = v[0];
    for (size_t i = 0; i < v.size(); ++i) {
        e = (i == 0) ? v[0] : e + k * (v[i] - e);
        if (i >= (size_t)(p - 1)) o[i] = e;
    }
    return o;
}
inline std::vector<double> ema_wilder(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (v.empty()) return o;
    double s = 0.0; double e = 0.0;
    for (size_t i = 0; i < v.size(); ++i) {
        s += v[i];
        if (i == (size_t)(p - 1)) e = s / p;
        else if (i >= (size_t)p) e = e + (v[i] - e) / p;
        if (i >= (size_t)(p - 1)) o[i] = e;
    }
    return o;
}
inline std::vector<double> stddev(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    std::vector<double> m = sma(v, p);
    std::vector<double> sq(v.size());
    for (size_t i = 0; i < v.size(); ++i) sq[i] = v[i] * v[i];
    std::vector<double> ms = sma(sq, p);
    for (size_t i = 0; i < v.size(); ++i)
        if (fin(m[i])) { double var = ms[i] - m[i] * m[i]; o[i] = std::sqrt(var > 0 ? var : 0.0); }
    return o;
}
inline std::vector<double> tr(const OHLCVSeries& s) {
    std::vector<double> o(s.size(), nanv());
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 0) o[i] = s[i].high - s[i].low;
        else o[i] = std::max({s[i].high - s[i].low, std::fabs(s[i].high - s[i - 1].close),
                              std::fabs(s[i].low - s[i - 1].close)});
    }
    return o;
}
inline std::vector<double> atr(const OHLCVSeries& s, int p) {
    return ema_wilder(tr(s), p);
}
inline std::vector<double> hh(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    for (size_t i = 0; i < v.size(); ++i) {
        if (i < (size_t)(p - 1)) continue;
        double m = v[i];
        for (int k = 1; k < p; ++k) m = std::max(m, v[i - (size_t)k]);
        o[i] = m;
    }
    return o;
}
inline std::vector<double> ll(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    for (size_t i = 0; i < v.size(); ++i) {
        if (i < (size_t)(p - 1)) continue;
        double m = v[i];
        for (int k = 1; k < p; ++k) m = std::min(m, v[i - (size_t)k]);
        o[i] = m;
    }
    return o;
}
inline std::vector<double> logret(const std::vector<double>& c) {
    std::vector<double> o(c.size(), 0.0);
    for (size_t i = 1; i < c.size(); ++i)
        if (c[i] > 0 && c[i - 1] > 0) o[i] = std::log(c[i] / c[i - 1]);
    return o;
}
}  // namespace detail_close_volatility


std::vector<double> ind_close_volatility(const OHLCVSeries& series, int period = 20) {
    using namespace detail_close_volatility;
    std::vector<double> lr = logret(series_closes(series));
    std::vector<double> sd = stddev(lr, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(sd[i])) o[i] = 100.0 * sd[i];
    return o;
}

class IndCloseVolatility {
private:
    Display* display_;
    int screen_;
    Window window_;
    GC gc_;
    SStyle_Manager* styles_;
    int x1_, y1_, x2_, y2_;
    std::string name_ = "IndCloseVolatility";
    std::string text_ = "CloseVolatility";
    std::string style_name_;
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    struct Line { std::string name; Srgb color; std::vector<double> v; };
    std::vector<Line> lines_;
    OHLCVSeries series_;
    double view_first_ = 0.0;
    double zoom_ = 1.0;
    bool dragging_ = false;
    int drag_start_x_ = 0;
    double drag_start_first_ = 0.0;
    bool cross_valid_ = false;
    int cross_x_ = 0, cross_y_ = 0;
    int period_;
    double mult_;
public:
    std::function<void(double, double)> on_view_change;
    IndCloseVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250),
        int period = 20, double mult = 2.0)
        : display_(display), screen_(screen), window_(window), gc_(gc), styles_(styles),
          x1_(x), y1_(y), x2_(x + width), y2_(y + height), series_(series),
          period_(period), mult_(mult) {
    auto result = ind_close_volatility(series, period);
    lines_.push_back(Line{"CloseVol", Srgb{150, 0, 150}, result});
    }
    ~IndCloseVolatility() {}
    void set_bounds(int x, int y, int width, int height) { x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height; }
    int width() { return x2_ - x1_; }
    int height() { return y2_ - y1_; }
    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }
    void set_visible(bool v) { visible_ = v; }
    bool get_visible() { return visible_; }
    void set_enabled(bool e) { enabled_ = e; }
    bool get_enabled() { return enabled_; }
    void set_style_name(std::string s) { style_name_ = s; }
    void set_text(std::string t) { text_ = t; }
    std::string get_text() { return text_; }
    SStyle style() { return styles_->resolve(style_name_); }
    int item_count() { return (int)series_.size(); }
    int line_count() { return (int)lines_.size(); }
    const std::vector<double>& line_values(int i) { return lines_[i].v; }
    double get_view_first() { return view_first_; }
    double get_zoom() { return zoom_; }
    void reset_view() { view_first_ = 0.0; zoom_ = 1.0; if (on_view_change) on_view_change(0.0, (double)item_count()); }
    int plot_x() { return x1_ + 56; }
    int plot_y() { return y1_ + 6; }
    int plot_w() { return width() - 56 - 8; }
    int plot_h() { return height() - 20 - 12; }
    double visible_count() { double n = (double)item_count(); return n <= 0 ? 1.0 : std::max(5.0, n / zoom_); }
    double item_w() { return plot_w() / visible_count(); }
    int x_center(int i) { return plot_x() + (int)((i - view_first_ + 0.5) * item_w()); }
    int y_at(double v, double lo, double hi) { return plot_y() + (int)((hi - v) / (hi - lo) * plot_h()); }
    static std::string fmt(double v) {
        std::ostringstream oss; oss.precision(2); oss << std::fixed << v;
        std::string s = oss.str();
        while (!s.empty() && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
        return s;
    }
    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long grid = styles_->get_color_rgb(225, 225, 225);
        unsigned long axis_col = styles_->get_color_rgb(150, 150, 150);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        double lo = 1e300, hi = -1e300;
        for (auto& ln : lines_) for (double v : ln.v)
            if (std::isfinite(v)) { lo = std::min(lo, v); hi = std::max(hi, v); }
        if (false) {
            for (auto& b : series_) { lo = std::min(lo, b.low); hi = std::max(hi, b.high); }
        }
        if (lo > hi) { lo = 0.0; hi = 1.0; }
        double pad = (hi - lo) * 0.06 + 1e-9; lo -= pad; hi += pad;
        int px = plot_x(), py = plot_y(), pw = plot_w(), ph = plot_h();
        double step = XGraph_nice_step(hi - lo, 6);
        double start = std::ceil(lo / step) * step;
        for (double v = start; v <= hi; v += step) {
            int gy = y_at(v, lo, hi);
            XSetForeground(display_, gc_, grid);
            XDrawLine(display_, window_, gc_, px, gy, px + pw, gy);
            std::string lab = fmt(v);
            int tw = xf::text_width(st, lab);
            xf::draw_text_at(display_, window_, gc_, st, px - tw - 6, gy + 4, lab);
        }
        if (false) {
            int bw = std::max(1, (int)(item_w() * 0.6));
            for (int i = 0; i < (int)series_.size(); ++i) {
                int cx = x_center(i);
                if (cx < px || cx > px + pw) continue;
                const OHLCV& b = series_[i];
                unsigned long c = styles_->get_color_rgb(b.close >= b.open ? 0 : 200, b.close >= b.open ? 150 : 30, b.close >= b.open ? 0 : 30);
                XSetForeground(display_, gc_, c);
                XDrawLine(display_, window_, gc_, cx, y_at(b.high, lo, hi), cx, y_at(b.low, lo, hi));
                int top = y_at(std::max(b.open, b.close), lo, hi);
                int bot = y_at(std::min(b.open, b.close), lo, hi);
                XFillRectangle(display_, window_, gc_, cx - bw / 2, top, bw, std::max(1, bot - top));
            }
        }
        for (auto& ln : lines_) {
            unsigned long col = styles_->get_color_Srgb(ln.color);
            XSetForeground(display_, gc_, col);
            xf::set_line_width(display_, gc_, 2);
            std::vector<XPoint> pts;
            auto flush = [&]() {
                if (pts.size() > 1) XDrawLines(display_, window_, gc_, pts.data(), (int)pts.size(), CoordModeOrigin);
                pts.clear();
            };
            for (int i = 0; i < (int)ln.v.size(); ++i) {
                if (!std::isfinite(ln.v[i])) { flush(); continue; }
                XPoint p; p.x = (short)x_center(i); p.y = (short)y_at(ln.v[i], lo, hi);
                pts.push_back(p);
            }
            flush();
            xf::set_line_width(display_, gc_, 1);
        }
        XSetForeground(display_, gc_, axis_col);
        XDrawLine(display_, window_, gc_, px, py + ph, px + pw, py + ph);
        XDrawRectangle(display_, window_, gc_, px, py, pw, ph);
        {   int lx = px + 8, ly = py + 8;
            for (auto& ln : lines_) {
                XSetForeground(display_, gc_, styles_->get_color_Srgb(ln.color));
                xf::set_line_width(display_, gc_, 2);
                XDrawLine(display_, window_, gc_, lx, ly + 4, lx + 18, ly + 4);
                xf::set_line_width(display_, gc_, 1);
                std::string last;
                for (int i = (int)ln.v.size() - 1; i >= 0; --i)
                    if (std::isfinite(ln.v[i])) { last = fmt(ln.v[i]); break; }
                xf::draw_text_at(display_, window_, gc_, st, lx + 24, ly + 8, ln.name + (last.empty() ? "" : " " + last));
                ly += 16;
            }
            xf::draw_text_at(display_, window_, gc_, st, lx, ly + 8, text_);
        }
        if (cross_valid_ && cross_x_ >= px && cross_x_ <= px + pw && cross_y_ >= py && cross_y_ <= py + ph) {
            XGCValues gv; gv.line_style = LineOnOffDash;
            XChangeGC(display_, gc_, GCLineStyle, &gv);
            XSetForeground(display_, gc_, axis_col);
            XDrawLine(display_, window_, gc_, cross_x_, py, cross_x_, py + ph);
            XDrawLine(display_, window_, gc_, px, cross_y_, px + pw, cross_y_);
            gv.line_style = LineSolid;
            XChangeGC(display_, gc_, GCLineStyle, &gv);
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    static double XGraph_nice_step(double range, int target) {
        double raw = range / target;
        double mag = std::pow(10.0, std::floor(std::log10(raw)));
        double norm = raw / mag;
        double step = (norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0;
        return step * mag;
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        double frac = plot_w() > 0 ? std::max(0.0, std::min(1.0, (double)(x - plot_x()) / plot_w())) : 0.0;
        if (button == 4) { zoom_ = std::min(8.0, zoom_ * 1.25); return true; }
        if (button == 5) { zoom_ = std::max(1.0, zoom_ / 1.25); return true; }
        if (button == 1) { dragging_ = true; drag_start_x_ = x; drag_start_first_ = view_first_; return true; }
        (void)frac;
        return false;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y;
        if (button == 1 && dragging_) { dragging_ = false; return true; }
        return false;
    }
    bool on_motion(int x, int y) {
        cross_valid_ = true; cross_x_ = x; cross_y_ = y;
        if (dragging_) {
            double total = (double)item_count(), count = visible_count();
            double delta = -(x - drag_start_x_) / item_w();
            view_first_ = std::max(0.0, std::min(std::max(0.0, total - count), drag_start_first_ + delta));
        }
        return true;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        double total = (double)item_count(), count = visible_count();
        switch (key) {
        case XK_Left: view_first_ = std::max(0.0, view_first_ - count * 0.1); return true;
        case XK_Right: view_first_ = std::min(std::max(0.0, total - count), view_first_ + count * 0.1); return true;
        case XK_plus: case XK_equal: zoom_ = std::min(8.0, zoom_ * 1.25); return true;
        case XK_minus: zoom_ = std::max(1.0, zoom_ / 1.25); return true;
        case XK_Home: case XK_r: case XK_R: reset_view(); return true;
        }
        return false;
    }
};

}  // namespace xframe
