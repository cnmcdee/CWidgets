#pragma once

#include "core/xframe_core.h"

//region TOpenPictureDialog

class TOpenPictureDialog

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

    // Lazarus modal results

    static const int mrNone = 0;

    static const int mrOk = 1;

    static const int mrCancel = 2;

    std::vector<std::string> files_;

    std::vector<std::string> sizes_;   // fake pixel dimensions per file

    int selected_ = 0;

    int top_index_ = 0;

    std::string directory_;

    std::string filter_;

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

    }

    int list_x2()

    {

        return x1_ + (width() - 24) * 52 / 100;

    }

    void draw_chrome()

    {

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, 22);

        SStyle tst = st;

        tst.text_color_ = styles_->get_color_rgb(255, 255, 255);

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 8, y1_ + 2, y1_ + 24, text_);

        XSetForeground(display_, gc_, styles_->get_color_rgb(200, 60, 50));

        XFillRectangle(display_, window_, gc_, x2_ - 24, y1_ + 4, 18, 18);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22, "X");

    }

    void draw_mini_button(int bx1, int by1, int bx2, int by2, const std::string& cap, bool down)

    {

        SStyle st = style();

        if (down)

        {

            unsigned long swap = st.fillColor_;

            st.fillColor_ = st.outlineColor_;

            st.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, st, bx1, by1, bx2, by2);

        xf::draw_text_centered(display_, window_, gc_, st, bx1, by1, bx2, by2, cap);

    }

    // A little landscape glyph drawn from primitives, varied per file
    // so changing the selection visibly changes the preview.

    void draw_preview_glyph(int px1, int py1, int px2, int py2, int seed)

    {

        int w = px2 - px1;

        int h = py2 - py1;

        // Sky

        XSetForeground(display_, gc_, styles_->get_color_rgb(150 + (seed * 13) % 60, 195, 235));

        XFillRectangle(display_, window_, gc_, px1, py1, w, h * 62 / 100);

        // Sun

        int sun_d = std::max(10, w / 7);

        int sun_x = px1 + w / 6 + (seed % 4) * (w / 6);

        XSetForeground(display_, gc_, styles_->get_color_rgb(250, 215, 60));

        XFillArc(display_, window_, gc_, sun_x, py1 + h / 8, sun_d, sun_d, 0, 360 * 64);

        // Far mountain

        int base = py1 + h * 62 / 100;

        XSetForeground(display_, gc_, styles_->get_color_rgb(110, 130, 160));

        for (int r = 0; r < h / 3; ++r)

        {

            int half = (r * w / 3) / std::max(1, h / 3);

            int cx = px1 + w / 3;

            XDrawLine(display_, window_, gc_, cx - half, base - h / 3 + r + h / 3, cx + half, base - h / 3 + r + h / 3);

        }

        // Ground

        XSetForeground(display_, gc_, styles_->get_color_rgb(90 + (seed * 7) % 40, 165, 90));

        XFillRectangle(display_, window_, gc_, px1, base, w, py2 - base);

        // Near hill triangle

        XSetForeground(display_, gc_, styles_->get_color_rgb(60, 130, 70));

        int hx = px1 + w * 2 / 3;

        int hh = h / 4;

        for (int r = 0; r < hh; ++r)

        {

            int half = (r * w / 4) / std::max(1, hh);

            XDrawLine(display_, window_, gc_, hx - half, base + hh - r, hx + half, base + hh - r);

        }

    }

