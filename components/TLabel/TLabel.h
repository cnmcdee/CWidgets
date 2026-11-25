#pragma once

#include "core/xframe_core.h"

//region TLabel

class TLabel

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

    int alignment_ = 0;   // 0 = left, 1 = centre, 2 = right

    bool wordwrap_ = false;

    bool autosize_ = true;

    bool show_box_ = false;

    bool pressed_ = false;

    int line_height()

    {

        SStyle st = style();

        if (!st.font_)

        {

            return 18;

        }

        return st.font_->ascent + st.font_->descent + 4;

    }

public:

    std::function<void()> on_click;

    TLabel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TLabel";

        text_ = "Label1";

    }

    ~TLabel() {}

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

    void set_alignment(int a)

    {

        alignment_ = a;

    }

    int get_alignment()

    {

        return alignment_;

    }

    void set_wordwrap(bool w)

    {

        wordwrap_ = w;

    }

    bool get_wordwrap()

    {

        return wordwrap_;

    }

    void set_autosize(bool a)

    {

        autosize_ = a;

    }

    bool get_autosize()

    {

        return autosize_;

    }

    void set_show_box(bool s)

    {

        show_box_ = s;

    }

    bool get_show_box()

    {

        return show_box_;

    }

    std::vector<std::string> split_lines()

    {

        std::vector<std::string> lines;

        std::string cur;

        for (size_t i = 0; i < text_.size(); ++i)

        {

            if (text_[i] == '\n')

            {

                lines.push_back(cur);

                cur.clear();

            }

            else

            {

                cur += text_[i];

            }

        }

        lines.push_back(cur);

        return lines;

    }

    int text_pixel_width()

    {

        SStyle st = style();

        int w = 0;

        for (auto& line : split_lines())

        {

            int lw = xf::text_width(st, line);

            if (lw > w)

            {

                w = lw;

            }

        }

        return w;

    }

    int text_pixel_height()

    {

        return (int)split_lines().size() * line_height();

    }

    void autosize_to_text()

    {

        int w = text_pixel_width();

        int h = text_pixel_height();

        if (w > 0)

        {

            x2_ = x1_ + w + 4;

        }

        if (h > 0)

        {

            y2_ = y1_ + h + 2;

        }

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

        if (show_box_)

        {

            xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        if (!st.font_)

        {

            return;

        }

        std::vector<std::string> lines = split_lines();

        int lh = line_height();

        for (int i = 0; i < (int)lines.size(); ++i)

        {

            int ly1 = y1_ + i * lh;

            int ly2 = ly1 + lh;

            if (ly1 > y2_)

            {

                break;

            }

            if (alignment_ == 1)

            {

                xf::draw_text_centered(display_, window_, gc_, st, x1_, ly1, x2_, ly2, lines[i]);

            }

            else if (alignment_ == 2)

            {

                int tw = xf::text_width(st, lines[i]);

                xf::draw_text_left(display_, window_, gc_, st, x2_ - tw - 2, ly1, ly2, lines[i]);

            }

            else

            {

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 2, ly1, ly2, lines[i]);

            }

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        pressed_ = true;

        focused_ = true;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        if (contains(x, y) && on_click)

        {

            on_click();

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

        if ((key == XK_Return || key == XK_space) && on_click)

        {

            on_click();

            return true;

        }

        return false;

    }

};

//endregion
