#pragma once

#include "core/xframe_core.h"

//region GtkAssistant

class GtkAssistant

{

public:

    // GtkAssistantPageType values.

    static const int PAGE_CONTENT = 0;

    static const int PAGE_INTRO = 1;

    static const int PAGE_CONFIRM = 2;

    static const int PAGE_SUMMARY = 3;

    static const int PAGE_PROGRESS = 4;

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_CANCEL = -6;

    static const int RESPONSE_CLOSE = -7;

    static const int RESPONSE_APPLY = -10;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the assistant title

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    struct AssistantPage

    {

        std::string title;

        std::string content;

        int type = PAGE_CONTENT;

        bool complete = true;

    };

    std::vector<AssistantPage> pages_;

    int current_page_ = 0;

    bool applied_ = false;      // Apply hit on the confirm page

    int response_ = RESPONSE_NONE;

    int pressed_button_ = -1;   // 0 back, 1 next, 2 apply/close, 3 cancel

    int header_height()

    {

        return 44;

    }

    int action_bar_height()

    {

        return 40;

    }

    bool is_last_page()

    {

        return current_page_ >= (int)pages_.size() - 1;

    }

    // Bottom bar buttons: Cancel left; Back / Next / Apply
    // right-aligned in that order.

    void button_rect(int which, int &bx1, int &by1, int &bx2, int &by2)

    {

        by2 = y2_ - 7;

        by1 = by2 - 26;

        if (which == 3)

        {

            bx1 = x1_ + 10;

            bx2 = bx1 + 80;

        }

        else

        {

            bx2 = x2_ - 10 - (2 - which) * 88;

            bx1 = bx2 - 80;

        }

    }

    // Which bottom buttons exist on the current page.

    bool button_exists(int which)

    {

        if (applied_)

        {

            return which == 2;   // only Close

        }

        if (which == 0)

        {

            return current_page_ > 0;

        }

        if (which == 1)

        {

            return !is_last_page();

        }

        if (which == 2)

        {

            return is_last_page();

        }

        return which == 3;

    }

    std::string button_label(int which)

