#pragma once

#include "core/xframe_core.h"

//region TImageList

class TImageList

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

    // Non-visual component: it stores the size every image in the
    // list shares, plus the name of each image slot. Other widgets
    // refer to images by their index in this list.

    int image_width_ = 32;

    int image_height_ = 32;

    std::vector<std::string> names_;

public:

    TImageList(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TImageList";

        text_ = "TImageList";

        add("imgNew");

        add("imgOpen");

        add("imgSave");

        add("imgClose");

    }

    ~TImageList() {}

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

    // Add an image slot by name; returns its index.

    int add(std::string image_name)

    {

        names_.push_back(image_name);

        return (int)names_.size() - 1;

    }

    void remove(int index)

    {

        if (index >= 0 && index < (int)names_.size())

        {

            names_.erase(names_.begin() + index);

        }

    }

    void clear()

    {

        names_.clear();

    }

    int count()

    {

        return (int)names_.size();

    }

    std::string get_name(int index)

    {

        if (index >= 0 && index < (int)names_.size())

        {

            return names_[index];

        }

        return "";

    }

    int index_of(std::string image_name)

    {

        for (size_t i = 0; i < names_.size(); ++i)

        {

            if (names_[i] == image_name)

            {

                return (int)i;

            }

        }

        return -1;

    }

    void set_image_size(int w, int h)

    {

        image_width_ = w;

        image_height_ = h;

    }

    int get_image_width()

    {

        return image_width_;

    }

    int get_image_height()

    {

        return image_height_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        // Design-time strip: one outlined box per image slot, with
        // its index inside and its name underneath, plus a summary.

        int box = 28;

        int cx = x1_ + 2;

        int cy = y1_ + 2;

        for (size_t i = 0; i < names_.size(); ++i)

        {

            if (cx + box > x2_ - 2)

            {

                break;

            }

            xf::draw_polished(display_, window_, gc_, st, cx, cy, cx + box, cy + box);

            // A tiny mountain-and-sun glyph stands in for the image.

            unsigned long glyph = styles_->get_color_rgb(90, 140, 90);

            XSetForeground(display_, gc_, glyph);

            XDrawLine(display_, window_, gc_, cx + 5, cy + box - 6, cx + 11, cy + 12);

            XDrawLine(display_, window_, gc_, cx + 11, cy + 12, cx + 16, cy + box - 6);

            XDrawLine(display_, window_, gc_, cx + 16, cy + box - 6, cx + box - 5, cy + box - 6);

            XFillArc(display_, window_, gc_, cx + box - 11, cy + 5, 5, 5, 0, 360 * 64);

            xf::draw_text_centered(display_, window_, gc_, st, cx, cy + box + 1, cx + box, cy + box + 13, std::to_string(i));

            cx += box + 6;

        }

        std::string summary = std::to_string(count()) + " images, " + std::to_string(image_width_) + "x" + std::to_string(image_height_);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 2, y2_ - 16, y2_ - 2, summary);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || button != 1)

        {

            return false;

        }

        focused_ = true;

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

        (void)key;

        (void)str;

        return false;

    }

};

//endregion
