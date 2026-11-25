#pragma once

#include "core/xframe_core.h"

//region GtkBox

class GtkBox

{

public:

    struct Rect
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    // Orientation values stored in orientation_.

    static const int ORIENTATION_HORIZONTAL = 0;

    static const int ORIENTATION_VERTICAL = 1;

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

    int orientation_ = ORIENTATION_HORIZONTAL;
    int spacing_ = 6;
    bool homogeneous_ = false;
    int preferred_size_ = 64;   // slot size along the axis when not homogeneous
    int sample_count_ = 3;      // slots drawn by draw() for the bare demo
    int selected_slot_ = -1;

public:

    GtkBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
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
        name_ = "GtkBox";
        text_ = "Box";
    }

    ~GtkBox() {}

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

    void set_orientation(int orientation) { orientation_ = (orientation == ORIENTATION_VERTICAL) ? ORIENTATION_VERTICAL : ORIENTATION_HORIZONTAL; }

    int get_orientation() { return orientation_; }

    void set_spacing(int spacing) { spacing_ = std::max(0, spacing); }

    int get_spacing() { return spacing_; }

    void set_homogeneous(bool homogeneous) { homogeneous_ = homogeneous; }

    bool get_homogeneous() { return homogeneous_; }

    void set_sample_count(int count) { sample_count_ = std::max(1, count); }

    int get_sample_count() { return sample_count_; }

    int get_selected_slot() { return selected_slot_; }

    // The rect child i of count children would occupy. Homogeneous
    // boxes split the axis evenly; otherwise each child keeps
    // preferred_size_ and the last one takes whatever is left.

    Rect child_rect(int i, int count)
    {
        Rect r { x1_, y1_, x2_, y2_ };

        if (count <= 0 || i < 0 || i >= count)
        {
            return r;
        }

        int pad = 4;
        int ax1 = (orientation_ == ORIENTATION_HORIZONTAL) ? x1_ + pad : y1_ + pad;
        int ax2 = (orientation_ == ORIENTATION_HORIZONTAL) ? x2_ - pad : y2_ - pad;
        int total = std::max(0, ax2 - ax1 - spacing_ * (count - 1));
        int start = ax1;
        int size = 0;

        for (int k = 0; k <= i; ++k)
        {
            if (homogeneous_)
            {
                size = total / count + ((k < total % count) ? 1 : 0);
            }
            else if (k == count - 1)
            {
                size = std::max(0, ax1 + total - start);
            }
            else
            {
                size = std::min(preferred_size_, std::max(0, ax1 + total - start));
            }

            if (k < i)
            {
                start += size + spacing_;
            }
        }

        if (orientation_ == ORIENTATION_HORIZONTAL)
        {
            r.x1 = start;
            r.x2 = start + size;
            r.y1 = y1_ + pad;
            r.y2 = y2_ - pad;
        }
        else
        {
            r.x1 = x1_ + pad;
            r.x2 = x2_ - pad;
            r.y1 = start;
            r.y2 = start + size;
        }

        return r;
    }

    void draw()
    {
        if (!visible_)
        {
            return;
        }

        SStyle st = style();

        // Faint frame behind the slots.

        SStyle frame = st;
        frame.fillColor_ = styles_->get_color_rgb(238, 238, 240);
        xf::draw_polished(display_, window_, gc_, frame, x1_, y1_, x2_, y2_);

        for (int i = 0; i < sample_count_; ++i)
        {
            Rect r = child_rect(i, sample_count_);
            SStyle slot = st;

            if (i == selected_slot_)
            {
                slot.fillColor_ = styles_->get_color_rgb(197, 220, 248);
            }

            xf::fill_rounded_rect(display_, window_, gc_, slot, r.x1, r.y1, r.x2, r.y2);
            xf::draw_rounded_outline(display_, window_, gc_, slot, r.x1, r.y1, r.x2, r.y2);
            xf::draw_text_centered(display_, window_, gc_, slot, r.x1, r.y1, r.x2, r.y2, "Child " + std::to_string(i + 1));
        }

        // A small arrow in the corner shows the packing direction.

        unsigned long arrow = styles_->get_color_rgb(120, 120, 130);

        if (orientation_ == ORIENTATION_HORIZONTAL)
        {
            xf::draw_triangle_right(display_, window_, gc_, arrow, x2_ - 14, y1_ + 10, 4);
        }
        else
        {
            xf::draw_triangle_down(display_, window_, gc_, arrow, x2_ - 12, y1_ + 8, 4);
        }

        if (focused_)
        {
            XSetForeground(display_, gc_, st.lineColor_);
            XDrawRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, height() - 4);
        }

        XFlush(display_);
    }

    bool on_button_press(int x, int y, unsigned int button)
    {
        if (!contains(x, y) || !enabled_ || button != 1)
        {
            return false;
        }

        focused_ = true;
        selected_slot_ = -1;

        for (int i = 0; i < sample_count_; ++i)
        {
            Rect r = child_rect(i, sample_count_);

            if (x >= r.x1 && x <= r.x2 && y >= r.y1 && y <= r.y2)
            {
                selected_slot_ = i;
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

        if (!focused_ || !enabled_)
        {
            return false;
        }

        if (key == XK_plus || key == XK_equal)
        {
            set_spacing(spacing_ + 2);
            return true;
        }

        if (key == XK_minus && spacing_ > 0)
        {
            set_spacing(spacing_ - 2);
            return true;
        }

        if (key == XK_Tab)
        {
            selected_slot_ = (selected_slot_ + 1) % sample_count_;
            return true;
        }

        return false;
    }

};

//endregion
