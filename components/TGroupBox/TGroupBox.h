#pragma once

#include "core/xframe_core.h"

//region TGroupBox

class TGroupBox

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

    std::string style_name_;

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

public:

    std::function<void()> on_click;

    TGroupBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TGroupBox";

        text_ = "GroupBox";

    }

    ~TGroupBox() {}

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

    std::string get_caption()

    {

        return text_;

    }

    void set_caption(std::string t)

    {

        text_ = t;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region Client area helpers

    int client_left()

    {

        return x1_ + 8;

    }

    int client_top()

    {

        return y1_ + 18;

    }

    int client_right()

    {

        return x2_ - 8;

    }

    int client_bottom()

    {

        return y2_ - 8;

    }

    int client_width()

    {

        return client_right() - client_left();

    }

    int client_height()

    {

        return client_bottom() - client_top();

    }

    void client_rect(int& cx1, int& cy1, int& cx2, int& cy2)

    {

        cx1 = client_left();

        cy1 = client_top();

        cx2 = client_right();

        cy2 = client_bottom();

    }

    bool in_client(int x, int y)

    {

        return x >= client_left() && x <= client_right() && y >= client_top() && y <= client_bottom();

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

        xf::fill_rounded_rect(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        if (!text_.empty() && st.font_)

        {

            int tw = xf::text_width(st, text_);

            int cap_x = x1_ + 10;

            XSetForeground(display_, gc_, st.fillColor_);

            XFillRectangle(display_, window_, gc_, cap_x - 3, y1_ - 1, tw + 6, 3);

            XFillRectangle(display_, window_, gc_, cap_x - 3, y1_, tw + 6, st.font_->ascent + st.font_->descent);

            xf::draw_text_left(display_, window_, gc_, st, cap_x, y1_ - 6, y1_ + 10, text_);

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

        if (on_click)

        {

            on_click();

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
