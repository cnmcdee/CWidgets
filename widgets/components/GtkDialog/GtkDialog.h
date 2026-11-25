#pragma once

#include "core/xframe_core.h"

//region GtkDialog

class GtkDialog

{

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

    // One row in the action area along the bottom of the dialog.

    struct DialogButton

    {

        std::string label;

        int response_id;

    };

    std::vector<DialogButton> buttons_;

    std::string message_;      // body text shown in the content area

    int response_ = -1;        // GTK_RESPONSE_NONE until a button is hit

    bool running_ = false;     // set by run(), cleared on response

    int pressed_button_ = -1;  // index of the button held down, or -1

    int title_bar_height()

    {

        return 26;

    }

    int action_bar_height()

    {

        return 40;

    }

    // Action buttons are right-aligned along the bottom bar,
    // each 88x26 with an 8px gap, last added = rightmost.

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

public:

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_REJECT = -2;

    static const int RESPONSE_ACCEPT = -3;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_OK = -5;

    static const int RESPONSE_CANCEL = -6;

    static const int RESPONSE_CLOSE = -7;

    static const int RESPONSE_YES = -8;

    static const int RESPONSE_NO = -9;

    static const int RESPONSE_APPLY = -10;

    static const int RESPONSE_HELP = -11;

    std::function<void(int)> on_response;

    GtkDialog(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkDialog";

        text_ = "Dialog";

        message_ = "This is a GtkDialog. Add buttons with add_button() and read the result back with get_response().";

        add_button("Cancel", RESPONSE_CANCEL);

        add_button("OK", RESPONSE_OK);

    }

    ~GtkDialog() {}

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

    //region GtkDialog specific API

    void set_title(std::string t)

    {

        text_ = t;

    }

    std::string get_title()

    {

        return text_;

    }

    void set_message(std::string m)

    {

        message_ = m;

    }

    std::string get_message()

    {

        return message_;

    }

    void add_button(std::string label, int response_id)

    {

        DialogButton b;

        b.label = label;

        b.response_id = response_id;

        buttons_.push_back(b);

    }

    void clear_buttons()

    {

        buttons_.clear();

    }

    int button_count()

    {

        return (int)buttons_.size();

    }

    // "Run" the dialog: from here on a button click records the
    // response and ends the run, like gtk_dialog_run().

    void run()

    {

        response_ = RESPONSE_NONE;

        running_ = true;

        visible_ = true;

    }

    bool is_running()

    {

        return running_;

    }

    int get_response()

    {

        return response_;

    }

    void respond(int response_id)

    {

        response_ = response_id;

        running_ = false;

        if (on_response)

        {

            on_response(response_id);

        }

    }

    // The content area between title bar and action bar.

    int content_x()

    {

        return x1_ + 12;

    }

    int content_y()

    {

        return y1_ + title_bar_height() + 10;

    }

    int content_width()

    {

        return width() - 24;

    }

    int content_height()

    {

        return y2_ - action_bar_height() - content_y() - 6;

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

        // Close box at the right of the title bar.

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        // Body message, word-wrapped by hand into the content area.

        int cy = content_y();

        int maxw = content_width();

        std::string word;

        std::string line;

        std::string body = message_ + " ";

        for (size_t i = 0; i < body.size(); ++i)

        {

            if (body[i] == ' ' || body[i] == '\n')

            {

                std::string trial = line.empty() ? word : line + " " + word;

                if (xf::text_width(st, trial) > maxw && !line.empty())

                {

                    xf::draw_text_at(display_, window_, gc_, st, content_x(), cy + 12, line);

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

                    xf::draw_text_at(display_, window_, gc_, st, content_x(), cy + 12, line);

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

            xf::draw_text_at(display_, window_, gc_, st, content_x(), cy + 12, line);

        }

        // Separator above the action area.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        // Action buttons.

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

        // Title-bar close box = delete event.

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

        if (key == XK_Return)

        {

            // Enter activates the last (default) button, if any.

            if (!buttons_.empty())

            {

                respond(buttons_.back().response_id);

            }

            return true;

        }

        return false;

    }

};

//endregion
