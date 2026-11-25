#pragma once

#include "core/xframe_core.h"

//region TDirectoryEdit

class TDirectoryEdit

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

    std::string buffer_ = "/home/user/projects";

    int cursor_ = 19;

    int button_w_ = 23;

    bool button_pressed_ = false;

    std::string root_directory_ = "/";

    bool show_hidden_ = false;

public:

    std::function<void()> on_click;

    std::function<void()> on_button_click;

    std::function<void()> on_change;

    TDirectoryEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TDirectoryEdit";

        text_ = buffer_;

    }

    ~TDirectoryEdit() {}

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

        buffer_ = t;

        text_ = t;

        cursor_ = (int)buffer_.size();

    }

    std::string get_text()

    {

        return buffer_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    std::string get_directory()

    {

        return buffer_;

    }

    void set_directory(std::string d)

    {

        set_text(d);

    }

    void set_root_directory(std::string d)

    {

        root_directory_ = d;

    }

    std::string get_root_directory()

    {

        return root_directory_;

    }

    void set_show_hidden(bool s)

    {

        show_hidden_ = s;

    }

    bool get_show_hidden()

    {

        return show_hidden_;

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

        int bx = x2_ - button_w_;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        std::string shown = buffer_;

        while (!shown.empty() && xf::text_width(st, shown) > bx - x1_ - 8)

        {

            shown.erase(shown.begin());

        }

        if (shown.size() != buffer_.size())

        {

            shown = "..." + shown.substr(std::min<size_t>(3, shown.size()));

        }

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y2_, shown);

        if (focused_ && enabled_ && buffer_.size() == shown.size())

        {

            std::string before = buffer_.substr(0, cursor_);

            int cx = x1_ + 4 + xf::text_width(st, before);

            if (cx < bx - 2)

            {

                XSetForeground(display_, gc_, st.text_color_);

                XDrawLine(display_, window_, gc_, cx, y1_ + 3, cx, y2_ - 3);

            }

        }

        // The folder button zone: a tiny folder glyph.

        SStyle bs = st;

        if (button_pressed_)

        {

            bs.fillColor_ = st.outlineColor_;

        }

        xf::draw_polished(display_, window_, gc_, bs, bx, y1_, x2_, y2_);

        int fx = bx + 5;

        int fy = y1_ + height() / 2 - 5;

        int fw = button_w_ - 10;

        XSetForeground(display_, gc_, styles_->get_color_rgb(240, 200, 90));

        XFillRectangle(display_, window_, gc_, fx, fy + 3, fw, 8);

        XFillRectangle(display_, window_, gc_, fx, fy, fw / 2, 4);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, fx, fy + 3, fw, 8);

        XDrawLine(display_, window_, gc_, fx, fy + 3, fx, fy);

        XDrawLine(display_, window_, gc_, fx, fy, fx + fw / 2, fy);

        XDrawLine(display_, window_, gc_, fx + fw / 2, fy, fx + fw / 2, fy + 3);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (x >= x2_ - button_w_)

        {

            button_pressed_ = true;

            // The host would open a directory picker rooted at
            // root_directory_ here.

            if (on_button_click)

            {

                on_button_click();

            }

        }

        else

        {

            cursor_ = (int)buffer_.size();

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !button_pressed_)

        {

            return false;

        }

        button_pressed_ = false;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        bool changed = false;

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                buffer_.erase(cursor_ - 1, 1);

                --cursor_;

                changed = true;

            }

        }

        else if (key == XK_Delete)

        {

            if (cursor_ < (int)buffer_.size())

            {

                buffer_.erase(cursor_, 1);

                changed = true;

            }

        }

        else if (key == XK_Left)

        {

            if (cursor_ > 0)

            {

                --cursor_;

            }

        }

        else if (key == XK_Right)

        {

            if (cursor_ < (int)buffer_.size())

            {

                ++cursor_;

            }

        }

        else if (key == XK_Home)

        {

            cursor_ = 0;

        }

        else if (key == XK_End)

        {

            cursor_ = (int)buffer_.size();

        }

        else if (str && str[0] >= 32 && str[0] < 127)

        {

            buffer_.insert(cursor_, 1, str[0]);

            ++cursor_;

            changed = true;

        }

        else

        {

            return false;

        }

        text_ = buffer_;

        if (changed && on_change)

        {

            on_change();

        }

        return true;

    }

};

//endregion
