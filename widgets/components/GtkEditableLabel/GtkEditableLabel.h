#pragma once

#include "core/xframe_core.h"

//region GtkEditableLabel

// A label that turns into a text entry when double-clicked (or when
// start_editing() is called). Enter commits, Escape cancels.

class GtkEditableLabel

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

    bool editing_ = false;

    std::string buffer_;       // edit buffer while editing_

    int cursor_ = 0;           // insertion point in buffer_

    int press_count_ = 0;      // consecutive presses -> double click

public:

    std::function<void()> on_editing_done;

    std::function<void()> on_editing_canceled;

    GtkEditableLabel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkEditableLabel";

        text_ = "Double-click to edit";

    }

    ~GtkEditableLabel() {}

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

        if (editing_)

        {

            buffer_ = t;

            cursor_ = (int)buffer_.size();

        }

    }

    std::string get_text()

    {

        return editing_ ? buffer_ : text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkEditableLabel API

    bool get_editing()

    {

        return editing_;

    }

    void start_editing()

    {

        if (!enabled_ || editing_)

        {

            return;

        }

        editing_ = true;

        focused_ = true;

        buffer_ = text_;

        cursor_ = (int)buffer_.size();

    }

    void stop_editing(bool apply)

    {

        if (!editing_)

        {

            return;

        }

        editing_ = false;

        if (apply)

        {

            text_ = buffer_;

            if (on_editing_done)

            {

                on_editing_done();

            }

        }

        else if (on_editing_canceled)

        {

            on_editing_canceled();

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

        if (!enabled_)

        {

            st.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        if (editing_)

        {

            // Entry look: white body, polished outline, buffer + bar.

            SStyle entry = st;

            entry.fillColor_ = styles_->get_color_rgb(255, 255, 255);

            xf::draw_polished(display_, window_, gc_, entry, x1_, y1_, x2_, y2_);

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 5, y1_, y2_, buffer_);

            if (focused_)

            {

                int cx = x1_ + 5 + xf::text_width(st, buffer_.substr(0, (size_t)cursor_));

                XSetForeground(display_, gc_, st.text_color_);

                XDrawLine(display_, window_, gc_, cx, y1_ + 4, cx, y2_ - 4);

            }

        }

        else

        {

            if (focused_)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(180, 180, 180));

                XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

            }

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 2, y1_, y2_, text_);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            if (!contains(x, y))

            {

                press_count_ = 0;

                if (editing_)

                {

                    stop_editing(true);   // click away commits, like GTK

                }

            }

            return false;

        }

        if (!enabled_)

        {

            return false;

        }

        focused_ = true;

        if (editing_)

        {

            // Place the cursor by click position inside the buffer.

            SStyle st = style();

            int rel = x - (x1_ + 5);

            int best = 0;

            int best_dist = 1 << 30;

            for (size_t i = 0; i <= buffer_.size(); ++i)

            {

                int cx = xf::text_width(st, buffer_.substr(0, i));

                int d = std::abs(cx - rel);

                if (d < best_dist)

                {

                    best_dist = d;

                    best = (int)i;

                }

            }

            cursor_ = best;

            return true;

        }

        ++press_count_;

        if (press_count_ >= 2)

        {

            press_count_ = 0;

            start_editing();

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

        if (!editing_ || !focused_)

        {

            return false;

        }

        if (key == XK_Return || key == XK_KP_Enter)

        {

            stop_editing(true);

            return true;

        }

        if (key == XK_Escape)

        {

            stop_editing(false);

            return true;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                buffer_.erase((size_t)(cursor_ - 1), 1);

                --cursor_;

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)buffer_.size())

            {

                buffer_.erase((size_t)cursor_, 1);

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

            if (cursor_ < (int)buffer_.size())

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

            cursor_ = (int)buffer_.size();

            return true;

        }

        if (str && str[0] >= 32 && str[0] != 127)

        {

            buffer_.insert((size_t)cursor_, 1, str[0]);

            ++cursor_;

            return true;

        }

        return false;

    }

};

//endregion
