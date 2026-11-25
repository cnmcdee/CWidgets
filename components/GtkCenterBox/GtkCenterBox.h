#pragma once

#include "core/xframe_core.h"

//region GtkCenterBox

class GtkCenterBox

{

public:

    struct Rect
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

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

    int baseline_position_ = 0;   // kept for API parity; layout ignores it
    int clicked_zone_ = -1;       // 0 = start, 1 = center, 2 = end

public:

    GtkCenterBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
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
        name_ = "GtkCenterBox";
        text_ = "CenterBox";
    }

    ~GtkCenterBox() {}

    void set_bounds(int x, int y, int width, int height) { x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height; }

    int width() { return x2_ - x1_; }

    int height() { return y2_ - y1_; }

    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }

    void set_visible(bool v) { visible_ = v; }

    bool get_visible() { return visible_; }

    void set_enabled(bool e) { enabled_ = e; }

    bool get_enabled() { return enabled_; }

    void set_style_name(std::string s) { style_name_ = s; }

    void set_text(std::string t) { text_ = t; }

    std::string get_text() { return text_; }

    SStyle style() { return styles_->resolve(style_name_); }

    void set_baseline_position(int position) { baseline_position_ = position; }

    int get_baseline_position() { return baseline_position_; }

    int get_clicked_zone() { return clicked_zone_; }

    // Each zone takes roughly a third of the width; the center zone
    // is positioned so its middle sits on the widget's middle.

    Rect start_rect()
    {
        int zone = std::max(1, width() / 3);
        return Rect { x1_ + 4, y1_ + 4, x1_ + zone - 4, y2_ - 4 };
    }

    Rect center_rect()
    {
        int zone = std::max(1, width() / 3);
        int cx1 = x1_ + width() / 2 - zone / 2;
        return Rect { cx1 + 4, y1_ + 4, cx1 + zone - 4, y2_ - 4 };
    }

    Rect end_rect()
    {
        int zone = std::max(1, width() / 3);
        return Rect { x2_ - zone + 4, y1_ + 4, x2_ - 4, y2_ - 4 };
    }

    void draw_zone(Rect r, const std::string& label, unsigned long tint, bool active)
    {
        SStyle st = style();
        st.fillColor_ = tint;

        if (active)
        {
            st.lineColor_ = styles_->get_color_rgb(30, 90, 180);
        }

        xf::fill_rounded_rect(display_, window_, gc_, st, r.x1, r.y1, r.x2, r.y2);
        xf::draw_rounded_outline(display_, window_, gc_, st, r.x1, r.y1, r.x2, r.y2);
        xf::draw_text_centered(display_, window_, gc_, st, r.x1, r.y1, r.x2, r.y2, label);
    }

    void draw()
    {
        if (!visible_)
        {
            return;
        }

        SStyle st = style();
        SStyle frame = st;
        frame.fillColor_ = styles_->get_color_rgb(238, 238, 240);
        xf::draw_polished(display_, window_, gc_, frame, x1_, y1_, x2_, y2_);

        draw_zone(start_rect(), "Start", styles_->get_color_rgb(220, 232, 248), clicked_zone_ == 0);
        draw_zone(center_rect(), "Center", styles_->get_color_rgb(226, 244, 224), clicked_zone_ == 1);
        draw_zone(end_rect(), "End", styles_->get_color_rgb(248, 232, 214), clicked_zone_ == 2);

        if (focused_)
        {
            XSetForeground(display_, gc_, st.lineColor_);
            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, height() - 4);
        }

        XFlush(display_);
    }

    bool zone_contains(Rect r, int x, int y) { return x >= r.x1 && x <= r.x2 && y >= r.y1 && y <= r.y2; }

    bool on_button_press(int x, int y, unsigned int button)
    {
        if (!contains(x, y) || !enabled_ || button != 1)
        {
            return false;
        }

        focused_ = true;
        clicked_zone_ = -1;

        if (zone_contains(start_rect(), x, y))
        {
            clicked_zone_ = 0;
        }
        else if (zone_contains(center_rect(), x, y))
        {
            clicked_zone_ = 1;
        }
        else if (zone_contains(end_rect(), x, y))
        {
            clicked_zone_ = 2;
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

        if (!focused_ || !enabled_)
        {
            return false;
        }

        if (key == XK_Left)
        {
            clicked_zone_ = std::max(0, (clicked_zone_ < 0 ? 1 : clicked_zone_) - 1);
            return true;
        }

        if (key == XK_Right)
        {
            clicked_zone_ = std::min(2, (clicked_zone_ < 0 ? 1 : clicked_zone_) + 1);
            return true;
        }

        return false;
    }

};

//endregion
