#pragma once

#include "core/xframe_core.h"

//region GtkInvisible

class GtkInvisible

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // payload data carried by the widget

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    // A GtkInvisible has no on-screen presence at all: in real GTK
    // it exists to own selections and receive events on behalf of
    // other code. Here it carries a data payload and a grab flag.

    bool has_grab_ = false;

    std::string selection_name_;   // e.g. "CLIPBOARD", "PRIMARY"

public:

    std::function<void()> on_grab;

    std::function<void()> on_ungrab;

    GtkInvisible(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkInvisible";

        text_ = "";

        selection_name_ = "CLIPBOARD";

    }

    ~GtkInvisible() {}

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

    // An invisible widget is never hit: it has no pixels.

    bool contains(int x, int y)

    {

        (void)x;

        (void)y;

        return false;

    }

    void set_visible(bool v)

    {

        // Accepted for interface parity; it stays invisible either
        // way, exactly like the real GtkInvisible.

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

    //region GtkInvisible specific API

    // The payload: in GTK terms this is the data the invisible
    // widget would hand out when its selection is requested.

    void set_data(std::string d)

    {

        text_ = d;

    }

    std::string get_data()

    {

        return text_;

    }

    void set_selection_name(std::string s)

    {

        selection_name_ = s;

    }

    std::string get_selection_name()

    {

        return selection_name_;

    }

    // Take / release the (simulated) selection grab.

    void grab()

    {

        if (has_grab_)

        {

            return;

        }

        has_grab_ = true;

        if (on_grab)

        {

            on_grab();

        }

    }

    void ungrab()

    {

        if (!has_grab_)

        {

            return;

        }

        has_grab_ = false;

        if (on_ungrab)

        {

            on_ungrab();

        }

    }

    bool has_grab()

    {

        return has_grab_;

    }

    //endregion

    void draw()

    {

        // Non-visual component: draw() intentionally does nothing,
        // not even a design-time glyph.

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        (void)button;

        return false;

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
