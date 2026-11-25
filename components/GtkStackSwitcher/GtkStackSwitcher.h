#pragma once

#include "core/xframe_core.h"

//region GtkStackSwitcher

class GtkStackSwitcher

{

public:

    struct Rect

    {

        int x1;

        int y1;

        int x2;

        int y2;

    };

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

    int selected_index_ = 0;

    int pressed_index_ = -1;

public:

    std::function<void(int)> on_page_selected;

    GtkStackSwitcher(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkStackSwitcher";

        text_ = "StackSwitcher";

        page_names_.push_back("home");

        page_names_.push_back("settings");

        page_names_.push_back("help");

        selected_index_ = 0;

    }

    ~GtkStackSwitcher() {}

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

    void set_page_names(std::vector<std::string> names)

    {

        page_names_ = names;

        if (selected_index_ >= (int)page_names_.size())

        {

            selected_index_ = page_names_.empty() ? 0 : (int)page_names_.size() - 1;

        }

    }

    std::vector<std::string> get_page_names()

    {

        return page_names_;

    }

    void set_selected_index(int i)

    {

        if (i < 0 || i >= (int)page_names_.size())

        {

            return;

        }

        selected_index_ = i;

    }

    int get_selected_index()

    {

        return selected_index_;

    }

    std::string get_selected_name()

    {

        if (selected_index_ < 0 || selected_index_ >= (int)page_names_.size())

        {

            return "";

        }

        return page_names_[selected_index_];

    }

    Rect button_rect(int index)

    {

        int n = (int)page_names_.size();

        if (n <= 0)

        {

            return Rect{x1_, y1_, x1_, y2_};

        }

        int bw = width() / n;

        int bx1 = x1_ + index * bw;

        int bx2 = (index == n - 1) ? x2_ : bx1 + bw;

        return Rect{bx1, y1_, bx2, y2_};

    }

    int index_at(int x, int y)

    {

        if (!contains(x, y))

        {

            return -1;

        }

        for (int i = 0; i < (int)page_names_.size(); ++i)

        {

            Rect r = button_rect(i);

            if (x >= r.x1 && x < r.x2)

            {

                return i;

            }

        }

        if (!page_names_.empty())

        {

            return (int)page_names_.size() - 1;

        }

        return -1;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        for (int i = 0; i < (int)page_names_.size(); ++i)

        {

            Rect r = button_rect(i);

            SStyle bst = st;

            bool sel = (i == selected_index_);

            if (sel || i == pressed_index_)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            if (!enabled_)

            {

                bst.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            xf::draw_polished(display_, window_, gc_, bst, r.x1, r.y1, r.x2, r.y2);

            xf::draw_text_centered(display_, window_, gc_, bst, r.x1, r.y1, r.x2, r.y2, page_names_[i]);

        }

        if (focused_ && !page_names_.empty())

        {

            Rect r = button_rect(selected_index_);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, r.x1 + 3, r.y1 + 3, (r.x2 - r.x1) - 6, (r.y2 - r.y1) - 6);

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

        int idx = index_at(x, y);

        if (idx < 0)

        {

            return true;

        }

        pressed_index_ = idx;

        selected_index_ = idx;

        if (on_page_selected)

        {

            on_page_selected(selected_index_);

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || pressed_index_ < 0)

        {

            return false;

        }

        pressed_index_ = -1;

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

        if (!focused_ || !enabled_ || page_names_.empty())

        {

            return false;

        }

        if (key == XK_Right)

        {

            selected_index_ = (selected_index_ + 1) % (int)page_names_.size();

        }

        else if (key == XK_Left)

        {

            selected_index_ = (selected_index_ + (int)page_names_.size() - 1) % (int)page_names_.size();

        }

        else

        {

            return false;

        }

        if (on_page_selected)

        {

            on_page_selected(selected_index_);

        }

        return true;

    }

};

//endregion
