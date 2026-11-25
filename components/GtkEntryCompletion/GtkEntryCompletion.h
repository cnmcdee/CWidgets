#pragma once

#include "core/xframe_core.h"

//region GtkEntryCompletion

class GtkEntryCompletion

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the entry buffer being completed

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::vector<std::string> items_;   // the completion list

    int cursor_ = 0;

    int min_key_length_ = 1;

    int selected_match_ = -1;   // highlighted row in the popup

    bool dismissed_ = false;    // Escape pressed; popup stays shut until text changes

    bool blink_on_ = true;

    std::string lowered(const std::string& s)

    {

        std::string out = s;

        for (auto& c : out)

        {

            if (c >= 'A' && c <= 'Z')

            {

                c = (char)(c + 32);

            }

        }

        return out;

    }

    // Items whose text starts with the current buffer (case-insensitive).

    std::vector<std::string> current_matches()

    {

        std::vector<std::string> out;

        if ((int)text_.size() < min_key_length_)

        {

            return out;

        }

        std::string prefix = lowered(text_);

        for (auto& item : items_)

        {

            if (lowered(item).compare(0, prefix.size(), prefix) == 0 && item != text_)

            {

                out.push_back(item);

            }

        }

        return out;

    }

    bool popup_visible()

    {

        return focused_ && !dismissed_ && !current_matches().empty();

    }

    int row_height()

    {

        return 20;

    }

    int popup_y1()

    {

        return y2_ + 2;

    }

    int popup_rows()

    {

        return std::min(6, (int)current_matches().size());

    }

    bool in_popup(int x, int y)

    {

        return popup_visible() && x >= x1_ && x <= x2_ && y >= popup_y1() && y <= popup_y1() + popup_rows() * row_height();

    }

    void accept_match(int row)

    {

        std::vector<std::string> m = current_matches();

        if (row < 0 || row >= (int)m.size())

        {

            return;

        }

        text_ = m[(size_t)row];

        cursor_ = (int)text_.size();

        selected_match_ = -1;

        dismissed_ = true;

        if (on_match_selected)

        {

            on_match_selected(text_);

        }

        if (on_changed)

        {

            on_changed();

        }

    }

