#pragma once

#include "core/xframe_core.h"
#include "../StockData.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

// // StockCandlestickChart - one filled candle per OHLCV bar: wick high-low,
// body open-close, green when close >= open, red otherwise.
// Attribution: XFrame.
class StockCandlestickChart {
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
    double view_first_ = 0.0;
    double view_count_ = 0.0;
    bool auto_y_ = true;
    double y_min_ = 0.0;
    double y_max_ = 1.0;
    int axis_w_ = 56;
    int axis_h_ = 20;
    bool dragging_ = false;
    int drag_start_x_ = 0;
    double drag_start_first_ = 0.0;
    bool cross_valid_ = false;
    int cross_x_ = 0;
    int cross_y_ = 0;

public:
    StockCandlestickChart(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height,
        const xframe::OHLCVSeries& series = xframe::make_seeded_series(250)) {
        display_ = display; screen_ = screen; window_ = window; gc_ = gc; styles_ = styles;
        x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height;
        name_ = "StockCandlestickChart"; text_ = "Candlestick"; series_ = series;

    }
    ~StockCandlestickChart() {}
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
    void set_series(const xframe::OHLCVSeries& s) { series_ = s; view_first_ = 0.0; view_count_ = 0.0;  }
    const xframe::OHLCVSeries& series() const { return series_; }
    int item_count() { return (int)series_.size(); }
    void set_y_range(double lo, double hi) { auto_y_ = false; y_min_ = lo; y_max_ = hi; }
    void set_auto_y(bool on) { auto_y_ = on; }
    void reset_view() { view_first_ = 0.0; view_count_ = (double)item_count(); }
    double get_view_first() { return view_first_; }
    double get_view_count() { return effective_count(); }
    void zoom_at(double factor, double frac_x) {
        double total = (double)item_count();
        if (total <= 0) return;
        double count = effective_count();
        double idx = view_first_ + frac_x * count;
        double nc = std::max(5.0, std::min(total, count * factor));
        view_first_ = std::max(0.0, std::min(total - nc, idx - frac_x * nc));
        view_count_ = nc;
    }
    double effective_count() {
        double total = (double)item_count();
        if (view_count_ <= 0 || view_count_ > total) return total;
        return view_count_;
    }
    void y_range(double& lo, double& hi) {
        if (!auto_y_) { lo = y_min_; hi = y_max_; return; }
        lo = 1e300; hi = -1e300;
        int i0 = (int)std::floor(view_first_);
        int i1 = (int)std::ceil(view_first_ + effective_count());
        for (int i = i0; i < i1 && i < (int)series_.size(); ++i) {
            if (i < 0) continue;
            lo = std::min(lo, series_[i].low); hi = std::max(hi, series_[i].high);
        }

        if (lo > hi) { lo = 0.0; hi = 1.0; return; }
        double pad = (hi - lo) * 0.06 + 1e-9;
        lo -= pad; hi += pad;
    }
    int plot_x() { return x1_ + axis_w_; }
    int plot_y() { return y1_ + 6; }
    int plot_w() { return width() - axis_w_ - 8; }
    int plot_h() { return height() - axis_h_ - 12; }
    double item_w() { double c = effective_count(); return c > 0 ? plot_w() / c : plot_w(); }
    int x_center(int i) { return plot_x() + (int)((i - view_first_ + 0.5) * item_w()); }
    int y_at(double v, double lo, double hi) { return plot_y() + (int)((hi - v) / (hi - lo) * plot_h()); }
    double value_at_y(int y, double lo, double hi) { return hi - (double)(y - plot_y()) / plot_h() * (hi - lo); }
    static double nice_step(double range, int target) {
        double raw = range / target;
        double mag = std::pow(10.0, std::floor(std::log10(raw)));
        double norm = raw / mag;
        double step = (norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0;
        return step * mag;
    }
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
        unsigned long up = styles_->get_color_rgb(0, 150, 0);
        unsigned long down = styles_->get_color_rgb(200, 30, 30);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        double lo = 0.0, hi = 1.0;
        y_range(lo, hi);
        int px = plot_x(), py = plot_y(), pw = plot_w(), ph = plot_h();
        double step = nice_step(hi - lo, 6);
        double start = std::ceil(lo / step) * step;
        for (double v = start; v <= hi; v += step) {
            int gy = y_at(v, lo, hi);
            XSetForeground(display_, gc_, grid);
            XDrawLine(display_, window_, gc_, px, gy, px + pw, gy);
            std::string lab = fmt(v);
            int tw = xf::text_width(st, lab);
            xf::draw_text_at(display_, window_, gc_, st, px - tw - 6, gy + 4, lab);
        }
        int i0 = (int)std::floor(view_first_);
        int i1 = (int)std::ceil(view_first_ + effective_count());
        {
            int bw = std::max(1, (int)(item_w() * 0.6));
            for (int i = i0; i < i1 && i < (int)series_.size(); ++i) {
                if (i < 0) continue;
                const xframe::OHLCV& b = series_[i];
                int cx = x_center(i);
                unsigned long c = (b.close >= b.open) ? up : down;
                XSetForeground(display_, gc_, c);
                XDrawLine(display_, window_, gc_, cx, y_at(b.high, lo, hi), cx, y_at(b.low, lo, hi));
                int top = y_at(std::max(b.open, b.close), lo, hi);
                int bot = y_at(std::min(b.open, b.close), lo, hi);
                XFillRectangle(display_, window_, gc_, cx - bw / 2, top, bw, std::max(1, bot - top));
            }
        }

        XSetForeground(display_, gc_, axis_col);
        XDrawLine(display_, window_, gc_, px, py + ph, px + pw, py + ph);
        {
            int target = std::max(1, pw / 90);
            int istep = (int)std::ceil(effective_count() / std::max(1, target));
            for (int i = i0; i < i1; i += istep) {
                if (i < 0 || i >= item_count()) continue;
                int cx = x_center(i);
                XDrawLine(display_, window_, gc_, cx, py + ph, cx, py + ph + 4);
                std::string lab = std::to_string(i);
                int tw = xf::text_width(st, lab);
                xf::draw_text_at(display_, window_, gc_, st, cx - tw / 2, py + ph + 16, lab);
            }
        }
        XSetForeground(display_, gc_, axis_col);
        XDrawRectangle(display_, window_, gc_, px, py, pw, ph);
        {
            std::string last;
            if (!series_.empty()) last = fmt(series_.back().close);
            xf::draw_text_at(display_, window_, gc_, st, px + 8, py + 14, text_ + (last.empty() ? "" : "  " + last));
        }
        if (cross_valid_ && cross_x_ >= px && cross_x_ <= px + pw && cross_y_ >= py && cross_y_ <= py + ph) {
            XGCValues gv; gv.line_style = LineOnOffDash;
            XChangeGC(display_, gc_, GCLineStyle, &gv);
            XSetForeground(display_, gc_, axis_col);
            XDrawLine(display_, window_, gc_, cross_x_, py, cross_x_, py + ph);
            XDrawLine(display_, window_, gc_, px, cross_y_, px + pw, cross_y_);
            gv.line_style = LineSolid;
            XChangeGC(display_, gc_, GCLineStyle, &gv);
            int idx = (int)std::floor(view_first_ + (cross_x_ - px) / item_w());
            std::string info = "i=" + std::to_string(idx) + "  v=" + fmt(value_at_y(cross_y_, lo, hi));
            if (idx >= 0 && idx < (int)series_.size()) {
                const xframe::OHLCV& b = series_[idx];
                info += "  O " + fmt(b.open) + " H " + fmt(b.high) + " L " + fmt(b.low) + " C " + fmt(b.close);
            }

            int tw = xf::text_width(st, info);
            int bx = std::min(cross_x_ + 12, px + pw - tw - 10);
            int by = py + 6;
            XSetForeground(display_, gc_, white);
            XFillRectangle(display_, window_, gc_, bx, by, tw + 8, 16);
            XSetForeground(display_, gc_, axis_col);
            XDrawRectangle(display_, window_, gc_, bx, by, tw + 8, 16);
            xf::draw_text_at(display_, window_, gc_, st, bx + 4, by + 12, info);
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true; cross_valid_ = true; cross_x_ = x; cross_y_ = y;
        int pw = plot_w();
        double frac = pw > 0 ? std::max(0.0, std::min(1.0, (double)(x - plot_x()) / pw)) : 0.0;
        if (button == 4) { zoom_at(0.8, frac); return true; }
        if (button == 5) { zoom_at(1.25, frac); return true; }
        if (button == 1) { dragging_ = true; drag_start_x_ = x; drag_start_first_ = view_first_; return true; }
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
            double total = (double)item_count();
            double count = effective_count();
            double delta = -(x - drag_start_x_) / item_w();
            view_first_ = std::max(0.0, std::min(std::max(0.0, total - count), drag_start_first_ + delta));
        }
        return true;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        double total = (double)item_count();
        double count = effective_count();
        switch (key) {
        case XK_Left: view_first_ = std::max(0.0, view_first_ - count * 0.1); return true;
        case XK_Right: view_first_ = std::min(std::max(0.0, total - count), view_first_ + count * 0.1); return true;
        case XK_plus: case XK_equal: zoom_at(0.8, 0.5); return true;
        case XK_minus: zoom_at(1.25, 0.5); return true;
        case XK_Home: case XK_r: case XK_R: reset_view(); return true;
        }
        return false;
    }
};
