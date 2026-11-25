#pragma once

#include "core/xframe_core.h"

#include <cctype>

#include <cmath>

#include <map>

#include <set>

#include <sstream>

//region XSpreadsheet

//
// A full spreadsheet grid in the XFrame format: column letters and
// row numbers, per-cell text / numbers / formulas, range selection,
// an in-cell editor, column resizing by dragging the header border,
// frozen header row and column, sorting by a column, and a real
// formula engine - expressions with + - * / ( ), A1 cell references,
// and SUM / AVG / MIN / MAX / COUNT over ranges like A1:B9, with
// dependency-ordered recalculation and cycle detection.
//

class XSpreadsheet

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

    struct Cell

    {

        std::string text;        // what the user typed

        double value = 0.0;      // computed value (number or formula)

        bool is_number = false;  // text parses as a number

        bool is_formula = false; // text starts with '='

        bool error = false;      // formula failed (ref / cycle / parse)

    };

    int cols_ = 12;

    int rows_ = 40;

    std::vector<std::vector<Cell>> cells_;   // [col][row]

    std::vector<int> col_widths_;

    int row_height_ = 20;

    int header_w_ = 46;          // row-number gutter

    int header_h_ = 22;          // column-letter band

    int active_col_ = 0;

    int active_row_ = 0;

    int sel_col_ = 0;            // selection far corner (range)

    int sel_row_ = 0;

    bool selecting_ = false;

    int scroll_col_ = 0;

    int scroll_row_ = 0;

    int frozen_cols_ = 1;        // row numbers are always frozen;

    int frozen_rows_ = 0;        // set 1 to freeze the first data row

    bool editing_ = false;

    std::string edit_text_;

    int edit_cursor_ = 0;

    int resizing_col_ = -1;      // header border being dragged

    int resize_start_x_ = 0;

    int resize_start_w_ = 0;

    std::string clipboard_;      // last copied range, TSV

