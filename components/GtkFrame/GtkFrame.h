#pragma once

#include "core/xframe_core.h"

//region GtkFrame

class GtkFrame

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

    std::string label_;

    double label_xalign_ = 0.0;

    int inset_ = 8;

    int top_inset_ = 14;

public:

    GtkFrame(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFrame";

        text_ = "Frame";

        label_ = "Frame";

    }

    ~GtkFrame() {}

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

        label_ = t;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    void set_label(std::string label)

    {

        label_ = label;

        text_ = label;

    }

    std::string get_label()

    {

        return label_;

    }

    void set_label_xalign(double a)

    {

        if (a < 0.0)

        {

            a = 0.0;

        }

        if (a > 1.0)

        {

            a = 1.0;

        }

        label_xalign_ = a;

    }

    double get_label_xalign()

    {

        return label_xalign_;

    }

    // Inner content area, inset past the border and the label band.

    void content_rect(int& cx1, int& cy1, int& cx2, int& cy2)

    {

        cx1 = x1_ + inset_;

        cy1 = y1_ + top_inset_ + 4;

        cx2 = x2_ - inset_;

        cy2 = y2_ - inset_;

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

        int top = y1_ + top_inset_ / 2;

        // Background fill inside the border.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 1, top + 1, width() - 2, y2_ - top - 2);

        // Label geometry along the top edge.

        int label_w = label_.empty() ? 0 : xf::text_width(st, label_);

        int label_pad = 4;

        int travel = width() - 2 * inset_ - label_w;

        if (travel < 0)

        {

            travel = 0;

        }

        int label_x = x1_ + inset_ + (int)(label_xalign_ * travel);

        // Border, drawn as four sides with a gap behind the label.

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        // Top side: two segments either side of the label gap.

        if (!label_.empty())

        {

            XDrawLine(display_, window_, gc_, x1_, top, label_x - label_pad, top);

            XDrawLine(display_, window_, gc_, label_x + label_w + label_pad, top, x2_, top);

        }

        else

        {

            XDrawLine(display_, window_, gc_, x1_, top, x2_, top);

        }

        XDrawLine(display_, window_, gc_, x2_, top, x2_, y2_);

        XDrawLine(display_, window_, gc_, x2_, y2_, x1_, y2_);

        XDrawLine(display_, window_, gc_, x1_, y2_, x1_, top);

        // Label text sitting over the gap, baseline centred on the top line.

        if (!label_.empty() && st.font_)

        {

            int baseline = top + (st.font_->ascent - st.font_->descent) / 2;

            xf::draw_text_at(display_, window_, gc_, st, label_x, baseline, label_);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
