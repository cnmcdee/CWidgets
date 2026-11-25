#pragma once

#include "core/xframe_core.h"

//region TComboBoxEx

class TComboBoxEx

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

    struct SExItem

    {

        std::string caption;

        int indent;        // Lazarus TComboExItem.Indent

        int image_index;   // -1 = no image

    };

    std::vector<SExItem> items_

    {

        {"Fruit", 0, 0},

        {"Apple", 1, 1},

        {"Banana", 1, 2},

        {"Cherry", 1, 3},

        {"Vegetables", 0, 4},

        {"Carrot", 1, 5},

        {"Potato", 1, 6}

    };

    int item_index_ = 1;

    bool dropped_ = false;

    int row_h_ = 18;

    int button_w_ = 18;

    // Stand-in image colours, one per image index (an ImageList
    // would supply real pixmaps in a full application).

    unsigned long image_color(int index)

    {

        static const int rgb[8][3] =

        {

            {200, 60, 60}, {90, 180, 90}, {240, 210, 60}, {150, 60, 180},

            {240, 140, 40}, {60, 160, 200}, {120, 90, 60}, {90, 90, 90}

        };

        if (index < 0)

        {

            index = 0;

        }

        const int* c = rgb[index % 8];

        return styles_->get_color_rgb(c[0], c[1], c[2]);

    }

    void draw_image_box(int index, int x, int y, int size, const SStyle& st)

    {

        if (index < 0)

        {

            return;

        }

        XSetForeground(display_, gc_, image_color(index));

        XFillRectangle(display_, window_, gc_, x, y, size, size);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x, y, size, size);

    }

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

public:

    std::function<void()> on_click;

    std::function<void()> on_change;

    TComboBoxEx(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TComboBoxEx";

        text_ = items_[item_index_].caption;

    }

    ~TComboBoxEx() {}

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

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[i].caption == t)

            {

                set_item_index(i);

                return;

            }

        }

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    int get_item_index()

    {

        return item_index_;

    }

    void set_item_index(int i)

    {

        if (i < 0 || i >= (int)items_.size())

        {

            return;

        }

        bool changed = i != item_index_;

        item_index_ = i;

        text_ = items_[i].caption;

        if (changed && on_change)

        {

            on_change();

        }

    }

    void add_item(std::string caption, int indent, int image_index)

    {

        items_.push_back(SExItem{caption, indent, image_index});

    }

    int get_count()

    {

        return (int)items_.size();

    }

    int get_item_indent(int i)

    {

        return (i >= 0 && i < (int)items_.size()) ? items_[i].indent : 0;

    }

    int get_item_image(int i)

    {

        return (i >= 0 && i < (int)items_.size()) ? items_[i].image_index : -1;

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

        // Selected item: image box + caption, at its indent level.

        if (item_index_ >= 0 && item_index_ < (int)items_.size())

        {

            SExItem& it = items_[item_index_];

            int tx = x1_ + 4 + it.indent * 12;

            draw_image_box(it.image_index, tx, y1_ + (height() - 12) / 2, 12, st);

            if (it.image_index >= 0)

            {

                tx += 16;

            }

            xf::draw_text_left(display_, window_, gc_, st, tx, y1_, y2_, it.caption);

        }

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

                if (i == item_index_)

                {

                    XSetForeground(display_, gc_, st.outlineColor_);

                    XFillRectangle(display_, window_, gc_, x1_ + 1, ry, width() - 2, row_h_);

                }

                SStyle rs = st;

                if (i == item_index_)

                {

                    rs.text_color_ = styles_->get_color_rgb(255, 255, 255);

                }

                int tx = x1_ + 4 + items_[i].indent * 12;

                draw_image_box(items_[i].image_index, tx, ry + (row_h_ - 12) / 2, 12, rs);

                if (items_[i].image_index >= 0)

                {

                    tx += 16;

                }

                xf::draw_text_left(display_, window_, gc_, rs, tx, ry, ry + row_h_, items_[i].caption);

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

                set_item_index(i);

                if (on_click)

                {

                    on_click();

                }

            }

            dropped_ = false;

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

        if (key == XK_Up)

        {

            set_item_index(item_index_ - 1);

            return true;

        }

        if (key == XK_Down)

        {

            if (!dropped_)

            {

                dropped_ = true;

            }

            else

            {

                set_item_index(item_index_ + 1);

            }

            return true;

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

        return false;

    }

};

//endregion
