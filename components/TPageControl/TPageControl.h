#pragma once

#include "core/xframe_core.h"

//region TPageControl

class TPageControl

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

    std::vector<std::string> pages_;

    int active_page_ = 0;

    bool show_close_buttons_ = false;   // the nboShowCloseButtons flag

    int tab_h_ = 24;

    int tab_width(int index, SStyle& st)

    {

        int w = xf::text_width(st, pages_[index]) + 20;

        if (show_close_buttons_)

        {

            w += 16;

        }

        return w;

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

    int tab_at(int x, SStyle& st)

    {

        for (size_t i = 0; i < pages_.size(); ++i)

        {

            int tx = tab_x((int)i, st);

            if (x >= tx && x < tx + tab_width((int)i, st))

            {

                return (int)i;

            }

        }

        return -1;

    }

    bool close_at(int x, int y, int index, SStyle& st)

    {

        if (!show_close_buttons_)

        {

            return false;

        }

        int tx = tab_x(index, st);

        int cx1 = tx + tab_width(index, st) - 15;

        return x >= cx1 && x <= cx1 + 11 && y >= y1_ + 6 && y <= y1_ + 17;

    }

public:

    std::function<void()> on_change;

    TPageControl(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPageControl";

        text_ = "TPageControl";

        add_page("TabSheet1");

        add_page("TabSheet2");

        add_page("TabSheet3");

    }

    ~TPageControl() {}

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

    void add_page(std::string caption)

    {

        pages_.push_back(caption);

    }

    void remove_page(int index)

    {

        if (index < 0 || index >= (int)pages_.size() || pages_.size() <= 1)

        {

            return;

        }

        pages_.erase(pages_.begin() + index);

        if (active_page_ >= (int)pages_.size())

        {

            active_page_ = (int)pages_.size() - 1;

        }

    }

    int page_count()

    {

        return (int)pages_.size();

    }

    void set_active_page(int index)

    {

        if (index >= 0 && index < (int)pages_.size() && index != active_page_)

        {

            active_page_ = index;

            if (on_change)

            {

                on_change();

            }

        }

    }

    int get_active_page()

    {

        return active_page_;

    }

    std::string get_active_caption()

    {

        if (active_page_ >= 0 && active_page_ < (int)pages_.size())

        {

            return pages_[active_page_];

        }

        return "";

    }

    void set_show_close_buttons(bool s)

    {

        show_close_buttons_ = s;

    }

    bool get_show_close_buttons()

    {

        return show_close_buttons_;

    }

    // The body rectangle a host would place the page's widgets in.

    void page_bounds(int& px1, int& py1, int& px2, int& py2)

    {

        px1 = x1_ + 3;

        py1 = y1_ + tab_h_ + 3;

        px2 = x2_ - 3;

        py2 = y2_ - 3;

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

        // Page body first: a filled panel with an outline.

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_ + tab_h_, width(), height() - tab_h_);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, (int)st.lineWidth_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_ + tab_h_, width() - 1, height() - tab_h_ - 1);

        // Tabs along the top; the active tab joins the body.

        for (size_t i = 0; i < pages_.size(); ++i)

        {

            int tx = tab_x((int)i, st);

            int tw = tab_width((int)i, st);

            bool active = ((int)i == active_page_);

            int ty1 = active ? y1_ : y1_ + 3;

            int ty2 = y1_ + tab_h_ + (active ? 1 : 0);

            xf::draw_polished(display_, window_, gc_, st, tx, ty1, tx + tw, ty2);

            if (active)

            {

                // Erase the body's top edge under the active tab.

                XSetForeground(display_, gc_, st.fillColor_);

                XDrawLine(display_, window_, gc_, tx + 2, y1_ + tab_h_, tx + tw - 2, y1_ + tab_h_);

            }

            int text_right = show_close_buttons_ ? tx + tw - 16 : tx + tw;

            xf::draw_text_centered(display_, window_, gc_, ts, tx, ty1, text_right, y1_ + tab_h_, pages_[i]);

            if (show_close_buttons_)

            {

                int cx = tx + tw - 10;

                int cy = y1_ + 11;

                XSetForeground(display_, gc_, ts.text_color_);

                xf::set_line_width(display_, gc_, 1);

                XDrawLine(display_, window_, gc_, cx - 3, cy - 3, cx + 3, cy + 3);

                XDrawLine(display_, window_, gc_, cx - 3, cy + 3, cx + 3, cy - 3);

            }

        }

        // The active page's name stands in for its contents.

        if (!pages_.empty())

        {

            std::string label = "Page: " + pages_[active_page_];

            xf::draw_text_centered(display_, window_, gc_, ts, x1_, y1_ + tab_h_, x2_, y2_, label);

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

        if (y <= y1_ + tab_h_)

        {

            SStyle st = style();

            int idx = tab_at(x, st);

            if (idx >= 0)

            {

                if (close_at(x, y, idx, st))

                {

                    remove_page(idx);

                    if (on_change)

                    {

                        on_change();

                    }

                    return true;

                }

                set_active_page(idx);

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

        if (!focused_ || !enabled_ || pages_.empty())

        {

            return false;

        }

        if (key == XK_Left)

        {

            set_active_page((active_page_ + (int)pages_.size() - 1) % (int)pages_.size());

            return true;

        }

        if (key == XK_Right)

        {

            set_active_page((active_page_ + 1) % (int)pages_.size());

            return true;

        }

        return false;

    }

};

//endregion
