#pragma once

#include "core/xframe_core.h"

//region GtkLinkButton

class GtkLinkButton

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

    bool visited_ = false;

    bool hover_ = false;

    std::string uri_ = "https://www.hotconfig.com";

public:

    std::function<void()> on_click;

    GtkLinkButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkLinkButton";

        text_ = "hotconfig.com";

    }

    ~GtkLinkButton() {}

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

    //region Link state

    void set_uri(std::string uri)

    {

        uri_ = uri;

    }

    std::string get_uri()

    {

        return uri_;

    }

    void set_visited(bool v)

    {

        visited_ = v;

    }

    bool get_visited()

    {

        return visited_;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        if (!st.font_ || text_.empty())

        {

            return;

        }

        // Link colours: blue, purple once visited, brighter on hover,
        // grey when the widget is disabled.

        unsigned long color;

        if (!enabled_)

        {

            color = styles_->get_color_rgb(140, 140, 140);

        }

        else if (visited_)

        {

            color = styles_->get_color_rgb(110, 45, 150);

        }

        else if (hover_)

        {

            color = styles_->get_color_rgb(60, 120, 255);

        }

        else

        {

            color = styles_->get_color_rgb(20, 80, 200);

        }

        XSetFont(display_, gc_, st.font_->fid);

        XSetForeground(display_, gc_, color);

        int tw = XTextWidth(st.font_, text_.c_str(), (int)text_.length());

        int text_x = x1_ + (width() - tw) / 2;

        int text_y = y1_ + (height() + st.font_->ascent - st.font_->descent) / 2;

        XDrawString(display_, window_, gc_, text_x, text_y, text_.c_str(), (int)text_.length());

        // The underline sits just under the baseline.

        xf::set_line_width(display_, gc_, 1);

        XDrawLine(display_, window_, gc_, text_x, text_y + st.font_->descent + 1, text_x + tw, text_y + st.font_->descent + 1);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

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

            visited_ = true;

            if (on_click)

            {

                on_click();

            }

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        bool inside = contains(x, y);

        if (inside != hover_)

        {

            hover_ = inside;

            return true;   // caller redraws, the colour changes

        }

        return inside;

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

            visited_ = true;

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
