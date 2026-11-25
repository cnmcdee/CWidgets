#pragma once

#include "core/xframe_core.h"

//region TPrinterSetupDialog

class TPrinterSetupDialog

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

    std::vector<std::string> printers_;

    std::vector<std::string> paper_sizes_;

    int sel_printer_ = 0;

    int sel_paper_ = 0;

    bool props_open_ = false;

    int orientation_ = 0;   // 0 = Portrait, 1 = Landscape

    bool duplex_ = false;

    std::string status_;

    int result_ = 0;

    bool in_rect(int px, int py, int rx1, int ry1, int rx2, int ry2)

    {

        return px >= rx1 && px <= rx2 && py >= ry1 && py <= ry2;

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

    void draw_list(int bx1, int by1, int bx2, int by2, const std::vector<std::string>& items, int sel)

    {

        SStyle st = style();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, bx1, by1, bx2 - bx1, by2 - by1);

        for (size_t i = 0; i < items.size(); ++i)

        {

            int ry = by1 + (int)i * 18;

            if (ry + 18 > by2)

            {

                break;

            }

            SStyle rst = st;

            if ((int)i == sel)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, bx1 + 1, ry, bx2 - bx1 - 2, 18);

                rst.text_color_ = styles_->get_color_rgb(255, 255, 255);

            }

            xf::draw_text_left(display_, window_, gc_, rst, bx1 + 6, ry, ry + 18, items[i]);

        }

    }

    void draw_radio(int bx, int by, bool on, const std::string& label)

    {

        SStyle st = style();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillArc(display_, window_, gc_, bx, by, 12, 12, 0, 360 * 64);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawArc(display_, window_, gc_, bx, by, 12, 12, 0, 360 * 64);

        if (on)

        {

            XSetForeground(display_, gc_, st.text_color_);

            XFillArc(display_, window_, gc_, bx + 4, by + 4, 5, 5, 0, 360 * 64);

        }

        xf::draw_text_left(display_, window_, gc_, st, bx + 17, by - 2, by + 14, label);

    }

    void draw_checkbox(int bx, int by, bool on, const std::string& label)

    {

        SStyle st = style();

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, bx, by, 13, 13);

        XSetForeground(display_, gc_, st.lineColor_);

        xf::set_line_width(display_, gc_, 1);

        XDrawRectangle(display_, window_, gc_, bx, by, 13, 13);

        if (on)

        {

            xf::draw_checkmark(display_, window_, gc_, st.text_color_, bx + 6, by + 6, 9);

        }

        xf::draw_text_left(display_, window_, gc_, st, bx + 18, by - 2, by + 15, label);

    }

public:

    std::function<void()> on_click;

    TPrinterSetupDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TPrinterSetupDialog";

        text_ = "Printer Setup";

        printers_ = {"Office LaserJet Pro", "PDF Printer", "Photo Inkjet 2600", "Network Plotter"};

        paper_sizes_ = {"A4", "Letter", "Legal", "A5", "A3"};

        status_ = "Ready";

    }

    ~TPrinterSetupDialog() {}

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

    std::string get_printer()

    {

        return printers_[(size_t)sel_printer_];

    }

    std::string get_paper_size()

    {

        return paper_sizes_[(size_t)sel_paper_];

    }

    std::string get_orientation()

    {

        return orientation_ == 0 ? "Portrait" : "Landscape";

    }

    bool get_duplex()

    {

        return duplex_;

    }

    bool get_properties_open()

    {

        return props_open_;

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

        int mid = x1_ + (width() - 24) * 58 / 100;

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 12, y1_ + 28, y1_ + 48, "Printers:");

        draw_list(x1_ + 12, y1_ + 50, mid, y1_ + 140, printers_, sel_printer_);

        xf::draw_text_left(display_, window_, gc_, st, mid + 10, y1_ + 28, y1_ + 48, "Paper size:");

        draw_list(mid + 10, y1_ + 50, x2_ - 12, y1_ + 140, paper_sizes_, sel_paper_);

        draw_mini_button(x1_ + 12, y1_ + 150, x1_ + 116, y1_ + 176, "Properties...", props_open_);

        xf::draw_text_left(display_, window_, gc_, st, x1_ + 126, y1_ + 150, y1_ + 176, status_);

        if (props_open_)

        {

            //region Properties panel for the selected printer

            int px1 = x1_ + 12;

            int py1 = y1_ + 188;

            int px2 = x2_ - 12;

            int py2 = y2_ - 48;

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 1);

            XDrawRectangle(display_, window_, gc_, px1, py1, px2 - px1, py2 - py1);

            xf::draw_text_left(display_, window_, gc_, st, px1 + 8, py1 - 16, py1 + 2, "Properties: " + get_printer());

            xf::draw_text_left(display_, window_, gc_, st, px1 + 12, py1 + 10, py1 + 30, "Orientation:");

            draw_radio(px1 + 110, py1 + 12, orientation_ == 0, "Portrait");

            draw_radio(px1 + 210, py1 + 12, orientation_ == 1, "Landscape");

            draw_checkbox(px1 + 12, py1 + 40, duplex_, "Duplex (print on both sides)");

            xf::draw_text_left(display_, window_, gc_, st, px1 + 12, py1 + 62, py1 + 82, "Paper: " + get_paper_size() + "    Quality: Normal (600 dpi)");

            //endregion

        }

        draw_mini_button(x2_ - 188, y2_ - 34, x2_ - 100, y2_ - 10, "Setup", false);

        draw_mini_button(x2_ - 92, y2_ - 34, x2_ - 12, y2_ - 10, "Cancel", false);

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_ || button != 1)

        {

            return false;

        }

        focused_ = true;

        if (in_rect(x, y, x2_ - 24, y1_ + 4, x2_ - 6, y1_ + 22))

        {

            result_ = mrCancel;

            return true;

        }

        int mid = x1_ + (width() - 24) * 58 / 100;

        if (in_rect(x, y, x1_ + 12, y1_ + 50, mid, y1_ + 140))

        {

            int idx = (y - (y1_ + 50)) / 18;

            if (idx >= 0 && idx < (int)printers_.size())

            {

                sel_printer_ = idx;

                status_ = printers_[(size_t)idx] + " selected";

            }

            return true;

        }

        if (in_rect(x, y, mid + 10, y1_ + 50, x2_ - 12, y1_ + 140))

        {

            int idx = (y - (y1_ + 50)) / 18;

            if (idx >= 0 && idx < (int)paper_sizes_.size())

            {

                sel_paper_ = idx;

            }

            return true;

        }

        if (in_rect(x, y, x1_ + 12, y1_ + 150, x1_ + 116, y1_ + 176))

        {

            props_open_ = !props_open_;

            status_ = props_open_ ? ("Properties for " + get_printer()) : "Ready";

            return true;

        }

        if (props_open_)

        {

            int px1 = x1_ + 12;

            int py1 = y1_ + 188;

            if (in_rect(x, y, px1 + 110, py1 + 10, px1 + 200, py1 + 26))

            {

                orientation_ = 0;

                return true;

            }

            if (in_rect(x, y, px1 + 210, py1 + 10, px1 + 320, py1 + 26))

            {

                orientation_ = 1;

                return true;

            }

            if (in_rect(x, y, px1 + 12, py1 + 40, px1 + 280, py1 + 53))

            {

                duplex_ = !duplex_;

                return true;

            }

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

            if (sel_printer_ > 0)

            {

                sel_printer_--;

            }

            return true;

        }

        if (key == XK_Down)

        {

            if (sel_printer_ < (int)printers_.size() - 1)

            {

                sel_printer_++;

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
