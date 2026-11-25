#pragma once

#include "core/xframe_core.h"

//region GtkFontChooserWidget

class GtkFontChooserWidget

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

    std::vector<std::string> families_;

    std::vector<std::string> face_styles_;

    std::vector<std::string> sizes_;

    int family_sel_ = 0;

    int style_sel_ = 0;

    int size_sel_ = 4;

    int active_col_ = 0;

    int row_height_ = 18;

    int header_height_ = 20;

    int list_top()

    {

        return y1_ + header_height_ + 4;

    }

    int list_bottom()

    {

        return y2_ - 48;

    }

    // Column geometry: family 42%, style 28%, size the rest.

    int col_x(int col)

    {

        int w = width() - 8;

        if (col == 0)

        {

            return x1_ + 4;

        }

        if (col == 1)

        {

            return x1_ + 4 + (w * 42) / 100;

        }

        return x1_ + 4 + (w * 70) / 100;

    }

    int col_w(int col)

    {

        int w = width() - 8;

        if (col == 0)

        {

            return (w * 42) / 100 - 4;

        }

        if (col == 1)

        {

            return (w * 28) / 100 - 4;

        }

        return w - (w * 70) / 100 - 4;

    }

    const std::vector<std::string>& list_for(int col)

    {

        if (col == 0)

        {

            return families_;

        }

        if (col == 1)

        {

            return face_styles_;

        }

        return sizes_;

    }

    int& sel_for(int col)

    {

        if (col == 0)

        {

            return family_sel_;

        }

        if (col == 1)

        {

            return style_sel_;

        }

        return size_sel_;

    }

    void draw_list(int col)

    {

        SStyle st = style();

        int lx = col_x(col);

        int lw = col_w(col);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx, list_top(), lw, list_bottom() - list_top());

        unsigned long sel_fill = styles_->get_color_rgb(70, 110, 180);

        const std::vector<std::string>& list = list_for(col);

        int sel = sel_for(col);

        for (int i = 0; i < (int)list.size(); ++i)

        {

            int ry = list_top() + 1 + i * row_height_;

            if (ry + row_height_ > list_bottom())

            {

                break;

            }

            SStyle row_style = st;

            if (i == sel)

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, lx + 1, ry, lw - 2, row_height_);

                row_style.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_left(display_, window_, gc_, row_style, lx + 6, ry, ry + row_height_, list[i]);

        }

    }

public:

    std::function<void()> on_font_changed;

    GtkFontChooserWidget(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkFontChooserWidget";

        text_ = "Font Chooser";

        families_ = {"Sans", "Serif", "Monospace", "DejaVu Sans", "DejaVu Serif", "Liberation Sans", "Liberation Mono", "Courier"};

        face_styles_ = {"Regular", "Bold", "Italic", "Bold Italic", "Light", "Book"};

        sizes_ = {"6", "8", "9", "10", "11", "12", "14", "16", "18", "24", "36", "48"};

    }

    ~GtkFontChooserWidget() {}

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

    //region Font chooser specific API

    std::string get_font_name()

    {

        return families_[family_sel_] + " " + face_styles_[style_sel_] + " " + sizes_[size_sel_];

    }

    std::string get_family()

    {

        return families_[family_sel_];

    }

    std::string get_face_style()

    {

        return face_styles_[style_sel_];

    }

    int get_font_size()

    {

        return std::atoi(sizes_[size_sel_].c_str());

    }

    void set_family(std::string family)

    {

        for (size_t i = 0; i < families_.size(); ++i)

        {

            if (families_[i] == family)

            {

                family_sel_ = (int)i;

                return;

            }

        }

    }

    void set_font_size(int size)

    {

        std::string wanted = std::to_string(size);

        for (size_t i = 0; i < sizes_.size(); ++i)

        {

            if (sizes_[i] == wanted)

            {

                size_sel_ = (int)i;

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

        // Column headers.

        xf::draw_text_left(display_, window_, gc_, st, col_x(0) + 2, y1_ + 2, y1_ + 2 + header_height_, "Font");

        xf::draw_text_left(display_, window_, gc_, st, col_x(1) + 2, y1_ + 2, y1_ + 2 + header_height_, "Style");

        xf::draw_text_left(display_, window_, gc_, st, col_x(2) + 2, y1_ + 2, y1_ + 2 + header_height_, "Size");

        draw_list(0);

        draw_list(1);

        draw_list(2);

        // Preview band.

        int py = list_bottom() + 6;

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_ + 4, py, width() - 8, y2_ - py - 4);

        xf::draw_text_centered(display_, window_, gc_, st, x1_ + 4, py, x2_ - 4, py + 20, "AaBbGg 0123456789");

        SStyle small = st;

        small.text_color_ = styles_->get_color_rgb(110, 110, 110);

        xf::draw_text_centered(display_, window_, gc_, small, x1_ + 4, py + 18, x2_ - 4, y2_ - 4, get_font_name());

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (y < list_top() || y > list_bottom())

        {

            return true;

        }

        for (int col = 0; col < 3; ++col)

        {

            if (x >= col_x(col) && x < col_x(col) + col_w(col))

            {

                int index = (y - list_top() - 1) / row_height_;

                if (index >= 0 && index < (int)list_for(col).size())

                {

                    if (sel_for(col) != index)

                    {

                        sel_for(col) = index;

                        if (on_font_changed)

                        {

                            on_font_changed();

                        }

                    }

                }

                return true;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        // Tab cycles the active list; arrows move its selection.

        if (key == XK_Tab)

        {

            active_col_ = (active_col_ + 1) % 3;

            return true;

        }

        int& sel = sel_for(active_col_);

        int count = (int)list_for(active_col_).size();

        if (key == XK_Down && sel < count - 1)

        {

            sel++;

        }

        else if (key == XK_Up && sel > 0)

        {

            sel--;

        }

        else

        {

            return false;

        }

        if (on_font_changed)

        {

            on_font_changed();

        }

        return true;

    }

};

//endregion
