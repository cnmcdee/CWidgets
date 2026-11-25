#pragma once

#include "core/xframe_core.h"

//region GtkExpander

class GtkExpander

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

    bool expanded_ = false;

    int header_height_ = 24;

    std::vector<std::string> body_lines_;

    bool in_header(int x, int y)

    {

        return x >= x1_ && x <= x2_ && y >= y1_ && y <= y1_ + header_height_;

    }

public:

    std::function<void(bool)> on_toggled;

    GtkExpander(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkExpander";

        text_ = "Details";

        body_lines_.push_back("Expanded content goes here.");

        body_lines_.push_back("Click the header to collapse.");

        body_lines_.push_back("Space or Return toggles too.");

    }

    ~GtkExpander() {}

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

    void set_expanded(bool e)

    {

        expanded_ = e;

    }

    bool get_expanded()

    {

        return expanded_;

    }

    void set_label(std::string label)

    {

        text_ = label;

    }

    std::string get_label()

    {

        return text_;

    }

    // Body area below the header; only meaningful when expanded.

    void body_rect(int& bx1, int& by1, int& bx2, int& by2)

    {

        bx1 = x1_;

        by1 = y1_ + header_height_;

        bx2 = x2_;

        by2 = y2_;

    }

    void toggle()

    {

        expanded_ = !expanded_;

        if (on_toggled)

        {

            on_toggled(expanded_);

        }

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

        // Header row.

        xf::fill_rounded_rect(display_, window_, gc_, st, x1_, y1_, x2_, y1_ + header_height_);

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y1_ + header_height_);

        int cy = y1_ + header_height_ / 2;

        if (expanded_)

        {

            xf::draw_triangle_down(display_, window_, gc_, st.text_color_, x1_ + 14, cy - 4, 6);

        }

        else

        {

            xf::draw_triangle_right(display_, window_, gc_, st.text_color_, x1_ + 11, cy, 6);

        }

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 26, y1_, y1_ + header_height_, text_);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, header_height_ - 4);

        }

        if (expanded_)

        {

            int bx1, by1, bx2, by2;

            body_rect(bx1, by1, bx2, by2);

            // Framed body area.

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, bx1 + 1, by1 + 1, bx2 - bx1 - 2, by2 - by1 - 2);

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, (int)st.lineWidth_);

            XDrawRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

            if (st.font_)

            {

                int line_h = st.font_->ascent + st.font_->descent + 4;

                int baseline = by1 + 8 + st.font_->ascent;

                for (size_t i = 0; i < body_lines_.size(); ++i)

                {

                    if (baseline > by2 - 4)

                    {

                        break;

                    }

                    xf::draw_text_at(display_, window_, gc_, st, bx1 + 10, baseline, body_lines_[i]);

                    baseline += line_h;

                }

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

        if (in_header(x, y))

        {

            toggle();

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

        if (key == XK_space || key == XK_Return)

        {

            toggle();

            return true;

        }

        return false;

    }

};

//endregion
