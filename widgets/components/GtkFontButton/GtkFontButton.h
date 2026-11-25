#pragma once

#include "core/xframe_core.h"

//region GtkFontButton

class GtkFontButton

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

    std::string font_name_ = "Sans";

    int font_size_ = 12;

    // The caption is the font description, e.g. "Sans 12"; keep the
    // two in step whenever either half changes.

    void sync_text()

    {

        text_ = font_name_ + " " + std::to_string(font_size_);

    }

public:

    std::function<void()> on_click;

    std::function<void(std::string)> on_font_set;

    GtkFontButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFontButton";

        sync_text();

    }

    ~GtkFontButton() {}

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

        // Setting the caption directly is setting the description.

        set_font_desc(t);

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region Font state

    void set_font_name(std::string name)

    {

        font_name_ = name;

        sync_text();

    }

    std::string get_font_name()

    {

        return font_name_;

    }

    void set_font_size(int size)

    {

        if (size > 0)

        {

            font_size_ = size;

        }

        sync_text();

    }

    int get_font_size()

    {

        return font_size_;

    }

    std::string get_font_desc()

    {

        return text_;

    }

    void set_font_desc(std::string desc)

    {

        // Split "Family Name 14" into the family and the size: the
        // last word counts as the size when it is all digits.

        size_t sp = desc.find_last_of(' ');

        if (sp != std::string::npos && sp + 1 < desc.size())

        {

            std::string tail = desc.substr(sp + 1);

            bool digits = true;

            for (char c : tail)

            {

                if (c < '0' || c > '9')

                {

                    digits = false;

                }

            }

            if (digits)

            {

                font_name_ = desc.substr(0, sp);

                font_size_ = std::atoi(tail.c_str());

                sync_text();

                return;

            }

        }

        font_name_ = desc;

        sync_text();

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

        if (pressed_)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // The real button previews the font itself; here the face
        // carries the description, which is what it falls back to
        // whenever a preview cannot be rendered either.

        xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y2_, text_);

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

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        if (contains(x, y))

        {

            if (on_font_set)

            {

                on_font_set(get_font_desc());

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

        if (key == XK_Return || key == XK_space)

        {

            if (on_font_set)

            {

                on_font_set(get_font_desc());

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
