#pragma once

#include "core/xframe_core.h"

//region GtkFixed

class GtkFixed

{

public:

    struct Rect
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    struct FixedChild
    {
        std::string name;
        int x;
        int y;
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

    std::vector<FixedChild> children_;
    int selected_ = -1;         // index into children_, -1 = none
    bool dragging_ = false;
    int drag_dx_ = 0;           // pointer offset inside the dragged child
    int drag_dy_ = 0;

public:

    GtkFixed(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)
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
        name_ = "GtkFixed";
        text_ = "Fixed";

        // Sample children at absolute positions, as GtkFixed expects.

        put("Button", 16, 16, 90, 32);
        put("Label", 16, 64, 120, 24);
        put("Entry", 150, 16, 140, 28);
    }

    ~GtkFixed() {}

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

    int find_child(std::string name)
    {
        for (int i = 0; i < (int)children_.size(); ++i)
        {
            if (children_[i].name == name)
            {
                return i;
            }
        }

        return -1;
    }

    int child_count() { return (int)children_.size(); }

    // Child positions are stored relative to this widget's origin;
    // putting an existing name replaces its entry.

    void put(std::string name, int x, int y, int w, int h)
    {
        int idx = find_child(name);
        FixedChild child { name, x, y, std::max(1, w), std::max(1, h) };

        if (idx >= 0)
        {
            children_[idx] = child;
        }
        else
        {
            children_.push_back(child);
        }
    }

    bool move(std::string name, int x, int y)
    {
        int idx = find_child(name);

        if (idx < 0)
        {
            return false;
        }

        children_[idx].x = x;
        children_[idx].y = y;
        return true;
    }

    bool remove(std::string name)
    {
        int idx = find_child(name);

        if (idx < 0)
        {
            return false;
        }

        children_.erase(children_.begin() + idx);

        if (selected_ == idx)
        {
            selected_ = -1;
        }
        else if (selected_ > idx)
        {
            selected_ -= 1;
        }

        return true;
    }

    // Absolute (window) rect of the named child; empty when missing.

    Rect child_rect(std::string name)
    {
        int idx = find_child(name);

        if (idx < 0)
        {
            return Rect { 0, 0, 0, 0 };
        }

        FixedChild& c = children_[idx];
        return Rect { x1_ + c.x, y1_ + c.y, x1_ + c.x + c.w, y1_ + c.y + c.h };
    }

    std::string get_selected_name()
    {
        if (selected_ < 0 || selected_ >= (int)children_.size())
        {
            return "";
        }

        return children_[selected_].name;
    }

    void draw()
    {
        if (!visible_)
        {
            return;
        }

        SStyle st = style();
        SStyle frame = st;
        frame.fillColor_ = styles_->get_color_rgb(240, 240, 242);
        xf::draw_polished(display_, window_, gc_, frame, x1_, y1_, x2_, y2_);

        unsigned long marker = styles_->get_color_rgb(200, 60, 60);

        for (int i = 0; i < (int)children_.size(); ++i)
        {
            FixedChild& c = children_[i];
            Rect r { x1_ + c.x, y1_ + c.y, x1_ + c.x + c.w, y1_ + c.y + c.h };
            SStyle cell = st;

            if (i == selected_)
            {
                cell.fillColor_ = styles_->get_color_rgb(197, 220, 248);
            }

            xf::fill_rounded_rect(display_, window_, gc_, cell, r.x1, r.y1, r.x2, r.y2);
            xf::draw_rounded_outline(display_, window_, gc_, cell, r.x1, r.y1, r.x2, r.y2);
            xf::draw_text_centered(display_, window_, gc_, cell, r.x1, r.y1, r.x2, r.y2, c.name);

            // Crosshair on the child's stored origin point.

            XSetForeground(display_, gc_, marker);
            xf::set_line_width(display_, gc_, 1);
            XDrawLine(display_, window_, gc_, r.x1 - 5, r.y1, r.x1 + 5, r.y1);
            XDrawLine(display_, window_, gc_, r.x1, r.y1 - 5, r.x1, r.y1 + 5);
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
        selected_ = -1;

        // Topmost child wins: search back to front.

        for (int i = (int)children_.size() - 1; i >= 0; --i)
        {
            FixedChild& c = children_[i];

            if (x >= x1_ + c.x && x <= x1_ + c.x + c.w && y >= y1_ + c.y && y <= y1_ + c.y + c.h)
            {
                selected_ = i;
                dragging_ = true;
                drag_dx_ = x - (x1_ + c.x);
                drag_dy_ = y - (y1_ + c.y);
                break;
            }
        }

        return true;
    }

    bool on_button_release(int x, int y, unsigned int button)
    {
        (void)x;
        (void)y;

        if (button != 1 || !dragging_)
        {
            return false;
        }

        dragging_ = false;
        return true;
    }

    bool on_motion(int x, int y)
    {
        if (!dragging_ || selected_ < 0 || selected_ >= (int)children_.size())
        {
            return false;
        }

        FixedChild& c = children_[selected_];
        c.x = std::max(0, std::min(width() - c.w, x - x1_ - drag_dx_));
        c.y = std::max(0, std::min(height() - c.h, y - y1_ - drag_dy_));
        return true;
    }

    bool on_key(KeySym key, const char* str)
    {
        (void)str;

        if (!focused_ || !enabled_ || selected_ < 0 || selected_ >= (int)children_.size())
        {
            return false;
        }

        FixedChild& c = children_[selected_];

        if (key == XK_Left) { c.x = std::max(0, c.x - 1); return true; }

        if (key == XK_Right) { c.x = std::min(width() - c.w, c.x + 1); return true; }

        if (key == XK_Up) { c.y = std::max(0, c.y - 1); return true; }

        if (key == XK_Down) { c.y = std::min(height() - c.h, c.y + 1); return true; }

        if (key == XK_Delete)
        {
            remove(c.name);
            return true;
        }

        return false;
    }

};

//endregion
