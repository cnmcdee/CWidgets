#pragma once

#include "core/xframe_core.h"

//region GtkLevelBar

// A segmented level indicator between min_value_ and max_value_.
// In discrete mode it draws one block per unit; offsets name
// threshold values ("low", "high"...) that tint the filled blocks.

struct GtkLevelOffset

{

    std::string name;

    double value;

};

class GtkLevelBar

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

    double min_value_ = 0.0;

    double max_value_ = 5.0;

    double value_ = 3.0;

    bool discrete_ = true;

    std::vector<GtkLevelOffset> offsets_;

public:

    GtkLevelBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkLevelBar";

        text_ = "GtkLevelBar";

        add_offset_value("low", 1.0);

        add_offset_value("high", 4.0);

    }

    ~GtkLevelBar() {}

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

    //region GtkLevelBar API

    void set_min_value(double v)

    {

        min_value_ = v;

        set_value(value_);

    }

    double get_min_value()

    {

        return min_value_;

    }

    void set_max_value(double v)

    {

        max_value_ = v;

        set_value(value_);

    }

    double get_max_value()

    {

        return max_value_;

    }

    void set_value(double v)

    {

        value_ = std::max(min_value_, std::min(max_value_, v));

    }

    double get_value()

    {

        return value_;

    }

    void set_discrete(bool d)

    {

        discrete_ = d;

    }

    bool get_discrete()

    {

        return discrete_;

    }

    void add_offset_value(std::string name, double value)

    {

        for (auto& o : offsets_)

        {

            if (o.name == name)

            {

                o.value = value;

                return;

            }

        }

        GtkLevelOffset o;

        o.name = name;

        o.value = value;

        offsets_.push_back(o);

    }

    bool get_offset_value(std::string name, double& out)

    {

        for (auto& o : offsets_)

        {

            if (o.name == name)

            {

                out = o.value;

                return true;

            }

        }

        return false;

    }

    void remove_offset_value(std::string name)

    {

        for (size_t i = 0; i < offsets_.size(); ++i)

        {

            if (offsets_[i].name == name)

            {

                offsets_.erase(offsets_.begin() + (long)i);

                return;

            }

        }

    }

    // Fill colour for the current value, honouring the offsets:
    // at or below "low" reads warning orange, otherwise green.

    unsigned long level_color()

    {

        if (!enabled_)

        {

            return styles_->get_color_rgb(150, 150, 150);

        }

        double low;

        if (get_offset_value("low", low) && value_ <= low)

        {

            return styles_->get_color_rgb(230, 130, 40);

        }

        return styles_->get_color_rgb(76, 160, 80);

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        double range = max_value_ - min_value_;

        if (range <= 0.0)

        {

            range = 1.0;

        }

        double frac = (value_ - min_value_) / range;

        unsigned long fill = level_color();

        if (discrete_)

        {

            int blocks = std::max(1, (int)range);

            int gap = 2;

            int bw = (width() - gap * (blocks - 1)) / blocks;

            int filled = (int)(frac * blocks + 0.5);

            for (int i = 0; i < blocks; ++i)

            {

                int bx = x1_ + i * (bw + gap);

                if (i < filled)

                {

                    XSetForeground(display_, gc_, fill);

                    XFillRectangle(display_, window_, gc_, bx, y1_ + 2, bw, height() - 4);

                }

                else

                {

                    XSetForeground(display_, gc_, styles_->get_color_rgb(215, 215, 215));

                    XFillRectangle(display_, window_, gc_, bx, y1_ + 2, bw, height() - 4);

                }

                XSetForeground(display_, gc_, st.lineColor_);

                XDrawRectangle(display_, window_, gc_, bx, y1_ + 2, bw - 1, height() - 5);

            }

        }

        else

        {

            XSetForeground(display_, gc_, styles_->get_color_rgb(215, 215, 215));

            XFillRectangle(display_, window_, gc_, x1_, y1_ + 2, width(), height() - 4);

            XSetForeground(display_, gc_, fill);

            XFillRectangle(display_, window_, gc_, x1_, y1_ + 2, (int)(width() * frac), height() - 4);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, y1_ + 2, width() - 1, height() - 5);

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

        double range = max_value_ - min_value_;

        double f = (double)(x - x1_) / std::max(1, width());

        set_value(min_value_ + f * range);

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

        if (key == XK_Right || key == XK_Up)

        {

            set_value(value_ + 1.0);

            return true;

        }

        if (key == XK_Left || key == XK_Down)

        {

            set_value(value_ - 1.0);

            return true;

        }

        return false;

    }

};

//endregion
