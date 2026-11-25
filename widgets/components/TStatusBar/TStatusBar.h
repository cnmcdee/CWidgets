#pragma once

#include "core/xframe_core.h"

//region TStatusBar

class TStatusBar

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

    struct TStatusPanel

    {

        int width;             // -1 = stretch to fill the rest

        std::string text;

    };

    std::vector<TStatusPanel> panels_;

    bool size_grip_ = true;

    int panel_x(int index)

    {

        int px = x1_;

        for (int i = 0; i < index && i < (int)panels_.size(); ++i)

        {

            px += panel_width(i);

        }

        return px;

    }

    int panel_width(int index)

    {

        if (panels_[index].width >= 0)

        {

            return panels_[index].width;

        }

        // A stretch panel takes whatever is left before the grip.

        int used = 0;

        for (size_t i = 0; i < panels_.size(); ++i)

        {

            if ((int)i != index && panels_[i].width >= 0)

            {

                used += panels_[i].width;

            }

        }

        int grip = size_grip_ ? 16 : 0;

        int rest = width() - used - grip;

        return rest < 0 ? 0 : rest;

    }

public:

    std::function<void(int)> on_panel_click;

    TStatusBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TStatusBar";

        text_ = "TStatusBar";

        add_panel(120, "Ready");

        add_panel(-1, "Line 1, Col 1");

        add_panel(60, "INS");

    }

    ~TStatusBar() {}

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

        // Simple panel mode: the whole bar is one panel of text.

        panels_.clear();

        add_panel(-1, t);

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

    void add_panel(int w, std::string t)

    {

        TStatusPanel panel;

        panel.width = w;

        panel.text = t;

        panels_.push_back(panel);

    }

    int panel_count()

    {

        return (int)panels_.size();

    }

    void set_panel_text(int index, std::string t)

    {

        if (index >= 0 && index < (int)panels_.size())

        {

            panels_[index].text = t;

        }

    }

    std::string get_panel_text(int index)

    {

        if (index >= 0 && index < (int)panels_.size())

        {

            return panels_[index].text;

        }

        return "";

    }

    void set_size_grip(bool g)

    {

        size_grip_ = g;

    }

    int panel_at(int x)

    {

        for (size_t i = 0; i < panels_.size(); ++i)

        {

            int px = panel_x((int)i);

            if (x >= px && x < px + panel_width((int)i))

            {

                return (int)i;

            }

        }

        return -1;

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

        // Bar body in the style fill, with a raised top edge.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawLine(display_, window_, gc_, x1_, y1_, x2_, y1_);

        unsigned long shadow = styles_->get_color_rgb(150, 150, 150);

        unsigned long light = styles_->get_color_rgb(235, 235, 235);

        for (size_t i = 0; i < panels_.size(); ++i)

        {

            int px = panel_x((int)i);

            int pw = panel_width((int)i);

            if (pw <= 0)

            {

                continue;

            }

            // Sunken panel edges: dark top/left, light bottom/right.

            XSetForeground(display_, gc_, shadow);

            XDrawLine(display_, window_, gc_, px + 2, y1_ + 3, px + pw - 3, y1_ + 3);

            XDrawLine(display_, window_, gc_, px + 2, y1_ + 3, px + 2, y2_ - 3);

            XSetForeground(display_, gc_, light);

            XDrawLine(display_, window_, gc_, px + 2, y2_ - 3, px + pw - 3, y2_ - 3);

            XDrawLine(display_, window_, gc_, px + pw - 3, y1_ + 3, px + pw - 3, y2_ - 3);

            xf::draw_text_left(display_, window_, gc_, ts, px + 6, y1_, y2_, panels_[i].text);

        }

        if (size_grip_)

        {

            // The size grip: three diagonal ridges in the corner.

            XSetForeground(display_, gc_, shadow);

            for (int i = 0; i < 3; ++i)

            {

                int off = 4 + i * 4;

                XDrawLine(display_, window_, gc_, x2_ - off, y2_ - 2, x2_ - 2, y2_ - off);

            }

            XSetForeground(display_, gc_, light);

            for (int i = 0; i < 3; ++i)

            {

                int off = 5 + i * 4;

                XDrawLine(display_, window_, gc_, x2_ - off, y2_ - 2, x2_ - 2, y2_ - off);

            }

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

        int panel = panel_at(x);

        if (panel >= 0 && on_panel_click)

        {

            on_panel_click(panel);

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
