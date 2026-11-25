#pragma once

#include "core/xframe_core.h"

#include <cctype>

//region TMaskEdit

class TMaskEdit

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

    std::string mask_;

    std::string values_;

    char blank_char_ = '_';

    int cursor_ = 0;

    bool is_editable(char m)

    {

        return m == '0' || m == '9' || m == 'L' || m == 'l' || m == 'A' || m == 'a';

    }

    int editable_count()

    {

        int n = 0;

        for (char m : mask_)

        {

            if (is_editable(m))

            {

                ++n;

            }

        }

        return n;

    }

    int value_index_at(int display_pos)

    {

        int vi = -1;

        int limit = display_pos;

        if (limit > (int)mask_.size())

        {

            limit = (int)mask_.size();

        }

        for (int i = 0; i < limit; ++i)

        {

            if (is_editable(mask_[i]))

            {

                ++vi;

            }

        }

        if (display_pos >= 0 && display_pos < (int)mask_.size() && is_editable(mask_[display_pos]))

        {

            return vi;

        }

        return -1;

    }

    int display_pos_of_value_index(int vi)

    {

        int seen = -1;

        for (int i = 0; i < (int)mask_.size(); ++i)

        {

            if (is_editable(mask_[i]))

            {

                ++seen;

                if (seen == vi)

                {

                    return i;

                }

            }

        }

        return -1;

    }

    bool valid_for_slot(char slot, char c)

    {

        if (slot == '0' || slot == '9')

        {

            return std::isdigit((unsigned char)c) != 0;

        }

        if (slot == 'L' || slot == 'l')

        {

            return std::isalpha((unsigned char)c) != 0;

        }

        if (slot == 'A' || slot == 'a')

        {

            return std::isalnum((unsigned char)c) != 0;

        }

        return false;

    }

    int next_editable(int from)

    {

        for (int i = from; i < (int)mask_.size(); ++i)

        {

            if (is_editable(mask_[i]))

            {

                return i;

            }

        }

        return -1;

    }

    int prev_editable(int from)

    {

        for (int i = from; i >= 0; --i)

        {

            if (is_editable(mask_[i]))

            {

                return i;

            }

        }

        return -1;

    }

    void ensure_values()

    {

        int n = editable_count();

        if ((int)values_.size() != n)

        {

            values_.assign(n, blank_char_);

        }

    }

