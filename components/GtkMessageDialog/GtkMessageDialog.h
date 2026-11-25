#pragma once

#include "core/xframe_core.h"

//region GtkMessageDialog

class GtkMessageDialog

{

public:

    // GtkMessageType values.

    static const int MESSAGE_INFO = 0;

    static const int MESSAGE_WARNING = 1;

    static const int MESSAGE_QUESTION = 2;

    static const int MESSAGE_ERROR = 3;

    // GtkButtonsType values.

    static const int BUTTONS_NONE = 0;

    static const int BUTTONS_OK = 1;

    static const int BUTTONS_CLOSE = 2;

    static const int BUTTONS_CANCEL = 3;

    static const int BUTTONS_YES_NO = 4;

    static const int BUTTONS_OK_CANCEL = 5;

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_OK = -5;

    static const int RESPONSE_CANCEL = -6;

    static const int RESPONSE_CLOSE = -7;

    static const int RESPONSE_YES = -8;

    static const int RESPONSE_NO = -9;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the primary message text

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    std::string secondary_;    // smaller text under the primary

    int message_type_ = MESSAGE_INFO;

    int buttons_type_ = BUTTONS_OK_CANCEL;

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;

    struct DialogButton

    {

        std::string label;

        int response_id;

    };

    std::vector<DialogButton> buttons_;

    int title_bar_height()

    {

        return 26;

    }

    int action_bar_height()

    {

        return 40;

    }

    void rebuild_buttons()

    {

        buttons_.clear();

        DialogButton b;

        switch (buttons_type_)

        {

            case BUTTONS_OK:

                b.label = "OK";

                b.response_id = RESPONSE_OK;

                buttons_.push_back(b);

                break;

            case BUTTONS_CLOSE:

                b.label = "Close";

                b.response_id = RESPONSE_CLOSE;

                buttons_.push_back(b);

                break;

            case BUTTONS_CANCEL:

                b.label = "Cancel";

                b.response_id = RESPONSE_CANCEL;

                buttons_.push_back(b);

                break;

            case BUTTONS_YES_NO:

                b.label = "No";

                b.response_id = RESPONSE_NO;

                buttons_.push_back(b);

                b.label = "Yes";

                b.response_id = RESPONSE_YES;

                buttons_.push_back(b);

                break;

            case BUTTONS_OK_CANCEL:

                b.label = "Cancel";

                b.response_id = RESPONSE_CANCEL;

                buttons_.push_back(b);

                b.label = "OK";

                b.response_id = RESPONSE_OK;

                buttons_.push_back(b);

                break;

            default:

                break;

        }

    }

    void button_rect(int index, int &bx1, int &by1, int &bx2, int &by2)

    {

        by2 = y2_ - 7;

        by1 = by2 - 26;

        bx2 = x2_ - 10 - ((int)buttons_.size() - 1 - index) * 96;

        bx1 = bx2 - 88;

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

    // Draw one wrapped paragraph starting at (x, top); returns the
    // y just below the last line drawn.

    int draw_wrapped(const SStyle& st, const std::string& body_text, int x, int top, int maxw)

    {

        int cy = top;

        std::string word;

        std::string line;

        std::string body = body_text + " ";

        for (size_t i = 0; i < body.size(); ++i)

        {

            if (body[i] == ' ' || body[i] == '\n')

            {

                std::string trial = line.empty() ? word : line + " " + word;

                if (xf::text_width(st, trial) > maxw && !line.empty())

                {

                    xf::draw_text_at(display_, window_, gc_, st, x, cy + 12, line);

                    cy += 18;

                    line = word;

                }

                else

                {

                    line = trial;

                }

                word.clear();

                if (body[i] == '\n')

                {

                    xf::draw_text_at(display_, window_, gc_, st, x, cy + 12, line);

                    cy += 18;

                    line.clear();

                }

            }

            else

            {

                word += body[i];

            }

        }

        if (!line.empty())

        {

            xf::draw_text_at(display_, window_, gc_, st, x, cy + 12, line);

            cy += 18;

        }

        return cy;

    }

public:

    std::function<void(int)> on_response;

    GtkMessageDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkMessageDialog";

        text_ = "Are you sure you want to continue?";

        secondary_ = "The document has unsaved changes. If you continue, those changes will be lost.";

        rebuild_buttons();

    }

    ~GtkMessageDialog() {}

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

    //region GtkMessageDialog specific API

    void set_message_text(std::string t)

    {

        text_ = t;

    }

    std::string get_message_text()

    {

        return text_;

    }

    void set_secondary_text(std::string t)

    {

        secondary_ = t;

    }

    std::string get_secondary_text()

    {

        return secondary_;

    }

    void set_message_type(int t)

    {

        message_type_ = t;

    }

    int get_message_type()

    {

        return message_type_;

    }

    void set_buttons_type(int t)

    {

        buttons_type_ = t;

        rebuild_buttons();

    }

    int get_buttons_type()

    {

        return buttons_type_;

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

        // Title bar (message dialogs carry no title in GTK, the
        // frame still reads better with one in XFrame).

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, tb - 2);

        SStyle tst = st;

        tst.text_color_ = st.fillColor_;

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, "Message");

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        // The message icon: a filled circle with a glyph inside,
        // coloured by message type.

        unsigned long icon_color;

        std::string glyph;

        if (message_type_ == MESSAGE_WARNING)

        {

            icon_color = styles_->get_color_rgb(237, 176, 32);

            glyph = "!";

        }

        else if (message_type_ == MESSAGE_ERROR)

        {

            icon_color = styles_->get_color_rgb(205, 35, 35);

            glyph = "X";

        }

        else if (message_type_ == MESSAGE_QUESTION)

        {

            icon_color = styles_->get_color_rgb(40, 110, 200);

            glyph = "?";

        }

        else

        {

            icon_color = styles_->get_color_rgb(40, 140, 90);

            glyph = "i";

        }

        int ix = x1_ + 16;

        int iy = y1_ + tb + 14;

        XSetForeground(display_, gc_, icon_color);

        XFillArc(display_, window_, gc_, ix, iy, 40, 40, 0, 360 * 64);

        SStyle gst = st;

        gst.text_color_ = styles_->get_color_rgb(255, 255, 255);

        xf::draw_text_centered(display_, window_, gc_, gst, ix, iy, ix + 40, iy + 40, glyph);

        // Primary + secondary text, wrapped, right of the icon.

        int tx = ix + 56;

        int maxw = x2_ - 12 - tx;

        int cy = draw_wrapped(st, text_, tx, iy, maxw);

        if (!secondary_.empty())

        {

            draw_wrapped(st, secondary_, tx, cy + 4, maxw);

        }

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        for (size_t i = 0; i < buttons_.size(); ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect((int)i, bx1, by1, bx2, by2);

            SStyle bst = st;

            if ((int)i == pressed_button_)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bst, bx1, by1, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, bst, bx1, by1, bx2, by2, buttons_[i].label);

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

        for (size_t i = 0; i < buttons_.size(); ++i)

        {

            int bx1, by1, bx2, by2;

            button_rect((int)i, bx1, by1, bx2, by2);

            if (in_rect(x, y, bx1, by1, bx2, by2))

            {

                pressed_button_ = (int)i;

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

            respond(buttons_[index].response_id);

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

        if (key == XK_Return && !buttons_.empty())

        {

            respond(buttons_.back().response_id);

            return true;

        }

        return false;

    }

};

//endregion
