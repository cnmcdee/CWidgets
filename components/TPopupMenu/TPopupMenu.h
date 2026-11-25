#pragma once

#include "core/xframe_core.h"

//region TPopupMenu

class TPopupMenu

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

    std::vector<std::string> items_;   // "-" = separator

    int hover_index_ = -1;

    int row_height_ = 22;

    void update_size()

    {

        SStyle st = style();

        int w = 100;

        for (auto& e : items_)

        {

            if (e != "-")

            {

                int tw = xf::text_width(st, e) + 36;

                if (tw > w)

                {

                    w = tw;

                }

            }

        }

        x2_ = x1_ + w;

        y2_ = y1_ + (int)items_.size() * row_height_ + 8;

    }

    int index_at(int x, int y)

    {

        if (x < x1_ || x > x2_ || y < y1_ || y > y2_)

        {

            return -1;

        }

        int row = (y - y1_ - 4) / row_height_;

        if (row < 0 || row >= (int)items_.size())

        {

            return -1;

        }

        return row;

    }

public:

    std::function<void()> on_click;

    std::function<void(std::string)> on_menu_click;

    TPopupMenu(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPopupMenu";

        text_ = "PopupMenu";

        add_item("Cut");

        add_item("Copy");

        add_item("Paste");

        add_separator();

        add_item("Select All");

        // A popup starts hidden; popup_at() shows it.

        visible_ = false;

        update_size();

    }

    ~TPopupMenu() {}

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

    void add_item(std::string caption)

    {

        items_.push_back(caption);

        update_size();

    }

    void add_separator()

    {

        items_.push_back("-");

        update_size();

    }

    void clear_items()

    {

        items_.clear();

        hover_index_ = -1;

        update_size();

    }

    int item_count()

    {

        return (int)items_.size();

    }

    std::string get_item(int index)

    {

        if (index >= 0 && index < (int)items_.size())

        {

            return items_[index];

        }

        return "";

    }

    void popup_at(int x, int y)

    {

        x1_ = x;

        y1_ = y;

        update_size();

        hover_index_ = -1;

        visible_ = true;

        focused_ = true;

    }

    void popup()

    {

        visible_ = true;

        focused_ = true;

    }

    void hide()

    {

        visible_ = false;

        hover_index_ = -1;

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

        for (int r = 0; r < (int)items_.size(); ++r)

        {

            std::string entry = items_[r];

            int ry = y1_ + 4 + r * row_height_;

            if (entry == "-")

            {

                XSetForeground(display_, gc_, st.lineColor_);

                xf::set_line_width(display_, gc_, 1);

                XDrawLine(display_, window_, gc_, x1_ + 6, ry + row_height_ / 2, x2_ - 6, ry + row_height_ / 2);

                continue;

            }

            if (r == hover_index_ && enabled_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, x1_ + 3, ry, width() - 6, row_height_);

                SStyle hl = st;

                hl.text_color_ = st.fillColor_;

                xf::draw_text_left(display_, window_, gc_, hl, x1_ + 12, ry, ry + row_height_, entry);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, ry, ry + row_height_, entry);

            }

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!visible_ || button != 1)

        {

            return false;

        }

        if (!contains(x, y))

        {

            // Clicking away dismisses the popup.

            hide();

            return true;

        }

        if (!enabled_)

        {

            return true;

        }

        int row = index_at(x, y);

        if (row >= 0)

        {

            std::string entry = items_[row];

            if (entry != "-")

            {

                hide();

                if (on_menu_click)

                {

                    on_menu_click(entry);

                }

                if (on_click)

                {

                    on_click();

                }

            }

            return true;

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

        if (!visible_)

        {

            return false;

        }

        if (contains(x, y))

        {

            hover_index_ = index_at(x, y);

            return true;

        }

        hover_index_ = -1;

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!visible_ || !focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Escape)

        {

            hide();

            return true;

        }

        if (key == XK_Down)

        {

            int n = (int)items_.size();

            if (n > 0)

            {

                hover_index_ = (hover_index_ + 1) % n;

                if (items_[hover_index_] == "-")

                {

                    hover_index_ = (hover_index_ + 1) % n;

                }

            }

            return true;

        }

        if (key == XK_Up)

        {

            int n = (int)items_.size();

            if (n > 0)

            {

                hover_index_ = (hover_index_ + n - 1) % n;

                if (items_[hover_index_] == "-")

                {

                    hover_index_ = (hover_index_ + n - 1) % n;

                }

            }

            return true;

        }

        if ((key == XK_Return || key == XK_space) && hover_index_ >= 0)

        {

            std::string entry = items_[hover_index_];

            hide();

            if (entry != "-" && on_menu_click)

            {

                on_menu_click(entry);

            }

            return true;

        }

        return false;

    }

};

//endregion