public:

    std::function<void()> on_click;

    std::function<void()> on_change;

    TMaskEdit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TMaskEdit";

        text_ = "";

        set_mask("(999) 000-0000");

        set_text("5551234");

        int first = next_editable(0);

        cursor_ = first >= 0 ? first : 0;

    }

    ~TMaskEdit() {}

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

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region Mask API

    void set_mask(std::string m)

    {

        mask_ = m;

        values_.assign(editable_count(), blank_char_);

        cursor_ = 0;

        int first = next_editable(0);

        if (first >= 0)

        {

            cursor_ = first;

        }

    }

    std::string get_mask()

    {

        return mask_;

    }

    void set_blank_char(char c)

    {

        char old = blank_char_;

        blank_char_ = c;

        for (char& v : values_)

        {

            if (v == old)

            {

                v = blank_char_;

            }

        }

    }

    char get_blank_char()

    {

        return blank_char_;

    }

    std::string display_text()

    {

        ensure_values();

        std::string out;

        int vi = 0;

        for (char m : mask_)

        {

            if (is_editable(m))

            {

                out += (vi < (int)values_.size() ? values_[vi] : blank_char_);

                ++vi;

            }

            else

            {

                out += m;

            }

        }

        return out;

    }

    std::string get_plain_text()

    {

        std::string out;

        for (char v : values_)

        {

            if (v != blank_char_)

            {

                out += v;

            }

        }

        return out;

    }

    void set_text(std::string t)

    {

        ensure_values();

        for (char& v : values_)

        {

            v = blank_char_;

        }

        int vi = 0;

        for (char c : t)

        {

            if (vi >= (int)values_.size())

            {

                break;

            }

            int dp = display_pos_of_value_index(vi);

            if (dp < 0)

            {

                break;

            }

            if (valid_for_slot(mask_[dp], c))

            {

                values_[vi] = c;

                ++vi;

            }

        }

        text_ = display_text();

    }

    std::string get_text()

    {

        return display_text();

    }

    int get_cursor()

    {

        return cursor_;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long grey = styles_->get_color_rgb(150, 150, 150);

        unsigned long dim = enabled_ ? st.text_color_ : grey;

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        std::string disp = display_text();

        int char_w = xf::text_width(st, "M");

        if (char_w <= 0)

        {

            char_w = 8;

        }

        int tx = x1_ + 6;

        int baseline = y1_ + ((height()) + (st.font_ ? st.font_->ascent - st.font_->descent : 8)) / 2;

        if (st.font_)

        {

            XSetFont(display_, gc_, st.font_->fid);

            for (int i = 0; i < (int)disp.size(); ++i)

            {

                char c = disp[i];

                unsigned long col = dim;

                if (c == blank_char_)

                {

                    col = grey;

                }

                else if (i < (int)mask_.size() && !is_editable(mask_[i]))

                {

                    col = styles_->get_color_rgb(90, 90, 90);

                }

                XSetForeground(display_, gc_, col);

                XDrawString(display_, window_, gc_, tx + i * char_w, baseline, &c, 1);

            }

        }

        if (focused_ && enabled_)

        {

            XSetForeground(display_, gc_, dim);

            int cx = tx + cursor_ * char_w;

            XDrawLine(display_, window_, gc_, cx, y1_ + 4, cx, y2_ - 4);

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

        SStyle st = style();

        int char_w = xf::text_width(st, "M");

        if (char_w <= 0)

        {

            char_w = 8;

        }

        int idx = (x - (x1_ + 6)) / char_w;

        if (idx < 0)

        {

            idx = 0;

        }

        int ne = next_editable(idx);

        if (ne >= 0)

        {

            cursor_ = ne;

        }

        else

        {

            int pe = prev_editable((int)mask_.size() - 1);

            if (pe >= 0)

            {

                cursor_ = pe;

            }

        }

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        ensure_values();

        if (key == XK_BackSpace)

        {

            int pe = prev_editable(cursor_ - 1);

            if (cursor_ < (int)mask_.size() && cursor_ >= 0 && is_editable(mask_[cursor_]))

            {

                int vi = value_index_at(cursor_);

                if (vi >= 0 && values_[vi] != blank_char_)

                {

                    values_[vi] = blank_char_;

                    if (on_change) on_change();

                    return true;

                }

            }

            if (pe >= 0)

            {

                cursor_ = pe;

                int vi = value_index_at(cursor_);

                if (vi >= 0)

                {

                    values_[vi] = blank_char_;

                }

                if (on_change) on_change();

                return true;

            }

            return false;

        }

        if (key == XK_Delete)

        {

            if (cursor_ >= 0 && cursor_ < (int)mask_.size() && is_editable(mask_[cursor_]))

            {

                int vi = value_index_at(cursor_);

                if (vi >= 0)

                {

                    values_[vi] = blank_char_;

                    if (on_change) on_change();

                }

                return true;

            }

            return false;

        }

        if (key == XK_Left)

        {

            int pe = prev_editable(cursor_ - 1);

            if (pe >= 0)

            {

                cursor_ = pe;

            }

            return true;

        }

        if (key == XK_Right)

        {

            int ne = next_editable(cursor_ + 1);

            if (ne >= 0)

            {

                cursor_ = ne;

            }

            return true;

        }

        if (key == XK_Home)

        {

            int f = next_editable(0);

            if (f >= 0) cursor_ = f;

            return true;

        }

        if (key == XK_End)

        {

            int l = prev_editable((int)mask_.size() - 1);

            if (l >= 0) cursor_ = l;

            return true;

        }

        if (str && str[0] && (unsigned char)str[0] >= 32 && (unsigned char)str[0] < 127)

        {

            char c = str[0];

            int slot = next_editable(cursor_);

            if (slot < 0)

            {

                return false;

            }

            if (!valid_for_slot(mask_[slot], c))

            {

                return false;

            }

            int vi = value_index_at(slot);

            if (vi < 0)

            {

                return false;

            }

            values_[vi] = c;

            cursor_ = slot;

            int ne = next_editable(slot + 1);

            if (ne >= 0)

            {

                cursor_ = ne;

            }

            if (on_change) on_change();

            return true;

        }

        return false;

    }

};

//endregion
