#pragma once

#include "core/xframe_core.h"

//region GtkStatusbar

// The bar at the bottom of a window: a sunken frame showing the
// most recent message from a stack. push() adds a message and
// returns its id; pop() removes the newest one.

class GtkStatusbar

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

    std::vector<std::string> messages_;   // back() is showing

    int next_id_ = 1;

public:

    GtkStatusbar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkStatusbar";

        text_ = "Ready";

        push(text_);

    }

    ~GtkStatusbar() {}

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

        if (!messages_.empty())

        {

            messages_.back() = t;

        }

        else

        {

            push(t);

        }

    }

    std::string get_text()

    {

        return messages_.empty() ? text_ : messages_.back();

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkStatusbar API

    unsigned int push(std::string message)

    {

        messages_.push_back(message);

        return (unsigned int)next_id_++;

    }

    void pop()

    {

        if (!messages_.empty())

        {

            messages_.pop_back();

        }

    }

    void remove_all()

    {

        messages_.clear();

    }

    int message_count()

    {

        return (int)messages_.size();

    }

    std::string current_message()

    {

        return messages_.empty() ? std::string("") : messages_.back();

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Bar body.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        // Sunken frame: dark line top/left, light line bottom/right.

        XSetForeground(display_, gc_, styles_->get_color_rgb(120, 120, 120));

        XDrawLine(display_, window_, gc_, x1_, y1_, x2_ - 1, y1_);

        XDrawLine(display_, window_, gc_, x1_, y1_, x1_, y2_ - 1);

        XSetForeground(display_, gc_, styles_->get_color_rgb(245, 245, 245));

        XDrawLine(display_, window_, gc_, x1_, y2_ - 1, x2_ - 1, y2_ - 1);

        XDrawLine(display_, window_, gc_, x2_ - 1, y1_, x2_ - 1, y2_ - 1);

        // Current message, left aligned, clipped at the grip.

        std::string msg = current_message();

        while (!msg.empty() && xf::text_width(st, msg) > width() - 28)

        {

            msg.pop_back();

        }

        SStyle lab = st;

        if (!enabled_)

        {

            lab.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        xf::draw_text_left(display_, window_, gc_, lab, x1_ + 6, y1_, y2_, msg);

        // Resize grip: three diagonal lines in the bottom-right.

        XSetForeground(display_, gc_, styles_->get_color_rgb(150, 150, 150));

        for (int i = 0; i < 3; ++i)

        {

            int off = 4 + i * 4;

            XDrawLine(display_, window_, gc_, x2_ - off, y2_ - 2, x2_ - 2, y2_ - off);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
