#pragma once

#include "core/xframe_core.h"

//region GtkPrintUnixDialog

class GtkPrintUnixDialog

{

public:

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_ACCEPT = -3;

    static const int RESPONSE_CANCEL = -6;

    static const int RESPONSE_APPLY = -10;

    // Page set choices (GtkPrintPages style).

    static const int PAGES_ALL = 0;

    static const int PAGES_CURRENT = 1;

    static const int PAGES_RANGES = 2;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the dialog title

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    struct Printer

    {

        std::string name;

        std::string status;      // "Idle - Office" etc.

        bool is_file;            // the "Print to File" pseudo-printer

    };

    std::vector<Printer> printers_;

    int printer_sel_ = 0;

    int copies_ = 1;

    bool collate_ = false;

    int pages_mode_ = PAGES_ALL;

    std::string page_range_ = "1-3";

    bool range_focused_ = false;

    bool landscape_ = false;

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;   // 0 cancel, 1 print, 2 copies-, 3 copies+

    int title_bar_height()

    {

        return 26;

    }

    int action_bar_height()

    {

        return 40;

    }

    int printer_row_height()

    {

        return 32;

    }

    int list_x1()

    {

        return x1_ + 12;

    }

    int list_x2()

    {

        return x1_ + 240;

    }

    int list_top()

    {

        return y1_ + title_bar_height() + 30;

    }

    int list_bottom()

    {

        return y2_ - action_bar_height() - 12;

    }

    // Right-hand settings panel geometry.

    int panel_x()

    {

        return list_x2() + 20;

    }

    void draw_radio(const SStyle& st, int cx, int cy, bool selected)

    {

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawArc(display_, window_, gc_, cx - 6, cy - 6, 12, 12, 0, 360 * 64);

        if (selected)

        {

            XSetForeground(display_, gc_, st.text_color_);

            XFillArc(display_, window_, gc_, cx - 3, cy - 3, 6, 6, 0, 360 * 64);

        }

    }

    void draw_check(const SStyle& st, int bx, int by, bool checked)

    {

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, bx, by, 13, 13);

        if (checked)

