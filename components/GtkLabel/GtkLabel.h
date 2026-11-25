#pragma once

#include "core/xframe_core.h"

//region GtkLabel

class GtkLabel

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

    double xalign_ = 0.0;      // 0 = left, 0.5 = centre, 1 = right

    double yalign_ = 0.5;

    bool wrap_ = false;

    bool selectable_ = false;

    int selection_start_ = -1; // character offsets when selectable_

    int selection_end_ = -1;

public:

    GtkLabel(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkLabel";

        text_ = "Label";

    }

    ~GtkLabel() {}

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

        selection_start_ = -1;

        selection_end_ = -1;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkLabel API

    void set_xalign(double a)

    {

        xalign_ = std::max(0.0, std::min(1.0, a));

    }

    double get_xalign()

    {

        return xalign_;

    }

    void set_yalign(double a)

    {

        yalign_ = std::max(0.0, std::min(1.0, a));

    }

    double get_yalign()

    {

        return yalign_;

    }

    void set_wrap(bool w)

    {

        wrap_ = w;

    }

    bool get_wrap()

    {

        return wrap_;

    }

    void set_selectable(bool s)

    {

        selectable_ = s;

        if (!s)

        {

            selection_start_ = -1;

            selection_end_ = -1;

        }

    }

    bool get_selectable()

    {

        return selectable_;

    }

    std::string get_selected_text()

    {

        if (selection_start_ < 0 || selection_end_ <= selection_start_)

        {

            return "";

        }

        return text_.substr((size_t)selection_start_, (size_t)(selection_end_ - selection_start_));

    }

    // Split text_ into lines that fit the widget width (word wrap).

    std::vector<std::string> wrap_lines(const SStyle& st)

    {

        std::vector<std::string> lines;

        if (!wrap_)

        {

            lines.push_back(text_);

            return lines;

        }

        std::string line;

        size_t pos = 0;

        while (pos <= text_.size())

        {

            size_t sp = text_.find(' ', pos);

            std::string word = text_.substr(pos, sp == std::string::npos ? std::string::npos : sp - pos);

            std::string trial = line.empty() ? word : line + " " + word;

            if (!line.empty() && xf::text_width(st, trial) > width() - 4)

            {

                lines.push_back(line);

                line = word;

            }

            else

            {

                line = trial;

            }

            if (sp == std::string::npos)

            {

                break;

            }

            pos = sp + 1;

        }

        lines.push_back(line);

        return lines;

    }

    //endregion

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

        std::vector<std::string> lines = wrap_lines(st);

        int line_h = st.font_ ? (st.font_->ascent + st.font_->descent + 2) : 14;

        int block_h = line_h * (int)lines.size();

        int top = y1_ + (int)((height() - block_h) * yalign_);

        for (size_t i = 0; i < lines.size(); ++i)

        {

            int ly1 = top + (int)i * line_h;

            int ly2 = ly1 + line_h;

            if (ly2 > y2_)

            {

                break;

            }

            int tw = xf::text_width(st, lines[i]);

            int tx = x1_ + 2 + (int)((width() - 4 - tw) * xalign_);

            // Selection highlight behind the selected characters.

            if (selectable_ && selection_start_ >= 0 && selection_end_ > selection_start_ && lines.size() == 1)

            {

                std::string before = text_.substr(0, (size_t)selection_start_);

                std::string sel = get_selected_text();

                int sx = tx + xf::text_width(st, before);

                int sw = xf::text_width(st, sel);

                XSetForeground(display_, gc_, styles_->get_color_rgb(160, 190, 230));

                XFillRectangle(display_, window_, gc_, sx, ly1 + 1, sw, line_h - 2);

            }

            xf::draw_text_at(display_, window_, gc_, st, tx, ly1 + (st.font_ ? st.font_->ascent : 11), lines[i]);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (selectable_)

        {

            // Start a selection at the start of the text for the demo:
            // dragging extends it in on_motion via character hit test.

            selection_start_ = char_at(x);

            selection_end_ = selection_start_;

            return true;

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

        if (selectable_ && selection_start_ >= 0 && contains(x, y))

        {

            selection_end_ = char_at(x);

            if (selection_end_ < selection_start_)

            {

                int swap = selection_start_;

                selection_start_ = selection_end_;

                selection_end_ = swap;

            }

            return true;

        }

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)key;

        (void)str;

        return false;

    }

private:

    // Which character index sits under pixel x (single-line labels).

    int char_at(int x)

    {

        SStyle st = style();

        int tw = xf::text_width(st, text_);

        int tx = x1_ + 2 + (int)((width() - 4 - tw) * xalign_);

        int best = 0;

        int best_dist = 1 << 30;

        for (size_t i = 0; i <= text_.size(); ++i)

        {

            int cx = tx + xf::text_width(st, text_.substr(0, i));

            int d = std::abs(cx - x);

            if (d < best_dist)

            {

                best_dist = d;

                best = (int)i;

            }

        }

        return best;

    }

};

//endregion
