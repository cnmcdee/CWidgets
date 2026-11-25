#pragma once

#include "core/xframe_core.h"

//region GtkPopover

class GtkPopover

{

public:

    // GtkPositionType values: where the popover sits relative to
    // the widget it points at, so the tail is on the opposite side.

    static const int POS_LEFT = 0;

    static const int POS_RIGHT = 1;

    static const int POS_TOP = 2;

    static const int POS_BOTTOM = 3;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // first content line

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::vector<std::string> lines_;   // content lines, text_ is line 0

    int position_ = POS_BOTTOM;        // tail drawn on the top edge

    int anchor_x_ = 0;                 // point the tail aims at

    int anchor_y_ = 0;

    bool modal_ = true;

    int tail_size()

    {

        return 9;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void()> on_show;

    std::function<void()> on_hide;

    GtkPopover(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkPopover";

        text_ = "Popover";

        lines_.push_back("Popover content");

        lines_.push_back("Pointed at its anchor widget.");

        anchor_x_ = x + width / 2;

        anchor_y_ = y;

    }

    ~GtkPopover() {}

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

        if (lines_.empty())

        {

            lines_.push_back(t);

        }

        else

        {

            lines_[0] = t;

        }

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkPopover specific API

    void add_line(std::string line)

    {

        lines_.push_back(line);

    }

    void clear_lines()

    {

        lines_.clear();

    }

    int line_count()

    {

        return (int)lines_.size();

    }

    // Aim the tail at an anchor point (usually the middle of the
    // widget the popover belongs to).

    void point_at(int x, int y)

    {

        anchor_x_ = x;

        anchor_y_ = y;

    }

    int get_anchor_x()

    {

        return anchor_x_;

    }

    int get_anchor_y()

    {

        return anchor_y_;

    }

    void set_position(int p)

    {

        position_ = p;

    }

    int get_position()

    {

        return position_;

    }

    void set_modal(bool m)

    {

        modal_ = m;

    }

    bool get_modal()

    {

        return modal_;

    }

    void popup()

    {

        visible_ = true;

        if (on_show)

        {

            on_show();

        }

    }

    void popdown()

    {

        visible_ = false;

        if (on_hide)

        {

            on_hide();

        }

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        int t = tail_size();

        // Panel bounds leave room for the tail on the anchor side.

        int px1 = x1_;

        int py1 = y1_;

        int px2 = x2_;

        int py2 = y2_;

        if (position_ == POS_BOTTOM)

        {

            py1 += t;

        }

        else if (position_ == POS_TOP)

        {

            py2 -= t;

        }

        else if (position_ == POS_RIGHT)

        {

            px1 += t;

        }

        else if (position_ == POS_LEFT)

        {

            px2 -= t;

        }

        // The tail: a filled triangle from the panel edge to the
        // anchor point, drawn first so the panel edge covers its base.

        XSetForeground(display_, gc_, st.fillColor_);

        if (position_ == POS_BOTTOM || position_ == POS_TOP)

        {

            int ax = anchor_x_;

            if (ax < px1 + 14)

            {

                ax = px1 + 14;

            }

            if (ax > px2 - 14)

            {

                ax = px2 - 14;

            }

            int edge = (position_ == POS_BOTTOM) ? py1 : py2;

            int tip = (position_ == POS_BOTTOM) ? py1 - t : py2 + t;

            for (int i = 0; i < t; ++i)

            {

                int yy = (position_ == POS_BOTTOM) ? tip + i : tip - i;

                int half = i;

                XDrawLine(display_, window_, gc_, ax - half, yy, ax + half, yy);

            }

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawLine(display_, window_, gc_, ax - t, edge, ax, tip);

            XDrawLine(display_, window_, gc_, ax + t, edge, ax, tip);

        }

        else

        {

            int ay = anchor_y_;

            if (ay < py1 + 14)

            {

                ay = py1 + 14;

            }

            if (ay > py2 - 14)

            {

                ay = py2 - 14;

            }

            int edge = (position_ == POS_RIGHT) ? px1 : px2;

            int tip = (position_ == POS_RIGHT) ? px1 - t : px2 + t;

            for (int i = 0; i < t; ++i)

            {

                int xx = (position_ == POS_RIGHT) ? tip + i : tip - i;

                int half = i;

                XDrawLine(display_, window_, gc_, xx, ay - half, xx, ay + half);

            }

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawLine(display_, window_, gc_, edge, ay - t, tip, ay);

            XDrawLine(display_, window_, gc_, edge, ay + t, tip, ay);

        }

        // The bubble panel itself.

        xf::draw_polished(display_, window_, gc_, st, px1, py1, px2, py2);

        // Content lines.

        int cy = py1 + 8;

        for (size_t i = 0; i < lines_.size(); ++i)

        {

            if (cy + 16 > py2 - 4)

            {

                break;

            }

            xf::draw_text_left(display_, window_, gc_, st, px1 + 12, cy, cy + 16, lines_[i]);

            cy += 17;

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!visible_ || button != 1)

        {

            return false;

        }

        if (contains(x, y))

        {

            focused_ = true;

            return true;

        }

        // A modal popover eats an outside click by closing.

        if (modal_)

        {

            popdown();

            return true;

        }

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

        (void)str;

        if (!visible_)

        {

            return false;

        }

        if (key == XK_Escape)

        {

            popdown();

            return true;

        }

        return false;

    }

};

//endregion
