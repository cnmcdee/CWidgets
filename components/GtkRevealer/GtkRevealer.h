#pragma once

#include "core/xframe_core.h"

//region GtkRevealer

class GtkRevealer

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

    bool reveal_child_ = true;

    double progress_ = 1.0;

    int transition_type_ = 1;

    std::string child_label_ = "Child";

public:

    std::function<void()> on_click;

    std::function<void(bool)> on_reveal_changed;

    GtkRevealer(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkRevealer";

        text_ = "Revealer";

        reveal_child_ = true;

        progress_ = 1.0;

        transition_type_ = 1;

    }

    ~GtkRevealer() {}

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

    void set_reveal_child(bool r)

    {

        if (r == reveal_child_)

        {

            return;

        }

        reveal_child_ = r;

        if (transition_type_ == 0)

        {

            progress_ = reveal_child_ ? 1.0 : 0.0;

        }

        if (on_reveal_changed)

        {

            on_reveal_changed(reveal_child_);

        }

    }

    bool get_reveal_child()

    {

        return reveal_child_;

    }

    bool revealed()

    {

        return progress_ > 0.99;

    }

    double get_progress()

    {

        return progress_;

    }

    void set_transition_type(int t)

    {

        transition_type_ = t;

        if (transition_type_ < 0)

        {

            transition_type_ = 0;

        }

        if (transition_type_ > 2)

        {

            transition_type_ = 2;

        }

    }

    int get_transition_type()

    {

        return transition_type_;

    }

    void set_child_label(std::string label)

    {

        child_label_ = label;

    }

    std::string get_child_label()

    {

        return child_label_;

    }

    bool step()

    {

        double target = reveal_child_ ? 1.0 : 0.0;

        if (progress_ < target)

        {

            progress_ += 0.2;

            if (progress_ > target)

            {

                progress_ = target;

            }

            return true;

        }

        if (progress_ > target)

        {

            progress_ -= 0.2;

            if (progress_ < target)

            {

                progress_ = target;

            }

            return true;

        }

        return false;

    }

    bool is_animating()

    {

        double target = reveal_child_ ? 1.0 : 0.0;

        return progress_ != target;

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

        if (progress_ <= 0.0)

        {

            unsigned long faint = styles_->get_color_rgb(180, 180, 180);

            XSetForeground(display_, gc_, faint);

            xf::set_line_width(display_, gc_, 1);

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y2_, "Hidden - click to reveal");

            XFlush(display_);

            return;

        }

        int pct = (int)(progress_ * 100.0 + 0.5);

        std::string label = "Revealed " + std::to_string(pct) + "%";

        if (transition_type_ == 2)

        {

            int ch = (int)(height() * progress_);

            if (ch < 4)

            {

                ch = 4;

            }

            xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y1_ + ch);

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y1_, x2_, y1_ + ch, child_label_);

            xf::draw_text_centered(display_, window_, gc_, st, x1_, y2_ - 22, x2_, y2_ - 2, label);

        }

        else

        {

            SStyle fst = st;

            if (transition_type_ == 1 && progress_ < 1.0)

            {

                int fade = 140 + (int)(115.0 * progress_);

                fst.text_color_ = styles_->get_color_rgb((unsigned char)fade, (unsigned char)fade, (unsigned char)fade);

                if (!enabled_)

                {

                    fst.text_color_ = styles_->get_color_rgb(140, 140, 140);

                }

            }

            xf::draw_polished(display_, window_, gc_, fst, x1_, y1_, x2_, y2_);

            xf::draw_text_centered(display_, window_, gc_, fst, x1_, y1_, x2_, y1_ + height() / 2, child_label_);

            xf::draw_text_centered(display_, window_, gc_, fst, x1_, y1_ + height() / 2, x2_, y2_, label);

        }

        if (reveal_child_)

        {

            xf::draw_triangle_down(display_, window_, gc_, st.text_color_, x2_ - 16, y1_ + 8, 5);

        }

        else

        {

            xf::draw_triangle_right(display_, window_, gc_, st.text_color_, x2_ - 16, y1_ + 12, 5);

        }

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

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

        set_reveal_child(!reveal_child_);

        step();

        if (on_click)

        {

            on_click();

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

        if (key == XK_space || key == XK_Return)

        {

            set_reveal_child(!reveal_child_);

            step();

            if (on_click)

            {

                on_click();

            }

            return true;

        }

        return false;

    }

};

//endregion
