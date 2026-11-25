#pragma once

#include "core/xframe_core.h"

//region GtkAboutDialog

class GtkAboutDialog

{

public:

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_CANCEL = -6;

    static const int RESPONSE_CLOSE = -7;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the program name

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::string version_;

    std::string copyright_;

    std::string comments_;

    std::string license_;

    std::string website_;

    std::vector<std::string> authors_;

    std::vector<std::string> artists_;

    bool show_credits_ = false;   // false: about page, true: credits page

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;     // 0 credits/back, 1 close, -1 none

    int title_bar_height()

    {

        return 26;

    }

    int action_bar_height()

    {

        return 40;

    }

    // Bottom bar: Credits (or Back) on the left, Close on the right.

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        by2 = y2_ - 7;

        by1 = by2 - 26;

        if (which == 0)

        {

            bx1 = x1_ + 10;

            bx2 = bx1 + 88;

        }

        else

        {

            bx2 = x2_ - 10;

            bx1 = bx2 - 88;

        }

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void(int)> on_response;

    GtkAboutDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkAboutDialog";

        text_ = "XFrame";

        version_ = "1.0.0";

        copyright_ = "Copyright (c) 2026 Chris McDermand";

        comments_ = "A hand-rolled X11 widget framework, written one primitive at a time.";

        license_ = "Licensed under the MIT License.";

        website_ = "hotconfig.com";

        authors_ = { "Chris McDermand <thinkmelt@protonmail.com>" };

        artists_ = { "Chris McDermand" };

    }

    ~GtkAboutDialog() {}

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

    //region GtkAboutDialog specific API

    void set_program_name(std::string n)

    {

        text_ = n;

    }

    std::string get_program_name()

    {

        return text_;

    }

    void set_version(std::string v)

    {

        version_ = v;

    }

    std::string get_version()

    {

        return version_;

    }

    void set_copyright(std::string c)

    {

        copyright_ = c;

    }

    std::string get_copyright()

    {

        return copyright_;

    }

    void set_comments(std::string c)

    {

        comments_ = c;

    }

    std::string get_comments()

    {

        return comments_;

    }

    void set_license(std::string l)

    {

        license_ = l;

    }

    std::string get_license()

    {

        return license_;

    }

    void set_website(std::string w)

    {

        website_ = w;

    }

    std::string get_website()

    {

        return website_;

    }

    void set_authors(std::vector<std::string> a)

    {

        authors_ = a;

    }

    void add_author(std::string a)

    {

        authors_.push_back(a);

    }

    void set_artists(std::vector<std::string> a)

    {

        artists_ = a;

    }

    void add_artist(std::string a)

    {

        artists_.push_back(a);

    }

    int get_response()

    {

        return response_;

    }

    void respond(int response_id)

    {

        response_ = response_id;

        if (on_response)

        {

            on_response(response_id);

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

        int tb = title_bar_height();

        // Title bar.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, tb - 2);

        SStyle tst = st;

        tst.text_color_ = st.fillColor_;

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, "About " + text_);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        int cx = x1_ + width() / 2;

        int top = y1_ + tb + 12;

        if (!show_credits_)

        {

            // Logo placeholder: a polished square with the
            // program's initial letter in it.

            xf::draw_polished(display_, window_, gc_, st, cx - 24, top, cx + 24, top + 48);

            std::string initial = text_.empty() ? "?" : text_.substr(0, 1);

            xf::draw_text_centered(display_, window_, gc_, st, cx - 24, top, cx + 24, top + 48, initial);

            int cy = top + 48;

            // Program name, then "Version x.y" under it.

            xf::draw_text_centered(display_, window_, gc_, st, x1_, cy, x2_, cy + 22, text_);

            cy += 22;

            xf::draw_text_centered(display_, window_, gc_, st, x1_, cy, x2_, cy + 18, "Version " + version_);

            cy += 26;

            xf::draw_text_centered(display_, window_, gc_, st, x1_ + 16, cy, x2_ - 16, cy + 18, comments_);

            cy += 24;

            xf::draw_text_centered(display_, window_, gc_, st, x1_ + 16, cy, x2_ - 16, cy + 18, copyright_);

            cy += 20;

            xf::draw_text_centered(display_, window_, gc_, st, x1_ + 16, cy, x2_ - 16, cy + 18, license_);

            cy += 20;

            SStyle wst = st;

            wst.text_color_ = styles_->get_color_rgb(40, 110, 200);

            xf::draw_text_centered(display_, window_, gc_, wst, x1_ + 16, cy, x2_ - 16, cy + 18, website_);

        }

        else

        {

            // Credits page: Written by / Artwork by sections.

            int cy = top;

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 16, cy, cy + 18, "Written by:");

            cy += 20;

            for (size_t i = 0; i < authors_.size(); ++i)

            {

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 28, cy, cy + 16, authors_[i]);

                cy += 17;

            }

            cy += 10;

            xf::draw_text_left(display_, window_, gc_, st, x1_ + 16, cy, cy + 18, "Artwork by:");

            cy += 20;

            for (size_t i = 0; i < artists_.size(); ++i)

            {

                xf::draw_text_left(display_, window_, gc_, st, x1_ + 28, cy, cy + 16, artists_[i]);

                cy += 17;

            }

        }

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        const char* labels[2] = { show_credits_ ? "Back" : "Credits", "Close" };

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            SStyle bst = st;

            if (pressed_button_ == i)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bst, bx1, by1, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, bst, bx1, by1, bx2, by2, labels[i]);

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

        if (in_rect(x, y, x2_ - 28, y1_, x2_ - 4, y1_ + title_bar_height()))

        {

            respond(RESPONSE_DELETE_EVENT);

            return true;

        }

        for (int i = 0; i < 2; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            if (in_rect(x, y, bx1, by1, bx2, by2))

            {

                pressed_button_ = i;

                return true;

            }

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        if (button != 1 || pressed_button_ < 0)

        {

            return false;

        }

        int index = pressed_button_;

        pressed_button_ = -1;

        int bx1, by1, bx2, by2;

        button_rect(index, bx1, by1, bx2, by2);

        if (in_rect(x, y, bx1, by1, bx2, by2))

        {

            if (index == 0)

            {

                show_credits_ = !show_credits_;

            }

            else

            {

                respond(RESPONSE_CLOSE);

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

        if (key == XK_Escape)

        {

            respond(RESPONSE_CANCEL);

            return true;

        }

        return false;

    }

};

//endregion
