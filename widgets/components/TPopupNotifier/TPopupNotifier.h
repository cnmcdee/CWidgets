#pragma once

#include "core/xframe_core.h"

//region TPopupNotifier

class TPopupNotifier

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;         // the notification body text

    std::string style_name_;   // "" = manager's active style

    bool visible_ = false;     // a notifier starts hidden until show()

    bool enabled_ = true;

    bool focused_ = false;

    std::string title_ = "Notification";

    // The close box in the top-right corner.

    void close_box(int& cx1, int& cy1, int& cx2, int& cy2)

    {

        cx2 = x2_ - 6;

        cx1 = cx2 - 14;

        cy1 = y1_ + 6;

        cy2 = cy1 + 14;

    }

    bool in_close_box(int x, int y)

    {

        int cx1, cy1, cx2, cy2;

        close_box(cx1, cy1, cx2, cy2);

        return x >= cx1 && x <= cx2 && y >= cy1 && y <= cy2;

    }

public:

    std::function<void()> on_close;

    TPopupNotifier(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPopupNotifier";

        text_ = "Your changes have been saved.";

        title_ = "Information";

        visible_ = true;   // shown in the demo so the panel is visible

    }

    ~TPopupNotifier() {}

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

    void set_title(std::string t)

    {

        title_ = t;

    }

    std::string get_title()

    {

        return title_;

    }

    void show()

    {

        visible_ = true;

    }

    void show_message(std::string title, std::string message)

    {

        title_ = title;

        text_ = message;

        visible_ = true;

    }

    void hide()

    {

        visible_ = false;

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

        // A soft information tint behind the panel.

        unsigned long tint = styles_->get_color_rgb(255, 248, 210);

        XSetForeground(display_, gc_, tint);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // An information disc at the left.

        unsigned long info = styles_->get_color_rgb(60, 120, 210);

        int cx = x1_ + 18;

        int cy = (y1_ + y2_) / 2;

        XSetForeground(display_, gc_, info);

        XFillArc(display_, window_, gc_, cx - 9, cy - 9, 18, 18, 0, 360 * 64);

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        xf::set_line_width(display_, gc_, 2);

        XDrawLine(display_, window_, gc_, cx, cy - 4, cx, cy + 4);

        XFillArc(display_, window_, gc_, cx - 1, cy - 7, 3, 3, 0, 360 * 64);

        xf::set_line_width(display_, gc_, 1);

        // Title along the top, body text under it.

        xf::draw_text_left(display_, window_, gc_, ts, x1_ + 34, y1_ + 4, y1_ + 24, title_);

        xf::draw_text_left(display_, window_, gc_, ts, x1_ + 34, y1_ + 24, y2_ - 4, text_);

        // Close box.

        int bx1, by1, bx2, by2;

        close_box(bx1, by1, bx2, by2);

        xf::draw_polished(display_, window_, gc_, st, bx1, by1, bx2, by2);

        XSetForeground(display_, gc_, ts.text_color_);

        XDrawLine(display_, window_, gc_, bx1 + 3, by1 + 3, bx2 - 3, by2 - 3);

        XDrawLine(display_, window_, gc_, bx1 + 3, by2 - 3, bx2 - 3, by1 + 3);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (in_close_box(x, y))

        {

            visible_ = false;

            if (on_close)

            {

                on_close();

            }

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

        if (!visible_)

        {

            return false;

        }

        if (key == XK_Escape)

        {

            visible_ = false;

            if (on_close)

            {

                on_close();

            }

            return true;

        }

        return false;

    }

};

//endregion
