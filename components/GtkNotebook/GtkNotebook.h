#pragma once

#include "core/xframe_core.h"

//region GtkNotebook

class GtkNotebook

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

    std::vector<std::string> pages_;

    int current_page_ = 0;

    int tab_height_ = 26;

public:

    std::function<void(int)> on_page_changed;

    GtkNotebook(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkNotebook";

        text_ = "Notebook";

        pages_.push_back("General");

        pages_.push_back("Advanced");

        pages_.push_back("About");

        current_page_ = 0;

    }

    ~GtkNotebook() {}

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

    void append_page(std::string label)

    {

        pages_.push_back(label);

    }

    void remove_page(int index)

    {

        if (index < 0 || index >= (int)pages_.size())

        {

            return;

        }

        pages_.erase(pages_.begin() + index);

        if (current_page_ >= (int)pages_.size())

        {

            current_page_ = pages_.empty() ? 0 : (int)pages_.size() - 1;

        }

    }

    void set_current_page(int i)

    {

        if (i < 0 || i >= (int)pages_.size() || i == current_page_)

        {

            return;

        }

        current_page_ = i;

        if (on_page_changed)

        {

            on_page_changed(current_page_);

        }

    }

    int get_current_page()

    {

        return current_page_;

    }

    int get_page_count()

    {

        return (int)pages_.size();

    }

    std::string get_page_label(int index)

    {

        if (index < 0 || index >= (int)pages_.size())

        {

            return "";

        }

        return pages_[index];

    }

    Rect tab_rect(int index)

    {

        SStyle st = style();

        int tx = x1_ + 4;

        for (int i = 0; i < index && i < (int)pages_.size(); ++i)

        {

            tx += xf::text_width(st, pages_[i]) + 24;

        }

        int tw = 0;

        if (index >= 0 && index < (int)pages_.size())

        {

            tw = xf::text_width(st, pages_[index]) + 24;

        }

        return Rect{tx, y1_, tx + tw, y1_ + tab_height_};

    }

    Rect page_rect()

    {

        return Rect{x1_, y1_ + tab_height_, x2_, y2_};

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

        Rect pr = page_rect();

        xf::draw_polished(display_, window_, gc_, st, pr.x1, pr.y1, pr.x2, pr.y2);

        for (int i = 0; i < (int)pages_.size(); ++i)

        {

            Rect tr = tab_rect(i);

            SStyle tst = st;

            if (i == current_page_)

            {

                tst.fillColor_ = styles_->get_color_rgb(245, 245, 245);

            }

            else

            {

                tst.fillColor_ = styles_->get_color_rgb(195, 195, 195);

                tst.text_color_ = styles_->get_color_rgb(70, 70, 70);

            }

            if (!enabled_)

            {

                tst.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            xf::fill_rounded_rect(display_, window_, gc_, tst, tr.x1, tr.y1 + 2, tr.x2, tr.y2 + 4);

            xf::draw_rounded_outline(display_, window_, gc_, tst, tr.x1, tr.y1 + 2, tr.x2, tr.y2 + 4);

            xf::draw_text_centered(display_, window_, gc_, tst, tr.x1, tr.y1, tr.x2, tr.y2, pages_[i]);

        }

        if (!pages_.empty())

        {

            xf::draw_text_centered(display_, window_, gc_, st, pr.x1, pr.y1, pr.x2, pr.y2, pages_[current_page_]);

        }

        else

        {

            xf::draw_text_centered(display_, window_, gc_, st, pr.x1, pr.y1, pr.x2, pr.y2, "(no pages)");

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

        for (int i = 0; i < (int)pages_.size(); ++i)

        {

            Rect tr = tab_rect(i);

            if (x >= tr.x1 && x <= tr.x2 && y >= tr.y1 && y <= tr.y2)

            {

                set_current_page(i);

                return true;

            }

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

        if (!focused_ || !enabled_ || pages_.empty())

        {

            return false;

        }

        if (key == XK_Right)

        {

            set_current_page((current_page_ + 1) % (int)pages_.size());

            return true;

        }

        if (key == XK_Left)

        {

            set_current_page((current_page_ + (int)pages_.size() - 1) % (int)pages_.size());

            return true;

        }

        return false;

    }

};

//endregion