public:

    std::function<void()> on_click;

    TOpenPictureDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TOpenPictureDialog";

        text_ = "Open Picture";

        directory_ = "/home/chris/Pictures";

        filter_ = "Picture files (*.png;*.jpg;*.bmp;*.gif;*.ico)";

        files_ = {"sunset.png", "mountain.jpg", "river.bmp", "forest.gif", "city.png", "beach.jpg", "desert.png", "lake.jpg", "harbour.png"};

        sizes_ = {"1920 x 1080", "1600 x 1200", "800 x 600", "640 x 480", "2048 x 1365", "1280 x 853", "1024 x 768", "1440 x 900", "900 x 600"};

        selected_ = 0;

    }

    ~TOpenPictureDialog() {}

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

    std::string get_filename()

    {

        if (selected_ < 0 || selected_ >= (int)files_.size())

        {

            return directory_;

        }

        return directory_ + "/" + files_[(size_t)selected_];

    }

    std::string get_filter()

    {

        return filter_;

    }

    int get_result()

    {

        return result_;

    }

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        draw_chrome();

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 28, y1_ + 50, "Look in:  " + directory_);

        //region File list (left)

        int lx1 = x1_ + 12;

        int ly1 = y1_ + 54;

        int lx2 = list_x2();

        int ly2 = y2_ - 76;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, lx1, ly1, lx2 - lx1, ly2 - ly1);

        int rows = (ly2 - ly1) / 18;

        for (int i = top_index_; i < (int)files_.size() && i < top_index_ + rows; ++i)

        {

            int ry = ly1 + (i - top_index_) * 18;

            SStyle rst = st;

            if (i == selected_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, lx1 + 1, ry, lx2 - lx1 - 2, 18);

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            // tiny picture glyph beside each name

            XSetForeground(display_, gc_, styles_->get_color_rgb(150, 195, 235));

            XFillRectangle(display_, window_, gc_, lx1 + 4, ry + 4, 12, 10);

            XSetForeground(display_, gc_, styles_->get_color_rgb(90, 165, 90));

            XFillRectangle(display_, window_, gc_, lx1 + 4, ry + 10, 12, 4);

            xf::draw_text_left(display_, window_, gc_, rst, lx1 + 20, ry, ry + 18, files_[(size_t)i]);

        }

        //endregion

        //region Preview (right)

        int px1 = lx2 + 10;

        int py1 = ly1;

        int px2 = x2_ - 12;

        int py2 = ly2 - 22;

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, px1, py1, px2 - px1, py2 - py1);

        draw_preview_glyph(px1 + 3, py1 + 3, px2 - 3, py2 - 3, selected_);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, px1, py1, px2 - px1, py2 - py1);

        std::string cap = files_[(size_t)selected_] + "   " + sizes_[(size_t)selected_];

        xf::draw_text_centered(display_, window_, gc_, st, px1, py2 + 2, px2, ly2, cap);

        //endregion

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y2_ - 68, y2_ - 46, "File name:");

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, x1_ + 96, y2_ - 68, x2_ - 108, 22);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_ + 96, y2_ - 68, x2_ - 108, 22);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 101, y2_ - 68, y2_ - 46, files_[(size_t)selected_]);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y2_ - 40, y2_ - 18, filter_);

        draw_mini_button(x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10, "Open", false);

        draw_mini_button(x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10, "Cancel", false);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 1 && in_rect(x, y, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22))

        {

            result_ = mrCancel;

            return true;

        }

        int lx1 = x1_ + 12;

        int ly1 = y1_ + 54;

        int lx2 = list_x2();

        int ly2 = y2_ - 76;

        if (in_rect(x, y, lx1, ly1, lx2, ly2))

        {

            int rows = (ly2 - ly1) / 18;

            if (button == 4)

            {

                if (top_index_ > 0)

                {

                    top_index_--;

                }

                return true;

            }

            if (button == 5)

            {

                if (top_index_ + rows < (int)files_.size())

                {

                    top_index_++;

                }

                return true;

            }

            if (button == 1)

            {

                int idx = top_index_ + (y - ly1) / 18;

                if (idx >= 0 && idx < (int)files_.size())

                {

                    selected_ = idx;

                }

                return true;

            }

            return false;

        }

        if (button != 1)

        {

            return false;

        }

        if (in_rect(x, y, x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10))

        {

            result_ = mrOk;

            return true;

        }

        if (in_rect(x, y, x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10))

        {

            result_ = mrCancel;

            return true;

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

        if (key == XK_Up)

        {

            if (selected_ > 0)

            {

                selected_--;

                if (selected_ < top_index_)

                {

                    top_index_ = selected_;

                }

            }

            return true;

        }

        if (key == XK_Down)

        {

            if (selected_ < (int)files_.size() - 1)

            {

                selected_++;

            }

            return true;

        }

        if (key == XK_Return)

        {

            result_ = mrOk;

            return true;

        }

        if (key == XK_Escape)

        {

            result_ = mrCancel;

            return true;

        }

        return false;

    }

};

//endregion
