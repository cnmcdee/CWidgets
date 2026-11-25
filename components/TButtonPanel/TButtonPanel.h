#pragma once

#include "core/xframe_core.h"

//region TButtonPanel

class TButtonPanel

{

public:

    // Button kinds (Lazarus TPanelButton values, as bit positions).

    static const int PB_OK = 0;

    static const int PB_CANCEL = 1;

    static const int PB_CLOSE = 2;

    static const int PB_HELP = 3;

    static const int PB_APPLY = 4;

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

    // Bitmask over the PB_ kinds: OK + Cancel by default.

    int show_buttons_ = (1 << PB_OK) | (1 << PB_CANCEL);

    int pressed_kind_ = -1;

    int button_w_ = 74;

    int gap_ = 6;

    // Draw / hit order, left to right inside the right-aligned group.

    const int order_[5] = {PB_OK, PB_CANCEL, PB_APPLY, PB_CLOSE, PB_HELP};

    const char* labels_[5] = {"OK", "Cancel", "Close", "Help", "Apply"};

    int kind_index(int kind)

    {

        // labels_ is indexed by kind directly.

        return kind;

    }

    int visible_count()

    {

        int n = 0;

        for (int k = 0; k < 5; ++k)

        {

            if (show_buttons_ & (1 << k))

            {

                ++n;

            }

        }

        return n;

    }

    // Rect of the button for kind, false when that kind is hidden.

    bool button_rect(int kind, int& bx1, int& bx2)

    {

        if (!(show_buttons_ & (1 << kind)))

        {

            return false;

        }

        int total = visible_count() * button_w_ + (visible_count() - 1) * gap_;

        int gx = x2_ - 8 - total;

        int pos = 0;

        for (int i = 0; i < 5; ++i)

        {

            int k = order_[i];

            if (!(show_buttons_ & (1 << k)))

            {

                continue;

            }

            if (k == kind)

            {

                bx1 = gx + pos * (button_w_ + gap_);

                bx2 = bx1 + button_w_;

                return true;

            }

            ++pos;

        }

        return false;

    }

    int kind_at(int x, int y)

    {

        int by1 = y1_ + 6;

        int by2 = y2_ - 6;

        if (y < by1 || y > by2)

        {

            return -1;

        }

        for (int i = 0; i < 5; ++i)

        {

            int bx1 = 0, bx2 = 0;

            if (button_rect(order_[i], bx1, bx2) && x >= bx1 && x <= bx2)

            {

                return order_[i];

            }

        }

        return -1;

    }

public:

    std::function<void()> on_click;

    std::function<void(int)> on_button;

    TButtonPanel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TButtonPanel";

        text_ = "TButtonPanel";

        // A typical dialog panel: OK, Cancel, Apply, Help.

        show_buttons_ = (1 << PB_OK) | (1 << PB_CANCEL) | (1 << PB_APPLY) | (1 << PB_HELP);

    }

    ~TButtonPanel() {}

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

    void set_show_buttons(int mask)

    {

        show_buttons_ = mask;

    }

    int get_show_buttons()

    {

        return show_buttons_;

    }

    void set_button_visible(int kind, bool show)

    {

        if (show)

        {

            show_buttons_ |= (1 << kind);

        }

        else

        {

            show_buttons_ &= ~(1 << kind);

        }

    }

    bool get_button_visible(int kind)

    {

        return (show_buttons_ & (1 << kind)) != 0;

    }

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

        // Panel body + a separator line along the top edge.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawLine(display_, window_, gc_, x1_, y1_, x2_, y1_);

        int by1 = y1_ + 6;

        int by2 = y2_ - 6;

        for (int i = 0; i < 5; ++i)

        {

            int kind = order_[i];

            int bx1 = 0, bx2 = 0;

            if (!button_rect(kind, bx1, bx2))

            {

                continue;

            }

            SStyle bs = st;

            if (kind == pressed_kind_)

            {

                unsigned long swap = bs.fillColor_;

                bs.fillColor_ = bs.outlineColor_;

                bs.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bs, bx1, by1, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, bs, bx1, by1, bx2, by2, labels_[kind_index(kind)]);

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

        pressed_kind_ = kind_at(x, y);

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1 || pressed_kind_ < 0)

        {

            return false;

        }

        int kind = pressed_kind_;

        pressed_kind_ = -1;

        if (kind_at(x, y) == kind)

        {

            if (on_button)

            {

                on_button(kind);

            }

            if (on_click)

            {

                on_click();

            }

        }

        return true;

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

        if (key == XK_Return && (show_buttons_ & (1 << PB_OK)))

        {

            if (on_button)

            {

                on_button(PB_OK);

            }

            return true;

        }

        if (key == XK_Escape && (show_buttons_ & (1 << PB_CANCEL)))

        {

            if (on_button)

            {

                on_button(PB_CANCEL);

            }

            return true;

        }

        return false;

    }

};

//endregion