    {

        if (which == 0)

        {

            return "Back";

        }

        if (which == 1)

        {

            return "Next";

        }

        if (which == 2)

        {

            return applied_ ? "Close" : "Apply";

        }

        return "Cancel";

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void(int)> on_response;

    std::function<void()> on_apply;

    std::function<void(int)> on_page_changed;

    GtkAssistant(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkAssistant";

        text_ = "Setup Assistant";

        append_page("Welcome", "This assistant will guide you through setting up the application, one page at a time.", PAGE_INTRO);

        append_page("Options", "Choose the options for the new setup. The defaults are fine for most people.", PAGE_CONTENT);

        append_page("Confirm", "Everything is ready. Press Apply to finish the setup with the options you chose.", PAGE_CONFIRM);

    }

    ~GtkAssistant() {}

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

    //region GtkAssistant specific API

    int append_page(std::string title, std::string content, int type)

    {

        AssistantPage p;

        p.title = title;

        p.content = content;

        p.type = type;

        pages_.push_back(p);

        return (int)pages_.size() - 1;

    }

    int get_n_pages()

    {

        return (int)pages_.size();

    }

    void set_page_complete(int page, bool complete)

    {

        if (page >= 0 && page < (int)pages_.size())

        {

            pages_[page].complete = complete;

        }

    }

    bool get_page_complete(int page)

    {

        if (page >= 0 && page < (int)pages_.size())

        {

            return pages_[page].complete;

        }

        return false;

    }

    void set_current_page(int page)

    {

        if (page >= 0 && page < (int)pages_.size() && page != current_page_)

        {

            current_page_ = page;

            if (on_page_changed)

            {

                on_page_changed(current_page_);

            }

        }

    }

    int get_current_page()

    {

        return current_page_;

    }

    std::string get_page_title(int page)

    {

        if (page >= 0 && page < (int)pages_.size())

        {

            return pages_[page].title;

        }

        return "";

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

    void next_page()

    {

        if (!is_last_page() && pages_[current_page_].complete)

        {

            set_current_page(current_page_ + 1);

        }

    }

    void previous_page()

    {

        if (current_page_ > 0)

        {

            set_current_page(current_page_ - 1);

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

        // Header: window title on the left, current step on the right.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, header_height() - 2);

        SStyle hst = st;

        hst.text_color_ = st.fillColor_;

        xf::draw_text_left(display_, window_, gc_, hst, x1_ + 12, y1_, y1_ + header_height(), text_);

        std::string step = "Step " + std::to_string(current_page_ + 1) + " of " + std::to_string((int)pages_.size());

        int sw = xf::text_width(hst, step);

        xf::draw_text_left(display_, window_, gc_, hst, x2_ - 12 - sw, y1_, y1_ + header_height(), step);

        // Sidebar: the page list, current page highlighted.

        int sx1 = x1_ + 10;

        int sx2 = x1_ + 150;

        int sy = y1_ + header_height() + 10;

        for (size_t i = 0; i < pages_.size(); ++i)

        {

            int ry1 = sy + (int)i * 24;

            int ry2 = ry1 + 22;

            if ((int)i == current_page_)

            {

                XSetForeground(display_, gc_, st.outlineColor_);

                XFillRectangle(display_, window_, gc_, sx1, ry1, sx2 - sx1, ry2 - ry1);

                SStyle sst = st;

                sst.text_color_ = st.fillColor_;

                xf::draw_text_left(display_, window_, gc_, sst, sx1 + 8, ry1, ry2, pages_[i].title);

            }

            else

            {

                SStyle sst = st;

                if ((int)i > current_page_)

                {

                    sst.text_color_ = styles_->get_color_rgb(140, 140, 140);

                }

                xf::draw_text_left(display_, window_, gc_, sst, sx1 + 8, ry1, ry2, pages_[i].title);

            }

        }

        // Content: current page title + body text.

        if (!pages_.empty())

        {

            AssistantPage &p = pages_[current_page_];

            int cx = sx2 + 16;

            int cy = sy;

            xf::draw_text_left(display_, window_, gc_, st, cx, cy, cy + 20, p.title);

            cy += 30;

            // Wrapped body text.

            int maxw = x2_ - 12 - cx;

            std::string word;

            std::string line;

            std::string body = (applied_ ? std::string("Setup complete. Press Close to finish.") : p.content) + " ";

            for (size_t i = 0; i < body.size(); ++i)

            {

                if (body[i] == ' ')

                {

                    std::string trial = line.empty() ? word : line + " " + word;

                    if (xf::text_width(st, trial) > maxw && !line.empty())

                    {

                        xf::draw_text_at(display_, window_, gc_, st, cx, cy + 12, line);

                        cy += 18;

                        line = word;

                    }

                    else

                    {

                        line = trial;

                    }

                    word.clear();

                }

                else

                {

                    word += body[i];

                }

            }

            if (!line.empty())

            {

                xf::draw_text_at(display_, window_, gc_, st, cx, cy + 12, line);

            }

        }

        // Action bar.

        int ay = y2_ - action_bar_height();

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawLine(display_, window_, gc_, x1_ + 2, ay, x2_ - 2, ay);

        for (int i = 0; i < 4; ++i)

        {

            if (!button_exists(i))

            {

                continue;

            }

            int bx1, by1, bx2, by2;

            button_rect(i, bx1, by1, bx2, by2);

            SStyle bst = st;

            bool usable = true;

            if (i == 1 && !pages_.empty() && !pages_[current_page_].complete)

            {

                usable = false;

            }

            if (!usable)

            {

                bst.text_color_ = styles_->get_color_rgb(140, 140, 140);

            }

            if (pressed_button_ == i)

            {

                unsigned long swap = bst.fillColor_;

                bst.fillColor_ = bst.outlineColor_;

                bst.outlineColor_ = swap;

            }

            xf::draw_polished(display_, window_, gc_, bst, bx1, by1, bx2, by2);

            xf::draw_text_centered(display_, window_, gc_, bst, bx1, by1, bx2, by2, button_label(i));

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

        // Sidebar clicks jump back to an earlier page (GTK lets
        // you revisit pages you have already completed).

        int sx1 = x1_ + 10;

        int sx2 = x1_ + 150;

        int sy = y1_ + header_height() + 10;

        if (x >= sx1 && x <= sx2 && y >= sy)

        {

            int index = (y - sy) / 24;

            if (index >= 0 && index < (int)pages_.size() && index < current_page_)

            {

                set_current_page(index);

                return true;

            }

        }

        for (int i = 0; i < 4; ++i)

        {

            if (!button_exists(i))

            {

                continue;

            }

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

        if (which == 0)

        {

            previous_page();

        }

        else if (which == 1)

        {

            next_page();

        }

        else if (which == 2)

        {

            if (applied_)

            {

                respond(RESPONSE_CLOSE);

            }

            else

            {

                applied_ = true;

                if (on_apply)

                {

                    on_apply();

                }

                respond(RESPONSE_APPLY);

            }

        }

        else

        {

            respond(RESPONSE_CANCEL);

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
