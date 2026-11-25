#pragma once

// IndCenterOfGravity.h -- CenterOfGravity momentum indicator widget (XFrame Stock Market section).
// Formula: Ehlers Center of Gravity: negative weighted average of price over period (weights 1..period, most recent heaviest), divided by the sum of prices; oscillates around zero with almost no lag.
// Output: output length == input length; leading warm-up bars are NaN until enough bars exist.
// Draw style: pane.
// Attribution: XFrame.

#include "core/xframe_core.h"
#include "../StockData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <string>
#include <vector>

namespace xframe {

namespace ind_ind_center_of_gravity_detail {
inline double nanv() { return std::numeric_limits<double>::quiet_NaN(); }
inline bool fin(double v) { return std::isfinite(v); }
inline std::vector<double> sma(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (p <= 0) return o;
    if (p == 1) return v;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i < (std::size_t)(p - 1)) continue;
        double s = 0.0;
        bool ok = true;
        for (int k = 0; k < p; ++k) {
            if (!fin(v[i - (std::size_t)k])) ok = false;
            else s += v[i - (std::size_t)k];
        }
        if (ok) o[i] = s / p;
    }
    return o;
}
inline std::vector<double> ema(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (p <= 0) return o;
    if (p == 1) return v;
    const double a = 2.0 / (p + 1.0);
    double prev = 0.0; bool have = false; int run = 0; double s = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (!fin(v[i])) { run = 0; s = 0.0; have = false; continue; }
        s += v[i]; ++run;
        if (!have && run >= p) { prev = s / p; o[i] = prev; have = true; }
        else if (have) { prev = a * v[i] + (1.0 - a) * prev; o[i] = prev; }
    }
    return o;
}
inline std::vector<double> wilder(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (p <= 0) return o;
    const double a = 1.0 / p;
    double prev = 0.0; bool have = false; int run = 0; double s = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (!fin(v[i])) { run = 0; s = 0.0; have = false; continue; }
        s += v[i]; ++run;
        if (!have && run >= p) { prev = s / p; o[i] = prev; have = true; }
        else if (have) { prev = a * v[i] + (1.0 - a) * prev; o[i] = prev; }
    }
    return o;
}
inline std::vector<double> stddev(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (p <= 1) return o;
    std::vector<double> m = sma(v, p);
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (!fin(m[i])) continue;
        bool ok = true;
        for (int k = 0; k < p; ++k) if (!fin(v[i - (std::size_t)k])) ok = false;
        if (!ok) continue;
        double s = 0.0;
        for (int k = 0; k < p; ++k) { double d = v[i - (std::size_t)k] - m[i]; s += d * d; }
        o[i] = std::sqrt(s / p);
    }
    return o;
}
inline double highest(const std::vector<double>& v, std::size_t i, int p) {
    double m = -1e300; for (int k = 0; k < p && i >= (std::size_t)k; ++k) m = std::max(m, v[i - (std::size_t)k]); return m;
}
inline double lowest(const std::vector<double>& v, std::size_t i, int p) {
    double m = 1e300; for (int k = 0; k < p && i >= (std::size_t)k; ++k) m = std::min(m, v[i - (std::size_t)k]); return m;
}
inline std::vector<double> rsi_series(const std::vector<double>& c, int p) {
    const std::size_t n = c.size();
    std::vector<double> o(n, nanv());
    if (n < (std::size_t)(p + 1) || p <= 0) return o;
    std::vector<double> g(n, 0.0), l(n, 0.0);
    for (std::size_t i = 1; i < n; ++i) { double d = c[i] - c[i-1]; g[i] = d > 0 ? d : 0.0; l[i] = d < 0 ? -d : 0.0; }
    std::vector<double> ag = wilder(g, p), al = wilder(l, p);
    for (std::size_t i = 0; i < n; ++i) {
        if (!fin(ag[i]) || !fin(al[i])) continue;
        double den = ag[i] + al[i];
        o[i] = den == 0.0 ? 50.0 : 100.0 * ag[i] / den;
    }
    return o;
}
inline std::vector<double> stoch_k(const std::vector<double>& h, const std::vector<double>& l,
                                    const std::vector<double>& c, int p) {
    std::vector<double> o(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) {
        if (i < (std::size_t)(p - 1)) continue;
        double hh = highest(h, i, p), ll = lowest(l, i, p);
        if (hh == ll) { o[i] = 50.0; continue; }
        o[i] = 100.0 * (c[i] - ll) / (hh - ll);
    }
    return o;
}
inline double linreg_slope_at(const std::vector<double>& v, std::size_t i, int p) {
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (int k = 0; k < p; ++k) { double x = (double)k, y = v[i - (std::size_t)(p - 1) + (std::size_t)k]; sx += x; sy += y; sxx += x*x; sxy += x*y; }
    double den = (double)p * sxx - sx * sx;
    return den == 0.0 ? 0.0 : ((double)p * sxy - sx * sy) / den;
}
inline double linreg_value_at(const std::vector<double>& v, std::size_t i, int p) {
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (int k = 0; k < p; ++k) { double x = (double)k, y = v[i - (std::size_t)(p - 1) + (std::size_t)k]; sx += x; sy += y; sxx += x*x; sxy += x*y; }
    double den = (double)p * sxx - sx * sx;
    if (den == 0.0) return sy / p;
    double slope = ((double)p * sxy - sx * sy) / den;
    double inter = (sy - slope * sx) / p;
    return inter + slope * (p - 1);
}
inline double linreg_r2_at(const std::vector<double>& v, std::size_t i, int p) {
    double mean = 0; for (int k = 0; k < p; ++k) mean += v[i - (std::size_t)(p - 1) + (std::size_t)k]; mean /= p;
    double ss_tot = 0, ss_res = 0;
    double slope = linreg_slope_at(v, i, p);
    double inter = mean - slope * (p - 1) / 2.0;
    for (int k = 0; k < p; ++k) { double y = v[i - (std::size_t)(p - 1) + (std::size_t)k]; double fit = inter + slope * k; ss_tot += (y - mean)*(y - mean); ss_res += (y - fit)*(y - fit); }
    return ss_tot == 0.0 ? 0.0 : 1.0 - ss_res / ss_tot;
}
inline std::vector<double> super_smoother(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (p < 2) return v;
    const double pi = 3.141592653589793;
    const double a1 = std::exp(-1.414 * pi / p);
    const double b1 = 2.0 * a1 * std::cos(1.414 * pi / p);
    const double c2 = b1, c3 = -a1 * a1, c1 = 1.0 - c2 - c3;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (!fin(v[i])) continue;
        double pm1 = (i >= 1 && fin(v[i-1])) ? v[i-1] : v[i];
        double o1 = (i >= 1 && fin(o[i-1])) ? o[i-1] : 0.0;
        double o2 = (i >= 2 && fin(o[i-2])) ? o[i-2] : 0.0;
        if (i < 2) { o[i] = v[i]; continue; }
        o[i] = c1 * (v[i] + pm1) / 2.0 + c2 * o1 + c3 * o2;
    }
    return o;
}
}  // namespace ind_ind_center_of_gravity_detail

