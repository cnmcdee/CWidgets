#pragma once

#include "core/xframe_core.h"

//region TNotebook

class TNotebook

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

    std::vector<std::string> pages_;

    int page_index_ = 0;

    void change_to(int index)

    {

        if (pages_.empty())

        {

            page_index_ = 0;

            return;

        }

        int clamped = std::max(0, std::min(index, (int)pages_.size() - 1));

        if (clamped != page_index_)

        {

            page_index_ = clamped;

            if (on_change_page)

            {

                on_change_page();

            }

        }

    }

public:

    std::function<void()> on_change_page;

    TNotebook(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TNotebook";

        text_ = "TNotebook";

        pages_.push_back("Page1");

        pages_.push_back("Page2");

        pages_.push_back("Page3");

        page_index_ = 0;

    }

    ~TNotebook() {}

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

    void add_page(std::string name)

    {

        pages_.push_back(name);

    }

    void delete_page(int index)

    {

        if (index < 0 || index >= (int)pages_.size())

        {

            return;

        }

        pages_.erase(pages_.begin() + index);

        if (page_index_ >= (int)pages_.size())

        {

            page_index_ = std::max(0, (int)pages_.size() - 1);

        }

    }

    void set_page_index(int index)

    {

        change_to(index);

    }

    int get_page_index()

    {

        return page_index_;

    }

    int page_count()

    {

        return (int)pages_.size();

    }

    std::string get_page_name(int index)

    {

        if (index < 0 || index >= (int)pages_.size())

        {

            return "";

        }

        return pages_[index];

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // The page area: a white-ish inset inside the frame. No tabs
        // are drawn -- page switching is by index only.

        int px1 = x1_ + 4;

        int py1 = y1_ + 4;

        int px2 = x2_ - 4;

        int py2 = y2_ - 4;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, px1, py1, px2 - px1, py2 - py1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, px1, py1, px2 - px1, py2 - py1);

        if (!pages_.empty())

        {

            SStyle page_style = st;

            page_style.text_color_ = styles_->get_color_rgb(0, 0, 0);

            xf::draw_text_centered(display_, window_, gc_, page_style, px1, py1 + 8, px2, py1 + 40, pages_[page_index_]);

            std::string status = "Page " + std::to_string(page_index_ + 1) + " of " + std::to_string((int)pages_.size());

            xf::draw_text_centered(display_, window_, gc_, page_style, px1, py2 - 32, px2, py2 - 8, status);

        }

        else

        {

            xf::draw_text_centered(display_, window_, gc_, st, px1, py1, px2, py2, "(no pages)");

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

        if (key == XK_Right || key == XK_Page_Down)

        {

            change_to(page_index_ + 1);

            return true;

        }

        if (key == XK_Left || key == XK_Page_Up)

        {

            change_to(page_index_ - 1);

            return true;

        }

        if (key == XK_Home)

        {

            change_to(0);

            return true;

        }

        if (key == XK_End)

        {

            change_to((int)pages_.size() - 1);

            return true;

        }

        return false;

    }

};

//endregion
