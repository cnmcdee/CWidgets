#pragma once
// StockRelativePerformance — XFrame Stock Market section.
// Each symbol rebased to 100 at its first close; lines show % performance vs the start.
// Attribution: XFrame.
#include "core/xframe_core.h"
#include "../StockData.h"
#include <X11/keysym.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

class StockRelativePerformance
{
private:
    Display* display_;
    int screen_;
    Window window_;
    GC gc_;
    SStyle_Manager* styles_;
    int x1_, y1_, x2_, y2_;
    std::string name_ = "StockRelativePerformance";
    std::string text_ = "StockRelativePerformance";
    std::string style_name_;
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    double view_first_ = 0.0;
    double view_count_ = 0.0;
    bool dragging_ = false;
    int drag_start_x_ = 0;
    double drag_start_first_ = 0.0;
    bool cross_valid_ = false;
    int cross_x_ = 0, cross_y_ = 0;
    static constexpr Srgb kPalette[8] = { {0,100,200},{200,30,30},{0,150,0},{230,126,34},{120,60,180},{0,150,150},{180,120,0},{100,100,100} };

    struct NamedSeries { std::string name; xframe::OHLCVSeries data; Srgb color{0, 100, 200}; };
    std::vector<NamedSeries> series_;

public:
    std::function<void(double, double)> on_view_change;
    StockRelativePerformance(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
        : display_(display), screen_(screen), window_(window), gc_(gc), styles_(styles), x1_(x), y1_(y), x2_(x + width), y2_(y + height)
    {
add_series("SYM-A", xframe::make_seeded_series(60, 42));
        add_series("SYM-B", xframe::make_seeded_series(60, 7));
        add_series("SYM-C", xframe::make_seeded_series(60, 99));
    }
    ~StockRelativePerformance() {}
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

    void add_series(const std::string& name, const xframe::OHLCVSeries& s) { NamedSeries ns; ns.name = name; ns.data = s; ns.color = kPalette[series_.size() % 8]; series_.push_back(ns); }
    void add_series(const std::string& name, const xframe::OHLCVSeries& s, Srgb color) { NamedSeries ns; ns.name = name; ns.data = s; ns.color = color; series_.push_back(ns); }
    void clear_series() { series_.clear(); view_first_ = 0.0; view_count_ = 0.0; }
    int series_count() { return (int)series_.size(); }
    int item_count() { size_t n = 0; for (auto& s : series_) n = std::max(n, s.data.size()); return (int)n; }
    std::vector<double> closes_of(int i) { return xframe::series_closes(series_[i].data); }

    std::vector<double> rebased(int series) { std::vector<double> out; if (series < 0 || series >= (int)series_.size() || series_[series].data.empty()) return out; double base = series_[series].data[0].close; if (base == 0) base = 1.0; for (auto& b : series_[series].data) out.push_back(b.close / base * 100.0); return out; }
    double performance_at(int series, int i) { auto r = rebased(series); if (i < 0 || i >= (int)r.size()) return 0.0; return r[i] - 100.0; }

    // ---- geometry / scale helpers ----
    int axis_w_ = 56, axis_h_ = 20;
    int plot_x() { return x1_ + axis_w_; }
    int plot_y() { return y1_ + 6; }
    int plot_w() { return width() - axis_w_ - 8; }
    int plot_h() { return height() - axis_h_ - 12; }
    double effective_count() { double t = (double)item_count(); if (view_count_ <= 0 || view_count_ > t) return t; return view_count_; }
    double item_w() { double c = effective_count(); return c > 0 ? plot_w() / c : (double)plot_w(); }
    int x_at(double i) { return plot_x() + (int)((i - view_first_) * item_w()); }
    int x_center(int i) { return plot_x() + (int)((i - view_first_ + 0.5) * item_w()); }
    int y_at(double v, double lo, double hi) { if (hi <= lo) return plot_y() + plot_h() / 2; return plot_y() + (int)((hi - v) / (hi - lo) * plot_h()); }
    static double nice_step(double range, int target) { if (range <= 0) return 1.0; double raw = range / target; double mag = std::pow(10.0, std::floor(std::log10(raw))); double norm = raw / mag; double step = (norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0; return step * mag; }
    static std::string fmt(double v) { std::ostringstream o; o.precision(2); o << std::fixed << v; std::string s = o.str(); while (!s.empty() && s.back() == '0') s.pop_back(); if (!s.empty() && s.back() == '.') s.pop_back(); return s.empty() ? "0" : s; }
    void fire_view_change() { if (on_view_change) on_view_change(view_first_, effective_count()); }
    void reset_view() { view_first_ = 0.0; view_count_ = (double)item_count(); fire_view_change(); }
    void zoom_at(double f, double frac) { double t = (double)item_count(); if (t <= 0) return; double c = effective_count(); double idx = view_first_ + frac * c; double nc = std::max(5.0, std::min(t, c * f)); view_first_ = std::max(0.0, std::min(t - nc, idx - frac * nc)); view_count_ = nc; fire_view_change(); }
    void draw_grid(double lo, double hi) {
        SStyle st = style();
        unsigned long grid = styles_->get_color_rgb(225, 225, 225);
        double step = nice_step(hi - lo, 6); if (step <= 0) step = 1.0;
        double start = std::ceil(lo / step) * step;
        for (double v = start; v <= hi + 1e-9; v += step) {
            int gy = y_at(v, lo, hi);
            XSetForeground(display_, gc_, grid);
            XDrawLine(display_, window_, gc_, plot_x(), gy, plot_x() + plot_w(), gy);
            std::string lab = fmt(v);
            int tw = xf::text_width(st, lab);
            xf::draw_text_at(display_, window_, gc_, st, plot_x() - tw - 6, gy + 4, lab);
        }
    }
    void draw_legend(const std::vector<std::pair<std::string, Srgb>>& items) {
        SStyle st = style();
        int lx = plot_x() + 4, ly = plot_y() + 4;
        for (auto& it : items) {
            unsigned long c = styles_->get_color_Srgb(it.second);
            XSetForeground(display_, gc_, c);
            XFillRectangle(display_, window_, gc_, lx, ly, 12, 3);
            xf::draw_text_at(display_, window_, gc_, st, lx + 16, ly + 11, it.first);
            lx += 16 + xf::text_width(st, it.first) + 18;
        }
    }
    void draw_border() { SStyle st = style(); XSetForeground(display_, gc_, st.lineColor_); XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height()); }
    void draw_crosshair(double lo, double hi) {
        if (!cross_valid_) return;
        SStyle st = style();
        unsigned long c = styles_->get_color_rgb(150, 150, 150);
        XSetForeground(display_, gc_, c);
        XDrawLine(display_, window_, gc_, cross_x_, plot_y(), cross_x_, plot_y() + plot_h());
        XDrawLine(display_, window_, gc_, plot_x(), cross_y_, plot_x() + plot_w(), cross_y_);
        (void)st; (void)lo; (void)hi;
    }
    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        double lo = 1e300, hi = -1e300;
        std::vector<std::vector<double>> reb;
        for (int k = 0; k < (int)series_.size(); ++k) { reb.push_back(rebased(k)); for (double v : reb.back()) { lo = std::min(lo, v); hi = std::max(hi, v); } }
        if (series_.empty()) { lo = 0; hi = 1; } if (hi <= lo) hi = lo + 1;
        draw_grid(lo, hi);
        int i0 = (int)std::floor(view_first_), i1 = (int)std::ceil(view_first_ + effective_count());
        if (lo < 100 && hi > 100) { unsigned long z = styles_->get_color_rgb(180, 180, 180); XSetForeground(display_, gc_, z); XDrawLine(display_, window_, gc_, plot_x(), y_at(100, lo, hi), plot_x() + plot_w(), y_at(100, lo, hi)); }
        for (int k = 0; k < (int)series_.size(); ++k) {
            unsigned long c = styles_->get_color_Srgb(series_[k].color);
            XSetForeground(display_, gc_, c);
            std::vector<XPoint> pts;
            for (int i = i0; i < i1 && i < (int)reb[k].size(); ++i) { if (i < 0) continue; XPoint p; p.x = (short)x_center(i); p.y = (short)y_at(reb[k][i], lo, hi); pts.push_back(p); }
            if (pts.size() > 1) XDrawLines(display_, window_, gc_, pts.data(), (int)pts.size(), CoordModeOrigin);
        }
        std::vector<std::pair<std::string, Srgb>> leg; for (auto& s : series_) leg.push_back({s.name, s.color});
        draw_legend(leg);
        draw_crosshair(lo, hi);
        draw_border();
        XFlush(display_);
    }

    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        double frac = plot_w() > 0 ? std::max(0.0, std::min(1.0, (double)(x - plot_x()) / plot_w())) : 0.0;
        if (button == 4) { zoom_at(0.8, frac); return true; }
        if (button == 5) { zoom_at(1.25, frac); return true; }
        if (button == 1) { dragging_ = true; drag_start_x_ = x; drag_start_first_ = view_first_; cross_valid_ = true; cross_x_ = x; cross_y_ = y; return true; }
        return false;
    }
    bool on_button_release(int x, int y, unsigned int button) { (void)x; (void)y; if (button == 1 && dragging_) { dragging_ = false; return true; } return false; }
    bool on_motion(int x, int y) {
        cross_valid_ = true; cross_x_ = x; cross_y_ = y;
        if (dragging_) { double t = (double)item_count(); double c = effective_count(); double d = -(x - drag_start_x_) / item_w(); view_first_ = std::max(0.0, std::min(std::max(0.0, t - c), drag_start_first_ + d)); fire_view_change(); }
        return true;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        double t = (double)item_count(), c = effective_count();
        switch (key) {
        case XK_Left: view_first_ = std::max(0.0, view_first_ - c * 0.1); fire_view_change(); return true;
        case XK_Right: view_first_ = std::min(std::max(0.0, t - c), view_first_ + c * 0.1); fire_view_change(); return true;
        case XK_plus: case XK_equal: zoom_at(0.8, 0.5); return true;
        case XK_minus: zoom_at(1.25, 0.5); return true;
        case XK_Home: case XK_r: case XK_R: reset_view(); return true;
        }
        return false;
    }
};