std::vector<double> ind_center_of_gravity(const std::vector<double>& closes, int period = 10)
{
    const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period - 1); i < n; ++i) {
        double num = 0.0, den = 0.0;
        for (int k = 0; k < period; ++k) {
            double w = (double)(k + 1);
            num += w * closes[i - (std::size_t)k];
            den += closes[i - (std::size_t)k];
        }
        out[i] = den == 0.0 ? 0.0 : -num / den;
    }
    return out;
}

}  // namespace xframe

// IndCenterOfGravity widget: follows the XGraph pattern (Display*/screen_/window_/GC,
// geometry, on_click-style interaction). Constructor takes an OHLCVSeries plus
// indicator parameters and draws the indicator (pane style).
class IndCenterOfGravity
{
private:
    Display* display_;
    int screen_;
    Window window_;
    GC gc_;
    SStyle_Manager* styles_;
    int x1_, y1_, x2_, y2_;
    std::string name_;
    std::string text_;
    std::string style_name_;
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    xframe::OHLCVSeries series_;
    std::vector<std::vector<double> > lines_;
    std::vector<std::string> line_names_;
    std::vector<Srgb> line_colors_;
    double view_first_ = 0.0;
    double view_count_ = 0.0;
    bool cross_valid_ = false;
    int cross_x_ = 0;
    int cross_y_ = 0;

public:
    IndCenterOfGravity(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
            int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)
    {
        display_ = display;
        screen_ = screen;
        window_ = window;
        gc_ = gc;
        styles_ = styles;
        x1_ = x;
        y1_ = y;
        x2_ = x + width;
        y2_ = y + height;
        name_ = "IndCenterOfGravity";
        text_ = "CenterOfGravity";
        series_ = series;
        std::vector<double> result = xframe::ind_center_of_gravity(xframe::series_closes(series), period);
        lines_.push_back(result);
        line_names_.push_back("CenterOfGravity");
        line_colors_.push_back(Srgb{0, 100, 200});

        view_count_ = (double)item_count();
    }

    ~IndCenterOfGravity() {}

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
    const std::vector<double>& line_values(int i) { return lines_.at((std::size_t)i); }
    const std::vector<std::vector<double> >& values() { return lines_; }

    void reset_view() { view_first_ = 0.0; view_count_ = (double)item_count(); }
    double get_view_first() { return view_first_; }
    double get_view_count() { return view_count_ > 0 ? view_count_ : (double)item_count(); }

