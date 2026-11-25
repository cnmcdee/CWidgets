#pragma once

#include "core/xframe_core.h"

//region GtkShortcutsWindow

class GtkShortcutsWindow

{

public:

    // Real GTK response ids.

    static const int RESPONSE_NONE = -1;

    static const int RESPONSE_DELETE_EVENT = -4;

    static const int RESPONSE_CLOSE = -7;

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;          // the window title

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    struct ShortcutRow

    {

        std::string keys;        // "Ctrl+S" - drawn as key caps

        std::string description;

    };

    struct ShortcutSection

    {

        std::string title;

        std::vector<ShortcutRow> rows;

    };

    std::vector<ShortcutSection> sections_;

    int scroll_ = 0;            // first visible content line

    int response_ = RESPONSE_NONE;

    int title_bar_height()

    {

        return 26;

    }

    int line_height()

    {

        return 22;

    }

    // Total content lines: one header line per section plus one
    // line per shortcut row, plus a gap line between sections.

    int total_lines()

    {

        int n = 0;

        for (size_t i = 0; i < sections_.size(); ++i)

        {

            n += 1 + (int)sections_[i].rows.size() + 1;

        }

        return n;

    }

    int visible_lines()

    {

        return (height() - title_bar_height() - 16) / line_height();

    }

    void clamp_scroll()

    {

        int max_scroll = total_lines() - visible_lines();

        if (max_scroll < 0)

        {

            max_scroll = 0;

        }

        if (scroll_ > max_scroll)

        {

            scroll_ = max_scroll;

        }

        if (scroll_ < 0)

        {

            scroll_ = 0;

        }

    }

    // Draw one key combination as a row of little key caps:
    // "Ctrl+Shift+S" becomes [Ctrl] [Shift] [S].

    void draw_key_caps(const SStyle& st, int x, int y1, int y2, const std::string& keys)

    {

        int cx = x;

        std::string part;

        std::string all = keys + "+";

        for (size_t i = 0; i < all.size(); ++i)

        {

            if (all[i] == '+')

            {

                int w = xf::text_width(st, part) + 10;

                xf::draw_polished(display_, window_, gc_, st, cx, y1 + 2, cx + w, y2 - 2);

                xf::draw_text_centered(display_, window_, gc_, st, cx, y1, cx + w, y2, part);

                cx += w + 4;

                part.clear();

            }

            else

            {

                part += all[i];

            }

        }

    }

    bool in_rect(int x, int y, int rx1, int ry1, int rx2, int ry2)

    {

        return x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2;

    }

public:

    std::function<void(int)> on_response;

    GtkShortcutsWindow(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkShortcutsWindow";

        text_ = "Keyboard Shortcuts";

        add_section("General");

        add_shortcut("General", "Ctrl+N", "New document");

        add_shortcut("General", "Ctrl+O", "Open a file");

        add_shortcut("General", "Ctrl+S", "Save the document");

        add_shortcut("General", "Ctrl+Q", "Quit the application");

        add_section("Editing");

        add_shortcut("Editing", "Ctrl+X", "Cut the selection");

        add_shortcut("Editing", "Ctrl+C", "Copy the selection");

        add_shortcut("Editing", "Ctrl+V", "Paste the clipboard");

        add_shortcut("Editing", "Ctrl+Z", "Undo the last action");

        add_shortcut("Editing", "Ctrl+Shift+Z", "Redo the last action");

        add_section("Navigation");

        add_shortcut("Navigation", "Ctrl+F", "Find text");

        add_shortcut("Navigation", "F3", "Find next");

        add_shortcut("Navigation", "Ctrl+Tab", "Switch document");

    }

    ~GtkShortcutsWindow() {}

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

    //region GtkShortcutsWindow specific API

    void add_section(std::string title)

