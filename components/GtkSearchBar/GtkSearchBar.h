#pragma once

#include "core/xframe_core.h"

//region GtkSearchBar

class GtkSearchBar

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the search entry's buffer inside the bar

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    bool revealed_ = true;            // the bar itself is shown

    bool search_mode_enabled_ = true; // search mode on/off inside the bar

    int cursor_ = 0;

    bool blink_on_ = true;

    // The entry area inside the bar, leaving room for the close button.

    int entry_x1()

    {

        return x1_ + 10;

    }

    int entry_x2()

    {

        return x2_ - 44;

    }

    int entry_y1()

    {

        return y1_ + 7;

    }

    int entry_y2()

    {

        return y2_ - 7;

    }

    bool in_close_zone(int x, int y)

    {

        return x >= x2_ - 36 && x <= x2_ - 8 && y >= y1_ + 6 && y <= y2_ - 6;

    }

    bool in_entry(int x, int y)

    {

        return x >= entry_x1() && x <= entry_x2() && y >= entry_y1() && y <= entry_y2();

    }

    void changed()

    {

        if (on_search_changed)

        {

            on_search_changed(text_);

        }

    }

public:

    std::function<void(const std::string&)> on_search_changed;

    std::function<void()> on_stop_search;

    GtkSearchBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkSearchBar";

        text_ = "";

    }

    ~GtkSearchBar() {}

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

        return visible_ && revealed_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_;

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

    //region GtkSearchBar specific API

    void set_reveal(bool reveal)

    {

        revealed_ = reveal;

    }

    bool get_reveal()

    {

        return revealed_;

    }

    void set_search_mode(bool enabled)

    {

        search_mode_enabled_ = enabled;

        if (enabled)

        {

            revealed_ = true;

            focused_ = true;

        }

    }

    bool get_search_mode()

    {

        return search_mode_enabled_;

    }

    // Typing forwarded from the host window starts a search, like
    // the real GtkSearchBar capturing key events.

    bool handle_event_key(KeySym key, const char* str)

    {

        if (!search_mode_enabled_)

        {

            if (str && str[0] >= 32 && str[0] != 127)

            {

                set_search_mode(true);

            }

            else

            {

                return false;

            }

        }

        focused_ = true;

        return on_key(key, str);

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

        if (!visible_ || !revealed_)

        {

            return;

        }

        SStyle st = style();

        // The bar panel itself.

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // Entry field inside the bar.

        SStyle body = st;

        body.fillColor_ = styles_->get_color_rgb(255, 255, 255);

        if (!search_mode_enabled_ || !enabled_)

        {

            body.fillColor_ = styles_->get_color_rgb(235, 235, 235);

        }

        xf::fill_rounded_rect(display_, window_, gc_, body, entry_x1(), entry_y1(), entry_x2(), entry_y2());

        if (focused_ && search_mode_enabled_)

        {

            SStyle ring = st;

            ring.lineColor_ = styles_->get_color_rgb(60, 120, 200);

            ring.lineWidth_ = 2;

            xf::draw_rounded_outline(display_, window_, gc_, ring, entry_x1(), entry_y1(), entry_x2(), entry_y2());

        }

        else

        {

            xf::draw_rounded_outline(display_, window_, gc_, st, entry_x1(), entry_y1(), entry_x2(), entry_y2());

        }

        int cy = (entry_y1() + entry_y2()) / 2;

        // Magnifier glyph.

        unsigned long grey = styles_->get_color_rgb(110, 110, 110);

        XSetForeground(display_, gc_, grey);

        xf::set_line_width(display_, gc_, 2);

        XDrawArc(display_, window_, gc_, entry_x1() + 7, cy - 6, 11, 11, 0, 360 * 64);

        XDrawLine(display_, window_, gc_, entry_x1() + 16, cy + 3, entry_x1() + 21, cy + 8);

        xf::set_line_width(display_, gc_, 1);

        if (text_.empty())

        {

            SStyle ph = st;

            ph.text_color_ = styles_->get_color_rgb(150, 150, 150);

            xf::draw_text_left(display_, window_, gc_, ph, entry_x1() + 26, entry_y1(), entry_y2(), "Search...");

        }

        else

        {

            SStyle ink = st;

            if (!search_mode_enabled_ || !enabled_)

            {

                ink.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            xf::draw_text_left(display_, window_, gc_, ink, entry_x1() + 26, entry_y1(), entry_y2(), text_);

        }

        if (focused_ && search_mode_enabled_ && enabled_ && blink_on_)

        {

            int cx = entry_x1() + 26 + xf::text_width(st, text_.substr(0, (size_t)cursor_));

            XSetForeground(display_, gc_, styles_->get_color_rgb(20, 20, 20));

            XDrawLine(display_, window_, gc_, cx, entry_y1() + 5, cx, entry_y2() - 5);

        }

        // Close button: a circle with an x, at the right of the bar.

        int kx = x2_ - 22;

        int ky = (y1_ + y2_) / 2;

        XSetForeground(display_, gc_, grey);

        XDrawArc(display_, window_, gc_, kx - 9, ky - 9, 18, 18, 0, 360 * 64);

        xf::set_line_width(display_, gc_, 2);

        XDrawLine(display_, window_, gc_, kx - 4, ky - 4, kx + 4, ky + 4);

        XDrawLine(display_, window_, gc_, kx - 4, ky + 4, kx + 4, ky - 4);

        xf::set_line_width(display_, gc_, 1);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        if (in_close_zone(x, y))

        {

            search_mode_enabled_ = false;

            if (on_stop_search)

            {

                on_stop_search();

            }

            return true;

        }

        if (in_entry(x, y))

        {

            focused_ = true;

            blink_on_ = true;

            search_mode_enabled_ = true;

            int px = x - (entry_x1() + 26);

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

            return true;

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

        if (!focused_ || !enabled_ || !search_mode_enabled_)

        {

            return false;

        }

        blink_on_ = true;

        if (key == XK_Escape)

        {

            search_mode_enabled_ = false;

            if (on_stop_search)

            {

                on_stop_search();

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

                changed();

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)text_.size())

            {

                text_.erase((size_t)cursor_, 1);

                changed();

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

            changed();

            return true;

        }

        return false;

    }

};

//endregion
