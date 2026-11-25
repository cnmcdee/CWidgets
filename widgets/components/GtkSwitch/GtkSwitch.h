#pragma once

#include "core/xframe_core.h"

//region GtkSwitch

class GtkSwitch

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // GtkSwitch carries no caption; kept for the standard API

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    bool active_ = false;

    bool dragging_ = false;

    int press_x_ = 0;

    int drag_x_ = 0;

public:

    std::function<void()> on_click;

    std::function<void(bool)> on_state_set;

    GtkSwitch(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkSwitch";

        text_ = "";

    }

    ~GtkSwitch() {}

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

    //region Switch state

    void set_active(bool a)

    {

        // Programmatic set, like gtk_switch_set_active: no signal.

        active_ = a;

    }

    bool get_active()

    {

        return active_;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        int th = height();

        int cy = y1_ + th / 2;

        // The track is a pill: corner radius of half the height.

        SStyle track = st;

        track.radius_ = (unsigned long)(th / 2);

        if (!enabled_)

        {

            track.fillColor_ = styles_->get_color_rgb(190, 190, 190);

        }

        else if (active_ || (dragging_ && drag_x_ > (x1_ + x2_) / 2))

        {

            track.fillColor_ = styles_->get_color_rgb(53, 132, 228);

        }

        else

        {

            track.fillColor_ = styles_->get_color_rgb(160, 160, 160);

        }

        xf::fill_rounded_rect(display_, window_, gc_, track, x1_, y1_, x2_, y2_);

        xf::draw_rounded_outline(display_, window_, gc_, track, x1_, y1_, x2_, y2_);

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        // The I / O glyphs at the ends of the track.

        if (active_)

        {

            XSetForeground(display_, gc_, white);

            xf::set_line_width(display_, gc_, 2);

            XDrawLine(display_, window_, gc_, x1_ + 10, cy - 4, x1_ + 10, cy + 4);

            xf::set_line_width(display_, gc_, 1);

        }

        else

        {

            XSetForeground(display_, gc_, white);

            xf::set_line_width(display_, gc_, 2);

            XDrawArc(display_, window_, gc_, x2_ - 14, cy - 4, 8, 8, 0, 360 * 64);

            xf::set_line_width(display_, gc_, 1);

        }

        // The knob: a white circle that slides with the state, and
        // follows the pointer while it is being dragged.

        int kd = th - 6;

        int knob_cx;

        if (dragging_)

        {

            knob_cx = drag_x_;

            int lo = x1_ + 3 + kd / 2;

            int hi = x2_ - 3 - kd / 2;

            if (knob_cx < lo)

            {

                knob_cx = lo;

            }

            if (knob_cx > hi)

            {

                knob_cx = hi;

            }

        }

        else if (active_)

        {

            knob_cx = x2_ - 3 - kd / 2;

        }

        else

        {

            knob_cx = x1_ + 3 + kd / 2;

        }

        XSetForeground(display_, gc_, white);

        XFillArc(display_, window_, gc_, knob_cx - kd / 2, cy - kd / 2, kd, kd, 0, 360 * 64);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawArc(display_, window_, gc_, knob_cx - kd / 2, cy - kd / 2, kd, kd, 0, 360 * 64);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ - 2, y1_ - 2, width() + 4, height() + 4);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        dragging_ = true;

        press_x_ = x;

        drag_x_ = x;

        focused_ = true;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)y;

        if (!dragging_ || button != 1)

        {

            return false;

        }

        dragging_ = false;

        bool now;

        int travel = x - press_x_;

        if (travel < 0)

        {

            travel = -travel;

        }

        if (travel < 6)

        {

            // A plain click toggles.

            now = !active_;

        }

        else

        {

            // A drag sets the state by which half it ended in.

            now = x > (x1_ + x2_) / 2;

        }

        if (now != active_)

        {

            active_ = now;

            if (on_state_set)

            {

                on_state_set(active_);

            }

            if (on_click)

            {

                on_click();

            }

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_)

        {

            return false;

        }

        drag_x_ = x;

        (void)y;

        return true;   // caller redraws, the knob follows

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Return || key == XK_space)

        {

            active_ = !active_;

            if (on_state_set)

            {

                on_state_set(active_);

            }

            if (on_click)

            {

                on_click();

            }

            return true;

        }

        return false;

    }

};

//endregion