        {

            xf::draw_checkmark(display_, window_, gc_, st.text_color_, bx + 7, by + 6, 10);

        }

    }

    // Small +/- and action buttons share one rect helper.
    // which: 0 cancel, 1 print (bottom bar), 2 copies minus,
    // 3 copies plus (in the settings panel).

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        if (which == 2 || which == 3)

        {

            by1 = list_top() + 4;

            by2 = by1 + 20;

            if (which == 2)

            {

                bx1 = panel_x() + 60;

            }

            else

            {

                bx1 = panel_x() + 118;

            }

            bx2 = bx1 + 22;

            return;

        }

        by2 = y2_ - 7;

        by1 = by2 - 26;

        bx2 = x2_ - 10 - (1 - which) * 96;

        bx1 = bx2 - 88;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void(int)> on_response;

    GtkPrintUnixDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkPrintUnixDialog";

        text_ = "Print";

        Printer p;

        p = { "HP LaserJet Pro M404", "Idle - Office", false };

        printers_.push_back(p);

        p = { "EPSON ET-2850", "Idle - Home", false };

        printers_.push_back(p);

        p = { "Canon PIXMA G3020", "Offline", false };

        printers_.push_back(p);

        p = { "Print to File", "PDF / PostScript", true };

        printers_.push_back(p);

    }

    ~GtkPrintUnixDialog() {}

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

    //region GtkPrintUnixDialog specific API

    void add_printer(std::string printer_name, std::string status)

    {

        Printer p;

        p.name = printer_name;

        p.status = status;

        p.is_file = false;

        printers_.push_back(p);

    }

    int printer_count()

    {

        return (int)printers_.size();

    }

    std::string get_selected_printer()

    {

        if (printer_sel_ >= 0 && printer_sel_ < (int)printers_.size())

        {

            return printers_[printer_sel_].name;

        }

        return "";

    }

    void set_copies(int c)

    {

        if (c < 1)

        {

            c = 1;

        }

        if (c > 999)

        {

            c = 999;

        }

        copies_ = c;

    }

    int get_copies()

    {

        return copies_;

    }

    void set_collate(bool c)

    {

        collate_ = c;

    }

    bool get_collate()

    {

        return collate_;

    }

    void set_pages_mode(int m)

    {

        pages_mode_ = m;

    }

    int get_pages_mode()

    {

        return pages_mode_;

    }

    void set_page_range(std::string r)

    {

        page_range_ = r;

    }

    std::string get_page_range()

    {

        return pages_mode_ == PAGES_RANGES ? page_range_ : "";

    }

    void set_landscape(bool l)

    {

        landscape_ = l;

    }

    bool get_landscape()

    {

        return landscape_;

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

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, text_);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        // Printer list.

        xf::draw_text_left(display_, window_, gc_, st, list_x1() + 2, list_top() - 22, list_top() - 4, "Printer");

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, list_x1(), list_top(), list_x2() - list_x1(), list_bottom() - list_top());

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, list_x1(), list_top(), list_x2() - list_x1(), list_bottom() - list_top());

        for (size_t i = 0; i < printers_.size(); ++i)

        {

            int ry1 = list_top() + 1 + (int)i * printer_row_height();

            int ry2 = ry1 + printer_row_height();

            if (ry2 > list_bottom())

            {

                break;

            }

            bool sel = (int)i == printer_sel_;

            if (sel)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, list_x1() + 1, ry1, list_x2() - list_x1() - 2, printer_row_height() - 1);

            }

            SStyle rst = st;

            if (sel)

            {

                rst.text_color_ = st.fillColor_;

            }

            xf::draw_text_left(display_, window_, gc_, rst, list_x1() + 8, ry1, ry1 + 16, printers_[i].name);

            SStyle sst = rst;

            if (!sel)

            {

                sst.text_color_ = styles_->get_color_rgb(120, 120, 120);

            }

            xf::draw_text_left(display_, window_, gc_, sst, list_x1() + 8, ry1 + 15, ry2, printers_[i].status);

        }

        // Settings panel: Copies row.

        int px = panel_x();

        int cy = list_top();

        xf::draw_text_left(display_, window_, gc_, st, px, cy, cy + 24, "Copies:");

        int mx1, my1, mx2, my2;

        button_rect(2, mx1, my1, mx2, my2);

        SStyle mst = st;

        if (pressed_button_ == 2)

        {

            unsigned long swap = mst.fillColor_;

            mst.fillColor_ = mst.outlineColor_;

            mst.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, mst, mx1, my1, mx2, my2);

        xf::draw_text_centered(display_, window_, gc_, mst, mx1, my1, mx2, my2, "-");

        // Copies value box.

        XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

        XFillRectangle(display_, window_, gc_, mx2 + 4, my1, 30, 20);

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, mx2 + 4, my1, 30, 20);

        xf::draw_text_centered(display_, window_, gc_, st, mx2 + 4, my1, mx2 + 34, my2, std::to_string(copies_));

        int px1b, py1b, px2b, py2b;

        button_rect(3, px1b, py1b, px2b, py2b);

        SStyle pst = st;

        if (pressed_button_ == 3)

        {

            unsigned long swap = pst.fillColor_;

            pst.fillColor_ = pst.outlineColor_;

            pst.outlineColor_ = swap;

        }

        xf::draw_polished(display_, window_, gc_, pst, px1b, py1b, px2b, py2b);

        xf::draw_text_centered(display_, window_, gc_, pst, px1b, py1b, px2b, py2b, "+");

        // Collate checkbox.

        cy += 32;

        draw_check(st, px, cy + 2, collate_);

        xf::draw_text_left(display_, window_, gc_, st, px + 20, cy, cy + 18, "Collate");

        // Pages radios.

        cy += 30;

        xf::draw_text_left(display_, window_, gc_, st, px, cy, cy + 18, "Pages:");

        const char* page_labels[3] = { "All", "Current", "Range:" };

        for (int i = 0; i < 3; ++i)

        {

            cy += 20;

            draw_radio(st, px + 8, cy + 8, pages_mode_ == i);

            xf::draw_text_left(display_, window_, gc_, st, px + 20, cy, cy + 16, page_labels[i]);

            if (i == 2)

            {

                // The range entry next to the Range radio.

                int ex1 = px + 76;

                int ex2 = px + 150;

                XSetForeground(display_, gc_, styles_->get_color_rgb(255, 255, 255));

                XFillRectangle(display_, window_, gc_, ex1, cy, ex2 - ex1, 17);

                XSetForeground(display_, gc_, range_focused_ ? st.lineColor_ : styles_->get_color_rgb(150, 150, 150));

                XDrawRectangle(display_, window_, gc_, ex1, cy, ex2 - ex1, 17);

                xf::draw_text_left(display_, window_, gc_, st, ex1 + 4, cy, cy + 17, page_range_);

            }

        }

        // Orientation radios.

        cy += 30;

        xf::draw_text_left(display_, window_, gc_, st, px, cy, cy + 18, "Orientation:");

        cy += 20;

        draw_radio(st, px + 8, cy + 8, !landscape_);

        xf::draw_text_left(display_, window_, gc_, st, px + 20, cy, cy + 16, "Portrait");

        cy += 20;

        draw_radio(st, px + 8, cy + 8, landscape_);

        xf::draw_text_left(display_, window_, gc_, st, px + 20, cy, cy + 16, "Landscape");

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        const char* labels[2] = { "Cancel", "Print" };

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

        range_focused_ = false;

        if (in_rect(x, y, x2_ - 28, y1_, x2_ - 4, y1_ + title_bar_height()))

        {

            respond(RESPONSE_DELETE_EVENT);

            return true;

        }

        // Printer rows.

        if (x >= list_x1() && x <= list_x2() && y >= list_top() && y <= list_bottom())

        {

            int row = (y - list_top() - 1) / printer_row_height();

            if (row >= 0 && row < (int)printers_.size())

            {

                printer_sel_ = row;

            }

            return true;

        }

        int px = panel_x();

        int cy = list_top();

        // Copies +/- buttons.

        for (int i = 2; i <= 3; ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            if (in_rect(x, y, bx1, by1, bx2, by2))

            {

                pressed_button_ = i;

                return true;

            }

        }

        // Collate checkbox.

        if (x >= px && x <= px + 110 && y >= cy + 32 && y <= cy + 50)

        {

            collate_ = !collate_;

            return true;

        }

        // Pages radios + range entry. (Draw order from list_top:
        // copies row, +32 collate, +62 "Pages:" label, radios at
        // +82/+102/+122, +152 orientation label, radios +172/+192.)

        int radio_base = cy + 82;

        for (int i = 0; i < 3; ++i)

        {

            int ry = radio_base + i * 20;

            if (x >= px && x <= px + 70 && y >= ry && y <= ry + 18)

            {

                pages_mode_ = i;

                return true;

            }

            if (i == 2 && x >= px + 76 && x <= px + 150 && y >= ry && y <= ry + 17)

            {

                pages_mode_ = PAGES_RANGES;

                range_focused_ = true;

                return true;

            }

        }

        // Orientation radios.

        int orient_base = radio_base + 90;

        for (int i = 0; i < 2; ++i)

        {

            int ry = orient_base + i * 20;

            if (x >= px && x <= px + 110 && y >= ry && y <= ry + 18)

            {

                landscape_ = (i == 1);

                return true;

            }

        }

        // Bottom buttons.

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

        int which = pressed_button_;

        pressed_button_ = -1;

        int bx1, by1, bx2, by2;

        button_rect(which, bx1, by1, bx2, by2);

        if (!in_rect(x, y, bx1, by1, bx2, by2))

        {

            return true;

        }

        if (which == 2)

        {

            set_copies(copies_ - 1);

        }

        else if (which == 3)

        {

            set_copies(copies_ + 1);

        }

        else if (which == 0)

        {

            respond(RESPONSE_CANCEL);

        }

        else

        {

            respond(RESPONSE_ACCEPT);

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (key == XK_Escape)

        {

            respond(RESPONSE_CANCEL);

            return true;

        }

        if (range_focused_)

        {

            if (key == XK_BackSpace)

            {

                if (!page_range_.empty())

                {

                    page_range_.pop_back();

                }

                return true;

            }

            if (str && ((str[0] >= '0' && str[0] <= '9') || str[0] == '-' || str[0] == ','))

            {

                page_range_ += str[0];

                return true;

            }

        }

        return false;

    }

};

//endregion
