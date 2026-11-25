#pragma once

#include "core/xframe_core.h"

//region TApplicationProperties

class TApplicationProperties

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

    // Non-visual component: it carries application-wide settings.
    // draw() only paints a small design-time info glyph.

    std::string title_ = "XFrame Application";

    std::string version_ = "1.0.0";

    std::string hint_ = "";

    std::string help_file_ = "";

    bool show_main_form_ = true;

    bool show_hint_ = true;

    int exception_count_ = 0;

    std::string last_exception_;

public:

    std::function<void()> on_click;

    // Lazarus OnException: called with the exception message.

    std::function<void(const std::string&)> on_exception;

    std::function<void()> on_minimize;

    std::function<void()> on_restore;

    TApplicationProperties(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TApplicationProperties";

        text_ = title_;

    }

    ~TApplicationProperties() {}

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

        text_ = t;

    }

    std::string get_title()

    {

        return title_;

    }

    void set_version(std::string v)

    {

        version_ = v;

    }

    std::string get_version()

    {

        return version_;

    }

    void set_hint(std::string h)

    {

        hint_ = h;

    }

    std::string get_hint()

    {

        return hint_;

    }

    void set_help_file(std::string f)

    {

        help_file_ = f;

    }

    std::string get_help_file()

    {

        return help_file_;

    }

    void set_show_main_form(bool s)

    {

        show_main_form_ = s;

    }

    bool get_show_main_form()

    {

        return show_main_form_;

    }

    void set_show_hint(bool s)

    {

        show_hint_ = s;

    }

    bool get_show_hint()

    {

        return show_hint_;

    }

    // Route an exception through the registered handler, exactly
    // like Application.OnException; unhandled ones are counted.

    void handle_exception(const std::string& message)

    {

        ++exception_count_;

        last_exception_ = message;

        if (on_exception)

        {

            on_exception(message);

        }

    }

    int get_exception_count()

    {

        return exception_count_;

    }

    std::string get_last_exception()

    {

        return last_exception_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Design-time info glyph: a rounded badge with an "i",
        // and the application title beside it.

        int s = std::min(width(), height());

        int gx = x1_;

        int gy = y1_ + (height() - s) / 2;

        SStyle badge = st;

        badge.radius_ = (unsigned long)(s / 2);

        xf::fill_rounded_rect(display_, window_, gc_, badge, gx, gy, gx + s, gy + s);

        xf::draw_rounded_outline(display_, window_, gc_, badge, gx, gy, gx + s, gy + s);

        xf::draw_text_centered(display_, window_, gc_, st, gx, gy, gx + s, gy + s, "i");

        std::string label = title_ + "  v" + version_;

        xf::draw_text_left(display_, window_, gc_, st, gx + s + 4, y1_, y2_, label);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (on_click)

        {

            on_click();

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
