#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkVolumeButton

class GtkVolumeButton

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

    bool pressed_ = false;

    double value_ = 0.7;          // 0..1, like GtkVolumeButton's scale

    bool muted_ = false;

    double value_before_mute_ = 0.7;

    int zone_at(int x)

    {

        if (x < x1_ + 18)

        {

            return -1;   // minus zone

        }

        if (x > x2_ - 18)

        {

            return 1;    // plus zone

        }

        return 0;        // middle: mute toggle

    }

    void changed()

    {

        if (on_value_changed)

        {

            on_value_changed(get_value());

        }

    }

    void adjust(double delta)

    {

        double v = value_ + delta;

        if (v < 0)

        {

            v = 0;

        }

        if (v > 1)

        {

            v = 1;

        }

        value_ = v;

        if (v > 0)

        {

            muted_ = false;

        }

        changed();

    }

public:

    std::function<void()> on_click;

    std::function<void(double)> on_value_changed;

    GtkVolumeButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkVolumeButton";

        text_ = "70%";

    }

    ~GtkVolumeButton() {}

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

    //region Value and mute

    void set_value(double v)

    {

        if (v < 0)

        {

            v = 0;

        }

        if (v > 1)

        {

            v = 1;

        }

        value_ = v;

        if (v > 0)

        {

            muted_ = false;

        }

    }

    double get_value()

    {

        return muted_ ? 0 : value_;

    }

    void set_muted(bool m)

    {

        if (m && !muted_)

        {

            value_before_mute_ = value_;

        }

        if (!m && muted_ && value_ == 0)

        {

            value_ = value_before_mute_;

        }

        muted_ = m;

    }

    bool get_muted()

    {

        return muted_;

    }

    //endregion

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

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int cy = y1_ + height() / 2;

        // Minus and plus glyphs in the end zones.

        XSetForeground(display_, gc_, st.text_color_);

        xf::set_line_width(display_, gc_, 2);

        XDrawLine(display_, window_, gc_, x1_ + 5, cy, x1_ + 13, cy);

        XDrawLine(display_, window_, gc_, x2_ - 13, cy, x2_ - 5, cy);

        XDrawLine(display_, window_, gc_, x2_ - 9, cy - 4, x2_ - 9, cy + 4);

        xf::set_line_width(display_, gc_, 1);

        // The speaker glyph: a body block and a cone.

        int sx = x1_ + 26;

        XSetForeground(display_, gc_, st.text_color_);

        XFillRectangle(display_, window_, gc_, sx, cy - 3, 4, 6);

        XPoint cone[4];

        cone[0].x = (short)(sx + 4);

        cone[0].y = (short)(cy - 3);

        cone[1].x = (short)(sx + 11);

        cone[1].y = (short)(cy - 7);

        cone[2].x = (short)(sx + 11);

        cone[2].y = (short)(cy + 7);

        cone[3].x = (short)(sx + 4);

        cone[3].y = (short)(cy + 3);

        XFillPolygon(display_, window_, gc_, cone, 4, Complex, CoordModeOrigin);

        double v = get_value();

        if (muted_ || v <= 0)

        {

            // A red cross where the sound waves would be.

            XSetForeground(display_, gc_, styles_->get_color_rgb(200, 60, 60));

            xf::set_line_width(display_, gc_, 2);

            XDrawLine(display_, window_, gc_, sx + 15, cy - 4, sx + 23, cy + 4);

            XDrawLine(display_, window_, gc_, sx + 15, cy + 4, sx + 23, cy - 4);

            xf::set_line_width(display_, gc_, 1);

        }

        else

        {

            // One sound wave, a second when past half volume.

            XSetForeground(display_, gc_, st.text_color_);

            XDrawArc(display_, window_, gc_, sx + 8, cy - 6, 12, 12, 300 * 64, 120 * 64);

            if (v > 0.5)

            {

                XDrawArc(display_, window_, gc_, sx + 12, cy - 9, 18, 18, 300 * 64, 120 * 64);

            }

        }

        // The percentage, centred in what is left.

        char buf[16];

        std::snprintf(buf, sizeof(buf), "%d%%", (int)(v * 100 + 0.5));

        xf::draw_text_centered(display_, window_, gc_, st, sx + 28, y1_, x2_ - 18, y2_, buf);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        pressed_ = true;

        focused_ = true;

        int zone = zone_at(x);

        if (zone < 0)

        {

            adjust(-0.05);

        }

        else if (zone > 0)

        {

            adjust(0.05);

        }

        else

        {

            set_muted(!muted_);

            changed();

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        if (contains(x, y) && on_click)

        {

            on_click();

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        (void)x;

        (void)y;

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Left || key == XK_Down)

        {

            adjust(-0.05);

            return true;

        }

        if (key == XK_Right || key == XK_Up)

        {

            adjust(0.05);

            return true;

        }

        if (key == XK_m)

        {

            set_muted(!muted_);

            changed();

            return true;

        }

        return false;

    }

};

//endregion
