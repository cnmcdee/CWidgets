#pragma once

#include "core/xframe_core.h"

//region GtkStack

class GtkStack

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

    std::vector<std::string> page_names_;

    std::string visible_child_name_;

    int transition_type_ = 0;

    int transition_duration_ = 250;

public:

    std::function<void(std::string)> on_visible_child_changed;

    GtkStack(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkStack";

        text_ = "Stack";

        page_names_.push_back("home");

        page_names_.push_back("settings");

        page_names_.push_back("help");

        visible_child_name_ = "home";

    }

    ~GtkStack() {}

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

    void add_named(std::string page_name)

    {

        for (auto& n : page_names_)

        {

            if (n == page_name)

            {

                return;

            }

        }

        page_names_.push_back(page_name);

        if (visible_child_name_.empty())

        {

            visible_child_name_ = page_name;

        }

    }

    bool has_page(std::string page_name)

    {

        for (auto& n : page_names_)

        {

            if (n == page_name)

            {

                return true;

            }

        }

        return false;

    }

    void set_visible_child_name(std::string page_name)

    {

        if (!has_page(page_name) || page_name == visible_child_name_)

        {

            return;

        }

        visible_child_name_ = page_name;

        if (on_visible_child_changed)

        {

            on_visible_child_changed(visible_child_name_);

        }

    }

    std::string get_visible_child_name()

    {

        return visible_child_name_;

    }

    int get_visible_index()

    {

        for (int i = 0; i < (int)page_names_.size(); ++i)

        {

            if (page_names_[i] == visible_child_name_)

            {

                return i;

            }

        }

        return -1;

    }

    int get_page_count()

    {

        return (int)page_names_.size();

    }

    void set_transition_type(int t)

    {

        transition_type_ = t;

    }

    int get_transition_type()

    {

        return transition_type_;

    }

    void set_transition_duration(int ms)

    {

        transition_duration_ = ms;

    }

    int get_transition_duration()

    {

        return transition_duration_;

    }

    void cycle(int delta)

    {

        if (page_names_.empty())

        {

            return;

        }

        int idx = get_visible_index();

        if (idx < 0)

        {

            idx = 0;

        }

        idx = (idx + delta + (int)page_names_.size()) % (int)page_names_.size();

        set_visible_child_name(page_names_[idx]);

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

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int mid = y1_ + height() / 2;

        xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_ + 10, x2_, mid + 20, visible_child_name_);

        int idx = get_visible_index();

        std::string info = "page " + std::to_string(idx + 1) + " of " + std::to_string((int)page_names_.size());

        SStyle small = st;

        small.text_color_ = styles_->get_color_rgb(110, 110, 110);

        if (!enabled_)

        {

            small.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        xf::draw_text_centered(display_, window_, gc_, small, x1_, mid + 20, x2_, y2_ - 10, info);

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

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Right || key == XK_Down)

        {

            cycle(1);

            return true;

        }

        if (key == XK_Left || key == XK_Up)

        {

            cycle(-1);

            return true;

        }

        return false;

    }

};

//endregion
