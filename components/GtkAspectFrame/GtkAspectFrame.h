#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkAspectFrame

class GtkAspectFrame

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

    double ratio_ = 1.0;

    double child_ratio_ = 1.0;

    bool obey_child_ = true;

    double xalign_ = 0.5;

    double yalign_ = 0.5;

    int inset_ = 6;

    double effective_ratio()

    {

        double r = obey_child_ ? child_ratio_ : ratio_;

        if (r <= 0.0)

        {

            r = 1.0;

        }

        return r;

    }

public:

    GtkAspectFrame(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkAspectFrame";

        text_ = "Aspect Frame";

    }

    ~GtkAspectFrame() {}

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

    void set_ratio(double r)

    {

        if (r > 0.0)

        {

            ratio_ = r;

        }

    }

    double get_ratio()

    {

        return ratio_;

    }

    void set_child_ratio(double r)

    {

        if (r > 0.0)

        {

            child_ratio_ = r;

        }

    }

    double get_child_ratio()

    {

        return child_ratio_;

    }

    void set_obey_child(bool o)

    {

        obey_child_ = o;

    }

    bool get_obey_child()

    {

        return obey_child_;

    }

    void set_alignment(double xa, double ya)

    {

        xalign_ = xa < 0.0 ? 0.0 : (xa > 1.0 ? 1.0 : xa);

        yalign_ = ya < 0.0 ? 0.0 : (ya > 1.0 ? 1.0 : ya);

    }

    // Largest rect of the effective aspect ratio that fits inside
    // the frame, positioned by the alignment values.

    void child_rect(int& cx1, int& cy1, int& cx2, int& cy2)

    {

        int ax1 = x1_ + inset_;

        int ay1 = y1_ + inset_;

        int ax2 = x2_ - inset_;

        int ay2 = y2_ - inset_;

        int aw = ax2 - ax1;

        int ah = ay2 - ay1;

        double r = effective_ratio();

        int cw = aw;

        int ch = (int)(aw / r);

        if (ch > ah)

        {

            ch = ah;

            cw = (int)(ah * r);

        }

        cx1 = ax1 + (int)(xalign_ * (aw - cw));

        cy1 = ay1 + (int)(yalign_ * (ah - ch));

        cx2 = cx1 + cw;

        cy2 = cy1 + ch;

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

        // Outer frame.

        xf::fill_rounded_rect(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int cx1, cy1, cx2, cy2;

        child_rect(cx1, cy1, cx2, cy2);

        // Computed child rect, drawn as a dashed-ish outline
        // (short segments along each side).

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        int dash = 6;

        int gap = 4;

        for (int x = cx1; x < cx2; x += dash + gap)

        {

            int xe = x + dash < cx2 ? x + dash : cx2;

            XDrawLine(display_, window_, gc_, x, cy1, xe, cy1);

            XDrawLine(display_, window_, gc_, x, cy2, xe, cy2);

        }

        for (int y = cy1; y < cy2; y += dash + gap)

        {

            int ye = y + dash < cy2 ? y + dash : cy2;

            XDrawLine(display_, window_, gc_, cx1, y, cx1, ye);

            XDrawLine(display_, window_, gc_, cx2, y, cx2, ye);

        }

        // Diagonal guides corner to corner.

        XSetForeground(display_, gc_, styles_->get_color_rgb(190, 190, 190));

        XDrawLine(display_, window_, gc_, cx1, cy1, cx2, cy2);

        XDrawLine(display_, window_, gc_, cx2, cy1, cx1, cy2);

        // Ratio text centred in the child rect.

        char buf[64];

        snprintf(buf, sizeof(buf), "%.3g : 1", effective_ratio());

        xf::draw_text_centered(display_, window_, gc_, st, cx1, cy1, cx2, cy2, std::string(buf));

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
