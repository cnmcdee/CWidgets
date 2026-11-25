#pragma once

#include "core/xframe_core.h"

#include <cstdio>

//region GtkVideo

// A video surface: dark picture area, a big play glyph in the
// middle while paused, and a progress line along the bottom.
// Clicking toggles play/pause; step() advances playback.

class GtkVideo

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

    std::string filename_;

    bool playing_ = false;

    double position_ = 0.25;   // 0..1 through the stream

    double duration_sec_ = 195.0;

    bool autoplay_ = false;

public:

    std::function<void()> on_play;

    std::function<void()> on_pause;

    GtkVideo(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkVideo";

        text_ = "video.mp4";

        filename_ = "video.mp4";

    }

    ~GtkVideo() {}

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

    //region GtkVideo API

    void set_filename(std::string f)

    {

        filename_ = f;

        text_ = f;

        position_ = 0.0;

        playing_ = autoplay_;

    }

    std::string get_filename()

    {

        return filename_;

    }

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

    void set_autoplay(bool a)

    {

        autoplay_ = a;

    }

    bool get_autoplay()

    {

        return autoplay_;

    }

    void set_position(double p)

    {

        position_ = std::max(0.0, std::min(1.0, p));

    }

    double get_position()

    {

        return position_;

    }

    void set_duration(double sec)

    {

        duration_sec_ = std::max(1.0, sec);

    }

    double get_duration()

    {

        return duration_sec_;

    }

    double get_time_sec()

    {

        return position_ * duration_sec_;

    }

    // Advance playback; call from a timer. Returns false at the end.

    bool step(double dt_sec)

    {

        if (!playing_)

        {

            return true;

        }

        position_ += dt_sec / duration_sec_;

        if (position_ >= 1.0)

        {

            position_ = 1.0;

            playing_ = false;

            return false;

        }

        return true;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Picture area: near black with a faint scene gradient.

        XSetForeground(display_, gc_, styles_->get_color_rgb(16, 16, 20));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XSetForeground(display_, gc_, styles_->get_color_rgb(28, 34, 48));

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height() / 2);

        // Title, top left.

        SStyle cap = st;

        cap.text_color_ = styles_->get_color_rgb(200, 200, 205);

        xf::draw_text_at(display_, window_, gc_, cap, x1_ + 6, y1_ + 16, text_);

        int cx = (x1_ + x2_) / 2;

        int cy = (y1_ + y2_) / 2;

        if (!playing_)

        {

            // Big play triangle in a ring.

            XSetForeground(display_, gc_, styles_->get_color_rgb(220, 220, 220));

            XDrawArc(display_, window_, gc_, cx - 26, cy - 26, 52, 52, 0, 360 * 64);

            xf::draw_triangle_right(display_, window_, gc_, styles_->get_color_rgb(235, 235, 235), cx - 8, cy, 16);

        }

        else

        {

            // Pause bars.

            XSetForeground(display_, gc_, styles_->get_color_rgb(235, 235, 235));

            XFillRectangle(display_, window_, gc_, cx - 12, cy - 14, 8, 28);

            XFillRectangle(display_, window_, gc_, cx + 4, cy - 14, 8, 28);

        }

        // Progress line along the bottom.

        int bar_y = y2_ - 6;

        XSetForeground(display_, gc_, styles_->get_color_rgb(70, 70, 76));

        XFillRectangle(display_, window_, gc_, x1_ + 4, bar_y, width() - 8, 3);

        XSetForeground(display_, gc_, styles_->get_color_rgb(230, 60, 60));

        XFillRectangle(display_, window_, gc_, x1_ + 4, bar_y, (int)((width() - 8) * position_), 3);

        if (focused_)

        {

            XSetForeground(display_, gc_, styles_->get_color_rgb(120, 120, 130));

            XDrawRectangle(display_, window_, gc_, x1_, y1_, width() - 1, height() - 1);

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

        if (playing_)

        {

            pause();

        }

        else

        {

            if (position_ >= 1.0)

            {

                position_ = 0.0;   // replay from the start

            }

            play();

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

            set_position(position_ + 0.05);

            return true;

        }

        if (key == XK_Left)

        {

            set_position(position_ - 0.05);

            return true;

        }

        return false;

    }

};

//endregion
