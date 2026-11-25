#pragma once

#include "core/xframe_core.h"

//region GtkTextView

class GtkTextView

{

private:

    Display *display_;

    int screen_;

    Window window_;

    GC gc_;

    SStyle_Manager *styles_;

    int x1_, y1_, x2_, y2_;

    std::string name_;

    std::string text_;   // the whole buffer; lines split on '\n'

    std::string style_name_;   // "" = manager's active style

    bool visible_ = true;

    bool enabled_ = true;

    bool focused_ = false;

    int cursor_ = 0;          // byte index into text_

    int top_line_ = 0;        // first visible display line

    bool editable_ = true;

    bool wrap_ = true;

    bool blink_on_ = true;

    int pad_ = 6;

    int line_height()

    {

        SStyle st = style();

        if (st.font_)

        {

            return st.font_->ascent + st.font_->descent + 4;

        }

        return 16;

    }

    int visible_rows()

    {

        return std::max(1, (height() - 2 * pad_) / line_height());

    }

    // Build the display lines as (start, length) pairs into text_,
    // wrapping each logical line at the inner pixel width.

    void build_lines(std::vector<std::pair<int, int>>& out)

    {

        out.clear();

        SStyle st = style();

        int inner = width() - 2 * pad_ - 4;

        int n = (int)text_.size();

        int line_start = 0;

        while (true)

        {

            int nl = (int)text_.find('\n', (size_t)line_start);

            int line_end = (nl == std::string::npos) ? n : nl;

            if (!wrap_ || inner <= 10)

            {

                out.push_back({line_start, line_end - line_start});

            }

            else

            {

                int seg_start = line_start;

                while (true)

                {

                    int seg_len = line_end - seg_start;

                    if (xf::text_width(st, text_.substr((size_t)seg_start, (size_t)seg_len)) <= inner)

                    {

                        out.push_back({seg_start, seg_len});

                        break;

                    }

                    // Find the longest fitting prefix, prefer a space break.

                    int fit = 0;

                    int last_space = -1;

                    for (int i = 1; i <= seg_len; ++i)

                    {

                        if (xf::text_width(st, text_.substr((size_t)seg_start, (size_t)i)) > inner)

                        {

                            break;

                        }

                        fit = i;

                        if (text_[(size_t)(seg_start + i - 1)] == ' ')

                        {

                            last_space = i;

                        }

                    }

                    if (fit <= 0)

                    {

                        fit = 1;

                    }

                    int brk = (last_space > 0) ? last_space : fit;

                    out.push_back({seg_start, brk});

                    seg_start += brk;

                    if (seg_start >= line_end)

                    {

                        break;

                    }

                }

            }

            if (nl == std::string::npos)

            {

                break;

            }

            line_start = nl + 1;

            if (line_start > n)

            {

                out.push_back({n, 0});

                break;

            }

        }

        if (out.empty())

        {

            out.push_back({0, 0});

        }

    }

    // Display line index that contains the cursor.

    int cursor_line(const std::vector<std::pair<int, int>>& lines)

    {

        for (int i = 0; i < (int)lines.size(); ++i)

        {

            int s = lines[(size_t)i].first;

            int e = s + lines[(size_t)i].second;

            if (cursor_ >= s && cursor_ <= e)

            {

                return i;

            }

        }

        return (int)lines.size() - 1;

    }

    void ensure_cursor_visible()

    {

        std::vector<std::pair<int, int>> lines;

        build_lines(lines);

        int cl = cursor_line(lines);

        if (cl < top_line_)

        {

            top_line_ = cl;

        }

        if (cl >= top_line_ + visible_rows())

        {

            top_line_ = cl - visible_rows() + 1;

        }

    }

    void changed()

    {

        if (on_changed)

        {

            on_changed();

        }

    }

public:

    std::function<void()> on_changed;

