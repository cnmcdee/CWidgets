#pragma once

#include "core/xframe_core.h"

//region GtkPasswordEntry

class GtkPasswordEntry

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the real password buffer, never drawn raw unless peeked

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::string placeholder_;

    int cursor_ = 0;

    bool show_text_ = false;   // the "show peek" toggle

    bool blink_on_ = true;

    // Width of one bullet cell.

    int cell_width()

    {

        return 12;

    }

    int text_left()

    {

        return x1_ + 8;

    }

    // The eye / peek toggle zone at the right edge.

    bool in_peek_zone(int x, int y)

    {

        return x >= x2_ - 28 && x <= x2_ - 2 && y >= y1_ + 2 && y <= y2_ - 2;

    }

public:

    std::function<void()> on_changed;

    std::function<void()> on_activate;

    GtkPasswordEntry(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkPasswordEntry";

        text_ = "secret42";

        placeholder_ = "Password";

        cursor_ = (int)text_.size();

    }

    ~GtkPasswordEntry() {}

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

        cursor_ = (int)text_.size();

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkPasswordEntry specific API

    void set_placeholder_text(std::string p)

    {

        placeholder_ = p;

    }

    std::string get_placeholder_text()

    {

        return placeholder_;

    }

    void set_show_peek_text(bool show)

    {

        show_text_ = show;

    }

    bool get_show_peek_text()

    {

        return show_text_;

    }

    void tick()

    {

        if (focused_)

        {

            blink_on_ = !blink_on_;

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

        SStyle body = st;

        body.fillColor_ = styles_->get_color_rgb(255, 255, 255);

        if (!enabled_)

        {

            body.fillColor_ = styles_->get_color_rgb(235, 235, 235);

        }

        xf::fill_rounded_rect(display_, window_, gc_, body, x1_, y1_, x2_, y2_);

        if (focused_)

        {

            SStyle ring = st;

            ring.lineColor_ = styles_->get_color_rgb(60, 120, 200);

            ring.lineWidth_ = 2;

            xf::draw_rounded_outline(display_, window_, gc_, ring, x1_, y1_, x2_, y2_);

        }

        else

        {

            xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        int cy = (y1_ + y2_) / 2;

        if (text_.empty())

        {

            SStyle ph = st;

            ph.text_color_ = styles_->get_color_rgb(150, 150, 150);

            xf::draw_text_left(display_, window_, gc_, ph, text_left(), y1_, y2_, placeholder_);

        }

        else if (show_text_)

        {

            xf::draw_text_left(display_, window_, gc_, st, text_left(), y1_, y2_, text_);

        }

        else

        {

            // One filled bullet per character.

            XSetForeground(display_, gc_, enabled_ ? st.text_color_ : styles_->get_color_rgb(140, 140, 140));

            for (int i = 0; i < (int)text_.size(); ++i)

            {

                int cx = text_left() + i * cell_width();

                if (cx + 8 > x2_ - 30)

                {

                    break;

                }

                XFillArc(display_, window_, gc_, cx, cy - 3, 7, 7, 0, 360 * 64);

            }

        }

        // Peek toggle: an eye outline; a slash when the text is hidden.

        unsigned long grey = styles_->get_color_rgb(110, 110, 110);

        XSetForeground(display_, gc_, grey);

        xf::set_line_width(display_, gc_, 1);

        int ex = x2_ - 15;

        XDrawArc(display_, window_, gc_, ex - 7, cy - 4, 14, 8, 0, 360 * 64);

        XFillArc(display_, window_, gc_, ex - 2, cy - 2, 4, 4, 0, 360 * 64);

        if (!show_text_)

        {

            XDrawLine(display_, window_, gc_, ex - 8, cy + 6, ex + 8, cy - 6);

        }

        // Cursor bar sits after the last visible cell.

        if (focused_ && enabled_ && blink_on_)

        {

            int cx;

            if (show_text_)

            {

                cx = text_left() + xf::text_width(st, text_.substr(0, (size_t)cursor_));

            }

            else

            {

                cx = text_left() + cursor_ * cell_width();

            }

            XSetForeground(display_, gc_, styles_->get_color_rgb(20, 20, 20));

            XDrawLine(display_, window_, gc_, cx, y1_ + 5, cx, y2_ - 5);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        blink_on_ = true;

        if (in_peek_zone(x, y))

        {

            show_text_ = !show_text_;

            return true;

        }

        if (show_text_)

        {

            int px = x - text_left();

            int best = (int)text_.size();

            int best_dist = 1 << 30;

            for (int i = 0; i <= (int)text_.size(); ++i)

            {

                int d = xf::text_width(style(), text_.substr(0, (size_t)i)) - px;

                if (d < 0)

                {

                    d = -d;

                }

                if (d < best_dist)

                {

                    best_dist = d;

                    best = i;

                }

            }

            cursor_ = best;

        }

        else

        {

            cursor_ = std::max(0, std::min((int)text_.size(), (x - text_left()) / cell_width()));

        }

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

        blink_on_ = true;

        if (key == XK_Return || key == XK_KP_Enter)

        {

            if (on_activate)

            {

                on_activate();

            }

            return true;

        }

        if (key == XK_Left)

        {

            if (cursor_ > 0)

            {

                --cursor_;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_ < (int)text_.size())

            {

                ++cursor_;

            }

            return true;

        }

        if (key == XK_Home)

        {

            cursor_ = 0;

            return true;

        }

        if (key == XK_End)

        {

            cursor_ = (int)text_.size();

            return true;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                text_.erase((size_t)(cursor_ - 1), 1);

                --cursor_;

                if (on_changed)

                {

                    on_changed();

                }

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)text_.size())

            {

                text_.erase((size_t)cursor_, 1);

                if (on_changed)

                {

                    on_changed();

                }

            }

            return true;

        }

        if (str && str[0] >= 32 && str[0] != 127)

        {

            for (int i = 0; str[i]; ++i)

            {

                if (str[i] < 32 || str[i] == 127)

                {

                    continue;

                }

                text_.insert((size_t)cursor_, 1, str[i]);

                ++cursor_;

            }

            if (on_changed)

            {

                on_changed();

            }

            return true;

        }

        return false;

    }

};

//endregion
