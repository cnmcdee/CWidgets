#pragma once

#include "core/xframe_core.h"

//region GtkLayout

class GtkLayout

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

    int virtual_width_ = 2000;
    int virtual_height_ = 1500;
    int hadjust_ = 0;           // horizontal scroll offset into the virtual area
    int vadjust_ = 0;           // vertical scroll offset into the virtual area
    int grid_step_ = 32;        // dot spacing in virtual coordinates
    int scroll_step_ = 32;      // one wheel notch / arrow key

public:

    GtkLayout(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
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
        name_ = "GtkLayout";
        text_ = "Layout";
    }

    ~GtkLayout() {}

    void set_bounds(int x, int y, int width, int height)
    {
        x1_ = x;
        y1_ = y;
        x2_ = x + width;
        y2_ = y + height;
        scroll_to(hadjust_, vadjust_);   // re-clamp against the new size
    }

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

    void set_virtual_size(int w, int h)
    {
        virtual_width_ = std::max(1, w);
        virtual_height_ = std::max(1, h);
        scroll_to(hadjust_, vadjust_);
    }

    int get_virtual_width() { return virtual_width_; }

    int get_virtual_height() { return virtual_height_; }

    // Offsets are clamped so the view never scrolls past the end of
    // the virtual area (or below zero).

    void scroll_to(int x, int y)
    {
        hadjust_ = std::max(0, std::min(x, std::max(0, virtual_width_ - width())));
        vadjust_ = std::max(0, std::min(y, std::max(0, virtual_height_ - height())));
    }

    void scroll_by(int dx, int dy) { scroll_to(hadjust_ + dx, vadjust_ + dy); }

    int get_hadjust() { return hadjust_; }

    int get_vadjust() { return vadjust_; }

    // The visible window, in virtual-area coordinates.

    Rect visible_rect() { return Rect { hadjust_, vadjust_, hadjust_ + width(), vadjust_ + height() }; }

    void draw()
    {
        if (!visible_)
        {
            return;
        }

        SStyle st = style();
        SStyle frame = st;
        frame.fillColor_ = styles_->get_color_rgb(250, 250, 250);
        xf::draw_polished(display_, window_, gc_, frame, x1_, y1_, x2_, y2_);

        // Grid dots across the visible window, shifted by the scroll
        // offsets so the pattern stays fixed to the virtual area.

        XSetForeground(display_, gc_, styles_->get_color_rgb(170, 175, 185));

        int first_vx = (hadjust_ / grid_step_) * grid_step_;
        int first_vy = (vadjust_ / grid_step_) * grid_step_;

        for (int vy = first_vy; vy < vadjust_ + height(); vy += grid_step_)
        {
            int sy = y1_ + (vy - vadjust_);

            if (sy < y1_ + 2 || sy > y2_ - 2)
            {
                continue;
            }

            for (int vx = first_vx; vx < hadjust_ + width(); vx += grid_step_)
            {
                int sx = x1_ + (vx - hadjust_);

                if (sx >= x1_ + 2 && sx <= x2_ - 2)
                {
                    XFillRectangle(display_, window_, gc_, sx - 1, sy - 1, 2, 2);
                }
            }
        }

        // Offset readout in the top-left corner.

        std::string info = "Offset " + std::to_string(hadjust_) + ", " + std::to_string(vadjust_) + " of " + std::to_string(virtual_width_) + "x" + std::to_string(virtual_height_);

        if (st.font_)
        {
            xf::draw_text_at(display_, window_, gc_, st, x1_ + 8, y1_ + 8 + st.font_->ascent, info);
        }

        draw_scrollbars();

        XFlush(display_);
    }

    // Thin proportional indicators along the right and bottom edges.

    void draw_scrollbars()
    {
        unsigned long track = styles_->get_color_rgb(225, 225, 228);
        unsigned long thumb = styles_->get_color_rgb(150, 155, 165);
        int bar = 8;

        XSetForeground(display_, gc_, track);
        XFillRectangle(display_, window_, gc_, x2_ - bar - 2, y1_ + 2, bar, height() - 4);

        int thumb_h = std::max(16, height() * height() / std::max(1, virtual_height_));
        int thumb_y = y1_ + 2 + (height() - 4 - thumb_h) * vadjust_ / std::max(1, std::max(1, virtual_height_ - height()));

        XSetForeground(display_, gc_, thumb);
        XFillRectangle(display_, window_, gc_, x2_ - bar - 2, thumb_y, bar, thumb_h);

        XSetForeground(display_, gc_, track);
        XFillRectangle(display_, window_, gc_, x1_ + 2, y2_ - bar - 2, width() - 4, bar);

        int thumb_w = std::max(16, width() * width() / std::max(1, virtual_width_));
        int thumb_x = x1_ + 2 + (width() - 4 - thumb_w) * hadjust_ / std::max(1, std::max(1, virtual_width_ - width()));

        XSetForeground(display_, gc_, thumb);
        XFillRectangle(display_, window_, gc_, thumb_x, y2_ - bar - 2, thumb_w, bar);
    }

    bool on_button_press(int x, int y, unsigned int button)
    {
        if (!contains(x, y) || !enabled_)
        {
            return false;
        }

        focused_ = true;

        // X11 wheel: 4 = up, 5 = down, 6 = left, 7 = right.

        if (button == 4) { scroll_by(0, -scroll_step_); return true; }

        if (button == 5) { scroll_by(0, scroll_step_); return true; }

        if (button == 6) { scroll_by(-scroll_step_, 0); return true; }

        if (button == 7) { scroll_by(scroll_step_, 0); return true; }

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

        if (key == XK_Left) { scroll_by(-scroll_step_, 0); return true; }

        if (key == XK_Right) { scroll_by(scroll_step_, 0); return true; }

        if (key == XK_Up) { scroll_by(0, -scroll_step_); return true; }

        if (key == XK_Down) { scroll_by(0, scroll_step_); return true; }

        if (key == XK_Page_Down) { scroll_by(0, height()); return true; }

        if (key == XK_Page_Up) { scroll_by(0, -height()); return true; }

        if (key == XK_Home) { scroll_to(0, 0); return true; }

        if (key == XK_End) { scroll_to(virtual_width_, virtual_height_); return true; }

        return false;
    }

};

//endregion
