#pragma once

#include "core/xframe_core.h"

#include <cstdio>

#include <ctime>

//region TTimeEdit

class TTimeEdit

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

    int hours_ = 13;

    int minutes_ = 45;

    int seconds_ = 30;

    int segment_ = 0;   // 0 = hours, 1 = minutes, 2 = seconds

    int digits_ = 0;

    bool show_seconds_ = true;

    int segment_value(int seg)

    {

        return seg == 0 ? hours_ : (seg == 1 ? minutes_ : seconds_);

    }

    int segment_max(int seg)

    {

        return seg == 0 ? 23 : 59;

    }

    void set_segment_value(int seg, int v)

    {

        if (v < 0)

        {

            v = 0;

        }

        if (v > segment_max(seg))

        {

            v = segment_max(seg);

        }

        if (seg == 0)

        {

            hours_ = v;

        }

        else if (seg == 1)

        {

            minutes_ = v;

        }

        else

        {

            seconds_ = v;

        }

        sync_text();

    }

    void step_segment(int dir)

    {

        int v = segment_value(segment_) + dir;

        if (v < 0)

        {

            v = segment_max(segment_);

        }

        if (v > segment_max(segment_))

        {

            v = 0;

        }

        set_segment_value(segment_, v);

        digits_ = 0;

    }

    void sync_text()

    {

        char buf[32];

        if (show_seconds_)

        {

            std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours_, minutes_, seconds_);

        }

        else

        {

            std::snprintf(buf, sizeof(buf), "%02d:%02d", hours_, minutes_);

        }

        text_ = buf;

    }

    int segment_count()

    {

        return show_seconds_ ? 3 : 2;

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_change;

    TTimeEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TTimeEdit";

        std::time_t now = std::time(nullptr);

        std::tm* lt = std::localtime(&now);

        if (lt)

        {

            hours_ = lt->tm_hour;

            minutes_ = lt->tm_min;

            seconds_ = lt->tm_sec;

        }

        sync_text();

    }

    ~TTimeEdit() {}

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

        int h = 0, m = 0, s = 0;

        int n = std::sscanf(t.c_str(), "%d:%d:%d", &h, &m, &s);

        if (n >= 2)

        {

            hours_ = h;

            minutes_ = m;

            seconds_ = n == 3 ? s : 0;

            set_segment_value(0, hours_);

            set_segment_value(1, minutes_);

            set_segment_value(2, seconds_);

        }

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    int get_hours()

    {

        return hours_;

    }

    int get_minutes()

    {

        return minutes_;

    }

    int get_seconds()

    {

        return seconds_;

    }

    void set_time(int h, int m, int s)

    {

        set_segment_value(0, h);

        set_segment_value(1, m);

        set_segment_value(2, s);

    }

    std::string get_time()

    {

        return text_;

    }

    void set_show_seconds(bool s)

    {

        show_seconds_ = s;

        if (segment_ > segment_count() - 1)

        {

            segment_ = segment_count() - 1;

        }

        sync_text();

    }

    bool get_show_seconds()

    {

        return show_seconds_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        // A small clock glyph at the right edge.

        int cx = x2_ - 11;

        int cy = y1_ + height() / 2;

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawArc(display_, window_, gc_, cx - 6, cy - 6, 12, 12, 0, 360 * 64);

        XDrawLine(display_, window_, gc_, cx, cy, cx, cy - 4);

        XDrawLine(display_, window_, gc_, cx, cy, cx + 3, cy + 1);

        if (focused_ && enabled_)

        {

            std::string prefix = segment_ == 0 ? "" : (segment_ == 1 ? "00:" : "00:00:");

            int sx = x1_ + 4 + xf::text_width(st, prefix);

            int sw = xf::text_width(st, "00");

            XSetForeground(display_, gc_, styles_->get_color_rgb(200, 220, 255));

            XFillRectangle(display_, window_, gc_, sx - 1, y1_ + 2, sw + 2, height() - 4);

        }

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y2_, text_);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        SStyle st = style();

        int rel = x - (x1_ + 4);

        int w_seg = xf::text_width(st, "00:");

        segment_ = rel < w_seg ? 0 : (rel < 2 * w_seg ? 1 : 2);

        if (segment_ > segment_count() - 1)

        {

            segment_ = segment_count() - 1;

        }

        digits_ = 0;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Left)

        {

            if (segment_ > 0)

            {

                --segment_;

            }

            digits_ = 0;

            return true;

        }

        if (key == XK_Right || key == XK_Tab)

        {

            if (segment_ < segment_count() - 1)

            {

                ++segment_;

            }

            digits_ = 0;

            return true;

        }

        if (key == XK_Up)

        {

            step_segment(1);

            if (on_change)

            {

                on_change();

            }

            return true;

        }

        if (key == XK_Down)

        {

            step_segment(-1);

            if (on_change)

            {

                on_change();

            }

            return true;

        }

        if (str && str[0] >= '0' && str[0] <= '9')

        {

            int digit = str[0] - '0';

            int v = digits_ == 0 ? digit : segment_value(segment_) * 10 + digit;

            set_segment_value(segment_, v);

            ++digits_;

            if (digits_ >= 2)

            {

                digits_ = 0;

                if (segment_ < segment_count() - 1)

                {

                    ++segment_;

                }

            }

            if (on_change)

            {

                on_change();

            }

            return true;

        }

        if (str && str[0] == ':')

        {

            if (segment_ < segment_count() - 1)

            {

                ++segment_;

            }

            digits_ = 0;

            return true;

        }

        return false;

    }

};

//endregion
