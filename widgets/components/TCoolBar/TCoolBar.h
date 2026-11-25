#pragma once

#include "core/xframe_core.h"

//region TCoolBar

class TCoolBar

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

    struct TCoolBand

    {

        std::string caption;

        int height = 26;

    };

    std::vector<TCoolBand> bands_;

    int drag_band_ = -1;       // band being dragged by its gripper

    int drag_grab_dy_ = 0;     // y offset of the grab inside the band

    int band_top(int index)

    {

        int top = y1_ + 2;

        for (int i = 0; i < index && i < (int)bands_.size(); ++i)

        {

            top += bands_[i].height + 2;

        }

        return top;

    }

    int band_at(int y)

    {

        for (size_t i = 0; i < bands_.size(); ++i)

        {

            int top = band_top((int)i);

            if (y >= top && y < top + bands_[i].height)

            {

                return (int)i;

            }

        }

        return -1;

    }

public:

    std::function<void()> on_reorder;

    TCoolBar(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TCoolBar";

        text_ = "TCoolBar";

        add_band("Standard", 26);

        add_band("Formatting", 26);

        add_band("Drawing", 26);

    }

    ~TCoolBar() {}

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

    void add_band(std::string caption, int h)

    {

        TCoolBand band;

        band.caption = caption;

        band.height = h;

        bands_.push_back(band);

    }

    int band_count()

    {

        return (int)bands_.size();

    }

    std::string get_band_caption(int index)

    {

        if (index >= 0 && index < (int)bands_.size())

        {

            return bands_[index].caption;

        }

        return "";

    }

    int get_drag_band()

    {

        return drag_band_;

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

        unsigned long dot_dark = styles_->get_color_rgb(140, 140, 140);

        unsigned long dot_light = styles_->get_color_rgb(235, 235, 235);

        XSetForeground(display_, gc_, st.fillColor_);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        for (size_t i = 0; i < bands_.size(); ++i)

        {

            int top = band_top((int)i);

            int bh = bands_[i].height;

            if ((int)i == drag_band_)

            {

                // The dragged band rides a little raised.

                xf::draw_polished(display_, window_, gc_, st, x1_ + 2, top, x2_ - 2, top + bh);

            }

            // Gripper: two dotted vertical ridges at the left.

            for (int col = 0; col < 2; ++col)

            {

                int gx = x1_ + 5 + col * 4;

                for (int gy = top + 4; gy < top + bh - 3; gy += 4)

                {

                    XSetForeground(display_, gc_, dot_dark);

                    XFillRectangle(display_, window_, gc_, gx, gy, 2, 2);

                    XSetForeground(display_, gc_, dot_light);

                    XFillRectangle(display_, window_, gc_, gx + 1, gy + 1, 1, 1);

                }

            }

            xf::draw_text_left(display_, window_, gc_, ts, x1_ + 16, top, top + bh, bands_[i].caption);

            // Separator under the band.

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawLine(display_, window_, gc_, x1_, top + bh + 1, x2_, top + bh + 1);

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

        // Only the gripper zone starts a band drag.

        if (x <= x1_ + 13)

        {

            int band = band_at(y);

            if (band >= 0)

            {

                drag_band_ = band;

                drag_grab_dy_ = y - band_top(band);

                return true;

            }

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1 || drag_band_ < 0)

        {

            return false;

        }

        drag_band_ = -1;

        if (on_reorder)

        {

            on_reorder();

        }

        return true;

    }

    bool on_motion(int x, int y)

    {

        (void)x;

        if (drag_band_ < 0)

        {

            return false;

        }

        // Work out which slot the grabbed band's centre is over,
        // and move the band there.

        int centre = y - drag_grab_dy_ + bands_[drag_band_].height / 2;

        int target = band_at(centre);

        if (target < 0)

        {

            target = (centre < y1_) ? 0 : (int)bands_.size() - 1;

        }

        if (target != drag_band_)

        {

            TCoolBand moving = bands_[drag_band_];

            bands_.erase(bands_.begin() + drag_band_);

            bands_.insert(bands_.begin() + target, moving);

            drag_band_ = target;

        }

        return true;

    }

    bool on_key(KeySym key, const char* str)

    {

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
