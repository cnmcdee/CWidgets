#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkViewport

class GtkViewport

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

    int hadjust_ = 0;

    int vadjust_ = 0;

    int frame_inset_ = 3;

public:

    GtkViewport(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkViewport";

        text_ = "Viewport";

    }

    ~GtkViewport() {}

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

    void set_hadjust(int v)

    {

        hadjust_ = v < 0 ? 0 : v;

    }

    int get_hadjust()

    {

        return hadjust_;

    }

    void set_vadjust(int v)

    {

        vadjust_ = v < 0 ? 0 : v;

    }

    int get_vadjust()

    {

        return vadjust_;

    }

    // Content area: the full bounds inset by the frame shadow.

    void content_rect(int& cx1, int& cy1, int& cx2, int& cy2)

    {

        cx1 = x1_ + frame_inset_;

        cy1 = y1_ + frame_inset_;

        cx2 = x2_ - frame_inset_;

        cy2 = y2_ - frame_inset_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        int cx1, cy1, cx2, cy2;

        content_rect(cx1, cy1, cx2, cy2);

        // Content background.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, cx1, cy1, cx2 - cx1, cy2 - cy1);

        // Sample content: grid lines shifted by the scroll offsets.

        XSetForeground(display_, gc_, styles_->get_color_rgb(210, 210, 210));

        xf::set_line_width(display_, gc_, 1);

        int cell = 32;

        int gx0 = cx1 - (hadjust_ % cell);

        for (int gx = gx0; gx <= cx2; gx += cell)

        {

            XDrawLine(display_, window_, gc_, gx, cy1, gx, cy2);

        }

        int gy0 = cy1 - (vadjust_ % cell);

        for (int gy = gy0; gy <= cy2; gy += cell)

        {

            XDrawLine(display_, window_, gc_, cx1, gy, cx2, gy);

        }

        // A marker block that visibly moves as the offsets change.

        int mx = cx1 + 48 - hadjust_;

        int my = cy1 + 48 - vadjust_;

        if (mx + 40 > cx1 && mx < cx2 && my + 24 > cy1 && my < cy2)

        {

            int fx1 = mx < cx1 ? cx1 : mx;

            int fy1 = my < cy1 ? cy1 : my;

            int fx2 = mx + 40 > cx2 ? cx2 : mx + 40;

            int fy2 = my + 24 > cy2 ? cy2 : my + 24;

            XSetForeground(display_, gc_, styles_->get_color_rgb(150, 180, 220));

            XFillRectangle(display_, window_, gc_, fx1, fy1, fx2 - fx1, fy2 - fy1);

        }

        // Offset readout in the content's own (scrolled) space corner.

        char buf[64];

        snprintf(buf, sizeof(buf), "offset %d, %d", hadjust_, vadjust_);

        xf::draw_text_at(display_, window_, gc_, st, cx1 + 8, cy1 + 18, std::string(buf));

        // Shadowed frame: light top/left, dark bottom/right, then
        // the reverse just inside, giving the sunken shadow look.

        unsigned long light = styles_->get_color_rgb(245, 245, 245);

        unsigned long dark = styles_->get_color_rgb(105, 105, 105);

        XSetForeground(display_, gc_, dark);

        XDrawLine(display_, window_, gc_, x1_, y1_, x2_ - 1, y1_);

        XDrawLine(display_, window_, gc_, x1_, y1_, x1_, y2_ - 1);

        XDrawLine(display_, window_, gc_, x1_ + 1, y1_ + 1, x2_ - 2, y1_ + 1);

        XDrawLine(display_, window_, gc_, x1_ + 1, y1_ + 1, x1_ + 1, y2_ - 2);

        XSetForeground(display_, gc_, light);

        XDrawLine(display_, window_, gc_, x1_, y2_ - 1, x2_ - 1, y2_ - 1);

        XDrawLine(display_, window_, gc_, x2_ - 1, y1_, x2_ - 1, y2_ - 1);

        XDrawLine(display_, window_, gc_, x1_ + 1, y2_ - 2, x2_ - 2, y2_ - 2);

        XDrawLine(display_, window_, gc_, x2_ - 2, y1_ + 1, x2_ - 2, y2_ - 2);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        // Mouse wheel scrolls the vertical adjustment.

        if (button == 4)

        {

            set_vadjust(vadjust_ - 24);

            return true;

        }

        if (button == 5)

        {

            set_vadjust(vadjust_ + 24);

            return true;

        }

        if (button == 6)

        {

            set_hadjust(hadjust_ - 24);

            return true;

        }

        if (button == 7)

        {

            set_hadjust(hadjust_ + 24);

            return true;

        }

        return button == 1;

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

        if (key == XK_Down)

        {

            set_vadjust(vadjust_ + 24);

            return true;

        }

        if (key == XK_Up)

        {

            set_vadjust(vadjust_ - 24);

            return true;

        }

        if (key == XK_Right)

        {

            set_hadjust(hadjust_ + 24);

            return true;

        }

        if (key == XK_Left)

        {

            set_hadjust(hadjust_ - 24);

            return true;

        }

        return false;

    }

};

//endregion