    GtkTextView(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "GtkTextView";

        text_ = "The quick brown fox jumps over the lazy dog.\n"

                "Pack my box with five dozen liquor jugs.\n"

                "\n"

                "Type here: lines wrap at the window edge,\n"

                "the arrow keys walk the cursor, and the\n"

                "view scrolls to keep the cursor in sight.";

        cursor_ = 0;

    }

    ~GtkTextView() {}

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

        cursor_ = 0;

        top_line_ = 0;

    }

    std::string get_text()

    {

        return text_;

    }

    SStyle style()

    {

        return styles_->resolve(style_name_);

    }

    //region GtkTextView specific API

    void set_editable(bool e)

    {

        editable_ = e;

    }

    bool get_editable()

    {

        return editable_;

    }

    void set_wrap_mode(bool w)

    {

        wrap_ = w;

    }

    bool get_wrap_mode()

    {

        return wrap_;

    }

    int get_cursor_position()

    {

        return cursor_;

    }

    void set_cursor_position(int pos)

    {

        cursor_ = std::max(0, std::min(pos, (int)text_.size()));

        ensure_cursor_visible();

    }

    void get_cursor_line_col(int& line, int& col)

    {

        line = 0;

        col = cursor_;

        for (int i = 0; i < cursor_ && i < (int)text_.size(); ++i)

        {

            if (text_[(size_t)i] == '\n')

            {

                ++line;

                col = cursor_ - i - 1;

            }

        }

    }

    void insert_at_cursor(const std::string& s)

    {

        if (!editable_)

        {

            return;

        }

        text_.insert((size_t)cursor_, s);

        cursor_ += (int)s.size();

        ensure_cursor_visible();

        changed();

    }

    int get_line_count()

    {

        std::vector<std::pair<int, int>> lines;

        build_lines(lines);

        return (int)lines.size();

    }

    void scroll_to_top()

    {

        top_line_ = 0;

    }

    void tick()

    {

        if (focused_)

        {

            blink_on_ = !blink_on_;

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

        SStyle body = st;

        body.fillColor_ = styles_->get_color_rgb(255, 255, 255);

        if (!enabled_)

        {

            body.fillColor_ = styles_->get_color_rgb(235, 235, 235);

        }

        xf::fill_rounded_rect(display_, window_, gc_, body, x1_, y1_, x2_, y2_);

        if (focused_)

        {

            SStyle ring = st;

            ring.lineColor_ = styles_->get_color_rgb(60, 120, 200);

            ring.lineWidth_ = 2;

            xf::draw_rounded_outline(display_, window_, gc_, ring, x1_, y1_, x2_, y2_);

        }

        else

        {

            xf::draw_rounded_outline(display_, window_, gc_, st, x1_, y1_, x2_, y2_);

        }

        std::vector<std::pair<int, int>> lines;

        build_lines(lines);

        int lh = line_height();

        int rows = visible_rows();

        SStyle ink = st;

        if (!enabled_)

        {

            ink.text_color_ = styles_->get_color_rgb(140, 140, 140);

        }

        int ascent = st.font_ ? st.font_->ascent : 11;

        for (int row = 0; row < rows; ++row)

        {

            int li = top_line_ + row;

            if (li >= (int)lines.size())

            {

                break;

            }

            int s = lines[(size_t)li].first;

            int len = lines[(size_t)li].second;

            int baseline = y1_ + pad_ + row * lh + ascent;

            xf::draw_text_at(display_, window_, gc_, ink, x1_ + pad_, baseline, text_.substr((size_t)s, (size_t)len));

        }

        // Cursor bar on its display line.

        if (focused_ && enabled_ && blink_on_)

        {

            int cl = cursor_line(lines);

            int row = cl - top_line_;

            if (row >= 0 && row < rows)

            {

                int s = lines[(size_t)cl].first;

                int cx = x1_ + pad_ + xf::text_width(st, text_.substr((size_t)s, (size_t)(cursor_ - s)));

                int ty = y1_ + pad_ + row * lh;

                XSetForeground(display_, gc_, styles_->get_color_rgb(20, 20, 20));

                XDrawLine(display_, window_, gc_, cx, ty + 1, cx, ty + lh - 3);

            }

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

        blink_on_ = true;

        std::vector<std::pair<int, int>> lines;

        build_lines(lines);

        int row = (y - y1_ - pad_) / line_height();

        int li = std::max(0, std::min(top_line_ + row, (int)lines.size() - 1));

        int s = lines[(size_t)li].first;

        int len = lines[(size_t)li].second;

        int px = x - x1_ - pad_;

        int best = 0;

        int best_dist = 1 << 30;

        SStyle st = style();

        for (int i = 0; i <= len; ++i)

        {

            int d = xf::text_width(st, text_.substr((size_t)s, (size_t)i)) - px;

            if (d < 0)

            {

                d = -d;

            }

            if (d < best_dist)

            {

                best_dist = d;

                best = i;

            }

        }

        cursor_ = s + best;

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

        if (!focused_ || !enabled_)

        {

            return false;

        }

        blink_on_ = true;

        if (key == XK_Left)

        {

            if (cursor_ > 0)

            {

                --cursor_;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Right)

        {

            if (cursor_ < (int)text_.size())

            {

                ++cursor_;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Home)

        {

            while (cursor_ > 0 && text_[(size_t)(cursor_ - 1)] != '\n')

            {

                --cursor_;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_End)

        {

            while (cursor_ < (int)text_.size() && text_[(size_t)cursor_] != '\n')

            {

                ++cursor_;

            }

            ensure_cursor_visible();

            return true;

        }

        if (key == XK_Up || key == XK_Down)

        {

            // Move by logical line, keeping the column where possible.

            int line, col;

            get_cursor_line_col(line, col);

            int target = (key == XK_Up) ? line - 1 : line + 1;

            // Find the start of the target logical line.

            int pos = 0;

            int cur = 0;

            while (cur < target && pos < (int)text_.size())

            {

                if (text_[(size_t)pos] == '\n')

                {

                    ++cur;

                }

                ++pos;

            }

            if (cur == target)

            {

                int end = pos;

                while (end < (int)text_.size() && text_[(size_t)end] != '\n')

                {

                    ++end;

                }

                cursor_ = std::min(pos + col, end);

                ensure_cursor_visible();

            }

            return true;

        }

        if (key == XK_Page_Up)

        {

            top_line_ = std::max(0, top_line_ - visible_rows());

            return true;

        }

        if (key == XK_Page_Down)

        {

            std::vector<std::pair<int, int>> lines;

            build_lines(lines);

            top_line_ = std::min(std::max(0, (int)lines.size() - visible_rows()), top_line_ + visible_rows());

            return true;

        }

        if (!editable_)

        {

            return false;

        }

        if (key == XK_Return || key == XK_KP_Enter)

        {

            text_.insert((size_t)cursor_, 1, '\n');

            ++cursor_;

            ensure_cursor_visible();

            changed();

            return true;

        }

        if (key == XK_BackSpace)

        {

            if (cursor_ > 0)

            {

                text_.erase((size_t)(cursor_ - 1), 1);

                --cursor_;

                ensure_cursor_visible();

                changed();

            }

            return true;

        }

        if (key == XK_Delete)

        {

            if (cursor_ < (int)text_.size())

            {

                text_.erase((size_t)cursor_, 1);

                changed();

            }

            return true;

        }

        if (key == XK_Tab)

        {

            return false;   // let Tab move focus, like the real widget

        }

        if (str && str[0] >= 32 && str[0] != 127)

        {

            for (int i = 0; str[i]; ++i)

            {

                if (str[i] < 32 || str[i] == 127)

                {

                    continue;

                }

                text_.insert((size_t)cursor_, 1, str[i]);

                ++cursor_;

            }

            ensure_cursor_visible();

            changed();

            return true;

        }

        return false;

    }

};

//endregion
