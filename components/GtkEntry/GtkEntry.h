#pragma once

#include "core/xframe_core.h"

//region GtkEntry

class GtkEntry

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

    int cursor_ = 0;          // insertion point, 0..text_.size()

    int sel_anchor_ = 0;      // selection anchor; selection is anchor..cursor_

    int scroll_px_ = 0;       // horizontal scroll offset in pixels

    int max_length_ = 0;      // 0 = unlimited

    bool editable_ = true;

    bool dragging_ = false;   // mouse held down, extending the selection

    bool blink_on_ = true;    // cursor blink phase

    // Pixel x of the text boundary at char index i, from text start.

    int char_x(int i)

    {

        if (i <= 0)

        {

            return 0;

        }

        return xf::text_width(style(), text_.substr(0, (size_t)i));

    }

    int inner_width()

    {

        return width() - 16;   // 8 px padding each side

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

    int index_at_pixel(int px)   // px relative to the text start

    {

        int best = 0;

        int best_dist = px < 0 ? -px : px;

        for (int i = 1; i <= (int)text_.size(); ++i)

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

        return best;

    }

    bool has_selection()

    {

        return sel_anchor_ != cursor_;

    }

    void delete_selection()

    {

        int a = std::min(sel_anchor_, cursor_);

        int b = std::max(sel_anchor_, cursor_);

        text_.erase((size_t)a, (size_t)(b - a));

        cursor_ = a;

        sel_anchor_ = a;

    }

public:

    std::function<void()> on_changed;

    std::function<void()> on_activate;

    GtkEntry(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkEntry";

        text_ = "Sample text";

        placeholder_ = "Type here...";

        cursor_ = (int)text_.size();

        sel_anchor_ = cursor_;

    }

    ~GtkEntry() {}

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

        sel_anchor_ = cursor_;

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

    //region GtkEntry specific API

    void set_placeholder_text(std::string p)

    {

        placeholder_ = p;

    }

    std::string get_placeholder_text()

    {

        return placeholder_;

    }

    void set_max_length(int m)

    {

        max_length_ = m;

    }

    int get_max_length()

    {

        return max_length_;

    }

    void set_editable(bool e)

    {

        editable_ = e;

    }

    bool get_editable()

    {

        return editable_;

    }

    int get_cursor_position()

    {

        return cursor_;

    }

    void set_cursor_position(int pos)

    {

        cursor_ = std::max(0, std::min(pos, (int)text_.size()));

        sel_anchor_ = cursor_;

        ensure_cursor_visible();

    }

    void select_region(int start, int end)

    {

        sel_anchor_ = std::max(0, std::min(start, (int)text_.size()));

        cursor_ = std::max(0, std::min(end, (int)text_.size()));

        ensure_cursor_visible();

    }

    bool get_selection_bounds(int& start, int& end)

    {

        if (!has_selection())

        {

            return false;

        }

        start = std::min(sel_anchor_, cursor_);

        end = std::max(sel_anchor_, cursor_);

        return true;

    }

    std::string get_selected_text()

    {

        int a = std::min(sel_anchor_, cursor_);

        int b = std::max(sel_anchor_, cursor_);

        return text_.substr((size_t)a, (size_t)(b - a));

    }

    void delete_selected_text()

    {

        if (has_selection() && editable_)

        {

            delete_selection();

            if (on_changed)

            {

                on_changed();

            }

        }

    }

    // Called by the host's timer to blink the cursor.

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

        // Entry body: white field with the polished outline.

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

        int tx = x1_ + 8;

        // Selection highlight behind the text.

        if (has_selection() && focused_)

        {

            int a = std::min(sel_anchor_, cursor_);

            int b = std::max(sel_anchor_, cursor_);

            int sx = tx + char_x(a) - scroll_px_;

            int ex = tx + char_x(b) - scroll_px_;

            XSetForeground(display_, gc_, styles_->get_color_rgb(180, 205, 245));

            XFillRectangle(display_, window_, gc_, sx, y1_ + 4, ex - sx, height() - 8);

        }

        if (text_.empty())

        {

            SStyle grey = st;

            grey.text_color_ = styles_->get_color_rgb(150, 150, 150);

            xf::draw_text_left(display_, window_, gc_, grey, tx, y1_, y2_, placeholder_);

        }

        else

        {

            SStyle ink = st;

            if (!enabled_)

            {

                ink.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            // Draw the visible run of characters only.

            for (int i = 0; i < (int)text_.size(); ++i)

            {

                int cx = tx + char_x(i) - scroll_px_;

                int cw = char_x(i + 1) - char_x(i);

                if (cx + cw < x1_ + 4 || cx > x2_ - 4)

                {

                    continue;

                }

                std::string ch = text_.substr((size_t)i, 1);

                int baseline = y1_ + (height() + (st.font_ ? st.font_->ascent - st.font_->descent : 10)) / 2;

                xf::draw_text_at(display_, window_, gc_, ink, cx, baseline, ch);

            }

        }

        // Blinking cursor bar.

        if (focused_ && enabled_ && blink_on_)

        {

            int cx = tx + char_x(cursor_) - scroll_px_;

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

        cursor_ = index_at_pixel(x - (x1_ + 8) + scroll_px_);

        sel_anchor_ = cursor_;

        dragging_ = true;

        ensure_cursor_visible();

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !dragging_)

        {

            return false;

        }

        dragging_ = false;

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_)

        {

            return false;

        }

        cursor_ = index_at_pixel(x - (x1_ + 8) + scroll_px_);

        ensure_cursor_visible();

        return true;

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

            if (has_selection())

            {

                cursor_ = std::min(sel_anchor_, cursor_);

            }

            else if (cursor_ > 0)

            {

                --cursor_;

            }

            sel_anchor_ = cursor_;

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Right)

        {

            if (has_selection())

            {

                cursor_ = std::max(sel_anchor_, cursor_);

            }

            else if (cursor_ < (int)text_.size())

            {

                ++cursor_;

            }

            sel_anchor_ = cursor_;

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Home)

        {

            cursor_ = 0;

            sel_anchor_ = cursor_;

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_End)

        {

            cursor_ = (int)text_.size();

            sel_anchor_ = cursor_;

            ensure_cursor_visible();

            return true;

        }

        if (!editable_)

        {

            return false;

        }

        if (key == XK_BackSpace)

        {

            if (has_selection())

            {

                delete_selection();

            }

            else if (cursor_ > 0)

            {

                text_.erase((size_t)(cursor_ - 1), 1);

                --cursor_;

                sel_anchor_ = cursor_;

            }

            else

            {

                return true;

            }

            ensure_cursor_visible();

            if (on_changed)

            {

                on_changed();

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (has_selection())

            {

                delete_selection();

            }

            else if (cursor_ < (int)text_.size())

            {

                text_.erase((size_t)cursor_, 1);

                sel_anchor_ = cursor_;

            }

            else

            {

                return true;

            }

            ensure_cursor_visible();

            if (on_changed)

            {

                on_changed();

            }

            return true;

        }

        // Printable text from XLookupString.

        if (str && str[0] >= 32 && str[0] != 127)

        {

            if (has_selection())

            {

                delete_selection();

            }

            for (int i = 0; str[i]; ++i)

            {

                if (str[i] < 32 || str[i] == 127)

                {

                    continue;

                }

                if (max_length_ > 0 && (int)text_.size() >= max_length_)

                {

                    break;

                }

                text_.insert((size_t)cursor_, 1, str[i]);

                ++cursor_;

            }

            sel_anchor_ = cursor_;

            ensure_cursor_visible();

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
