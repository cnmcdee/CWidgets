#pragma once

#include "core/xframe_core.h"

//region TTabControl

class TTabControl

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

    std::vector<std::string> tabs_;

    int tab_index_ = 0;

    int tab_position_ = 0;     // 0 = top, 1 = bottom

    int tab_h_ = 24;

    int tab_width(int index, SStyle& st)

    {

        return xf::text_width(st, tabs_[index]) + 22;

    }

    int tab_x(int index, SStyle& st)

    {

        int tx = x1_ + 2;

        for (int i = 0; i < index; ++i)

        {

            tx += tab_width(i, st) + 2;

        }

        return tx;

    }

    int tab_y1()

    {

        return (tab_position_ == 0) ? y1_ : y2_ - tab_h_;

    }

    int tab_at(int x, SStyle& st)

    {

        for (size_t i = 0; i < tabs_.size(); ++i)

        {

            int tx = tab_x((int)i, st);

            if (x >= tx && x < tx + tab_width((int)i, st))

            {

                return (int)i;

            }

        }

        return -1;

    }

public:

    std::function<void()> on_change;

    TTabControl(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TTabControl";

        text_ = "TTabControl";

        add_tab("General");

        add_tab("Advanced");

        add_tab("About");

    }

    ~TTabControl() {}

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

    void add_tab(std::string caption)

    {

        tabs_.push_back(caption);

    }

    int tab_count()

    {

        return (int)tabs_.size();

    }

    void set_tab_index(int index)

    {

        if (index >= 0 && index < (int)tabs_.size() && index != tab_index_)

        {

            tab_index_ = index;

            if (on_change)

            {

                on_change();

            }

        }

    }

    int get_tab_index()

    {

        return tab_index_;

    }

    std::string get_active_caption()

    {

        if (tab_index_ >= 0 && tab_index_ < (int)tabs_.size())

        {

            return tabs_[tab_index_];

        }

        return "";

    }

    void set_tab_position(int p)

    {

        tab_position_ = p;

    }

    int get_tab_position()

    {

        return tab_position_;

    }

    // The display rectangle inside the tab strip — where the host
    // shows whatever belongs to the current tab.

    void display_bounds(int& dx1, int& dy1, int& dx2, int& dy2)

    {

        dx1 = x1_ + 3;

        dx2 = x2_ - 3;

        if (tab_position_ == 0)

        {

            dy1 = y1_ + tab_h_ + 3;

            dy2 = y2_ - 3;

        }

        else

        {

            dy1 = y1_ + 3;

            dy2 = y2_ - tab_h_ - 3;

        }

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        SStyle ts = st;

        if (!enabled_)

        {

            ts.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        // Body panel: unlike TPageControl it holds no pages of its
        // own — the host redraws this area when on_change fires.

        int by1 = (tab_position_ == 0) ? y1_ + tab_h_ : y1_;

        int by2 = (tab_position_ == 0) ? y2_ : y2_ - tab_h_;

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, by1, width(), by2 - by1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, by1, width() - 1, by2 - by1 - 1);

        int ty1 = tab_y1();

        for (size_t i = 0; i < tabs_.size(); ++i)

        {

            int tx = tab_x((int)i, st);

            int tw = tab_width((int)i, st);

            bool active = ((int)i == tab_index_);

            int top = ty1;

            int bottom = ty1 + tab_h_;

            if (tab_position_ == 0)

            {

                top = active ? ty1 : ty1 + 3;

                bottom = ty1 + tab_h_ + (active ? 1 : 0);

            }

            else

            {

                top = ty1 - (active ? 1 : 0);

                bottom = active ? ty1 + tab_h_ : ty1 + tab_h_ - 3;

            }

            xf::draw_polished(display_, window_, gc_, st, tx, top, tx + tw, bottom);

            if (active)

            {

                // Join the active tab to the body.

                XSetForeground(display_, gc_, st.fillColor_);

                if (tab_position_ == 0)

                {

                    XDrawLine(display_, window_, gc_, tx + 2, y1_ + tab_h_, tx + tw - 2, y1_ + tab_h_);

                }

                else

                {

                    XDrawLine(display_, window_, gc_, tx + 2, y2_ - tab_h_, tx + tw - 2, y2_ - tab_h_);

                }

            }

            xf::draw_text_centered(display_, window_, gc_, ts, tx, ty1, tx + tw, ty1 + tab_h_, tabs_[i]);

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

        int ty1 = tab_y1();

        if (y >= ty1 && y <= ty1 + tab_h_)

        {

            SStyle st = style();

            int idx = tab_at(x, st);

            if (idx >= 0)

            {

                set_tab_index(idx);

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

        if (!focused_ || !enabled_ || tabs_.empty())

        {

            return false;

        }

        if (key == XK_Left)

        {

            set_tab_index((tab_index_ + (int)tabs_.size() - 1) % (int)tabs_.size());

            return true;

        }

        if (key == XK_Right)

        {

            set_tab_index((tab_index_ + 1) % (int)tabs_.size());

            return true;

        }

        return false;

    }

};

//endregion
