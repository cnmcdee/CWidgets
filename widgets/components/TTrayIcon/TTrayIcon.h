#pragma once

#include "core/xframe_core.h"

//region TTrayIcon

class TTrayIcon

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

    // TTrayIcon is non-visual at run time: it lives in the system
    // tray. The state below is its real data; draw() only paints a
    // small design-time glyph so the component can be placed.

    std::string hint_ = "XFrame Tray Icon";

    std::string icon_name_ = "app_icon";

    std::string pop_up_menu_ = "";   // name of the bound TPopupMenu

    bool tray_visible_ = false;

    bool armed_ = false;   // first press seen, a second is a double click

public:

    std::function<void()> on_click;

    std::function<void()> on_dbl_click;

    std::function<void()> on_mouse_down;

    TTrayIcon(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TTrayIcon";

        text_ = "TTrayIcon";

    }

    ~TTrayIcon() {}

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

    // The tray tooltip (Lazarus Hint property).

    void set_hint(std::string h)

    {

        hint_ = h;

    }

    std::string get_hint()

    {

        return hint_;

    }

    void set_icon_name(std::string n)

    {

        icon_name_ = n;

    }

    std::string get_icon_name()

    {

        return icon_name_;

    }

    void set_pop_up_menu(std::string menu_name)

    {

        pop_up_menu_ = menu_name;

    }

    std::string get_pop_up_menu()

    {

        return pop_up_menu_;

    }

    // Show / hide the icon in the tray (Lazarus ShowIcon / HideIcon).

    void show_icon()

    {

        tray_visible_ = true;

    }

    void hide_icon()

    {

        tray_visible_ = false;

    }

    bool icon_visible()

    {

        return tray_visible_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Design-time glyph: a small tray slot with the app dot,
        // plus the tooltip text beside it.

        int s = std::min(width(), height());

        int gx = x1_;

        int gy = y1_ + (height() - s) / 2;

        XSetForeground(display_, gc_, styles_->get_color_rgb(90, 90, 100));

        XFillRectangle(display_, window_, gc_, gx, gy, s, s);

        XSetForeground(display_, gc_, styles_->get_color_rgb(240, 200, 90));

        XFillArc(display_, window_, gc_, gx + s / 4, gy + s / 4, s / 2, s / 2, 0, 360 * 64);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, gx, gy, s - 1, s - 1);

        xf::draw_text_left(display_, window_, gc_, st, gx + s + 4, y1_, y2_, hint_);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 3 && !pop_up_menu_.empty())

        {

            // A right click would pop up pop_up_menu_ in a real tray;
            // the host is told through on_mouse_down.

            if (on_mouse_down)

            {

                on_mouse_down();

            }

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        if (on_mouse_down)

        {

            on_mouse_down();

        }

        if (armed_)

        {

            armed_ = false;

            if (on_dbl_click)

            {

                on_dbl_click();

            }

        }

        else

        {

            armed_ = true;

            if (on_click)

            {

                on_click();

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

        // Moving away breaks the double-click arming.

        if (!contains(x, y))

        {

            armed_ = false;

        }

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