    void draw()
    {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long grid = styles_->get_color_rgb(225, 225, 225);
        unsigned long axis_col = styles_->get_color_rgb(150, 150, 150);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int px = x1_ + 56, py = y1_ + 6;
        int pw = width() - 64, ph = height() - 32;
        if (pw < 10 || ph < 10) return;
        double lo = 1e300, hi = -1e300;
        int total = item_count();
        int i0 = (int)std::floor(view_first_);
        int i1 = (int)std::ceil(view_first_ + get_view_count());
        if (i0 < 0) i0 = 0;
        if (i1 > total) i1 = total;

        for (auto& line : lines_) {
            for (int i = i0; i < i1; ++i) {
                if (i >= 0 && i < (int)line.size() && std::isfinite(line[(std::size_t)i])) {
                    lo = std::min(lo, line[(std::size_t)i]);
                    hi = std::max(hi, line[(std::size_t)i]);
                }
            }
        }
        if (lo > hi) { lo = 0.0; hi = 1.0; }
        if (hi - lo < 1e-12) { hi = lo + 1.0; }
        double pad = (hi - lo) * 0.06 + 1e-9;
        lo -= pad;
        hi += pad;
        auto yat = [&](double v) { return py + (int)((hi - v) / (hi - lo) * ph); };
        auto xat = [&](int i) { return px + (int)(((double)i - view_first_) / get_view_count() * pw); };
        for (int g = 0; g <= 4; ++g) {
            double v = lo + (hi - lo) * g / 4.0;
            int gy = yat(v);
            XSetForeground(display_, gc_, grid);
            XDrawLine(display_, window_, gc_, px, gy, px + pw, gy);
            std::string lab = std::to_string((int)std::round(v));
            xf::draw_text_at(display_, window_, gc_, st, px - 52, gy + 4, lab);
        }
        if (lo < 0.0 && hi > 0.0) {
            XSetForeground(display_, gc_, axis_col);
            XDrawLine(display_, window_, gc_, px, yat(0.0), px + pw, yat(0.0));
        }
        for (std::size_t li = 0; li < lines_.size(); ++li) {
            XSetForeground(display_, gc_, styles_->get_color_Srgb(line_colors_[li]));
            xf::set_line_width(display_, gc_, 2);
            for (int i = i0 + 1; i < i1; ++i) {
                if (i < 1 || i >= (int)lines_[li].size()) continue;
                double a = lines_[li][(std::size_t)(i - 1)], b = lines_[li][(std::size_t)i];
                if (!std::isfinite(a) || !std::isfinite(b)) continue;
                XDrawLine(display_, window_, gc_, xat(i - 1), yat(a), xat(i), yat(b));
            }
            xf::set_line_width(display_, gc_, 1);
        }
        XSetForeground(display_, gc_, axis_col);
        XDrawRectangle(display_, window_, gc_, px, py, pw, ph);
        {
            int lx = px + 8, ly = py + 8;
            for (std::size_t li = 0; li < lines_.size(); ++li) {
                XSetForeground(display_, gc_, styles_->get_color_Srgb(line_colors_[li]));
                XDrawLine(display_, window_, gc_, lx, ly + 4, lx + 18, ly + 4);
                std::string last;
                for (int i = (int)lines_[li].size() - 1; i >= 0; --i) {
                    if (std::isfinite(lines_[li][(std::size_t)i])) { last = std::to_string(lines_[li][(std::size_t)i]); break; }
                }
                xf::draw_text_at(display_, window_, gc_, st, lx + 24, ly + 8, line_names_[li] + (last.empty() ? "" : " " + last));
                ly += 16;
            }
            xf::draw_text_at(display_, window_, gc_, st, px + pw - 120, py + ph + 16, text_);
        }
        if (cross_valid_ && cross_x_ >= px && cross_x_ <= px + pw && cross_y_ >= py && cross_y_ <= py + ph) {
            XSetForeground(display_, gc_, axis_col);
            XDrawLine(display_, window_, gc_, cross_x_, py, cross_x_, py + ph);
            XDrawLine(display_, window_, gc_, px, cross_y_, px + pw, cross_y_);
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }

    bool on_button_press(int x, int y, unsigned int button)
    {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        if (button == 4) { view_count_ = std::max(5.0, get_view_count() * 0.8); return true; }
        if (button == 5) { view_count_ = std::min((double)item_count(), get_view_count() * 1.25); return true; }
        if (button == 1) return true;
        return false;
    }

    bool on_button_release(int x, int y, unsigned int button)
    {
        (void)x; (void)y;
        return button == 1 && focused_;
    }

    bool on_motion(int x, int y)
    {
        cross_valid_ = true;
        cross_x_ = x;
        cross_y_ = y;
        return contains(x, y);
    }

    bool on_key(KeySym key, const char* str)
    {
        (void)str;
        if (!focused_ || !enabled_) return false;
        double total = (double)item_count();
        double count = get_view_count();
        switch (key) {
        case XK_Left:
            view_first_ = std::max(0.0, view_first_ - count * 0.1);
            return true;
        case XK_Right:
            view_first_ = std::min(std::max(0.0, total - count), view_first_ + count * 0.1);
            return true;
        case XK_Home:
        case XK_r:
        case XK_R:
            reset_view();
            return true;
        }
        return false;
    }
};