    {

        for (size_t i = 0; i < sections_.size(); ++i)

        {

            if (sections_[i].title == title)

            {

                return;

            }

        }

        ShortcutSection s;

        s.title = title;

        sections_.push_back(s);

    }

    void add_shortcut(std::string section, std::string keys, std::string description)

    {

        add_section(section);

        for (size_t i = 0; i < sections_.size(); ++i)

        {

            if (sections_[i].title == section)

            {

                ShortcutRow r;

                r.keys = keys;

                r.description = description;

                sections_[i].rows.push_back(r);

                return;

            }

        }

    }

    int section_count()

    {

        return (int)sections_.size();

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

    void scroll_by(int lines)

    {

        scroll_ += lines;

        clamp_scroll();

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        clamp_scroll();

        SStyle st = style();

        xf::draw_polished(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        int tb = title_bar_height();

        // Title bar with a close box.

        XSetForeground(display_, gc_, st.outlineColor_);

        XFillRectangle(display_, window_, gc_, x1_ + 2, y1_ + 2, width() - 4, tb - 2);

        SStyle tst = st;

        tst.text_color_ = st.fillColor_;

        xf::draw_text_left(display_, window_, gc_, tst, x1_ + 10, y1_, y1_ + tb, text_);

        xf::draw_text_centered(display_, window_, gc_, tst, x2_ - 28, y1_, x2_ - 4, y1_ + tb, "X");

        // Content lines, walked with the scroll offset applied.

        int line = 0;

        int top = y1_ + tb + 8;

        int bottom = y2_ - 8;

        for (size_t s = 0; s < sections_.size(); ++s)

        {

            // Section header line.

            if (line >= scroll_)

            {

                int ly = top + (line - scroll_) * line_height();

                if (ly + line_height() <= bottom)

                {

                    SStyle sst = st;

                    sst.text_color_ = st.lineColor_;

                    xf::draw_text_left(display_, window_, gc_, sst, x1_ + 14, ly, ly + line_height(), sections_[s].title);

                }

            }

            ++line;

            for (size_t r = 0; r < sections_[s].rows.size(); ++r)

            {

                if (line >= scroll_)

                {

                    int ly = top + (line - scroll_) * line_height();

                    if (ly + line_height() <= bottom)

                    {

                        draw_key_caps(st, x1_ + 14, ly, ly + line_height(), sections_[s].rows[r].keys);

                        xf::draw_text_left(display_, window_, gc_, st, x1_ + 190, ly, ly + line_height(), sections_[s].rows[r].description);

                    }

                }

                ++line;

            }

            ++line;   // gap line between sections

        }

        // Scrollbar hint on the right edge when content overflows.

        if (total_lines() > visible_lines())

        {

            int track_y1 = top;

            int track_y2 = bottom;

            int thumb_h = (track_y2 - track_y1) * visible_lines() / total_lines();

            int thumb_y = track_y1 + (track_y2 - track_y1 - thumb_h) * scroll_ / std::max(1, total_lines() - visible_lines());

            XSetForeground(display_, gc_, st.outlineColor_);

            XFillRectangle(display_, window_, gc_, x2_ - 8, thumb_y, 4, thumb_h);

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

            scroll_by(-3);

            return true;

        }

        if (button == 5)

        {

            scroll_by(3);

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        if (in_rect(x, y, x2_ - 28, y1_, x2_ - 4, y1_ + title_bar_height()))

        {

            respond(RESPONSE_DELETE_EVENT);

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

        if (key == XK_Down)

        {

            scroll_by(1);

            return true;

        }

        if (key == XK_Up)

        {

            scroll_by(-1);

            return true;

        }

        if (key == XK_Page_Down)

        {

            scroll_by(visible_lines());

            return true;

        }

        if (key == XK_Page_Up)

        {

            scroll_by(-visible_lines());

            return true;

        }

        if (key == XK_Escape)

        {

            respond(RESPONSE_CLOSE);

            return true;

        }

        return false;

    }

};

//endregion
