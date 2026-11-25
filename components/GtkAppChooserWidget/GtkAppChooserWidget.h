#pragma once

#include "core/xframe_core.h"

#include <ctime>

//region GtkAppChooserWidget

class GtkAppChooserWidget

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

    struct App

    {

        std::string name;

        std::string glyph;

        std::string description;

    };

    std::vector<App> apps_;

    int selected_ = 0;

    int scroll_row_ = 0;

    int row_height_ = 34;

    long last_click_ms_ = 0;

    int last_click_row_ = -1;

    long now_ms()

    {

        return (long)((double)std::clock() * 1000.0 / (double)CLOCKS_PER_SEC);

    }

    int visible_rows()

    {

        return std::max(1, (height() - 4) / row_height_);

    }

    int row_at(int y)

    {

        if (y < y1_ + 2)

        {

            return -1;

        }

        int index = scroll_row_ + (y - y1_ - 2) / row_height_;

        if (index < 0 || index >= (int)apps_.size())

        {

            return -1;

        }

        return index;

    }

public:

    std::function<void()> on_app_selected;

    std::function<void()> on_app_activated;

    GtkAppChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkAppChooserWidget";

        text_ = "App Chooser";

        add_app("Firefox Web Browser", "F", "Browse the web");

        add_app("Files", "Fi", "Browse your files");

        add_app("Terminal", "T", "Command line shell");

        add_app("Text Editor", "Te", "Edit plain text files");

        add_app("Calculator", "C", "Do sums");

        add_app("Image Viewer", "I", "Look at pictures");

        add_app("Music Player", "M", "Play your tunes");

        add_app("Software Centre", "S", "Install applications");

    }

    ~GtkAppChooserWidget() {}

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

    //region App chooser specific API

    void add_app(std::string app_name, std::string glyph, std::string description)

    {

        App app;

        app.name = app_name;

        app.glyph = glyph;

        app.description = description;

        apps_.push_back(app);

    }

    int app_count()

    {

        return (int)apps_.size();

    }

    int get_selected()

    {

        return selected_;

    }

    std::string get_app_name()

    {

        if (selected_ >= 0 && selected_ < (int)apps_.size())

        {

            return apps_[selected_].name;

        }

        return "";

    }

    void select_app(std::string app_name)

    {

        for (size_t i = 0; i < apps_.size(); ++i)

        {

            if (apps_[i].name == app_name)

            {

                selected_ = (int)i;

                return;

            }

        }

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

        unsigned long sel_fill = styles_->get_color_rgb(70, 110, 180);

        unsigned long icon_fill = styles_->get_color_rgb(120, 160, 210);

        for (int i = scroll_row_; i < (int)apps_.size(); ++i)

        {

            int ry = y1_ + 2 + (i - scroll_row_) * row_height_;

            if (ry + row_height_ > y2_ - 2)

            {

                break;

            }

            SStyle row_style = st;

            if (i == selected_)

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, x1_ + 2, ry, width() - 4, row_height_);

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            // Icon box with the app glyph.

            int ix = x1_ + 8;

            int iy = ry + (row_height_ - 24) / 2;

            SStyle icon_style = st;

            icon_style.fillColor_ = icon_fill;

            icon_style.radius_ = 4;

            xf::fill_rounded_rect(display_, window_, gc_, icon_style, ix, iy, ix + 24, iy + 24);

            xf::draw_rounded_outline(display_, window_, gc_, icon_style, ix, iy, ix + 24, iy + 24);

            SStyle glyph_style = st;

            glyph_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            xf::draw_text_centered(display_, window_, gc_, glyph_style, ix, iy, ix + 24, iy + 24, apps_[i].glyph);

            xf::draw_text_left(display_, window_, gc_, row_style, x1_ + 42, ry, ry + row_height_ / 2 + 3, apps_[i].name);

            SStyle desc_style = row_style;

            if (i != selected_)

            {

                desc_style.text_color_ = styles_->get_color_rgb(120, 120, 120);

            }

            xf::draw_text_left(display_, window_, gc_, desc_style, x1_ + 42, ry + row_height_ / 2, ry + row_height_, apps_[i].description);

        }

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 4)

        {

            scroll_row_ = std::max(0, scroll_row_ - 2);

            return true;

        }

        if (button == 5)

        {

            scroll_row_ += 2;

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        int row = row_at(y);

        if (row < 0)

        {

            return true;

        }

        long now = now_ms();

        bool double_click = (row == last_click_row_ && now - last_click_ms_ < 400);

        last_click_row_ = row;

        last_click_ms_ = now;

        if (row != selected_)

        {

            selected_ = row;

            if (on_app_selected)

            {

                on_app_selected();

            }

        }

        if (double_click && on_app_activated)

        {

            on_app_activated();

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

        if (!focused_ || !enabled_ || apps_.empty())

        {

            return false;

        }

        if (key == XK_Down && selected_ < (int)apps_.size() - 1)

        {

            selected_++;

        }

        else if (key == XK_Up && selected_ > 0)

        {

            selected_--;

        }

        else if (key == XK_Return)

        {

            if (on_app_activated)

            {

                on_app_activated();

            }

            return true;

        }

        else

        {

            return false;

        }

        if (selected_ < scroll_row_)

        {

            scroll_row_ = selected_;

        }

        if (selected_ >= scroll_row_ + visible_rows())

        {

            scroll_row_ = selected_ - visible_rows() + 1;

        }

        if (on_app_selected)

        {

            on_app_selected();

        }

        return true;

    }

};

//endregion
