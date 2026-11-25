#pragma once

#include "core/xframe_core.h"

//region TTrackBar

class TTrackBar

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

    int orientation_ = 0;      // 0 = horizontal, 1 = vertical

    int min_ = 0;

    int max_ = 100;

    int position_ = 40;

    int frequency_ = 10;       // a tick mark every frequency_ units

    int line_size_ = 1;

    int page_size_ = 10;

    bool dragging_thumb_ = false;

    // The track runs between these pixel points (set by track_span).

    void track_span(int& t1, int& t2, int& centre)

    {

        if (orientation_ == 0)

        {

            t1 = x1_ + 12;

            t2 = x2_ - 12;

            centre = y1_ + height() / 2 - 4;

        }

        else

        {

            t1 = y1_ + 12;

            t2 = y2_ - 12;

            centre = x1_ + width() / 2 - 4;

        }

    }

    int position_to_pixel(int pos)

    {

        int t1, t2, centre;

        track_span(t1, t2, centre);

        if (max_ <= min_)

        {

            return t1;

        }

        return t1 + (pos - min_) * (t2 - t1) / (max_ - min_);

    }

    int pixel_to_position(int pixel)

    {

        int t1, t2, centre;

        track_span(t1, t2, centre);

        if (t2 <= t1)

        {

            return min_;

        }

        int pos = min_ + (pixel - t1) * (max_ - min_) / (t2 - t1);

        if (pos < min_)

        {

            pos = min_;

        }

        if (pos > max_)

        {

            pos = max_;

        }

        return pos;

    }

    void thumb_rect(int& tx1, int& ty1, int& tx2, int& ty2)

    {

        int c = position_to_pixel(position_);

        int t1, t2, centre;

        track_span(t1, t2, centre);

        if (orientation_ == 0)

        {

            tx1 = c - 7;

            tx2 = c + 7;

            ty1 = centre - 10;

            ty2 = centre + 10;

        }

        else

        {

            tx1 = centre - 10;

            tx2 = centre + 10;

            ty1 = c - 7;

            ty2 = c + 7;

        }

    }

    void set_position_internal(int pos, bool notify)

    {

        if (pos < min_)

        {

            pos = min_;

        }

        if (pos > max_)

        {

            pos = max_;

        }

        if (pos != position_)

        {

            position_ = pos;

            if (notify && on_change)

            {

                on_change();

            }

        }

    }

public:

    std::function<void()> on_change;

    TTrackBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TTrackBar";

        text_ = "TTrackBar";

    }

    ~TTrackBar() {}

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

    void set_orientation(int o)

    {

        orientation_ = o;

    }

    int get_orientation()

    {

        return orientation_;

    }

    void set_min(int m)

    {

        min_ = m;

        set_position_internal(position_, false);

    }

    int get_min()

    {

        return min_;

    }

    void set_max(int m)

    {

        max_ = m;

        set_position_internal(position_, false);

    }

    int get_max()

    {

        return max_;

    }

    void set_position(int p)

    {

        set_position_internal(p, false);

    }

    int get_position()

    {

        return position_;

    }

    void set_frequency(int f)

    {

        frequency_ = f < 1 ? 1 : f;

    }

    int get_frequency()

    {

        return frequency_;

    }

    void set_line_size(int s)

    {

        line_size_ = s;

    }

    void set_page_size(int s)

    {

        page_size_ = s;

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

        int t1, t2, centre;

        track_span(t1, t2, centre);

        unsigned long groove = styles_->get_color_rgb(120, 120, 120);

        unsigned long tick = st.lineColor_;

        // The groove the thumb slides in.

        XSetForeground(display_, gc_, groove);

        xf::set_line_width(display_, gc_, 2);

        if (orientation_ == 0)

        {

            XDrawLine(display_, window_, gc_, t1, centre, t2, centre);

        }

        else

        {

            XDrawLine(display_, window_, gc_, centre, t1, centre, t2);

        }

        xf::set_line_width(display_, gc_, 1);

        // Tick marks at every frequency_ step.

        XSetForeground(display_, gc_, tick);

        if (frequency_ > 0 && max_ > min_)

        {

            for (int v = min_; v <= max_; v += frequency_)

            {

                int p = position_to_pixel(v);

                int len = (v == min_ || v == max_) ? 7 : 5;

                if (orientation_ == 0)

                {

                    XDrawLine(display_, window_, gc_, p, centre + 12, p, centre + 12 + len);

                }

                else

                {

                    XDrawLine(display_, window_, gc_, centre + 12, p, centre + 12 + len, p);

                }

            }

        }

        // The thumb itself, polished like a small button.

        int tx1, ty1, tx2, ty2;

        thumb_rect(tx1, ty1, tx2, ty2);

        xf::draw_polished(display_, window_, gc_, st, tx1, ty1, tx2, ty2);

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 1, y1_ + 1, width() - 2, height() - 2);

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

        int tx1, ty1, tx2, ty2;

        thumb_rect(tx1, ty1, tx2, ty2);

        if (x >= tx1 && x <= tx2 && y >= ty1 && y <= ty2)

        {

            dragging_thumb_ = true;

            return true;

        }

        // A click on the track pages towards the click.

        int click_pos = pixel_to_position(orientation_ == 0 ? x : y);

        if (click_pos < position_)

        {

            set_position_internal(position_ - page_size_, true);

        }

        else if (click_pos > position_)

        {

            set_position_internal(position_ + page_size_, true);

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || !dragging_thumb_)

        {

            return false;

        }

        dragging_thumb_ = false;

        return true;

    }

    bool on_motion(int x, int y)

    {

        if (!dragging_thumb_)

        {

            return false;

        }

        set_position_internal(pixel_to_position(orientation_ == 0 ? x : y), true);

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)str;

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Left || key == XK_Down)

        {

            set_position_internal(position_ - line_size_, true);

            return true;

        }

        if (key == XK_Right || key == XK_Up)

        {

            set_position_internal(position_ + line_size_, true);

            return true;

        }

        if (key == XK_Page_Down)

        {

            set_position_internal(position_ - page_size_, true);

            return true;

        }

        if (key == XK_Page_Up)

        {

            set_position_internal(position_ + page_size_, true);

            return true;

        }

        if (key == XK_Home)

        {

            set_position_internal(min_, true);

            return true;

        }

        if (key == XK_End)

        {

            set_position_internal(max_, true);

            return true;

        }

        return false;

    }

};

//endregion
