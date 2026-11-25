#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkProgressBar

// A trough with a filled portion showing fraction_ (0..1), with an
// optional text overlay. In pulse mode a block slides back and
// forth instead — pulse() advances it.

class GtkProgressBar

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

    double fraction_ = 0.45;

    bool show_text_ = true;

    bool custom_text_ = false;   // text_ set by the caller wins

    bool pulsing_ = false;

    double pulse_pos_ = 0.0;     // 0..1 position of the sliding block

    int pulse_dir_ = 1;

    bool inverted_ = false;

public:

    GtkProgressBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkProgressBar";

        text_ = "";

    }

    ~GtkProgressBar() {}

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

        custom_text_ = true;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkProgressBar API

    void set_fraction(double f)

    {

        fraction_ = std::max(0.0, std::min(1.0, f));

    }

    double get_fraction()

    {

        return fraction_;

    }

    void set_show_text(bool s)

    {

        show_text_ = s;

    }

    bool get_show_text()

    {

        return show_text_;

    }

    void set_pulsing(bool p)

    {

        pulsing_ = p;

    }

    bool get_pulsing()

    {

        return pulsing_;

    }

    void set_inverted(bool i)

    {

        inverted_ = i;

    }

    bool get_inverted()

    {

        return inverted_;

    }

    // One step of activity-mode animation: the block bounces.

    void pulse()

    {

        pulsing_ = true;

        pulse_pos_ += 0.06 * pulse_dir_;

        if (pulse_pos_ > 0.8)

        {

            pulse_pos_ = 0.8;

            pulse_dir_ = -1;

        }

        if (pulse_pos_ < 0.0)

        {

            pulse_pos_ = 0.0;

            pulse_dir_ = 1;

        }

    }

    std::string display_text()

    {

        if (custom_text_ && !text_.empty())

        {

            return text_;

        }

        char buf[24];

        std::snprintf(buf, sizeof(buf), "%d%%", (int)(fraction_ * 100.0 + 0.5));

        return std::string(buf);

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Trough: light sunken body.

        SStyle trough = st;

        trough.fillColor_ = styles_->get_color_rgb(225, 225, 225);

        xf::draw_polished(display_, window_, gc_, trough, x1_, y1_, x2_, y2_);

        unsigned long fill = enabled_ ? styles_->get_color_rgb(52, 101, 164) : styles_->get_color_rgb(150, 150, 150);

        int inner_w = width() - 4;

        if (pulsing_)

        {

            int block_w = std::max(10, inner_w / 5);

            int bx = x1_ + 2 + (int)((inner_w - block_w) * pulse_pos_);

            XSetForeground(display_, gc_, fill);

            XFillRectangle(display_, window_, gc_, bx, y1_ + 2, block_w, height() - 4);

        }

        else if (fraction_ > 0.0)

        {

            int fw = (int)(inner_w * fraction_);

            XSetForeground(display_, gc_, fill);

            if (inverted_)

            {

                XFillRectangle(display_, window_, gc_, x2_ - 2 - fw, y1_ + 2, fw, height() - 4);

            }

            else

            {

                XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, fw, height() - 4);

            }

        }

        if (show_text_ && !pulsing_)

        {

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y2_, display_text());

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

        // Clicking the bar sets the fraction, like a slider.

        pulsing_ = false;

        double f = (double)(x - (x1_ + 2)) / std::max(1, width() - 4);

        set_fraction(inverted_ ? 1.0 - f : f);

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

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Right || key == XK_Up)

        {

            pulsing_ = false;

            set_fraction(fraction_ + 0.05);

            return true;

        }

        if (key == XK_Left || key == XK_Down)

        {

            pulsing_ = false;

            set_fraction(fraction_ - 0.05);

            return true;

        }

        return false;

    }

};

//endregion
