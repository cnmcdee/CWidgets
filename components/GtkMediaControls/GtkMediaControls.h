#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkMediaControls

// The control strip for a media player: play/pause button on the
// left, a seek bar in the middle, elapsed / total time on the
// right. Clicking or dragging in the seek bar seeks.

class GtkMediaControls

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

    bool playing_ = false;

    bool dragging_ = false;

    double position_sec_ = 42.0;

    double duration_sec_ = 195.0;

    std::string media_label_ = "song.mp3";

public:

    std::function<void(double)> on_seek;   // new position, seconds

    std::function<void()> on_play;

    std::function<void()> on_pause;

    GtkMediaControls(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkMediaControls";

        text_ = "GtkMediaControls";

    }

    ~GtkMediaControls() {}

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

    //region GtkMediaControls API

    void play()

    {

        if (!playing_)

        {

            playing_ = true;

            if (on_play)

            {

                on_play();

            }

        }

    }

    void pause()

    {

        if (playing_)

        {

            playing_ = false;

            if (on_pause)

            {

                on_pause();

            }

        }

    }

    bool get_playing()

    {

        return playing_;

    }

    void set_position(double sec)

    {

        position_sec_ = std::max(0.0, std::min(duration_sec_, sec));

    }

    double get_position()

    {

        return position_sec_;

    }

    void set_duration(double sec)

    {

        duration_sec_ = std::max(1.0, sec);

        set_position(position_sec_);

    }

    double get_duration()

    {

        return duration_sec_;

    }

    void set_media_label(std::string s)

    {

        media_label_ = s;

    }

    std::string get_media_label()

    {

        return media_label_;

    }

    bool step(double dt_sec)

    {

        if (!playing_)

        {

            return true;

        }

        position_sec_ += dt_sec;

        if (position_sec_ >= duration_sec_)

        {

            position_sec_ = duration_sec_;

            playing_ = false;

            return false;

        }

        return true;

    }

    std::string format_time(double sec)

    {

        int total = (int)sec;

        char buf[32];

        std::snprintf(buf, sizeof(buf), "%d:%02d", total / 60, total % 60);

        return std::string(buf);

    }

    //endregion

    //region Geometry

    void button_rect(int& rx, int& ry, int& rw, int& rh)

    {

        rw = 30;

        rh = height() - 8;

        rx = x1_ + 4;

        ry = y1_ + 4;

    }

    void seek_rect(int& rx, int& ry, int& rw, int& rh)

    {

        rx = x1_ + 44;

        rw = std::max(10, (x2_ - 92) - rx);

        ry = (y1_ + y2_) / 2 - 3;

        rh = 6;

    }

    double seek_fraction_at(int x)

    {

        int rx, ry, rw, rh;

        seek_rect(rx, ry, rw, rh);

        return std::max(0.0, std::min(1.0, (double)(x - rx) / rw));

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int bx, by, bw, bh;

        button_rect(bx, by, bw, bh);

        SStyle btn = st;

        xf::draw_polished(display_, window_, gc_, btn, bx, by, bx + bw, by + bh);

        int cx = bx + bw / 2;

        int cy = by + bh / 2;

        if (playing_)

        {

            XSetForeground(display_, gc_, st.text_color_);

            XFillRectangle(display_, window_, gc_, cx - 6, cy - 6, 4, 12);

            XFillRectangle(display_, window_, gc_, cx + 2, cy - 6, 4, 12);

        }

        else

        {

            xf::draw_triangle_right(display_, window_, gc_, st.text_color_, cx - 4, cy, 7);

        }

        // Seek bar: track, fill, knob.

        int sx, sy, sw, sh;

        seek_rect(sx, sy, sw, sh);

        XSetForeground(display_, gc_, styles_->get_color_rgb(150, 150, 150));

        XFillRectangle(display_, window_, gc_, sx, sy, sw, sh);

        double frac = duration_sec_ > 0 ? position_sec_ / duration_sec_ : 0.0;

        XSetForeground(display_, gc_, styles_->get_color_rgb(52, 101, 164));

        XFillRectangle(display_, window_, gc_, sx, sy, (int)(sw * frac), sh);

        int knob_x = sx + (int)(sw * frac);

        XSetForeground(display_, gc_, styles_->get_color_rgb(240, 240, 240));

        XFillArc(display_, window_, gc_, knob_x - 5, cy - 5, 10, 10, 0, 360 * 64);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawArc(display_, window_, gc_, knob_x - 5, cy - 5, 10, 10, 0, 360 * 64);

        // Time labels at the right.

        std::string times = format_time(position_sec_) + " / " + format_time(duration_sec_);

        SStyle lab = st;

        if (!enabled_)

        {

            lab.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        int tw = xf::text_width(lab, times);

        xf::draw_text_at(display_, window_, gc_, lab, x2_ - 4 - tw, y1_ + (height() + (st.font_ ? st.font_->ascent - st.font_->descent : 10)) / 2, times);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        int bx, by, bw, bh;

        button_rect(bx, by, bw, bh);

        if (x >= bx && x <= bx + bw && y >= by && y <= by + bh)

        {

            if (playing_)

            {

                pause();

            }

            else

            {

                play();

            }

            return true;

        }

        int sx, sy, sw, sh;

        seek_rect(sx, sy, sw, sh);

        if (x >= sx - 2 && x <= sx + sw + 2 && y >= y1_ && y <= y2_)

        {

            dragging_ = true;

            position_sec_ = seek_fraction_at(x) * duration_sec_;

            if (on_seek)

            {

                on_seek(position_sec_);

            }

            return true;

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button == 1 && dragging_)

        {

            dragging_ = false;

            return true;

        }

        return false;

    }

    bool on_motion(int x, int y)

    {

        if (dragging_)

        {

            position_sec_ = seek_fraction_at(x) * duration_sec_;

            if (on_seek)

            {

                on_seek(position_sec_);

            }

            return true;

        }

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

        if (key == XK_space)

        {

            if (playing_)

            {

                pause();

            }

            else

            {

                play();

            }

            return true;

        }

        if (key == XK_Right)

        {

            set_position(position_sec_ + 5.0);

            return true;

        }

        if (key == XK_Left)

        {

            set_position(position_sec_ - 5.0);

            return true;

        }

        return false;

    }

};

//endregion
