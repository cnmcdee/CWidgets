#pragma once

#include "core/xframe_core.h"

//region GtkSearchEntry

class GtkSearchEntry

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the real text buffer

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::string placeholder_;

    int cursor_ = 0;

    int scroll_px_ = 0;

    bool blink_on_ = true;

    int char_x(int i)

    {

        if (i <= 0)

        {

            return 0;

        }

        return xf::text_width(style(), text_.substr(0, (size_t)i));

    }

    // Text starts after the magnifier glyph.

    int text_left()

    {

        return x1_ + 26;

    }

    // The clear (x) zone at the right edge, live when text is present.

    bool in_clear_zone(int x, int y)

    {

        return !text_.empty() && x >= x2_ - 24 && x <= x2_ - 2 && y >= y1_ + 2 && y <= y2_ - 2;

    }

    int inner_width()

    {

        return (x2_ - 24) - text_left();

    }

    void ensure_cursor_visible()

    {

        int cx = char_x(cursor_);

        if (cx - scroll_px_ > inner_width())

        {

            scroll_px_ = cx - inner_width();

        }

        if (cx < scroll_px_)

        {

            scroll_px_ = cx;

        }

        if (scroll_px_ < 0)

        {

            scroll_px_ = 0;

        }

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

    std::function<void()> on_activate;

    GtkSearchEntry(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkSearchEntry";

        text_ = "";

        placeholder_ = "Search...";

    }

    ~GtkSearchEntry() {}

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

        scroll_px_ = 0;

        ensure_cursor_visible();

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkSearchEntry specific API

    void set_placeholder_text(std::string p)

    {

        placeholder_ = p;

    }

    std::string get_placeholder_text()

    {

        return placeholder_;

    }

    void clear()

    {

        if (!text_.empty())

        {

            text_.clear();

            cursor_ = 0;

            scroll_px_ = 0;

            changed();

        }

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

        // Magnifier glyph: circle + handle, in grey.

        unsigned long grey = styles_->get_color_rgb(110, 110, 110);

        XSetForeground(display_, gc_, grey);

        xf::set_line_width(display_, gc_, 2);

        XDrawArc(display_, window_, gc_, x1_ + 7, cy - 6, 11, 11, 0, 360 * 64);

        XDrawLine(display_, window_, gc_, x1_ + 16, cy + 3, x1_ + 21, cy + 8);

        xf::set_line_width(display_, gc_, 1);

        // Text or placeholder.

        if (text_.empty())

        {

            SStyle ph = st;

            ph.text_color_ = styles_->get_color_rgb(150, 150, 150);

            xf::draw_text_left(display_, window_, gc_, ph, text_left(), y1_, y2_, placeholder_);

        }

        else

        {

            SStyle ink = st;

            if (!enabled_)

            {

                ink.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            int baseline = y1_ + (height() + (st.font_ ? st.font_->ascent - st.font_->descent : 10)) / 2;

            for (int i = 0; i < (int)text_.size(); ++i)

            {

                int cx = text_left() + char_x(i) - scroll_px_;

                int cw = char_x(i + 1) - char_x(i);

                if (cx + cw < x1_ + 24 || cx > x2_ - 26)

                {

                    continue;

                }

                xf::draw_text_at(display_, window_, gc_, ink, cx, baseline, text_.substr((size_t)i, 1));

            }

            // Clear (x) zone.

            XSetForeground(display_, gc_, grey);

            xf::set_line_width(display_, gc_, 2);

            int kx = x2_ - 13;

            XDrawLine(display_, window_, gc_, kx - 4, cy - 4, kx + 4, cy + 4);

            XDrawLine(display_, window_, gc_, kx - 4, cy + 4, kx + 4, cy - 4);

            xf::set_line_width(display_, gc_, 1);

        }

        if (focused_ && enabled_ && blink_on_)

        {

            int cx = text_left() + char_x(cursor_) - scroll_px_;

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

        if (in_clear_zone(x, y))

        {

            clear();

            return true;

        }

        // Place the cursor by click position.

        int px = x - text_left() + scroll_px_;

        int best = (int)text_.size();

        int best_dist = 1 << 30;

        for (int i = 0; i <= (int)text_.size(); ++i)

        {

            int d = char_x(i) - px;

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

        ensure_cursor_visible();

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

        if (key == XK_Escape)

        {

            clear();

            return true;

        }

        if (key == XK_Left)

        {

            if (cursor_ > 0)

            {

                --cursor_;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_ < (int)text_.size())

            {

                ++cursor_;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Home)

        {

            cursor_ = 0;

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_End)

        {

            cursor_ = (int)text_.size();

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                text_.erase((size_t)(cursor_ - 1), 1);

                --cursor_;

                ensure_cursor_visible();

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

            ensure_cursor_visible();

            changed();

            return true;

        }

        return false;

    }

};

//endregion
