#pragma once

#include "core/xframe_core.h"

//region TLabeledEdit

enum TLabelPosition

{

    lpAbove,

    lpBelow,

    lpLeft,

    lpRight

};

struct TLabeledEditRect

{

    int x1;

    int y1;

    int x2;

    int y2;

};

class TLabeledEdit

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

    std::string style_name_;

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::string label_text_ = "Name:";

    TLabelPosition label_position_ = lpAbove;

    int cursor_ = 0;

    int label_width()

    {

        return xf::text_width(style(), label_text_) + 6;

    }

public:

    TLabeledEditRect label_rect()

    {

        TLabeledEditRect r{x1_, y1_, x2_, y2_};

        if (label_position_ == lpAbove)

        {

            r = TLabeledEditRect{x1_, y1_, x2_, y1_ + 16};

        }

        else if (label_position_ == lpBelow)

        {

            r = TLabeledEditRect{x1_, y2_ - 16, x2_, y2_};

        }

        else if (label_position_ == lpLeft)

        {

            r = TLabeledEditRect{x1_, y1_, x1_ + label_width(), y2_};

        }

        else if (label_position_ == lpRight)

        {

            r = TLabeledEditRect{x2_ - label_width(), y1_, x2_, y2_};

        }

        return r;

    }

    TLabeledEditRect edit_rect()

    {

        TLabeledEditRect r{x1_, y1_, x2_, y2_};

        if (label_position_ == lpAbove)

        {

            r = TLabeledEditRect{x1_, y1_ + 18, x2_, y2_};

        }

        else if (label_position_ == lpBelow)

        {

            r = TLabeledEditRect{x1_, y1_, x2_, y2_ - 18};

        }

        else if (label_position_ == lpLeft)

        {

            r = TLabeledEditRect{x1_ + label_width() + 2, y1_, x2_, y2_};

        }

        else if (label_position_ == lpRight)

        {

            r = TLabeledEditRect{x1_, y1_, x2_ - label_width() - 2, y2_};

        }

        return r;

    }

    TLabeledEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TLabeledEdit";

        text_ = "Edit text";

        cursor_ = (int)text_.size();

    }

    ~TLabeledEdit() {}

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

        cursor_ = (int)text_.size();

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    void set_label_text(std::string t)

    {

        label_text_ = t;

    }

    std::string get_label_text()

    {

        return label_text_;

    }

    void set_label_position(TLabelPosition p)

    {

        label_position_ = p;

    }

    TLabelPosition get_label_position()

    {

        return label_position_;

    }

    int get_cursor()

    {

        return cursor_;

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

        TLabeledEditRect lr = label_rect();

        TLabeledEditRect er = edit_rect();

        // The bound label.

        if (label_position_ == lpRight)

        {

            xf::draw_text_left(display_, window_, gc_, st, lr.x1 + 2, lr.y1, lr.y2, label_text_);

        }

        else if (label_position_ == lpLeft)

        {

            xf::draw_text_left(display_, window_, gc_, st, lr.x1, lr.y1, lr.y2, label_text_);

        }

        else

        {

            xf::draw_text_left(display_, window_, gc_, st, lr.x1, lr.y1, lr.y2, label_text_);

        }

        // The edit box: white inset fill plus the style outline.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, er.x1, er.y1, er.x2 - er.x1, er.y2 - er.y1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, er.x1, er.y1, er.x2 - er.x1, er.y2 - er.y1);

        SStyle edit_style = st;

        edit_style.text_color_ = enabled_ ? styles_->get_color_rgb(0, 0, 0) : styles_->get_color_rgb(140, 140, 140);

        xf::draw_text_left(display_, window_, gc_, edit_style, er.x1 + 4, er.y1, er.y2, text_);

        if (focused_ && enabled_)

        {

            std::string before = text_.substr(0, (size_t)cursor_);

            int cursor_x = er.x1 + 4 + xf::text_width(edit_style, before);

            XSetForeground(display_, gc_, styles_->get_color_rgb(0, 0, 0));

            XDrawLine(display_, window_, gc_, cursor_x, er.y1 + 3, cursor_x, er.y2 - 3);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!enabled_ || button != 1)

        {

            return false;

        }

        TLabeledEditRect er = edit_rect();

        if (!visible_ || x < er.x1 || x > er.x2 || y < er.y1 || y > er.y2)

        {

            return false;

        }

        focused_ = true;

        // Place the cursor at the character nearest the click: walk
        // the prefix widths until they pass the click offset.

        SStyle st = style();

        int offset = x - (er.x1 + 4);

        cursor_ = (int)text_.size();

        for (int i = 0; i <= (int)text_.size(); ++i)

        {

            int w = xf::text_width(st, text_.substr(0, (size_t)i));

            if (w >= offset)

            {

                cursor_ = i;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                text_.erase((size_t)cursor_ - 1, 1);

                cursor_--;

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)text_.size())

            {

                text_.erase((size_t)cursor_, 1);

            }

            return true;

        }

        if (key == XK_Left)

        {

            if (cursor_ > 0)

            {

                cursor_--;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_ < (int)text_.size())

            {

                cursor_++;

            }

            return true;

        }

        if (key == XK_Home)

        {

            cursor_ = 0;

            return true;

        }

        if (key == XK_End)

        {

            cursor_ = (int)text_.size();

            return true;

        }

        if (str && str[0] >= 32 && str[0] < 127)

        {

            text_.insert((size_t)cursor_, 1, str[0]);

            cursor_++;

            return true;

        }

        return false;

    }

};

//endregion
