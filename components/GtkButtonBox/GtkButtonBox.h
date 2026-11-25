#pragma once

#include "core/xframe_core.h"

//region GtkButtonBox

class GtkButtonBox

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

    // Layout styles: 0 spread, 1 edge, 2 start, 3 end, 4 center, 5 expand.

    int layout_style_ = 2;

    int button_width_ = 90;

    int button_height_ = 30;

    int spacing_ = 8;

    std::vector<std::string> labels_;

    int pressed_index_ = -1;

    int focused_index_ = 0;

public:

    std::function<void(int)> on_button_clicked;

    GtkButtonBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkButtonBox";

        text_ = "ButtonBox";

        labels_ = { "OK", "Cancel", "Help" };

    }

    ~GtkButtonBox() {}

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

    void set_layout_style(int s)

    {

        if (s >= 0 && s <= 5)

        {

            layout_style_ = s;

        }

    }

    int get_layout_style()

    {

        return layout_style_;

    }

    void set_spacing(int s)

    {

        spacing_ = std::max(0, s);

    }

    void set_button_size(int w, int h)

    {

        button_width_ = std::max(20, w);

        button_height_ = std::max(16, h);

    }

    void set_labels(std::vector<std::string> labels)

    {

        labels_ = labels;

        pressed_index_ = -1;

        focused_index_ = 0;

    }

    int button_count()

    {

        return (int)labels_.size();

    }

    // Geometry of button i out of count buttons, per the layout style.
    // Returns false when i is out of range.

    bool button_rect(int i, int count, int& rx1, int& ry1, int& rx2, int& ry2)

    {

        if (i < 0 || i >= count || count <= 0)

        {

            return false;

        }

        int bw = button_width_;

        int total = count * bw + (count - 1) * spacing_;

        int start_x = x1_;

        int step = bw + spacing_;

        if (layout_style_ == 0)   // spread: equal slots across the width

        {

            step = width() / count;

            start_x = x1_ + (step - bw) / 2;

        }

        else if (layout_style_ == 1)   // edge: first left, last right

        {

            if (count == 1)

            {

                start_x = x1_;

            }

            else

            {

                step = (width() - bw) / (count - 1);

                start_x = x1_;

            }

        }

        else if (layout_style_ == 3)   // end: packed at the right

        {

            start_x = x2_ - total;

        }

        else if (layout_style_ == 4)   // center: packed in the middle

        {

            start_x = x1_ + (width() - total) / 2;

        }

        else if (layout_style_ == 5)   // expand: buttons stretch to fill

        {

            bw = (width() - (count - 1) * spacing_) / count;

            start_x = x1_;

            step = bw + spacing_;

        }

        // style 2 (start) and the fall-through cases pack from x1_.

        rx1 = start_x + i * step;

        rx2 = rx1 + bw;

        ry1 = y1_ + (height() - button_height_) / 2;

        ry2 = ry1 + button_height_;

        return true;

    }

    bool button_rect(int i, int& rx1, int& ry1, int& rx2, int& ry2)

    {

        return button_rect(i, (int)labels_.size(), rx1, ry1, rx2, ry2);

    }

    int index_at(int x, int y)

    {

        for (int i = 0; i < (int)labels_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (button_rect(i, rx1, ry1, rx2, ry2) && x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2)

            {

                return i;

            }

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

        // Bar area: a flat fill so the buttons read as a group.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        for (int i = 0; i < (int)labels_.size(); ++i)

        {

            int rx1, ry1, rx2, ry2;

            if (!button_rect(i, rx1, ry1, rx2, ry2))

            {

                continue;

            }

            SStyle btn = st;

            if (!enabled_)

            {

                btn.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            if (i == pressed_index_)

            {

                unsigned long swap = btn.fillColor_;

                btn.fillColor_ = btn.outlineColor_;

                btn.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, btn, rx1, ry1, rx2, ry2);

            xf::draw_text_centered(display_, window_, gc_, btn, rx1, ry1, rx2, ry2, labels_[i]);

            if (focused_ && i == focused_index_)

            {

                XSetForeground(display_, gc_, btn.lineColor_);

                XDrawRectangle(display_, window_, gc_, rx1 + 3, ry1 + 3, (rx2 - rx1) - 6, (ry2 - ry1) - 6);

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

        int index = index_at(x, y);

        if (index < 0)

        {

            return false;

        }

        pressed_index_ = index;

        focused_index_ = index;

        focused_ = true;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (pressed_index_ < 0 || button != 1)

        {

            return false;

        }

        int released = pressed_index_;

        pressed_index_ = -1;

        if (index_at(x, y) == released && on_button_clicked)

        {

            on_button_clicked(released);

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

        if (!focused_ || !enabled_ || labels_.empty())

        {

            return false;

        }

        if (key == XK_Right || key == XK_Tab)

        {

            focused_index_ = std::min((int)labels_.size() - 1, focused_index_ + 1);

            return true;

        }

        if (key == XK_Left)

        {

            focused_index_ = std::max(0, focused_index_ - 1);

            return true;

        }

        if (key == XK_Return || key == XK_space)

        {

            if (on_button_clicked)

            {

                on_button_clicked(focused_index_);

            }

            return true;

        }

        return false;

    }

};

//endregion
