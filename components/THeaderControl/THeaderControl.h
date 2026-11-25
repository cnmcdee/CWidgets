#pragma once

#include "core/xframe_core.h"

//region THeaderControl

class THeaderControl

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

    struct THeaderSection

    {

        std::string caption;

        int width;

    };

    std::vector<THeaderSection> sections_;

    int sort_section_ = -1;    // -1 = no sort arrow

    bool sort_ascending_ = true;

    int drag_section_ = -1;    // divider being dragged (section to its left)

    int pressed_section_ = -1;

    int section_x(int index)

    {

        int sx = x1_;

        for (int i = 0; i < index && i < (int)sections_.size(); ++i)

        {

            sx += sections_[i].width;

        }

        return sx;

    }

    // Divider index under x, or -1. Divider i sits between section
    // i and section i + 1.

    int divider_at(int x)

    {

        for (size_t i = 0; i + 1 < sections_.size(); ++i)

        {

            int dx = section_x((int)i + 1);

            if (x >= dx - 3 && x <= dx + 3)

            {

                return (int)i;

            }

        }

        return -1;

    }

    int section_at(int x)

    {

        for (size_t i = 0; i < sections_.size(); ++i)

        {

            int sx = section_x((int)i);

            if (x >= sx && x < sx + sections_[i].width)

            {

                return (int)i;

            }

        }

        return -1;

    }

public:

    std::function<void(int)> on_section_click;

    std::function<void(int)> on_section_resize;

    THeaderControl(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "THeaderControl";

        text_ = "THeaderControl";

        add_section("Name", 120);

        add_section("Size", 80);

        add_section("Type", 100);

        add_section("Modified", 110);

        sort_section_ = 0;

    }

    ~THeaderControl() {}

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

    void add_section(std::string caption, int w)

    {

        THeaderSection section;

        section.caption = caption;

        section.width = w;

        sections_.push_back(section);

    }

    int section_count()

    {

        return (int)sections_.size();

    }

    int get_section_width(int index)

    {

        if (index >= 0 && index < (int)sections_.size())

        {

            return sections_[index].width;

        }

        return 0;

    }

    void set_section_width(int index, int w)

    {

        if (index >= 0 && index < (int)sections_.size())

        {

            sections_[index].width = w < 24 ? 24 : w;

        }

    }

    int get_sort_section()

    {

        return sort_section_;

    }

    bool get_sort_ascending()

    {

        return sort_ascending_;

    }

    void set_sort(int section, bool ascending)

    {

        sort_section_ = section;

        sort_ascending_ = ascending;

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

        for (size_t i = 0; i < sections_.size(); ++i)

        {

            int sx = section_x((int)i);

            int sw = sections_[i].width;

            SStyle hs = st;

            if ((int)i == pressed_section_ || (int)i == drag_section_)

            {

                unsigned long swap = hs.fillColor_;

                hs.fillColor_ = hs.outlineColor_;

                hs.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, hs, sx, y1_, sx + sw, y2_);

            xf::draw_text_left(display_, window_, gc_, ts, sx + 6, y1_, y2_, sections_[i].caption);

            if ((int)i == sort_section_)

            {

                int cx = sx + sw - 12;

                int cy = (y1_ + y2_) / 2;

                if (sort_ascending_)

                {

                    xf::draw_triangle_up(display_, window_, gc_, ts.text_color_, cx, cy + 3, 5);

                }

                else

                {

                    xf::draw_triangle_down(display_, window_, gc_, ts.text_color_, cx, cy - 3, 5);

                }

            }

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

        int divider = divider_at(x);

        if (divider >= 0)

        {

            drag_section_ = divider;

            return true;

        }

        pressed_section_ = section_at(x);

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1)

        {

            return false;

        }

        if (drag_section_ >= 0)

        {

            drag_section_ = -1;

            return true;

        }

        if (pressed_section_ >= 0)

        {

            int clicked = pressed_section_;

            pressed_section_ = -1;

            if (section_at(x) == clicked)

            {

                if (sort_section_ == clicked)

                {

                    sort_ascending_ = !sort_ascending_;

                }

                else

                {

                    sort_section_ = clicked;

                    sort_ascending_ = true;

                }

                if (on_section_click)

                {

                    on_section_click(clicked);

                }

            }

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        (void)y;

        if (drag_section_ < 0)

        {

            return false;

        }

        int left_edge = section_x(drag_section_);

        int new_width = x - left_edge;

        if (new_width < 24)

        {

            new_width = 24;

        }

        if (new_width != sections_[drag_section_].width)

        {

            sections_[drag_section_].width = new_width;

            if (on_section_resize)

            {

                on_section_resize(drag_section_);

            }

        }

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_ || sections_.empty())

        {

            return false;

        }

        if (key == XK_Left || key == XK_Right)

        {

            int s = sort_section_ < 0 ? 0 : sort_section_;

            s += (key == XK_Right) ? 1 : -1;

            if (s < 0)

            {

                s = 0;

            }

            if (s >= (int)sections_.size())

            {

                s = (int)sections_.size() - 1;

            }

            sort_section_ = s;

            return true;

        }

        if (key == XK_Up)

        {

            sort_ascending_ = true;

            return true;

        }

        if (key == XK_Down)

        {

            sort_ascending_ = false;

            return true;

        }

        return false;

    }

};

//endregion
