#pragma once

#include "core/xframe_core.h"

//region TToolBar

class TToolBar

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

    struct TToolButton

    {

        std::string caption;

        bool separator = false;

        bool dropdown = false;

        bool checked = false;

    };

    std::vector<TToolButton> buttons_;

    bool wrapable_ = true;

    bool flat_ = false;

    int pressed_index_ = -1;

    struct ButtonRect

    {

        int index;

        int rx1, ry1, rx2, ry2;

    };

    std::vector<ButtonRect> rects_;

    int button_width(TToolButton& b, SStyle& st)

    {

        if (b.separator)

        {

            return 10;

        }

        int w = xf::text_width(st, b.caption) + 16;

        if (b.dropdown)

        {

            w += 12;

        }

        return w;

    }

    // Lay the buttons out left to right, wrapping onto a new row
    // when wrapable_ and the row is full. Row height is 28.

    void layout()

    {

        rects_.clear();

        SStyle st = style();

        int row_h = 28;

        int cx = x1_ + 3;

        int cy = y1_ + 3;

        for (size_t i = 0; i < buttons_.size(); ++i)

        {

            int w = button_width(buttons_[i], st);

            if (wrapable_ && cx + w > x2_ - 3 && cx > x1_ + 3)

            {

                cx = x1_ + 3;

                cy += row_h + 2;

            }

            ButtonRect r;

            r.index = (int)i;

            r.rx1 = cx;

            r.ry1 = cy;

            r.rx2 = cx + w;

            r.ry2 = cy + row_h;

            rects_.push_back(r);

            cx += w + 2;

        }

    }

    int button_at(int x, int y)

    {

        layout();

        for (auto& r : rects_)

        {

            if (x >= r.rx1 && x <= r.rx2 && y >= r.ry1 && y <= r.ry2)

            {

                return r.index;

            }

        }

        return -1;

    }

public:

    std::function<void(int)> on_button_click;

    TToolBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TToolBar";

        text_ = "TToolBar";

        add_button("New");

        add_button("Open");

        add_button("Save");

        add_separator();

        add_button("Cut");

        add_button("Copy");

        add_button("Paste");

        add_separator();

        add_button("Help", true);

    }

    ~TToolBar() {}

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

    void add_button(std::string caption, bool dropdown = false)

    {

        TToolButton b;

        b.caption = caption;

        b.dropdown = dropdown;

        buttons_.push_back(b);

    }

    void add_separator()

    {

        TToolButton b;

        b.separator = true;

        buttons_.push_back(b);

    }

    int button_count()

    {

        return (int)buttons_.size();

    }

    void set_wrapable(bool w)

    {

        wrapable_ = w;

    }

    bool get_wrapable()

    {

        return wrapable_;

    }

    void set_flat(bool f)

    {

        flat_ = f;

    }

    void set_checked(int index, bool c)

    {

        if (index >= 0 && index < (int)buttons_.size())

        {

            buttons_[index].checked = c;

        }

    }

    bool get_checked(int index)

    {

        if (index >= 0 && index < (int)buttons_.size())

        {

            return buttons_[index].checked;

        }

        return false;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        SStyle ts = st;

        if (!enabled_)

        {

            ts.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        // Bar background.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_, y2_ - 1, x2_, y2_ - 1);

        layout();

        for (auto& r : rects_)

        {

            TToolButton& b = buttons_[r.index];

            if (b.separator)

            {

                int cx = (r.rx1 + r.rx2) / 2;

                XSetForeground(display_, gc_, styles_->get_color_rgb(150, 150, 150));

                XDrawLine(display_, window_, gc_, cx, r.ry1 + 4, cx, r.ry2 - 4);

                continue;

            }

            if (!flat_ || pressed_index_ == r.index || b.checked)

            {

                SStyle bs = st;

                if (pressed_index_ == r.index || b.checked)

                {

                    unsigned long swap = bs.fillColor_;

                    bs.fillColor_ = bs.outlineColor_;

                    bs.outlineColor_ = swap;

                }

                xf::draw_polished(display_, window_, gc_, bs, r.rx1, r.ry1, r.rx2, r.ry2);

            }

            int text_right = b.dropdown ? r.rx2 - 12 : r.rx2;

            xf::draw_text_centered(display_, window_, gc_, ts, r.rx1, r.ry1, text_right, r.ry2, b.caption);

            if (b.dropdown)

            {

                xf::draw_triangle_down(display_, window_, gc_, ts.text_color_, r.rx2 - 6, (r.ry1 + r.ry2) / 2 - 2, 4);

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

        int idx = button_at(x, y);

        if (idx >= 0 && !buttons_[idx].separator)

        {

            pressed_index_ = idx;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1 || pressed_index_ < 0)

        {

            return false;

        }

        int idx = pressed_index_;

        pressed_index_ = -1;

        if (button_at(x, y) == idx && on_button_click)

        {

            on_button_click(idx);

        }

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Tab || key == XK_Return)

        {

            return false;   // focus travel / activation is the host's job

        }

        return false;

    }

};

//endregion
