#pragma once

#include "core/xframe_core.h"

//region GtkOverlay

class GtkOverlay

{

public:

    struct Rect
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    // Alignment values, stored as plain ints in the overlay specs.

    static constexpr int ALIGN_START = 0;

    static constexpr int ALIGN_CENTER = 1;

    static constexpr int ALIGN_END = 2;

    struct OverlaySpec
    {
        std::string name;
        int halign;
        int valign;
        int w;
        int h;
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

    std::vector<OverlaySpec> overlays_;
    int margin_ = 8;            // gap between an overlay and the edge it hugs
    int selected_overlay_ = -1;

public:

    std::function<void(std::string)> on_overlay_click;

    GtkOverlay(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
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
        name_ = "GtkOverlay";
        text_ = "Main Child";

        // Preloaded overlays: a badge top-end, a toast bottom-center.

        add_overlay("Badge", ALIGN_END, ALIGN_START);
        add_overlay("Toast", ALIGN_CENTER, ALIGN_END);
    }

    ~GtkOverlay() {}

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

    // The main child always fills the whole overlay area.

    Rect main_rect() { return Rect { x1_, y1_, x2_, y2_ }; }

    int clamp_align(int align) { return std::max(ALIGN_START, std::min(ALIGN_END, align)); }

    void add_overlay(std::string name, int halign, int valign) { add_overlay(name, halign, valign, 110, 36); }

    void add_overlay(std::string name, int halign, int valign, int w, int h)
    {
        OverlaySpec spec { name, clamp_align(halign), clamp_align(valign), std::max(8, w), std::max(8, h) };
        overlays_.push_back(spec);
    }

    bool remove_overlay(std::string name)
    {
        for (int i = 0; i < (int)overlays_.size(); ++i)
        {
            if (overlays_[i].name == name)
            {
                overlays_.erase(overlays_.begin() + i);

                if (selected_overlay_ == i)
                {
                    selected_overlay_ = -1;
                }

                return true;
            }
        }

        return false;
    }

    int overlay_count() { return (int)overlays_.size(); }

    int get_selected_overlay() { return selected_overlay_; }

    // Position of overlay i, worked out from its two alignments.

    Rect overlay_rect(int i)
    {
        if (i < 0 || i >= (int)overlays_.size())
        {
            return Rect { 0, 0, 0, 0 };
        }

        OverlaySpec& o = overlays_[i];
        int ox1 = x1_ + margin_;

        if (o.halign == ALIGN_CENTER)
        {
            ox1 = x1_ + (width() - o.w) / 2;
        }
        else if (o.halign == ALIGN_END)
        {
            ox1 = x2_ - margin_ - o.w;
        }

        int oy1 = y1_ + margin_;

        if (o.valign == ALIGN_CENTER)
        {
            oy1 = y1_ + (height() - o.h) / 2;
        }
        else if (o.valign == ALIGN_END)
        {
            oy1 = y2_ - margin_ - o.h;
        }

        return Rect { ox1, oy1, ox1 + o.w, oy1 + o.h };
    }

    void draw()
    {
        if (!visible_)
        {
            return;
        }

        SStyle st = style();

        // Main child area.

        SStyle main_style = st;
        main_style.fillColor_ = styles_->get_color_rgb(232, 238, 246);
        xf::draw_polished(display_, window_, gc_, main_style, x1_, y1_, x2_, y2_);
        xf::draw_text_centered(display_, window_, gc_, main_style, x1_, y1_, x2_, y2_, text_);

        // Overlays float above the main child; solid accent fills
        // stand in for translucency, which plain X fills cannot do.

        const unsigned long tints[3] = {
            styles_->get_color_rgb(70, 110, 180),
            styles_->get_color_rgb(60, 150, 90),
            styles_->get_color_rgb(190, 110, 40)
        };

        for (int i = 0; i < (int)overlays_.size(); ++i)
        {
            Rect r = overlay_rect(i);
            SStyle chip = st;
            chip.fillColor_ = tints[i % 3];
            chip.lineColor_ = styles_->get_color_rgb(30, 30, 30);
            chip.text_color_ = styles_->get_color_rgb(255, 255, 255);

            if (i == selected_overlay_)
            {
                chip.lineColor_ = styles_->get_color_rgb(255, 220, 60);
            }

            xf::fill_rounded_rect(display_, window_, gc_, chip, r.x1, r.y1, r.x2, r.y2);
            xf::draw_rounded_outline(display_, window_, gc_, chip, r.x1, r.y1, r.x2, r.y2);
            xf::draw_text_centered(display_, window_, gc_, chip, r.x1, r.y1, r.x2, r.y2, overlays_[i].name);
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
        selected_overlay_ = -1;

        // Overlays sit on top, so they are hit-tested first.

        for (int i = (int)overlays_.size() - 1; i >= 0; --i)
        {
            Rect r = overlay_rect(i);

            if (x >= r.x1 && x <= r.x2 && y >= r.y1 && y <= r.y2)
            {
                selected_overlay_ = i;

                if (on_overlay_click)
                {
                    on_overlay_click(overlays_[i].name);
                }

                break;
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

        if (key == XK_Tab && !overlays_.empty())
        {
            selected_overlay_ = (selected_overlay_ + 1) % (int)overlays_.size();
            return true;
        }

        if ((key == XK_Return || key == XK_space) && selected_overlay_ >= 0 && selected_overlay_ < (int)overlays_.size() && on_overlay_click)
        {
            on_overlay_click(overlays_[selected_overlay_].name);
            return true;
        }

        return false;
    }

};

//endregion