public:

    std::function<void(int, int)> on_cell_change;   // (col, row)

    std::function<void(int, int)> on_select_cell;   // (col, row)

    XSpreadsheet(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)

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

        name_ = "XSpreadsheet";

        text_ = "Spreadsheet";

        cells_.assign(cols_, std::vector<Cell>(rows_));

        col_widths_.assign(cols_, 84);

        // Preload a small working example: a parts list with a
        // formula column and totals, so the demo shows the engine.

        set_cell_text(0, 0, "Part");

        set_cell_text(1, 0, "Qty");

        set_cell_text(2, 0, "Price");

        set_cell_text(3, 0, "Total");

        const char* parts[5] = {"Widget", "Gadget", "Sprocket", "Gizmo", "Doohickey"};

        const char* qty[5] = {"12", "5", "40", "8", "3"};

        const char* price[5] = {"9.50", "24.00", "3.75", "15.20", "49.99"};

        for (int i = 0; i < 5; ++i)

        {

            set_cell_text(0, i + 1, parts[i]);

            set_cell_text(1, i + 1, qty[i]);

            set_cell_text(2, i + 1, price[i]);

            std::string f = "=B" + std::to_string(i + 2) + "*C" + std::to_string(i + 2);

            set_cell_text(3, i + 1, f);

        }

        set_cell_text(0, 6, "Grand total");

        set_cell_text(3, 6, "=SUM(D2:D6)");

        set_cell_text(0, 7, "Average line");

        set_cell_text(3, 7, "=AVG(D2:D6)");

        recalculate();

    }

    ~XSpreadsheet() {}

    //region Standard plumbing

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

    //endregion

    //region Grid model

    void set_size(int cols, int rows)

    {

        cols_ = std::max(1, cols);

        rows_ = std::max(1, rows);

        cells_.assign(cols_, std::vector<Cell>(rows_));

        col_widths_.assign(cols_, 84);

        active_col_ = active_row_ = sel_col_ = sel_row_ = 0;

        scroll_col_ = scroll_row_ = 0;

    }

    int get_cols()

    {

        return cols_;

    }

    int get_rows()

    {

        return rows_;

    }

    void set_col_width(int col, int w)

    {

        if (col >= 0 && col < cols_)

        {

            col_widths_[col] = std::max(24, w);

        }

    }

    int get_col_width(int col)

    {

        return (col >= 0 && col < cols_) ? col_widths_[col] : 0;

    }

    void set_frozen_rows(int n)

    {

        frozen_rows_ = std::max(0, std::min(n, rows_ - 1));

    }

    void set_frozen_cols(int n)

    {

        frozen_cols_ = std::max(0, std::min(n, cols_ - 1));

    }

    static std::string col_name(int col)

    {

        std::string s;

        int c = col;

        do

        {

            s = char('A' + (c % 26)) + s;

            c = c / 26 - 1;

        } while (c >= 0);

        return s;

    }

    static bool parse_ref(const std::string& s, size_t& pos, int& col, int& row)

    {

        size_t i = pos;

        col = 0;

        bool any_letter = false;

        while (i < s.size() && std::isalpha((unsigned char)s[i]))

        {

            col = col * 26 + (std::toupper((unsigned char)s[i]) - 'A' + 1);

            ++i;

            any_letter = true;

        }

        if (!any_letter)

        {

            return false;

        }

        --col;

        row = 0;

        bool any_digit = false;

        while (i < s.size() && std::isdigit((unsigned char)s[i]))

        {

            row = row * 10 + (s[i] - '0');

            ++i;

            any_digit = true;

        }

        if (!any_digit)

        {

            return false;

        }

        --row;

        pos = i;

        return true;

    }

    void set_cell_text(int col, int row, const std::string& text)

    {

        if (col < 0 || col >= cols_ || row < 0 || row >= rows_)

        {

            return;

        }

        Cell& c = cells_[col][row];

        c.text = text;

        c.is_formula = (!text.empty() && text[0] == '=');

        c.error = false;

        if (!c.is_formula)

        {

            char* end = nullptr;

            c.value = std::strtod(text.c_str(), &end);

            c.is_number = (end != text.c_str() && *end == 0);

            if (!c.is_number)

            {

                c.value = 0.0;

            }

        }

    }

    std::string get_cell_text(int col, int row)

    {

        if (col < 0 || col >= cols_ || row < 0 || row >= rows_)

        {

            return "";

        }

        return cells_[col][row].text;

    }

    double get_cell_value(int col, int row)

    {

        if (col < 0 || col >= cols_ || row < 0 || row >= rows_)

        {

            return 0.0;

        }

        return cells_[col][row].value;

    }

    std::string get_cell_display(int col, int row)

    {

        if (col < 0 || col >= cols_ || row < 0 || row >= rows_)

        {

            return "";

        }

        Cell& c = cells_[col][row];

        if (c.text.empty())

        {

            return "";

        }

        if (c.is_formula)

        {

            if (c.error)

            {

                return "#ERR";

            }

            return format_number(c.value);

        }

        if (c.is_number)

        {

            return format_number(c.value);

        }

        return c.text;

    }

    static std::string format_number(double v)

    {

        if (std::fabs(v - std::round(v)) < 1e-9 && std::fabs(v) < 1e15)

        {

            return std::to_string((long long)std::round(v));

        }

        std::ostringstream oss;

        oss.precision(6);

        oss << std::fixed << v;

        std::string s = oss.str();

        while (!s.empty() && s.back() == '0')

        {

            s.pop_back();

        }

        if (!s.empty() && s.back() == '.')

        {

            s.pop_back();

        }

        return s;

    }

    int get_active_col()

    {

        return active_col_;

    }

    int get_active_row()

    {

        return active_row_;

    }

    void set_active_cell(int col, int row)

    {

        active_col_ = std::max(0, std::min(col, cols_ - 1));

        active_row_ = std::max(0, std::min(row, rows_ - 1));

        sel_col_ = active_col_;

        sel_row_ = active_row_;

        ensure_visible(active_col_, active_row_);

    }

    void sort_by_column(int col, bool ascending = true)

    {

        if (col < 0 || col >= cols_ || rows_ < 2)

        {

            return;

        }

        std::vector<int> order(rows_);

        for (int i = 0; i < rows_; ++i)

        {

            order[i] = i;

        }

        std::stable_sort(order.begin(), order.end(), [&](int a, int b) {

            const Cell& ca = cells_[col][a];

            const Cell& cb = cells_[col][b];

            int cmp = 0;

            if (ca.is_number && cb.is_number)

            {

                cmp = (ca.value < cb.value) ? -1 : (ca.value > cb.value ? 1 : 0);

            } else {

                cmp = ca.text.compare(cb.text);

            }

            return ascending ? (cmp < 0) : (cmp > 0);

        });

        std::vector<std::vector<Cell>> next = cells_;

        for (int c = 0; c < cols_; ++c)

        {

            for (int r = 0; r < rows_; ++r)

            {

                next[c][r] = cells_[c][order[r]];

            }

        }

        cells_ = next;

        recalculate();

    }

    std::string copy_selection()

    {

        int c0 = std::min(active_col_, sel_col_);

        int c1 = std::max(active_col_, sel_col_);

        int r0 = std::min(active_row_, sel_row_);

        int r1 = std::max(active_row_, sel_row_);

        std::string out;

        for (int r = r0; r <= r1; ++r)

        {

            for (int c = c0; c <= c1; ++c)

            {

                if (c > c0)

                {

                    out += '\t';

                }

                out += get_cell_display(c, r);

            }

            if (r < r1)

            {

                out += '\n';

            }

        }

        clipboard_ = out;

        return out;

    }

    std::string get_clipboard()

    {

        return clipboard_;

    }

    //endregion

    //region Formula engine

    void recalculate()

    {

        std::set<long> visiting;

        std::set<long> done;

        for (int c = 0; c < cols_; ++c)

        {

            for (int r = 0; r < rows_; ++r)

            {

                if (cells_[c][r].is_formula)

                {

                    eval_cell(c, r, visiting, done);

                }

            }

        }

    }

    double eval_cell(int col, int row, std::set<long>& visiting, std::set<long>& done)

    {

        if (col < 0 || col >= cols_ || row < 0 || row >= rows_)

        {

            return 0.0;

        }

        Cell& c = cells_[col][row];

        if (!c.is_formula)

        {

            return c.is_number ? c.value : 0.0;

        }

        long key = (long)col * 1000000 + row;

        if (done.count(key))

        {

            return c.value;

        }

        if (visiting.count(key))

        {

            c.error = true;   // circular reference

            return 0.0;

        }

        visiting.insert(key);

        size_t pos = 1;   // skip '='

        bool ok = true;

        double v = parse_expr(c.text, pos, visiting, done, ok);

        if (pos < c.text.size())

        {

            ok = false;   // trailing junk

        }

        c.error = !ok;

        c.value = ok ? v : 0.0;

        visiting.erase(key);

        done.insert(key);

        return c.value;

    }

    void skip_spaces(const std::string& s, size_t& pos)

    {

        while (pos < s.size() && s[pos] == ' ')

        {

            ++pos;

        }

    }

    double parse_expr(const std::string& s, size_t& pos, std::set<long>& visiting, std::set<long>& done, bool& ok)

    {

        double v = parse_term(s, pos, visiting, done, ok);

        while (ok)

        {

            skip_spaces(s, pos);

            if (pos < s.size() && (s[pos] == '+' || s[pos] == '-'))

            {

                char op = s[pos++];

                double rhs = parse_term(s, pos, visiting, done, ok);

                v = (op == '+') ? v + rhs : v - rhs;

            } else {

                break;

            }

        }

        return v;

    }

    double parse_term(const std::string& s, size_t& pos, std::set<long>& visiting, std::set<long>& done, bool& ok)

    {

        double v = parse_factor(s, pos, visiting, done, ok);

        while (ok)

        {

            skip_spaces(s, pos);

            if (pos < s.size() && (s[pos] == '*' || s[pos] == '/'))

            {

                char op = s[pos++];

                double rhs = parse_factor(s, pos, visiting, done, ok);

                if (op == '*')

                {

                    v *= rhs;

                } else {

                    if (rhs == 0.0)

                    {

                        ok = false;

                        return 0.0;

                    }

                    v /= rhs;

                }

            } else {

                break;

            }

        }

        return v;

    }

    double parse_factor(const std::string& s, size_t& pos, std::set<long>& visiting, std::set<long>& done, bool& ok)

    {

        skip_spaces(s, pos);

        if (pos >= s.size())

        {

            ok = false;

            return 0.0;

        }

        if (s[pos] == '(')

        {

            ++pos;

            double v = parse_expr(s, pos, visiting, done, ok);

            skip_spaces(s, pos);

            if (pos >= s.size() || s[pos] != ')')

            {

                ok = false;

                return 0.0;

            }

            ++pos;

            return v;

        }

        if (s[pos] == '-')

        {

            ++pos;

            return -parse_factor(s, pos, visiting, done, ok);

        }

        if (std::isdigit((unsigned char)s[pos]) || s[pos] == '.')

        {

            char* end = nullptr;

            double v = std::strtod(s.c_str() + pos, &end);

            pos += (size_t)(end - (s.c_str() + pos));

            return v;

        }

        if (std::isalpha((unsigned char)s[pos]))

        {

            // Function name? letters followed by '('

            size_t save = pos;

            std::string fname;

            while (pos < s.size() && std::isalpha((unsigned char)s[pos]))

            {

                fname += char(std::toupper((unsigned char)s[pos]));

                ++pos;

            }

            skip_spaces(s, pos);

            if (pos < s.size() && s[pos] == '(' &&

                (fname == "SUM" || fname == "AVG" || fname == "MIN" || fname == "MAX" || fname == "COUNT"))

            {

                ++pos;

                int c0 = 0, r0 = 0, c1 = 0, r1 = 0;

                skip_spaces(s, pos);

                if (!parse_ref(s, pos, c0, r0))

                {

                    ok = false;

                    return 0.0;

                }

                skip_spaces(s, pos);

                if (pos < s.size() && s[pos] == ':')

                {

                    ++pos;

                    skip_spaces(s, pos);

                    if (!parse_ref(s, pos, c1, r1))

                    {

                        ok = false;

                        return 0.0;

                    }

                } else {

                    c1 = c0;

                    r1 = r0;

                }

                skip_spaces(s, pos);

                if (pos >= s.size() || s[pos] != ')')

                {

                    ok = false;

                    return 0.0;

                }

                ++pos;

                return apply_function(fname, c0, r0, c1, r1, visiting, done, ok);

            }

            // Plain cell reference

            pos = save;

            int col = 0, row = 0;

            if (!parse_ref(s, pos, col, row))

            {

                ok = false;

                return 0.0;

            }

            return eval_cell(col, row, visiting, done);

        }

        ok = false;

        return 0.0;

    }

    double apply_function(const std::string& fname, int c0, int r0, int c1, int r1, std::set<long>& visiting, std::set<long>& done, bool& ok)

    {

        int ca = std::min(c0, c1), cb = std::max(c0, c1);

        int ra = std::min(r0, r1), rb = std::max(r0, r1);

        double sum = 0.0;

        double mn = 0.0, mx = 0.0;

        int count = 0;

        for (int c = ca; c <= cb; ++c)

        {

            for (int r = ra; r <= rb; ++r)

            {

                if (c < 0 || c >= cols_ || r < 0 || r >= rows_)

                {

                    continue;

                }

                Cell& cell = cells_[c][r];

                bool numeric = cell.is_number || cell.is_formula;

                if (!numeric || cell.text.empty())

                {

                    continue;

                }

                double v = eval_cell(c, r, visiting, done);

                if (cell.error)

                {

                    ok = false;

                    return 0.0;

                }

                sum += v;

                if (count == 0 || v < mn)

                {

                    mn = v;

                }

                if (count == 0 || v > mx)

                {

                    mx = v;

                }

                ++count;

            }

        }

        if (fname == "SUM" || fname == "COUNT")

        {

            return fname == "SUM" ? sum : (double)count;

        }

        if (count == 0)

        {

            ok = false;

            return 0.0;

        }

        if (fname == "AVG")

        {

            return sum / count;

        }

        if (fname == "MIN")

        {

            return mn;

        }

        return mx;   // MAX

    }

    //endregion

    //region Geometry

    int col_left(int col)

    {

        int x = x1_ + header_w_;

        int start = (col < frozen_cols_) ? 0 : scroll_col_ + frozen_cols_;

        int from = (col < frozen_cols_) ? 0 : frozen_cols_;

        (void)start;

        for (int c = from; c < col; ++c)

        {

            if (c < frozen_cols_ || c >= scroll_col_ + frozen_cols_)

            {

                x += col_widths_[c];

            }

        }

        if (col >= frozen_cols_)

        {

            // frozen columns occupy their width first

            x = x1_ + header_w_;

            for (int c = 0; c < frozen_cols_; ++c)

            {

                x += col_widths_[c];

            }

            for (int c = scroll_col_ + frozen_cols_; c < col; ++c)

            {

                x += col_widths_[c];

            }

        }

        return x;

    }

    int row_top(int row)

    {

        if (row < frozen_rows_)

        {

            return y1_ + header_h_ + row * row_height_;

        }

        return y1_ + header_h_ + frozen_rows_ * row_height_ + (row - scroll_row_ - frozen_rows_) * row_height_;

    }

    void visible_cols(int& first, int& last)

    {

        first = scroll_col_ + frozen_cols_;

        last = first;

        int x = col_left(first);

        while (last < cols_ && x < x2_)

        {

            x += col_widths_[last];

            ++last;

        }

    }

    void visible_rows(int& first, int& last)

    {

        first = scroll_row_ + frozen_rows_;

        last = first;

        int y = row_top(first);

        while (last < rows_ && y < y2_)

        {

            y += row_height_;

            ++last;

        }

    }

    bool cell_at(int x, int y, int& col, int& row)

    {

        if (x < x1_ + header_w_ || y < y1_ + header_h_)

        {

            return false;

        }

        // frozen columns

        int cx = x1_ + header_w_;

        for (int c = 0; c < frozen_cols_; ++c)

        {

            if (x < cx + col_widths_[c])

            {

                col = c;

                goto col_done;

            }

            cx += col_widths_[c];

        }

        for (int c = scroll_col_ + frozen_cols_; c < cols_; ++c)

        {

            if (x < cx + col_widths_[c])

            {

                col = c;

                goto col_done;

            }

            cx += col_widths_[c];

        }

        return false;

    col_done:

        int cy = y1_ + header_h_;

        for (int r = 0; r < frozen_rows_; ++r)

        {

            if (y < cy + row_height_)

            {

                row = r;

                return true;

            }

            cy += row_height_;

        }

        for (int r = scroll_row_ + frozen_rows_; r < rows_; ++r)

        {

            if (y < cy + row_height_)

            {

                row = r;

                return true;

            }

            cy += row_height_;

        }

        return false;

    }

    void ensure_visible(int col, int row)

    {

        if (col >= frozen_cols_)

        {

            if (col < scroll_col_ + frozen_cols_)

            {

                scroll_col_ = col - frozen_cols_;

            }

            int first = 0, last = 0;

            visible_cols(first, last);

            while (col >= last && scroll_col_ < cols_)

            {

                ++scroll_col_;

                visible_cols(first, last);

            }

        }

        if (row >= frozen_rows_)

        {

            if (row < scroll_row_ + frozen_rows_)

            {

                scroll_row_ = row - frozen_rows_;

            }

            int first = 0, last = 0;

            visible_rows(first, last);

            while (row >= last && scroll_row_ < rows_)

            {

                ++scroll_row_;

                visible_rows(first, last);

            }

        }

    }

    //endregion

    //region Editing

    void begin_edit(bool replace)

    {

        editing_ = true;

        focused_ = true;

        edit_text_ = replace ? "" : cells_[active_col_][active_row_].text;

        edit_cursor_ = (int)edit_text_.size();

    }

    void commit_edit()

    {

        if (!editing_)

        {

            return;

        }

        editing_ = false;

        set_cell_text(active_col_, active_row_, edit_text_);

        recalculate();

        if (on_cell_change)

        {

            on_cell_change(active_col_, active_row_);

        }

    }

    void cancel_edit()

    {

        editing_ = false;

    }

    //endregion

    void draw()

    {

        if (!visible_)

        {

            return;

        }

        SStyle st = style();

        unsigned long white = styles_->get_color_rgb(255, 255, 255);

        unsigned long grid = styles_->get_color_rgb(205, 205, 205);

        unsigned long header_fill = styles_->get_color_rgb(232, 232, 232);

        unsigned long active_fill = styles_->get_color_rgb(198, 220, 245);

        unsigned long sel_fill = styles_->get_color_rgb(220, 235, 250);

        // Body

        XSetForeground(display_, gc_, white);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        int fc0 = 0, lc0 = 0, fr0 = 0, lr0 = 0;

        visible_cols(fc0, lc0);

        visible_rows(fr0, lr0);

        auto draw_cell = [&](int c, int r) {

            int cx = col_left(c);

            int cy = row_top(r);

            int cw = col_widths_[c];

            bool in_sel = c >= std::min(active_col_, sel_col_) && c <= std::max(active_col_, sel_col_) &&

                          r >= std::min(active_row_, sel_row_) && r <= std::max(active_row_, sel_row_);

            if (in_sel && !(c == active_col_ && r == active_row_))

            {

                XSetForeground(display_, gc_, sel_fill);

                XFillRectangle(display_, window_, gc_, cx, cy, cw, row_height_);

            }

            XSetForeground(display_, gc_, grid);

            XDrawRectangle(display_, window_, gc_, cx, cy, cw, row_height_);

            std::string disp = get_cell_display(c, r);

            if (!disp.empty() && !(editing_ && c == active_col_ && r == active_row_))

            {

                Cell& cell = cells_[c][r];

                bool numeric = cell.is_number || cell.is_formula;

                if (numeric)

                {

                    int tw = xf::text_width(st, disp);

                    xf::draw_text_at(display_, window_, gc_, st, cx + cw - tw - 4, cy + row_height_ - 5, disp);

                } else {

                    xf::draw_text_at(display_, window_, gc_, st, cx + 4, cy + row_height_ - 5, disp);

                }

            }

        };

        for (int c = 0; c < frozen_cols_; ++c)

        {

            for (int r = 0; r < frozen_rows_; ++r)

            {

                draw_cell(c, r);

            }

            for (int r = fr0; r < lr0; ++r)

            {

                draw_cell(c, r);

            }

        }

        for (int c = fc0; c < lc0; ++c)

        {

            for (int r = 0; r < frozen_rows_; ++r)

            {

                draw_cell(c, r);

            }

            for (int r = fr0; r < lr0; ++r)

            {

                draw_cell(c, r);

            }

        }

        // Column headers

        XSetForeground(display_, gc_, header_fill);

        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), header_h_);

        auto draw_col_header = [&](int c) {

            int cx = col_left(c);

            int cw = col_widths_[c];

            bool active = (c >= std::min(active_col_, sel_col_) && c <= std::max(active_col_, sel_col_));

            if (active)

            {

                XSetForeground(display_, gc_, active_fill);

                XFillRectangle(display_, window_, gc_, cx, y1_, cw, header_h_);

            }

            XSetForeground(display_, gc_, grid);

            XDrawRectangle(display_, window_, gc_, cx, y1_, cw, header_h_);

            std::string nm = col_name(c);

            int tw = xf::text_width(st, nm);

            xf::draw_text_at(display_, window_, gc_, st, cx + (cw - tw) / 2, y1_ + header_h_ - 6, nm);

        };

        for (int c = 0; c < frozen_cols_; ++c)

        {

            draw_col_header(c);

        }

        for (int c = fc0; c < lc0; ++c)

        {

            draw_col_header(c);

        }

        // Row headers

        XSetForeground(display_, gc_, header_fill);

        XFillRectangle(display_, window_, gc_, x1_, y1_ + header_h_, header_w_, height() - header_h_);

        auto draw_row_header = [&](int r) {

            int cy = row_top(r);

            bool active = (r >= std::min(active_row_, sel_row_) && r <= std::max(active_row_, sel_row_));

            if (active)

            {

                XSetForeground(display_, gc_, active_fill);

                XFillRectangle(display_, window_, gc_, x1_, cy, header_w_, row_height_);

            }

            XSetForeground(display_, gc_, grid);

            XDrawRectangle(display_, window_, gc_, x1_, cy, header_w_, row_height_);

            std::string nm = std::to_string(r + 1);

            int tw = xf::text_width(st, nm);

            xf::draw_text_at(display_, window_, gc_, st, x1_ + (header_w_ - tw) / 2, cy + row_height_ - 5, nm);

        };

        for (int r = 0; r < frozen_rows_; ++r)

        {

            draw_row_header(r);

        }

        for (int r = fr0; r < lr0; ++r)

        {

            draw_row_header(r);

        }

        // Corner box

        XSetForeground(display_, gc_, header_fill);

        XFillRectangle(display_, window_, gc_, x1_, y1_, header_w_, header_h_);

        XSetForeground(display_, gc_, grid);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, header_w_, header_h_);

        // Frozen separators

        if (frozen_rows_ > 0)

        {

            int fy = y1_ + header_h_ + frozen_rows_ * row_height_;

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 2);

            XDrawLine(display_, window_, gc_, x1_, fy, x2_, fy);

            xf::set_line_width(display_, gc_, 1);

        }

        // Active cell outline + editor

        {

            int cx = col_left(active_col_);

            int cy = row_top(active_row_);

            int cw = col_widths_[active_col_];

            XSetForeground(display_, gc_, st.lineColor_);

            xf::set_line_width(display_, gc_, 2);

            XDrawRectangle(display_, window_, gc_, cx, cy, cw, row_height_);

            xf::set_line_width(display_, gc_, 1);

            if (editing_)

            {

                XSetForeground(display_, gc_, white);

                XFillRectangle(display_, window_, gc_, cx + 1, cy + 1, cw - 2, row_height_ - 2);

                xf::draw_text_at(display_, window_, gc_, st, cx + 4, cy + row_height_ - 5, edit_text_);

                int tw = xf::text_width(st, edit_text_.substr(0, edit_cursor_));

                XSetForeground(display_, gc_, st.text_color_);

                XDrawLine(display_, window_, gc_, cx + 4 + tw, cy + 3, cx + 4 + tw, cy + row_height_ - 3);

            }

        }

        // Outer frame

        XSetForeground(display_, gc_, st.lineColor_);

        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());

        XFlush(display_);

    }

    bool on_button_press(int x, int y, unsigned int button)

    {

        if (!contains(x, y) || !enabled_)

        {

            return false;

        }

        focused_ = true;

        if (button == 4)   // wheel up

        {

            scroll_row_ = std::max(0, scroll_row_ - 3);

            return true;

        }

        if (button == 5)   // wheel down

        {

            scroll_row_ = std::min(std::max(0, rows_ - 1), scroll_row_ + 3);

            return true;

        }

        if (button != 1)

        {

            return false;

        }

        // Column resize: within 3px of a header border

        if (y >= y1_ && y < y1_ + header_h_ && x >= x1_ + header_w_)

        {

            for (int c = 0; c < cols_; ++c)

            {

                int edge = col_left(c) + col_widths_[c];

                if (std::abs(x - edge) <= 3)

                {

                    resizing_col_ = c;

                    resize_start_x_ = x;

                    resize_start_w_ = col_widths_[c];

                    return true;

                }

            }

        }

        int c = 0, r = 0;

        if (cell_at(x, y, c, r))

        {

            if (editing_ && !(c == active_col_ && r == active_row_))

            {

                commit_edit();

            }

            bool dbl = false;   // (double-click starts edit below via key path too)

            (void)dbl;

            if (c == active_col_ && r == active_row_ && focused_)

            {

                // second press on the active cell begins editing

                if (!editing_)

                {

                    begin_edit(false);

                    return true;

                }

            }

            active_col_ = c;

            active_row_ = r;

            sel_col_ = c;

            sel_row_ = r;

            selecting_ = true;

            if (on_select_cell)

            {

                on_select_cell(c, r);

            }

            return true;

        }

        // Header clicks select whole column / row

        if (y >= y1_ && y < y1_ + header_h_ && x >= x1_ + header_w_)

        {

            for (int c = 0; c < cols_; ++c)

            {

                if (x >= col_left(c) && x < col_left(c) + col_widths_[c])

                {

                    active_col_ = c;

                    sel_col_ = c;

                    active_row_ = 0;

                    sel_row_ = rows_ - 1;

                    return true;

                }

            }

        }

        if (x >= x1_ && x < x1_ + header_w_ && y >= y1_ + header_h_)

        {

            int r = 0, dummy = 0;

            if (cell_at(x1_ + header_w_ + 1, y, dummy, r))

            {

                active_row_ = r;

                sel_row_ = r;

                active_col_ = 0;

                sel_col_ = cols_ - 1;

                return true;

            }

        }

        return true;

    }

    bool on_button_release(int x, int y, unsigned int button)

    {

        (void)x;

        (void)y;

        if (button != 1)

        {

            return false;

        }

        bool was = selecting_ || resizing_col_ >= 0;

        selecting_ = false;

        resizing_col_ = -1;

        return was;

    }

    bool on_motion(int x, int y)

    {

        if (resizing_col_ >= 0)

        {

            col_widths_[resizing_col_] = std::max(24, resize_start_w_ + (x - resize_start_x_));

            return true;

        }

        if (selecting_)

        {

            int c = 0, r = 0;

            if (cell_at(x, y, c, r))

            {

                sel_col_ = c;

                sel_row_ = r;

                ensure_visible(c, r);

                return true;

            }

        }

        return false;

    }

    bool on_key(KeySym key, const char* str)

    {

        if (!focused_ || !enabled_)

        {

            return false;

        }

        if (editing_)

        {

            if (key == XK_Return || key == XK_KP_Enter)

            {

                commit_edit();

                set_active_cell(active_col_, std::min(rows_ - 1, active_row_ + 1));

                return true;

            }

            if (key == XK_Escape)

            {

                cancel_edit();

                return true;

            }

            if (key == XK_Tab)

            {

                commit_edit();

                set_active_cell(std::min(cols_ - 1, active_col_ + 1), active_row_);

                return true;

            }

            if (key == XK_BackSpace)

            {

                if (edit_cursor_ > 0)

                {

                    edit_text_.erase(edit_cursor_ - 1, 1);

                    --edit_cursor_;

                }

                return true;

            }

            if (key == XK_Delete)

            {

                if (edit_cursor_ < (int)edit_text_.size())

                {

                    edit_text_.erase(edit_cursor_, 1);

                }

                return true;

            }

            if (key == XK_Left)

            {

                edit_cursor_ = std::max(0, edit_cursor_ - 1);

                return true;

            }

            if (key == XK_Right)

            {

                edit_cursor_ = std::min((int)edit_text_.size(), edit_cursor_ + 1);

                return true;

            }

            if (key == XK_Home)

            {

                edit_cursor_ = 0;

                return true;

            }

            if (key == XK_End)

            {

                edit_cursor_ = (int)edit_text_.size();

                return true;

            }

            if (str && str[0] >= 32 && str[0] < 127)

            {

                edit_text_.insert(edit_cursor_, 1, str[0]);

                ++edit_cursor_;

                return true;

            }

            return false;

        }

        // Not editing: navigation + start typing to edit

        int c = active_col_, r = active_row_;

        switch (key)

        {

        case XK_Left:  c = std::max(0, c - 1); break;

        case XK_Right: c = std::min(cols_ - 1, c + 1); break;

        case XK_Up:    r = std::max(0, r - 1); break;

        case XK_Down:  r = std::min(rows_ - 1, r + 1); break;

        case XK_Page_Up:   r = std::max(0, r - 10); break;

        case XK_Page_Down: r = std::min(rows_ - 1, r + 10); break;

        case XK_Home:  c = 0; break;

        case XK_End:   c = cols_ - 1; break;

        case XK_Return:

        case XK_KP_Enter:

            begin_edit(false);

            return true;

        case XK_F2:

            begin_edit(false);

            return true;

        case XK_Delete:

        case XK_BackSpace:

            set_cell_text(c, r, "");

            recalculate();

            if (on_cell_change)

            {

                on_cell_change(c, r);

            }

            return true;

        default:

            if (str && str[0] >= 32 && str[0] < 127)

            {

                begin_edit(true);

                edit_text_ = std::string(1, str[0]);

                edit_cursor_ = 1;

                return true;

            }

            return false;

        }

        set_active_cell(c, r);

        if (on_select_cell)

        {

            on_select_cell(c, r);

        }

        return true;

    }

};

//endregion
