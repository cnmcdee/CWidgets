#pragma once

#include "core/xframe_core.h"

#include <cmath>

#include <sstream>

//region XGraph

//
// A graphing widget in the XFrame format: multiple data series on
// one plot, each drawn as a line, as OHLC bars, or as candlesticks.
// Value axis with nice 1/2/5 grid steps, an index/time axis along
// the bottom, a legend, a mouse crosshair with a data readout,
// mouse-wheel zoom around the cursor, and drag panning - the same
// interaction model as the fdesigner StockChart, in raw X11.
//

struct XGraphBar

{

    double open;

    double high;

    double low;

    double close;

};

class XGraph

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    struct Series

    {

        std::string name;

        Srgb color{0, 100, 200};

        int kind = 0;              // 0 = line, 1 = OHLC bars, 2 = candlesticks

        std::vector<double> line;  // kind 0 data

        std::vector<XGraphBar> bars;   // kind 1 / 2 data

        Srgb up_color{0, 150, 0};

        Srgb down_color{200, 30, 30};

    };

    std::vector<Series> series_;

    std::vector<std::string> x_labels_;

    double view_first_ = 0.0;      // first visible item index

    double view_count_ = 0.0;      // visible items (0 = all)

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

    std::function<void(double, double)> on_view_change;   // (first, count)

    XGraph(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "XGraph";

        text_ = "Graph";

        // Preload a demo: 48 OHLC bars from a seeded random walk,
        // with their 5-bar moving average as a line series on top.

        std::vector<XGraphBar> bars;

        unsigned int seed = 12345;

        auto rnd = [&]() {

            seed = seed * 1103515245 + 12345;

            return ((seed >> 16) & 0x7fff) / 32767.0;

        };

        double price = 100.0;

        for (int i = 0; i < 48; ++i)

        {

            XGraphBar b;

            b.open = price;

            b.close = price + (rnd() - 0.46) * 4.0;

            b.high = std::max(b.open, b.close) + rnd() * 1.2;

            b.low = std::min(b.open, b.close) - rnd() * 1.2;

            bars.push_back(b);

            price = b.close;

        }

        add_ohlc_series("DEMO", Srgb{60, 60, 60}, bars, true);

        std::vector<double> ma;

        for (size_t i = 0; i < bars.size(); ++i)

        {

            double sum = 0.0;

            int n = 0;

            for (int k = 0; k < 5 && (int)i - k >= 0; ++k)

            {

                sum += bars[i - k].close;

                ++n;

            }

            ma.push_back(sum / n);

        }

        add_line_series("MA(5)", Srgb{230, 126, 34}, ma);

        std::vector<std::string> labels;

        int mins = 9 * 60 + 30;

        for (int i = 0; i < 48; ++i)

        {

            int m = mins + i * 30;

            std::string s = std::to_string(m / 60) + ":" + (m % 60 < 10 ? "0" : "") + std::to_string(m % 60);

            labels.push_back(s);

        }

        set_x_labels(labels);

    }

    ~XGraph() {}

    //region Standard plumbing

    void set_bounds(int x, int y, int width, int height)

    {

        x1_ = x;

        y1_ = y;

        x2_ = x + width;

        y2_ = y + height;

    }

    int width()

    {

        return x2_ - x1_;

    }

    int height()

    {

        return y2_ - y1_;

    }

    bool contains(int x, int y)

    {

        return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_;

    }

    void set_visible(bool v)

    {

        visible_ = v;

    }

    bool get_visible()

    {

        return visible_;

    }

    void set_enabled(bool e)

    {

        enabled_ = e;

    }

    bool get_enabled()

    {

        return enabled_;

    }

    void set_style_name(std::string s)

    {

        style_name_ = s;

    }

    void set_text(std::string t)

    {

        text_ = t;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //endregion

    //region Series API

    int item_count()

    {

        size_t n = 0;

        for (auto& s : series_)

        {

            n = std::max(n, std::max(s.line.size(), s.bars.size()));

        }

        return (int)n;

    }

    void clear_series()

    {

        series_.clear();

        view_first_ = 0.0;

        view_count_ = 0.0;

    }

    void add_line_series(const std::string& name, Srgb color, const std::vector<double>& values)

    {

        Series s;

        s.name = name;

        s.color = color;

        s.kind = 0;

        s.line = values;

        series_.push_back(s);

    }

    void add_ohlc_series(const std::string& name, Srgb color, const std::vector<XGraphBar>& bars, bool candlesticks = false)

    {

        Series s;

        s.name = name;

        s.color = color;

        s.kind = candlesticks ? 2 : 1;

        s.bars = bars;

        series_.push_back(s);

    }

    void set_series_kind(int index, int kind)

    {

        if (index >= 0 && index < (int)series_.size())

        {

            series_[index].kind = kind;

        }

    }

    void set_series_up_down(int index, Srgb up, Srgb down)

    {

        if (index >= 0 && index < (int)series_.size())

        {

            series_[index].up_color = up;

            series_[index].down_color = down;

        }

    }

    int series_count()

    {

        return (int)series_.size();

    }

    void set_x_labels(const std::vector<std::string>& labels)

    {

        x_labels_ = labels;

    }

    void set_y_range(double lo, double hi)

    {

        auto_y_ = false;

        y_min_ = lo;

        y_max_ = hi;

    }

    void set_auto_y(bool on)

    {

        auto_y_ = on;

    }

    void reset_view()

    {

        view_first_ = 0.0;

        view_count_ = (double)item_count();

        fire_view_change();

    }

    double get_view_first()

    {

        return view_first_;

    }

    double get_view_count()

    {

        return effective_count();

    }

    void zoom_at(double factor, double frac_x)

    {

        double total = (double)item_count();

        if (total <= 0)

        {

            return;

        }

        double count = effective_count();

        double idx = view_first_ + frac_x * count;

        double nc = std::max(5.0, std::min(total, count * factor));

        view_first_ = std::max(0.0, std::min(total - nc, idx - frac_x * nc));

        view_count_ = nc;

        fire_view_change();

    }

    //endregion

    //region Scale math

    double effective_count()

    {

        double total = (double)item_count();

        if (view_count_ <= 0 || view_count_ > total)

        {

            return total;

        }

        return view_count_;

    }

    void y_range(double& lo, double& hi)

    {

        if (!auto_y_)

        {

            lo = y_min_;

            hi = y_max_;

            return;

        }

        lo = 1e300;

        hi = -1e300;

        int i0 = (int)std::floor(view_first_);

        int i1 = (int)std::ceil(view_first_ + effective_count());

        for (auto& s : series_)

        {

            if (s.kind == 0)

            {

                for (int i = i0; i < i1 && i < (int)s.line.size(); ++i)

                {

                    if (i < 0) continue;

                    lo = std::min(lo, s.line[i]);

                    hi = std::max(hi, s.line[i]);

                }

            } else {

                for (int i = i0; i < i1 && i < (int)s.bars.size(); ++i)

                {

                    if (i < 0) continue;

                    lo = std::min(lo, s.bars[i].low);

                    hi = std::max(hi, s.bars[i].high);

                }

            }

        }

        if (lo > hi)

        {

            lo = 0.0;

            hi = 1.0;

            return;

        }

        double pad = (hi - lo) * 0.06 + 1e-9;

        lo -= pad;

        hi += pad;

    }

    int plot_x()

    {

        return x1_ + axis_w_;

    }

    int plot_y()

    {

        return y1_ + 6;

    }

    int plot_w()

    {

        return width() - axis_w_ - 8;

    }

    int plot_h()

    {

        return height() - axis_h_ - 12;

    }

    double item_w()

    {

        double c = effective_count();

        return c > 0 ? plot_w() / c : plot_w();

    }

    int x_at(double index)

    {

        return plot_x() + (int)((index - view_first_) * item_w());

    }

    int x_center(int i)

    {

        return plot_x() + (int)((i - view_first_ + 0.5) * item_w());

    }

    int y_at(double v, double lo, double hi)

    {

        return plot_y() + (int)((hi - v) / (hi - lo) * plot_h());

    }

    double value_at_y(int y, double lo, double hi)

    {

        return hi - (double)(y - plot_y()) / plot_h() * (hi - lo);

    }

    static double nice_step(double range, int target)

    {

        double raw = range / target;

        double mag = std::pow(10.0, std::floor(std::log10(raw)));

        double norm = raw / mag;

        double step = (norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0;

        return step * mag;

    }

    static std::string fmt(double v)

    {

        std::ostringstream oss;

        oss.precision(2);

        oss << std::fixed << v;

        std::string s = oss.str();

        while (!s.empty() && s.back() == '0')

        {

            s.pop_back();

        }

        if (!s.empty() && s.back() == '.')

        {

            s.pop_back();

        }

        return s;

    }

    void fire_view_change()

    {

        if (on_view_change)

        {

            on_view_change(view_first_, effective_count());

        }

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long grid = styles_->get_color_rgb(225, 225, 225);

        unsigned long axis_col = styles_->get_color_rgb(150, 150, 150);

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        double lo = 0.0, hi = 1.0;

        y_range(lo, hi);

        int px = plot_x(), py = plot_y(), pw = plot_w(), ph = plot_h();

        // Gridlines + value axis

        double step = nice_step(hi - lo, 6);

        double start = std::ceil(lo / step) * step;

        for (double v = start; v <= hi; v += step)

        {

            int gy = y_at(v, lo, hi);

            XSetForeground(display_, gc_, grid);

            XDrawLine(display_, window_, gc_, px, gy, px + pw, gy);

            std::string lab = fmt(v);

            int tw = xf::text_width(st, lab);

            xf::draw_text_at(display_, window_, gc_, st, px - tw - 6, gy + 4, lab);

        }

        // Series

        int i0 = (int)std::floor(view_first_);

        int i1 = (int)std::ceil(view_first_ + effective_count());

        for (auto& s : series_)

        {

            unsigned long col = styles_->get_color_Srgb(s.color);

            unsigned long up = styles_->get_color_Srgb(s.up_color);

            unsigned long down = styles_->get_color_Srgb(s.down_color);

            if (s.kind == 0)

            {

                std::vector<XPoint> pts;

                for (int i = i0; i < i1 && i < (int)s.line.size(); ++i)

                {

                    if (i < 0) continue;

                    XPoint p;

                    p.x = (short)x_center(i);

                    p.y = (short)y_at(s.line[i], lo, hi);

                    pts.push_back(p);

                }

                if (pts.size() > 1)

                {

                    XSetForeground(display_, gc_, col);

                    xf::set_line_width(display_, gc_, 2);

                    XDrawLines(display_, window_, gc_, pts.data(), (int)pts.size(), CoordModeOrigin);

                    xf::set_line_width(display_, gc_, 1);

                }

            } else {

                int bw = std::max(1, (int)(item_w() * 0.6));

                for (int i = i0; i < i1 && i < (int)s.bars.size(); ++i)

                {

                    if (i < 0) continue;

                    const XGraphBar& b = s.bars[i];

                    int cx = x_center(i);

                    unsigned long c = (b.close >= b.open) ? up : down;

                    XSetForeground(display_, gc_, c);

                    XDrawLine(display_, window_, gc_, cx, y_at(b.high, lo, hi), cx, y_at(b.low, lo, hi));

                    if (s.kind == 1)   // OHLC ticks

                    {

                        int half = std::max(1, (int)(item_w() * 0.35));

                        XDrawLine(display_, window_, gc_, cx - half, y_at(b.open, lo, hi), cx, y_at(b.open, lo, hi));

                        XDrawLine(display_, window_, gc_, cx, y_at(b.close, lo, hi), cx + half, y_at(b.close, lo, hi));

                    } else {           // candle body

                        int top = y_at(std::max(b.open, b.close), lo, hi);

                        int bot = y_at(std::min(b.open, b.close), lo, hi);

                        XFillRectangle(display_, window_, gc_, cx - bw / 2, top, bw, std::max(1, bot - top));

                    }

                }

            }

        }

        // Index / time axis

        XSetForeground(display_, gc_, axis_col);

        XDrawLine(display_, window_, gc_, px, py + ph, px + pw, py + ph);

        {

            int target = std::max(1, pw / 90);

            int istep = (int)std::ceil(effective_count() / std::max(1, target));

            for (int i = i0; i < i1; i += istep)

            {

                if (i < 0 || i >= (int)x_labels_.size())

                {

                    continue;

                }

                int cx = x_center(i);

                XDrawLine(display_, window_, gc_, cx, py + ph, cx, py + ph + 4);

                int tw = xf::text_width(st, x_labels_[i]);

                xf::draw_text_at(display_, window_, gc_, st, cx - tw / 2, py + ph + 16, x_labels_[i]);

            }

        }

        // Plot border

        XSetForeground(display_, gc_, axis_col);

        XDrawRectangle(display_, window_, gc_, px, py, pw, ph);

        // Legend

        {

            int lx = px + 8;

            int ly = py + 8;

            for (auto& s : series_)

            {

                unsigned long col = styles_->get_color_Srgb(s.color);

                XSetForeground(display_, gc_, col);

                xf::set_line_width(display_, gc_, 2);

                XDrawLine(display_, window_, gc_, lx, ly + 4, lx + 18, ly + 4);

                xf::set_line_width(display_, gc_, 1);

                std::string last;

                if (s.kind == 0 && !s.line.empty())

                {

                    last = fmt(s.line.back());

                }

                if (s.kind != 0 && !s.bars.empty())

                {

                    last = fmt(s.bars.back().close);

                }

                xf::draw_text_at(display_, window_, gc_, st, lx + 24, ly + 8, s.name + (last.empty() ? "" : " " + last));

                ly += 16;

            }

        }

        // Crosshair + readout

        if (cross_valid_ && cross_x_ >= px && cross_x_ <= px + pw && cross_y_ >= py && cross_y_ <= py + ph)

        {

            XGCValues gv;

            gv.line_style = LineOnOffDash;

            XChangeGC(display_, gc_, GCLineStyle, &gv);

            XSetForeground(display_, gc_, axis_col);

            XDrawLine(display_, window_, gc_, cross_x_, py, cross_x_, py + ph);

            XDrawLine(display_, window_, gc_, px, cross_y_, px + pw, cross_y_);

            gv.line_style = LineSolid;

            XChangeGC(display_, gc_, GCLineStyle, &gv);

            int idx = (int)std::floor(view_first_ + (cross_x_ - px) / item_w());

            std::string info;

            if (idx >= 0 && idx < (int)x_labels_.size())

            {

                info = x_labels_[idx];

            }

            info += "  v=" + fmt(value_at_y(cross_y_, lo, hi));

            for (auto& s : series_)

            {

                if (s.kind == 0 && idx >= 0 && idx < (int)s.line.size())

                {

                    info += "  " + s.name + " " + fmt(s.line[idx]);

                }

                if (s.kind != 0 && idx >= 0 && idx < (int)s.bars.size())

                {

                    const XGraphBar& b = s.bars[idx];

                    info += "  O " + fmt(b.open) + " H " + fmt(b.high) + " L " + fmt(b.low) + " C " + fmt(b.close);

                }

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

        // Outer frame

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        int px = plot_x(), pw = plot_w();

        double frac = pw > 0 ? std::max(0.0, std::min(1.0, (double)(x - px) / pw)) : 0.0;

        if (button == 4)   // wheel up: zoom in

        {

            zoom_at(0.8, frac);

            return true;

        }

        if (button == 5)   // wheel down: zoom out

        {

            zoom_at(1.25, frac);

            return true;

        }

        if (button == 1)

        {

            dragging_ = true;

            drag_start_x_ = x;

            drag_start_first_ = view_first_;

            return true;

        }

        return false;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button == 1 && dragging_)

        {

            dragging_ = false;

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        cross_valid_ = true;

        cross_x_ = x;

        cross_y_ = y;

        if (dragging_)

        {

            double total = (double)item_count();

            double count = effective_count();

            double delta = -(x - drag_start_x_) / item_w();

            view_first_ = std::max(0.0, std::min(std::max(0.0, total - count), drag_start_first_ + delta));

            fire_view_change();

        }

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        double total = (double)item_count();

        double count = effective_count();

        switch (key)

        {

        case XK_Left:

            view_first_ = std::max(0.0, view_first_ - count * 0.1);

            fire_view_change();

            return true;

        case XK_Right:

            view_first_ = std::min(std::max(0.0, total - count), view_first_ + count * 0.1);

            fire_view_change();

            return true;

        case XK_plus:

        case XK_equal:

            zoom_at(0.8, 0.5);

            return true;

        case XK_minus:

            zoom_at(1.25, 0.5);

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

//endregion
