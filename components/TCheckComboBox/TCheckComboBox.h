#pragma once

#include "core/xframe_core.h"

//region TCheckComboBox

class TCheckComboBox

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

    struct SCheckItem

    {

        std::string caption;

        bool checked;

    };

    std::vector<SCheckItem> items_

    {

        {"Bold", true},

        {"Italic", false},

        {"Underline", true},

        {"Strikeout", false},

        {"Small caps", false}

    };

    bool dropped_ = false;

    int row_h_ = 18;

    int button_w_ = 18;

    int list_y1()

    {

        return y2_ + 1;

    }

    int list_y2()

    {

        return list_y1() + (int)items_.size() * row_h_ + 2;

    }

    bool in_list(int x, int y)

    {

        return dropped_ && x >= x1_ && x <= x2_ && y >= list_y1() && y <= list_y2();

    }

    // The field caption summarises the checked set.

    std::string caption_text()

    {

        int n = get_checked_count();

        if (n == 0)

        {

            return "(none)";

        }

        if (n <= 2)

        {

            std::string out;

            for (auto& it : items_)

            {

                if (it.checked)

                {

                    if (!out.empty())

                    {

                        out += ", ";

                    }

                    out += it.caption;

                }

            }

            return out;

        }

        return std::to_string(n) + " items checked";

    }

    void refresh_text()

    {

        text_ = caption_text();

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_change;

    TCheckComboBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCheckComboBox";

        refresh_text();

    }

    ~TCheckComboBox() {}

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

        return visible_ && ((x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_) || in_list(x, y));

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

        (void)t;   // the caption is derived from the checked set

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    int get_count()

    {

        return (int)items_.size();

    }

    int get_checked_count()

    {

        int n = 0;

        for (auto& it : items_)

        {

            if (it.checked)

            {

                ++n;

            }

        }

        return n;

    }

    bool is_checked(int i)

    {

        return i >= 0 && i < (int)items_.size() && items_[i].checked;

    }

    void set_checked(int i, bool c)

    {

        if (i < 0 || i >= (int)items_.size() || items_[i].checked == c)

        {

            return;

        }

        items_[i].checked = c;

        refresh_text();

        if (on_change)

        {

            on_change();

        }

    }

    void toggle_checked(int i)

    {

        if (i >= 0 && i < (int)items_.size())

        {

            set_checked(i, !items_[i].checked);

        }

    }

    void add_item(std::string caption, bool checked)

    {

        items_.push_back(SCheckItem{caption, checked});

        refresh_text();

    }

    std::string get_checked_text()

    {

        std::string out;

        for (auto& it : items_)

        {

            if (it.checked)

            {

                if (!out.empty())

                {

                    out += ", ";

                }

                out += it.caption;

            }

        }

        return out;

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

        int bx = x2_ - button_w_;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 4, y1_, y2_, text_);

        xf::draw_polished(display_, window_, gc_, st, bx, y1_, x2_, y2_);

        xf::draw_triangle_down(display_, window_, gc_, st.text_color_, bx + button_w_ / 2, y1_ + height() / 2 - 2, 4);

        if (dropped_)

        {

            int ly = list_y1();

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, x1_, ly, width(), list_y2() - ly);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, ly, width() - 1, list_y2() - ly - 1);

            for (int i = 0; i < (int)items_.size(); ++i)

            {

                int ry = ly + 1 + i * row_h_;

                // Checkbox square.

                int cx = x1_ + 4;

                int cy = ry + (row_h_ - 12) / 2;

                XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

                XFillRectangle(display_, window_, gc_, cx, cy, 12, 12);

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, cx, cy, 12, 12);

                if (items_[i].checked)

                {

                    xf::draw_checkmark(display_, window_, gc_, st.text_color_, cx + 6, cy + 6, 10);

                }

                xf::draw_text_left(display_, window_, gc_, st, cx + 17, ry, ry + row_h_, items_[i].caption);

            }

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!visible_ || !enabled_ || button != 1)

        {

            return false;

        }

        if (in_list(x, y))

        {

            int i = (y - list_y1() - 1) / row_h_;

            if (i >= 0 && i < (int)items_.size())

            {

                toggle_checked(i);

                if (on_click)

                {

                    on_click();

                }

            }

            // The list stays open so several items can be toggled.

            return true;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            focused_ = true;

            dropped_ = !dropped_;

            return true;

        }

        dropped_ = false;

        return false;

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

        if (key == XK_Return || key == XK_space)

        {

            dropped_ = !dropped_;

            return true;

        }

        if (key == XK_Escape)

        {

            dropped_ = false;

            return true;

        }

        if (key == XK_Down && !dropped_)

        {

            dropped_ = true;

            return true;

        }

        return false;

    }

};

//endregion
