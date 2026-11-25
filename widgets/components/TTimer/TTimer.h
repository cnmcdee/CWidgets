#pragma once

#include "core/xframe_core.h"

//region TTimer

class TTimer

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

    bool enabled_ = true;      // the timer's own Enabled property

    bool focused_ = false;

    // Non-visual component: the host's event loop calls tick() with
    // the current time in milliseconds, and on_timer fires every
    // interval_ ms while the timer is enabled.

    int interval_ = 1000;

    long last_fire_ms_ = -1;   // -1 = never ticked yet

    long fire_count_ = 0;

public:

    std::function<void()> on_timer;

    TTimer(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TTimer";

        text_ = "TTimer";

    }

    ~TTimer() {}

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

        if (!enabled_)

        {

            last_fire_ms_ = -1;   // restart the clock when re-enabled

        }

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

    void set_interval(int ms)

    {

        interval_ = ms < 1 ? 1 : ms;

    }

    int get_interval()

    {

        return interval_;

    }

    long get_fire_count()

    {

        return fire_count_;

    }

    // Called by the host loop with a millisecond clock. The first
    // tick just starts the clock; after that on_timer fires each
    // time a full interval has elapsed.

    void tick(long now_ms)

    {

        if (!enabled_)

        {

            return;

        }

        if (last_fire_ms_ < 0)

        {

            last_fire_ms_ = now_ms;

            return;

        }

        if (now_ms - last_fire_ms_ >= interval_)

        {

            last_fire_ms_ = now_ms;

            ++fire_count_;

            if (on_timer)

            {

                on_timer();

            }

        }

    }

    void reset()

    {

        last_fire_ms_ = -1;

        fire_count_ = 0;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Design-time icon: a small clock face in a polished tile,
        // with the interval written beside it.

        int size = height() < 30 ? height() : 30;

        if (size < 12)

        {

            size = 12;

        }

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x1_ + size, y1_ + size);

        int cx = x1_ + size / 2;

        int cy = y1_ + size / 2;

        int r = size / 2 - 4;

        unsigned long hand = enabled_ ? st.text_color_ : styles_->get_color_rgb(150, 150, 150);

        XSetForeground(display_, gc_, hand);

        xf::set_line_width(display_, gc_, 1);

        XDrawArc(display_, window_, gc_, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);

        // Tick marks at 12, 3, 6 and 9.

        XDrawPoint(display_, window_, gc_, cx, cy - r + 1);

        XDrawPoint(display_, window_, gc_, cx, cy + r - 1);

        XDrawPoint(display_, window_, gc_, cx - r + 1, cy);

        XDrawPoint(display_, window_, gc_, cx + r - 1, cy);

        // Hands at ten past twelve.

        xf::set_line_width(display_, gc_, 2);

        XDrawLine(display_, window_, gc_, cx, cy, cx, cy - r + 3);

        XDrawLine(display_, window_, gc_, cx, cy, cx + r - 3, cy + 2);

        xf::set_line_width(display_, gc_, 1);

        std::string label = "TTimer: " + std::to_string(interval_) + " ms";

        if (!enabled_)

        {

            label += " (off)";

        }

        xf::draw_text_left(display_, window_, gc_, st, x1_ + size + 6, y1_, y1_ + size, label);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        (void)button;

        return false;

    }

    bool on_motion(int x, int y)

    {

        (void)x;

        (void)y;

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
