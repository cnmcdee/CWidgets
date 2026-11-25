#pragma once

#include "core/xframe_core.h"

//region TBitBtnKind

enum TBitBtnKind

{

    bkCustom,

    bkOK,

    bkCancel,

    bkHelp,

    bkYes,

    bkNo,

    bkClose,

    bkAbort,

    bkRetry,

    bkIgnore,

    bkAll,

    bkNoToAll,

    bkYesToAll

};

//endregion

//region TBitBtn

class TBitBtn

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

    std::string style_name_;

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    bool pressed_ = false;

    TBitBtnKind kind_ = bkOK;

    int modal_result_ = 1;

    int result_ = 0;

    int glyph_width_ = 22;

    // Modal results follow LCL TModalResult: mrNone = 0, mrOK = 1,
    // mrCancel = 2, mrAbort = 3, mrRetry = 4, mrIgnore = 5,
    // mrYes = 6, mrNo = 7, mrAll = 8, mrNoToAll = 9, mrYesToAll = 10,
    // mrClose = 11. bkClose maps to mrClose (11) here so a dialog can
    // tell a Close button apart from "no result yet" (result_ = 0).

    int modal_result_for_kind(TBitBtnKind k)

    {

        switch (k)

        {

            case bkOK: return 1;

            case bkCancel: return 2;

            case bkAbort: return 3;

            case bkRetry: return 4;

            case bkIgnore: return 5;

            case bkYes: return 6;

            case bkNo: return 7;

            case bkAll: return 8;

            case bkNoToAll: return 9;

            case bkYesToAll: return 10;

            case bkClose: return 11;

            case bkHelp: return 0;

            case bkCustom: return 0;

        }

        return 0;

    }

    std::string caption_for_kind(TBitBtnKind k)

    {

        switch (k)

        {

            case bkOK: return "OK";

            case bkCancel: return "Cancel";

            case bkHelp: return "Help";

            case bkYes: return "Yes";

            case bkNo: return "No";

            case bkClose: return "Close";

            case bkAbort: return "Abort";

            case bkRetry: return "Retry";

            case bkIgnore: return "Ignore";

            case bkAll: return "All";

            case bkNoToAll: return "No to All";

            case bkYesToAll: return "Yes to All";

            case bkCustom: return text_;

        }

        return text_;

    }

    void apply_kind()

    {

        modal_result_ = modal_result_for_kind(kind_);

        if (kind_ != bkCustom)

        {

            text_ = caption_for_kind(kind_);

        }

    }

    void draw_cross(int cx, int cy, int size, unsigned long color)

    {

        XSetForeground(display_, gc_, color);

        xf::set_line_width(display_, gc_, 2);

        int s = size / 2;

        XDrawLine(display_, window_, gc_, cx - s, cy - s, cx + s, cy + s);

        XDrawLine(display_, window_, gc_, cx + s, cy - s, cx - s, cy + s);

        xf::set_line_width(display_, gc_, 1);

    }

    void draw_glyph(int gx1, int gy1, int gx2, int gy2, const SStyle& st)

    {

        int cx = (gx1 + gx2) / 2;

        int cy = (gy1 + gy2) / 2;

        unsigned long ink = enabled_ ? st.text_color_ : styles_->get_color_rgb(140, 140, 140);

        unsigned long green = styles_->get_color_rgb(30, 160, 60);

        unsigned long red = styles_->get_color_rgb(200, 40, 40);

        if (!enabled_)

        {

            green = ink;

            red = ink;

        }

        switch (kind_)

        {

            case bkOK:

            case bkYes:

            case bkYesToAll:

            case bkAll:

                xf::draw_checkmark(display_, window_, gc_, green, cx, cy, 14);

                break;

            case bkCancel:

            case bkNo:

            case bkClose:

            case bkNoToAll:

                draw_cross(cx, cy, 12, red);

                break;

            case bkHelp:

                xf::draw_text_centered(display_, window_, gc_, st, gx1, gy1, gx2, gy2, "?");

                break;

            case bkAbort:

                XSetForeground(display_, gc_, red);

                XFillArc(display_, window_, gc_, cx - 7, cy - 7, 14, 14, 0, 360 * 64);

                XSetForeground(display_, gc_, st.fillColor_);

                XFillRectangle(display_, window_, gc_, cx - 5, cy - 1, 10, 3);

                break;

            case bkRetry:

                XSetForeground(display_, gc_, ink);

                xf::set_line_width(display_, gc_, 2);

                XDrawArc(display_, window_, gc_, cx - 7, cy - 7, 14, 14, 30 * 64, 270 * 64);

                xf::draw_triangle_right(display_, window_, gc_, ink, cx + 5, cy - 7, 4);

                xf::set_line_width(display_, gc_, 1);

                break;

            case bkIgnore:

                xf::draw_triangle_right(display_, window_, gc_, ink, cx - 2, cy, 6);

                xf::draw_triangle_right(display_, window_, gc_, ink, cx + 3, cy, 6);

                break;

            case bkCustom:

                XSetForeground(display_, gc_, ink);

                XDrawRectangle(display_, window_, gc_, cx - 6, cy - 6, 12, 12);

                XFillRectangle(display_, window_, gc_, cx - 3, cy - 3, 6, 6);

                break;

        }

    }

public:

    std::function<void()> on_click;

    TBitBtn(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "TBitBtn";

        kind_ = bkOK;

        text_ = "OK";

        apply_kind();

    }

    ~TBitBtn() {}

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

        if (kind_ != bkCustom)

        {

            kind_ = bkCustom;

            modal_result_ = 0;

        }

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    void set_kind(TBitBtnKind k)

    {

        kind_ = k;

        apply_kind();

    }

    TBitBtnKind get_kind()

    {

        return kind_;

    }

    void set_modal_result(int r)

    {

        modal_result_ = r;

    }

    int get_modal_result()

    {

        return result_;

    }

    int get_kind_modal_result()

    {

        return modal_result_;

    }

    void set_glyph_width(int w)

    {

        glyph_width_ = w;

    }

    int get_glyph_width()

    {

        return glyph_width_;

    }

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

        int pad = 4;

        int gx1 = x1_ + pad + (pressed_ ? 1 : 0);

        int gy1 = y1_ + pad;

        int gx2 = gx1 + glyph_width_;

        int gy2 = y2_ - pad;

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, gx1, gy1, gx2 - gx1, gy2 - gy1);

        draw_glyph(gx1, gy1, gx2, gy2, st);

        int tx1 = gx2 + 4;

        xf::draw_text_centered(display_, window_, gc_, st, tx1, y1_, x2_ - pad, y2_, text_);

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

            result_ = modal_result_;

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

            result_ = modal_result_;

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
