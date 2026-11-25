#pragma once

#include "core/xframe_core.h"

//region GtkFileChooserButton

class GtkFileChooserButton

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

    bool pressed_ = false;

    std::string filename_ = "notes.txt";

    std::string title_ = "Select a File";

public:

    std::function<void()> on_click;

    std::function<void(std::string)> on_file_set;

    GtkFileChooserButton(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFileChooserButton";

        text_ = filename_;

    }

    ~GtkFileChooserButton() {}

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

        set_filename(t);

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region File state

    void set_filename(std::string filename)

    {

        filename_ = filename;

        text_ = filename_;

    }

    std::string get_filename()

    {

        return filename_;

    }

    void set_title(std::string title)

    {

        title_ = title;

    }

    std::string get_title()

    {

        return title_;

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

        if (pressed_)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        // The file name reads from the left; a separated zone at the
        // right carries the folder glyph.

        int zone = 30;

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 8, y1_, y2_, text_);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawLine(display_, window_, gc_, x2_ - zone, y1_ + 4, x2_ - zone, y2_ - 4);

        //region Folder glyph

        int cx = x2_ - zone / 2;

        int cy = y1_ + height() / 2;

        unsigned long paper = styles_->get_color_rgb(245, 205, 100);

        XSetForeground(display_, gc_, paper);

        XFillRectangle(display_, window_, gc_, cx - 8, cy - 4, 16, 11);

        XFillRectangle(display_, window_, gc_, cx - 8, cy - 7, 7, 3);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, cx - 8, cy - 4, 16, 11);

        //endregion

        if (focused_)

        {

            XSetForeground(display_, gc_, st.lineColor_);

            XDrawRectangle(display_, window_, gc_, x1_ + 3, y1_ + 3, width() - 6, height() - 6);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        pressed_ = true;

        focused_ = true;

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (!pressed_ || button != 1)

        {

            return false;

        }

        pressed_ = false;

        if (contains(x, y))

        {

            // The real button opens a file chooser titled title_;
            // here the click simply reports the current file.

            if (on_file_set)

            {

                on_file_set(filename_);

            }

            if (on_click)

            {

                on_click();

            }

        }

        return true;

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

        if (key == XK_Return || key == XK_space)

        {

            if (on_file_set)

            {

                on_file_set(filename_);

            }

            if (on_click)

            {

                on_click();

            }

            return true;

        }

        return false;

    }

};

//endregion
