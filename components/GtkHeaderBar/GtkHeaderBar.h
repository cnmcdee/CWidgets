#pragma once

#include "core/xframe_core.h"

//region GtkHeaderBar

class GtkHeaderBar

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

    std::string title_;

    std::string subtitle_;

    bool show_title_buttons_ = true;

    bool show_close_button_ = true;

    bool show_minimize_button_ = true;

    bool show_maximize_button_ = true;

    std::string last_button_clicked_;

    int box_size_ = 16;

    int box_gap_ = 6;

    // Right edge of the decoration cluster, walking left per button.

    int box_right(int slot)

    {

        return x2_ - 8 - slot * (box_size_ + box_gap_);

    }

public:

    std::function<void(std::string)> on_decoration_clicked;

    GtkHeaderBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkHeaderBar";

        text_ = "HeaderBar";

        title_ = "Untitled";

        subtitle_ = "";

        last_button_clicked_ = "";

    }

    ~GtkHeaderBar() {}

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

        title_ = t;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    void set_title(std::string t)

    {

        title_ = t;

    }

    std::string get_title()

    {

        return title_;

    }

    void set_subtitle(std::string s)

    {

        subtitle_ = s;

    }

    std::string get_subtitle()

    {

        return subtitle_;

    }

    void set_show_title_buttons(bool s)

    {

        show_title_buttons_ = s;

    }

    bool get_show_title_buttons()

    {

        return show_title_buttons_;

    }

    void set_show_close_button(bool s)

    {

        show_close_button_ = s;

    }

    void set_show_minimize_button(bool s)

    {

        show_minimize_button_ = s;

    }

    void set_show_maximize_button(bool s)

    {

        show_maximize_button_ = s;

    }

    std::string get_last_button_clicked()

    {

        return last_button_clicked_;

    }

    // Decoration boxes: close is rightmost, then maximize, then minimize.

    void close_rect(int& rx1, int& ry1, int& rx2, int& ry2)

    {

        rx2 = box_right(0);

        rx1 = rx2 - box_size_;

        ry1 = y1_ + (height() - box_size_) / 2;

        ry2 = ry1 + box_size_;

    }

    void maximize_rect(int& rx1, int& ry1, int& rx2, int& ry2)

    {

        rx2 = box_right(1);

        rx1 = rx2 - box_size_;

        ry1 = y1_ + (height() - box_size_) / 2;

        ry2 = ry1 + box_size_;

    }

    void minimize_rect(int& rx1, int& ry1, int& rx2, int& ry2)

    {

        rx2 = box_right(2);

        rx1 = rx2 - box_size_;

        ry1 = y1_ + (height() - box_size_) / 2;

        ry2 = ry1 + box_size_;

    }

    std::string decoration_at(int x, int y)

    {

        if (!show_title_buttons_)

        {

            return "";

        }

        int rx1, ry1, rx2, ry2;

        if (show_close_button_)

        {

            close_rect(rx1, ry1, rx2, ry2);

            if (x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2)

            {

                return "close";

            }

        }

        if (show_maximize_button_)

        {

            maximize_rect(rx1, ry1, rx2, ry2);

            if (x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2)

            {

                return "maximize";

            }

        }

        if (show_minimize_button_)

        {

            minimize_rect(rx1, ry1, rx2, ry2);

            if (x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2)

            {

                return "minimize";

            }

        }

        return "";

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // Title on the upper line, subtitle dimmer on the line below.

        if (subtitle_.empty())

        {

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y2_, title_);

        }

        else

        {

            int mid = y1_ + height() / 2;

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, mid + 4, title_);

            SStyle sub = st;

            sub.text_color_ = styles_->get_color_rgb(110, 110, 110);

            xf::draw_text_centered(display_, window_, gc_, sub, x1_, mid - 2, x2_, y2_, subtitle_);

        }

        if (show_title_buttons_)

        {

            unsigned long glyph = enabled_ ? st.text_color_ : styles_->get_color_rgb(140, 140, 140);

            int rx1, ry1, rx2, ry2;

            if (show_close_button_)

            {

                close_rect(rx1, ry1, rx2, ry2);

                xf::draw_polished(display_, window_, gc_, st, rx1, ry1, rx2, ry2);

                XSetForeground(display_, gc_, glyph);

                xf::set_line_width(display_, gc_, 1);

                XDrawLine(display_, window_, gc_, rx1 + 4, ry1 + 4, rx2 - 4, ry2 - 4);

                XDrawLine(display_, window_, gc_, rx2 - 4, ry1 + 4, rx1 + 4, ry2 - 4);

            }

            if (show_maximize_button_)

            {

                maximize_rect(rx1, ry1, rx2, ry2);

                xf::draw_polished(display_, window_, gc_, st, rx1, ry1, rx2, ry2);

                XSetForeground(display_, gc_, glyph);

                XDrawRectangle(display_, window_, gc_, rx1 + 4, ry1 + 4, (rx2 - rx1) - 8, (ry2 - ry1) - 8);

            }

            if (show_minimize_button_)

            {

                minimize_rect(rx1, ry1, rx2, ry2);

                xf::draw_polished(display_, window_, gc_, st, rx1, ry1, rx2, ry2);

                XSetForeground(display_, gc_, glyph);

                XDrawLine(display_, window_, gc_, rx1 + 4, ry2 - 5, rx2 - 4, ry2 - 5);

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

        std::string hit = decoration_at(x, y);

        if (hit.empty())

        {

            return true;

        }

        last_button_clicked_ = hit;

        if (on_decoration_clicked)

        {

            on_decoration_clicked(hit);

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
