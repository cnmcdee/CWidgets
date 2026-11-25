#pragma once

#include "core/xframe_core.h"

//region GtkInfoBar

// A coloured message bar shown under a toolbar: an icon-ish glyph,
// the message, optional action buttons, and a close box. Clicking
// a button or the close box fires on_response with its id.

struct GtkInfoBarButton

{

    std::string label;

    int response_id;

};

class GtkInfoBar

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

    int message_type_ = 0;     // 0 info, 1 warning, 2 error, 3 question

    bool show_close_button_ = true;

    std::vector<GtkInfoBarButton> buttons_;

public:

    std::function<void(int)> on_response;   // -1 = closed

    GtkInfoBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkInfoBar";

        text_ = "Your changes have been saved.";

        add_button("Undo", 1);

    }

    ~GtkInfoBar() {}

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

    //region GtkInfoBar API

    void set_message_type(int t)

    {

        message_type_ = std::max(0, std::min(3, t));

    }

    int get_message_type()

    {

        return message_type_;

    }

    void set_show_close_button(bool s)

    {

        show_close_button_ = s;

    }

    bool get_show_close_button()

    {

        return show_close_button_;

    }

    void add_button(std::string label, int response_id)

    {

        GtkInfoBarButton b;

        b.label = label;

        b.response_id = response_id;

        buttons_.push_back(b);

    }

    void clear_buttons()

    {

        buttons_.clear();

    }

    int button_count()

    {

        return (int)buttons_.size();

    }

    unsigned long bar_color()

    {

        switch (message_type_)

        {

            case 1: return styles_->get_color_rgb(250, 232, 175);   // warning

            case 2: return styles_->get_color_rgb(246, 200, 200);   // error

            case 3: return styles_->get_color_rgb(205, 238, 205);   // question

            default: return styles_->get_color_rgb(205, 226, 246);  // info

        }

    }

    unsigned long glyph_color()

    {

        switch (message_type_)

        {

            case 1: return styles_->get_color_rgb(190, 130, 20);

            case 2: return styles_->get_color_rgb(185, 40, 40);

            case 3: return styles_->get_color_rgb(50, 140, 60);

            default: return styles_->get_color_rgb(52, 101, 164);

        }

    }

    //endregion

    //region Geometry

    void close_rect(int& rx, int& ry, int& rw, int& rh)

    {

        rw = 18;

        rh = 18;

        rx = x2_ - rw - 5;

        ry = y1_ + (height() - rh) / 2;

    }

    void button_rect(int index, int& rx, int& ry, int& rw, int& rh)

    {

        SStyle st = style();

        int cx, cy, cw, ch;

        close_rect(cx, cy, cw, ch);

        int right = show_close_button_ ? cx - 6 : x2_ - 6;

        // Lay buttons out right-to-left from the close box.

        rx = right;

        for (int i = (int)buttons_.size() - 1; i >= index; --i)

        {

            rw = xf::text_width(st, buttons_[(size_t)i].label) + 16;

            rx -= rw;

            if (i > index)

            {

                rx -= 6;

            }

        }

        rh = height() - 12;

        ry = y1_ + 6;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        XSetForeground(display_, gc_, bar_color());

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_, y2_ - 1, x2_ - 1, y2_ - 1);

        // Glyph: a filled disc with the type letter in it.

        int cy = (y1_ + y2_) / 2;

        XSetForeground(display_, gc_, glyph_color());

        XFillArc(display_, window_, gc_, x1_ + 7, cy - 9, 18, 18, 0, 360 * 64);

        SStyle glyph = st;

        glyph.text_color_ = styles_->get_color_rgb(255, 255, 255);

        const char* letter = message_type_ == 2 ? "!" : (message_type_ == 3 ? "?" : "i");

        xf::draw_text_centered(display_, window_, gc_, glyph, x1_ + 7, cy - 9, x1_ + 25, cy + 9, letter);

        // Message text.

        SStyle lab = st;

        lab.text_color_ = styles_->get_color_rgb(30, 30, 30);

        if (!enabled_)

        {

            lab.text_color_ = styles_->get_color_rgb(130, 130, 130);

        }

        xf::draw_text_left(display_, window_, gc_, lab, x1_ + 32, y1_, y2_, text_);

        // Action buttons.

        for (size_t i = 0; i < buttons_.size(); ++i)

        {

            int bx, by, bw, bh;

            button_rect((int)i, bx, by, bw, bh);

            SStyle btn = st;

            btn.fillColor_ = styles_->get_color_rgb(245, 245, 245);

            xf::draw_polished(display_, window_, gc_, btn, bx, by, bx + bw, by + bh);

            xf::draw_text_centered(display_, window_, gc_, btn, bx, by, bx + bw, by + bh, buttons_[i].label);

        }

        // Close box: an x in a small square.

        if (show_close_button_)

        {

            int cx, cy2, cw, ch;

            close_rect(cx, cy2, cw, ch);

            XSetForeground(display_, gc_, styles_->get_color_rgb(90, 90, 90));

            xf::set_line_width(display_, gc_, 2);

            XDrawLine(display_, window_, gc_, cx + 4, cy2 + 4, cx + cw - 4, cy2 + ch - 4);

            XDrawLine(display_, window_, gc_, cx + cw - 4, cy2 + 4, cx + 4, cy2 + ch - 4);

            xf::set_line_width(display_, gc_, 1);

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

        if (show_close_button_)

        {

            int cx, cy, cw, ch;

            close_rect(cx, cy, cw, ch);

            if (x >= cx && x <= cx + cw && y >= cy && y <= cy + ch)

            {

                visible_ = false;

                if (on_response)

                {

                    on_response(-1);

                }

                return true;

            }

        }

        for (size_t i = 0; i < buttons_.size(); ++i)

        {

            int bx, by, bw, bh;

            button_rect((int)i, bx, by, bw, bh);

            if (x >= bx && x <= bx + bw && y >= by && y <= by + bh)

            {

                if (on_response)

                {

                    on_response(buttons_[i].response_id);

                }

                return true;

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

        if (key == XK_Escape && show_close_button_)

        {

            visible_ = false;

            if (on_response)

            {

                on_response(-1);

            }

            return true;

        }

        return false;

    }

};

//endregion
