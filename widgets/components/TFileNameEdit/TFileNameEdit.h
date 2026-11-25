#pragma once

#include "core/xframe_core.h"

//region TFileNameEdit

class TFileNameEdit

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

    std::string buffer_ = "/home/user/document.txt";

    int cursor_ = 23;

    int button_w_ = 23;

    bool button_pressed_ = false;

    // Lazarus TDialogKind: dkOpen, dkSave, dkOpenRecent, dkSelectDirectory...

    int dialog_kind_ = 0;   // 0 = open, 1 = save, 2 = open-recent

    std::string filter_ = "All files (*.*)|*.*";

    std::string initial_directory_ = "/home/user";

    std::string default_extension_ = ".txt";

public:

    std::function<void()> on_click;

    std::function<void()> on_button_click;

    std::function<void()> on_change;

    TFileNameEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TFileNameEdit";

        text_ = buffer_;

    }

    ~TFileNameEdit() {}

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

    std::string get_filename()

    {

        return buffer_;

    }

    void set_filename(std::string f)

    {

        set_text(f);

    }

    void set_dialog_kind(int k)

    {

        dialog_kind_ = k;

    }

    int get_dialog_kind()

    {

        return dialog_kind_;

    }

    void set_filter(std::string f)

    {

        filter_ = f;

    }

    std::string get_filter()

    {

        return filter_;

    }

    void set_initial_directory(std::string d)

    {

        initial_directory_ = d;

    }

    std::string get_initial_directory()

    {

        return initial_directory_;

    }

    void set_default_extension(std::string e)

    {

        default_extension_ = e;

    }

    std::string get_default_extension()

    {

        return default_extension_;

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

        // Long paths are clipped to the field by drawing only the
        // tail that fits (the file name end is what matters).

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

        if (focused_ && enabled_)

        {

            std::string before = buffer_.substr(0, cursor_);

            int cx = x1_ + 4 + xf::text_width(st, before);

            if (cx < bx - 2 && buffer_.size() == shown.size())

            {

                XSetForeground(display_, gc_, st.text_color_);

                XDrawLine(display_, window_, gc_, cx, y1_ + 3, cx, y2_ - 3);

            }

        }

        // The "..." browse button zone.

        SStyle bs = st;

        if (button_pressed_)

        {

            bs.fillColor_ = st.outlineColor_;

        }

        xf::draw_polished(display_, window_, gc_, bs, bx, y1_, x2_, y2_);

        xf::draw_text_centered(display_, window_, gc_, bs, bx, y1_, x2_, y2_, "...");

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

            // In a real app the host now runs the file dialog named
            // by dialog_kind_ and drops the result into the field.

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
