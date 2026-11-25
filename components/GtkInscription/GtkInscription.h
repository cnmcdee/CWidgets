#pragma once

#include "core/xframe_core.h"

//region GtkInscription

// A single-line text display that never wraps and never ellipsizes:
// the text is simply clipped to the widget's width, mid-glyph if
// needed, like GTK's GtkInscription. min_chars / nat_chars express
// the size the widget would like to be given.

class GtkInscription

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

    int min_chars_ = 0;

    int nat_chars_ = 0;

    double xalign_ = 0.0;

public:

    GtkInscription(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkInscription";

        text_ = "Inscription text that is far too long for its box";

        min_chars_ = 8;

        nat_chars_ = 24;

    }

    ~GtkInscription() {}

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

    //region GtkInscription API

    void set_min_chars(int n)

    {

        min_chars_ = std::max(0, n);

    }

    int get_min_chars()

    {

        return min_chars_;

    }

    void set_nat_chars(int n)

    {

        nat_chars_ = std::max(0, n);

    }

    int get_nat_chars()

    {

        return nat_chars_;

    }

    void set_xalign(double a)

    {

        xalign_ = std::max(0.0, std::min(1.0, a));

    }

    double get_xalign()

    {

        return xalign_;

    }

    // The width the widget asks for, in pixels, for a char count.

    int width_for_chars(const SStyle& st, int chars)

    {

        if (chars <= 0)

        {

            return 0;

        }

        return xf::text_width(st, std::string((size_t)chars, 'M')) + 4;

    }

    int min_width_px()

    {

        return width_for_chars(style(), min_chars_);

    }

    int nat_width_px()

    {

        return width_for_chars(style(), nat_chars_);

    }

    //endregion

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

        // Clip: keep the longest prefix that fits. The final glyph
        // may overrun by a pixel or two, exactly like the real
        // widget clipping mid-glyph; we simply stop before it.

        std::string shown = text_;

        while (!shown.empty() && xf::text_width(st, shown) > width() - 2)

        {

            shown.pop_back();

        }

        int tw = xf::text_width(st, shown);

        int tx = x1_ + 1 + (int)((width() - 2 - tw) * xalign_);

        xf::draw_text_left(display_, window_, gc_, st, tx, y1_, y2_, shown);

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
