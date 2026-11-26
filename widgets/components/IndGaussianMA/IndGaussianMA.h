#pragma once
#include "core/xframe_core.h"
#include "../StockData.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
// IndGaussianMA — XFrame Stock Market trend indicator widget.
// Gaussian-weighted moving average, sigma period/6 centred on the window
// Output length equals input length; leading warm-up bars are NaN.
// Attribution: XFrame.
namespace xframe {

namespace detail_gaussianma {
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
}  // namespace detail_gaussianma

std::vector<double> ind_gaussian_ma(const std::vector<double>& closes, int period) { std::vector<double> o(closes.size(), detail_gaussianma::nan_v()); if (period < 1) period = 1; double sigma = period / 6.0, centre = (period - 1) / 2.0, ws = 0.0; std::vector<double> w((size_t)period); for (int i = 0; i < period; ++i) { w[(size_t)i] = std::exp(-0.5 * ((i - centre) * (i - centre)) / (sigma * sigma)); ws += w[(size_t)i]; } for (size_t i = 0; i < closes.size(); ++i) { if (i + 1 < (size_t)period) continue; double s = 0.0; for (int j = 0; j < period; ++j) s += closes[i - (size_t)(period - 1) + (size_t)j] * w[(size_t)j]; o[i] = s / ws; } return o; }
class IndGaussianMA {
private:
    Display* display_; int screen_; Window window_; GC gc_;
    SStyle_Manager* styles_; int x1_, y1_, x2_, y2_;
    std::string name_ = "IndGaussianMA"; std::string text_ = "GaussianMA";
    std::string style_name_; bool visible_ = true; bool enabled_ = true; bool focused_ = false;
    OHLCVSeries series_;
    std::vector<std::vector<double>> lines_;
    std::vector<std::string> line_names_;
    std::vector<Srgb> line_colors_;
    int selected_ = -1; int hover_ = -1;
    bool pane_ = false;
    int axis_w_ = 56; int axis_h_ = 20;
    int period_ = 14;

public:
    IndGaussianMA(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = 14)
        : display_(display), screen_(screen), window_(window), gc_(gc), styles_(styles),
          x1_(x), y1_(y), x2_(x + width), y2_(y + height), series_(series) {
        period_ = period;

        lines_.push_back(ind_gaussian_ma(series_closes(series), period_));
        line_names_.push_back("GaussianMA(14)");
        line_colors_.push_back(Srgb{150, 90, 30});

    }
    ~IndGaussianMA() {}
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
