#pragma once

#include "core/xframe_core.h"

//region GtkEventBox

class GtkEventBox

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

    bool above_child_ = false;

    bool visible_window_ = true;

    bool design_time_ = true;

    std::string last_event_;

    int event_count_ = 0;

public:

    std::function<void()> on_click;

    GtkEventBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkEventBox";

        text_ = "EventBox";

        last_event_ = "";

    }

    ~GtkEventBox() {}

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

    void set_above_child(bool a)

    {

        above_child_ = a;

    }

    bool get_above_child()

    {

        return above_child_;

    }

    void set_visible_window(bool v)

    {

        visible_window_ = v;

    }

    bool get_visible_window()

    {

        return visible_window_;

    }

    void set_design_time(bool d)

    {

        design_time_ = d;

    }

    std::string get_last_event()

    {

        return last_event_;

    }

    int get_event_count()

    {

        return event_count_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        if (visible_window_)

        {

            // Very faint fill so the catch area is just barely visible.

            XSetForeground(display_, gc_, styles_->get_color_rgb(245, 245, 248));

            XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

            XSetForeground(display_, gc_, styles_->get_color_rgb(200, 200, 205));

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

            SStyle label = st;

            label.text_color_ = styles_->get_color_rgb(150, 150, 155);

            xf::draw_text_centered(display_, window_, gc_, label, x1_, y1_, x2_, y2_, text_);

        }

        else if (design_time_)

        {

            // No window of its own: just a dotted outline at design time.

            XSetForeground(display_, gc_, styles_->get_color_rgb(170, 170, 175));

            for (int x = x1_; x < x2_; x += 6)

            {

                XDrawLine(display_, window_, gc_, x, y1_, std::min(x + 3, x2_), y1_);

                XDrawLine(display_, window_, gc_, x, y2_ - 1, std::min(x + 3, x2_), y2_ - 1);

            }

            for (int y = y1_; y < y2_; y += 6)

            {

                XDrawLine(display_, window_, gc_, x1_, y, x1_, std::min(y + 3, y2_));

                XDrawLine(display_, window_, gc_, x2_ - 1, y, x2_ - 1, std::min(y + 3, y2_));

            }

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

        last_event_ = "press " + std::to_string(x) + "," + std::to_string(y);

        event_count_++;

        if (on_click)

        {

            on_click();

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        last_event_ = "release " + std::to_string(x) + "," + std::to_string(y);

        event_count_++;

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!contains(x, y))

        {

            return false;

        }

        last_event_ = "motion " + std::to_string(x) + "," + std::to_string(y);

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        last_event_ = "key " + std::to_string((int)key);

        event_count_++;

        return true;

    }

};

//endregion