public:

    std::function<void()> on_changed;

    std::function<void(const std::string&)> on_match_selected;

    std::function<void()> on_activate;

    GtkEntryCompletion(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkEntryCompletion";

        text_ = "";

        items_ = {"apple", "apricot", "avocado", "banana", "blackberry", "blueberry", "cherry", "coconut", "fig", "grape"};

    }

    ~GtkEntryCompletion() {}

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

        if (!visible_)

        {

            return false;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            return true;

        }

        return in_popup(x, y);

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

        dismissed_ = false;

        selected_match_ = -1;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkEntryCompletion specific API

    void add_completion(std::string item)

    {

        items_.push_back(item);

    }

    void remove_completion(std::string item)

    {

        for (int i = 0; i < (int)items_.size(); ++i)

        {

            if (items_[(size_t)i] == item)

            {

                items_.erase(items_.begin() + i);

                return;

            }

        }

    }

    void clear_completions()

    {

        items_.clear();

    }

    int get_completion_count()

    {

        return (int)items_.size();

    }

    void set_minimum_key_length(int n)

    {

        min_key_length_ = std::max(0, n);

    }

    int get_minimum_key_length()

    {

        return min_key_length_;

    }

    int get_match_count()

    {

        return (int)current_matches().size();

    }

    void complete()

    {

        // Programmatic completion: accept the first current match.

        if (!current_matches().empty())

        {

            accept_match(0);

        }

    }

    void tick()

    {

        if (focused_)

        {

            blink_on_ = !blink_on_;

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

        SStyle body = st;

        body.fillColor_ = styles_->get_color_rgb(255, 255, 255);

        if (!enabled_)

        {

            body.fillColor_ = styles_->get_color_rgb(235, 235, 235);

        }

        xf::fill_rounded_rect(display_, window_, gc_, body, x1_, y1_, x2_, y2_);

        if (focused_)

        {

            SStyle ring = st;

            ring.lineColor_ = styles_->get_color_rgb(60, 120, 200);

            ring.lineWidth_ = 2;

            xf::draw_rounded_outline(display_, window_, gc_, ring, x1_, y1_, x2_, y2_);

        }

        else

        {

            xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        if (text_.empty())

        {

            SStyle ph = st;

            ph.text_color_ = styles_->get_color_rgb(150, 150, 150);

            xf::draw_text_left(display_, window_, gc_, ph, x1_ + 8, y1_, y2_, "Start typing: ap, ba, bl...");

        }

        else

        {

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 8, y1_, y2_, text_);

        }

        if (focused_ && enabled_ && blink_on_)

        {

            int cx = x1_ + 8 + xf::text_width(st, text_.substr(0, (size_t)cursor_));

            XSetForeground(display_, gc_, styles_->get_color_rgb(20, 20, 20));

            XDrawLine(display_, window_, gc_, cx, y1_ + 5, cx, y2_ - 5);

        }

        // The completion popup under the entry.

        if (popup_visible())

        {

            std::vector<std::string> m = current_matches();

            int py = popup_y1();

            int ph = popup_rows() * row_height();

            XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

            XFillRectangle(display_, window_, gc_, x1_, py, width(), ph);

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_, py, width(), ph);

            for (int row = 0; row < popup_rows(); ++row)

            {

                int ry = py + row * row_height();

                if (row == selected_match_)

                {

                    XSetForeground(display_, gc_, styles_->get_color_rgb(195, 215, 245));

                    XFillRectangle(display_, window_, gc_, x1_ + 1, ry + 1, width() - 2, row_height() - 1);

                }

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 8, ry, ry + row_height(), m[(size_t)row]);

            }

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!enabled_ || button != 1)

        {

            return false;

        }

        if (in_popup(x, y))

        {

            focused_ = true;

            accept_match((y - popup_y1()) / row_height());

            return true;

        }

        if (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_)

        {

            focused_ = true;

            blink_on_ = true;

            dismissed_ = false;

            int px = x - (x1_ + 8);

            int best = (int)text_.size();

            int best_dist = 1 << 30;

            for (int i = 0; i <= (int)text_.size(); ++i)

            {

                int d = xf::text_width(style(), text_.substr(0, (size_t)i)) - px;

                if (d < 0)

                {

                    d = -d;

                }

                if (d < best_dist)

                {

                    best_dist = d;

                    best = i;

                }

            }

            cursor_ = best;

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

        if (!in_popup(x, y))

        {

            return false;

        }

        int row = (y - popup_y1()) / row_height();

        if (row == selected_match_)

        {

            return false;

        }

        selected_match_ = row;

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        if (!focused_ || !enabled_)

        {

            return false;

        }

        blink_on_ = true;

        if (key == XK_Down)

        {

            std::vector<std::string> m = current_matches();

            if (!m.empty())

            {

                dismissed_ = false;

                selected_match_ = std::min(selected_match_ + 1, (int)m.size() - 1);

            }

            return true;

        }

        if (key == XK_Up)

        {

            if (selected_match_ > 0)

            {

                --selected_match_;

            }

            return true;

        }

        if (key == XK_Tab)

        {

            std::vector<std::string> m = current_matches();

            if (!m.empty())

            {

                accept_match(selected_match_ >= 0 ? selected_match_ : 0);

                return true;

            }

            return false;   // no matches: let Tab move focus

        }

        if (key == XK_Return || key == XK_KP_Enter)

        {

            if (popup_visible() && selected_match_ >= 0)

            {

                accept_match(selected_match_);

                return true;

            }

            if (on_activate)

            {

                on_activate();

            }

            return true;

        }

        if (key == XK_Escape)

        {

            dismissed_ = true;

            selected_match_ = -1;

            return true;

        }

        if (key == XK_Left)

        {

            if (cursor_ > 0)

            {

                --cursor_;

            }

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_ < (int)text_.size())

            {

                ++cursor_;

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

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                text_.erase((size_t)(cursor_ - 1), 1);

                --cursor_;

                dismissed_ = false;

                selected_match_ = -1;

                if (on_changed)

                {

                    on_changed();

                }

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)text_.size())

            {

                text_.erase((size_t)cursor_, 1);

                dismissed_ = false;

                selected_match_ = -1;

                if (on_changed)

                {

                    on_changed();

                }

            }

            return true;

        }

        if (str && str[0] >= 32 && str[0] != 127)

        {

            for (int i = 0; str[i]; ++i)

            {

                if (str[i] < 32 || str[i] == 127)

                {

                    continue;

                }

                text_.insert((size_t)cursor_, 1, str[i]);

                ++cursor_;

            }

            dismissed_ = false;

            selected_match_ = -1;

            if (on_changed)

            {

                on_changed();

            }

            return true;

        }

        return false;

    }

};

//endregion
