#pragma once

#include "core/xframe_core.h"

//region GtkStackSidebar

class GtkStackSidebar

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

    int row_height_ = 30;

public:

    std::function<void(int)> on_page_selected;

    GtkStackSidebar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkStackSidebar";

        text_ = "StackSidebar";

        page_names_.push_back("home");

        page_names_.push_back("settings");

        page_names_.push_back("help");

        selected_index_ = 0;

    }

    ~GtkStackSidebar() {}

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

    void set_row_height(int h)

    {

        if (h >= 16)

        {

            row_height_ = h;

        }

    }

    int get_row_height()

    {

        return row_height_;

    }

    Rect row_rect(int index)

    {

        int ry1 = y1_ + index * row_height_;

        return Rect{x1_, ry1, x2_, ry1 + row_height_};

    }

    int index_at(int x, int y)

    {

        if (!contains(x, y))

        {

            return -1;

        }

        int idx = (y - y1_) / row_height_;

        if (idx < 0 || idx >= (int)page_names_.size())

        {

            return -1;

        }

        return idx;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        for (int i = 0; i < (int)page_names_.size(); ++i)

        {

            Rect r = row_rect(i);

            if (r.y1 >= y2_)

            {

                break;

            }

            SStyle rst = st;

            if (!enabled_)

            {

                rst.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            if (i == selected_index_)

            {

                XSetForeground(display_, gc_, styles_->get_color_rgb(70, 120, 200));

                XFillRectangle(display_, window_, gc_, r.x1 + 2, r.y1 + 1, (r.x2 - r.x1) - 4, (r.y2 - r.y1) - 2);

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

                if (!enabled_)

                {

                    rst.text_color_ = styles_->get_color_rgb(200, 200, 200);

                }

                xf::draw_text_left(display_, window_, gc_, rst, r.x1 + 12, r.y1, r.y2, page_names_[i]);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, rst, r.x1 + 12, r.y1, r.y2, page_names_[i]);

            }

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawLine(display_, window_, gc_, r.x1 + 4, r.y2, r.x2 - 4, r.y2);

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, height() - 4);

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

        if (!focused_ || !enabled_ || page_names_.empty())

        {

            return false;

        }

        if (key == XK_Down)

        {

            selected_index_ = (selected_index_ + 1) % (int)page_names_.size();

        }

        else if (key == XK_Up)

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
