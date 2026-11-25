# Misc / X

Everything that fits nowhere else: a transparent event catcher, the application metadata holder, and the two custom X-prefixed widgets — a chart and a spreadsheet — that are not modelled on any GTK or Lazarus control.

> Every widget here shares the common XFrame widget API — same constructor, set_bounds/draw/event handlers, visible/enabled state. The shared contract is documented in docs/index.md (written by the coordinator). Each subsection below lists only what is specific to that widget.

## GtkEventBox

A transparent event-catching box. By default it draws nothing at all — it exists to receive mouse and keyboard events over an area (for example over a child widget) and report them. With a visible window it draws a pale box with its text; in design-time mode it draws a marker box instead.

**Constructor:** `GtkEventBox(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the text drawn when the window is visible
- `set_above_child(bool a)` / `get_above_child()` — whether the box receives events before its child does
- `set_visible_window(bool v)` / `get_visible_window()` — whether the box draws a visible background (pale fill + outline + centred text)
- `set_design_time(bool d)` — design-time mode: draws a marker box so the box can be seen in a designer
- `get_last_event()` — description of the most recent event the box received
- `get_event_count()` — how many events the box has received
- signal `on_click` (`std::function<void()>`) — fires on a click in the box

**Example:**
```cpp
#include "GtkEventBox.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkEventBox - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    GtkEventBox widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.set_text("EventBox");
    widget.set_visible_window(true);
    widget.on_click = []{ std::printf("clicked\n"); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** With both visible_window and design_time off, draw() paints nothing — the widget is invisible but still receives events.

## TApplicationProperties

Application metadata with a visual badge — not a non-visual component, despite the name. It draws a round "i" badge plus a "<title> v<version>" label, and holds the application's title, version, hint text and help file. handle_exception() records an exception and fires on_exception.

**Constructor:** `TApplicationProperties(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the widget text
- `set_title(std::string t)` / `get_title()` — the application title shown in the badge label
- `set_version(std::string v)` / `get_version()` — the application version (drawn as "v" + version)
- `set_hint(std::string h)` / `get_hint()` — the hint text
- `set_help_file(std::string f)` / `get_help_file()` — the help file path
- `set_show_main_form(bool s)` / `get_show_main_form()` — whether the main form is shown
- `set_show_hint(bool s)` / `get_show_hint()` — whether hints are shown
- `handle_exception(const std::string& message)` — record an exception: it is counted, remembered as the last exception, and on_exception fires with the message
- `get_exception_count()` — how many exceptions have been recorded
- `get_last_exception()` — the most recent exception message
- signal `on_click` (`std::function<void()>`) — fires on a click on the badge
- signal `on_exception` (`std::function<void(const std::string&)>`) — fires with the exception message when handle_exception() is called
- signal `on_minimize` (`std::function<void()>`) — declared on the widget
- signal `on_restore` (`std::function<void()>`) — declared on the widget

**Example:**
```cpp
#include "TApplicationProperties.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "TApplicationProperties - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    TApplicationProperties widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.set_title("Demo App");
    widget.set_version("1.0");
    widget.set_hint("A demo application");
    widget.on_exception = [](const std::string& m){ std::printf("exception: %s\n", m.c_str()); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** The on_minimize and on_restore signals are declared but no call site for either appears in the header — nothing in the widget fires them, so it is unclear from the header how the host is meant to trigger them.

## XGraph

A chart widget — one of the two custom X-prefixed widgets, not modelled on a GTK or Lazarus control. It draws a white plot area with a grid and axes, and plots line series, OHLC bars or candlesticks. The view pans and zooms: zoom_at() zooms around a point in the view, reset_view() restores the full range, and on_view_change reports the new view.

**Constructor:** `XGraph(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the widget text
- `item_count()` — total number of data points across all series
- `clear_series()` — removes all series
- `add_line_series(const std::string& name, Srgb color, const std::vector<double>& values)` — adds a line series (kind 0)
- `add_ohlc_series(const std::string& name, Srgb color, const std::vector<XGraphBar>& bars, bool candlesticks = false)` — adds an OHLC bar series (kind 1) or a candlestick series (kind 2); each `XGraphBar` is `{ double open, high, low, close; }`
- `set_series_kind(int index, int kind)` — change a series kind: 0 = line, 1 = OHLC bars, 2 = candlesticks
- `set_series_up_down(int index, Srgb up, Srgb down)` — up / down colours for a candlestick series
- `series_count()` — number of series
- `set_x_labels(const std::vector<std::string>& labels)` — labels for the x axis, one per item
- `set_y_range(double lo, double hi)` — fixed y range (turns auto-y off)
- `set_auto_y(bool on)` — automatic y range from the visible data (on by default)
- `reset_view()` — show the whole series again
- `get_view_first()` / `get_view_count()` — first visible item index / number of visible items
- `zoom_at(double factor, double frac_x)` — zoom by a factor around a fractional position in the view
- `effective_count()` — number of items the view covers after zooming
- `y_range(double& lo, double& hi)` — the y range actually being drawn
- `plot_x()` / `plot_y()` / `plot_w()` / `plot_h()` — the plot area rectangle in pixels
- `item_w()` — pixel width of one item
- `x_at(double index)` / `x_center(int i)` — pixel x for an item index
- `y_at(double v, double lo, double hi)` — pixel y for a value
- `value_at_y(int y, double lo, double hi)` — value for a pixel y
- `fire_view_change()` — fire on_view_change() with the current view
- `static double nice_step(double range, int target)` — a "nice" axis step for a range (used by draw() for the grid lines)
- `static std::string fmt(double v)` — formats a value for the axis labels (used by draw())
- signal `on_view_change` (`std::function<void(double, double)>`) — fires with (first visible index, visible count) when the view changes

**Example:**
```cpp
#include "XGraph.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "XGraph - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    XGraph widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.add_line_series("Price", Srgb{70, 110, 180}, std::vector<double>{1.0, 2.0, 1.5, 3.0, 2.5});
    widget.set_x_labels(std::vector<std::string>{"Mon", "Tue", "Wed", "Thu", "Fri"});
    widget.set_auto_y(true);
    widget.on_view_change = [](double a, double b){ std::printf("view %f %f\n", a, b); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Series kinds are plain ints (0 / 1 / 2) — there are no named constants in the header. Panning and zooming come from mouse / key input handled by the widget; the host can drive the same view with zoom_at() and reset_view().

## XSpreadsheet

A spreadsheet widget — the other custom X-prefixed widget. It draws a grid with a frozen row-number column, a header row, an active cell and a selection, and it evaluates formulas. Cells hold text, numbers or formulas starting with '='; the formula engine supports + - * / ( ), A1 cell references and SUM / AVG / MIN / MAX / COUNT over ranges like A1:B9. The constructor pre-fills a sample sheet with values, a formula column and totals.

**Constructor:** `XSpreadsheet(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height)`

**API** (widget-specific methods/properties/signals ONLY — skip the common ones):
- `set_text(std::string t)` / `get_text()` — the widget text
- `set_size(int cols, int rows)` — resize the sheet (default 12 columns x 40 rows)
- `get_cols()` / `get_rows()` — sheet size
- `set_col_width(int col, int w)` / `get_col_width(int col)` — width of one column
- `set_frozen_rows(int n)` / `set_frozen_cols(int n)` — frozen rows / columns (1 column is frozen by default, for the row numbers; 0 rows frozen by default)
- `set_cell_text(int col, int row, const std::string& text)` / `get_cell_text(int col, int row)` — the raw text of a cell (a formula when it starts with '=')
- `get_cell_value(int col, int row)` — the computed numeric value of a cell
- `get_cell_display(int col, int row)` — the text as drawn (computed value for formulas)
- `get_active_col()` / `get_active_row()` — the active cell
- `set_active_cell(int col, int row)` — make a cell active
- `sort_by_column(int col, bool ascending = true)` — sort the rows by one column
- `copy_selection()` — the selected range as tab-separated text
- `get_clipboard()` — the last copied text
- `recalculate()` — re-evaluate every formula
- `eval_cell(int col, int row, std::set<long>& visiting, std::set<long>& done)` — evaluate one cell (formula-engine entry point; the visiting / done sets are cycle detection)
- `skip_spaces(const std::string& s, size_t& pos)` / `parse_expr(...)` / `parse_term(...)` / `parse_factor(...)` / `apply_function(...)` — the formula parser internals; public, but not part of the normal API
- `col_left(int col)` / `row_top(int row)` — pixel position of a column / row
- `visible_cols(int& first, int& last)` / `visible_rows(int& first, int& last)` — which columns / rows are on screen
- `cell_at(int x, int y, int& col, int& row)` — cell under a point
- `ensure_visible(int col, int row)` — scroll so a cell is visible
- `begin_edit(bool replace)` / `commit_edit()` / `cancel_edit()` — start / finish / abandon editing the active cell
- `static std::string col_name(int col)` — column letters for an index ("A", "B", ...)
- `static std::string format_number(double v)` — formats a numeric cell for display
- `static bool parse_ref(const std::string& s, size_t& pos, int& col, int& row)` — parse an A1-style cell reference out of a formula string
- signal `on_cell_change` (`std::function<void(int, int)>`) — fires with (column, row) when a cell's text changes
- signal `on_select_cell` (`std::function<void(int, int)>`) — fires with (column, row) when the active cell changes

**Example:**
```cpp
#include "XSpreadsheet.h"
#include <cstdio>
#include <string>
#include <vector>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "XSpreadsheet - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    XSpreadsheet widget(display, screen, window, gc, &styles, 20, 20, 720, 480);
    widget.set_size(6, 10);
    widget.set_cell_text(0, 0, "Item");
    widget.set_cell_text(0, 1, "10");
    widget.set_cell_text(0, 2, "20");
    widget.set_cell_text(0, 3, "=SUM(A2:A3)");
    widget.set_active_cell(0, 3);
    widget.on_cell_change = [](int c, int r){ std::printf("cell %d,%d\n", c, r); };
    widget.draw();
    bool running = true;
    while (running)
    {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type)
        {
        case Expose:
            handled = true;
            break;
        case ButtonPress:
            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case ButtonRelease:
            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            break;
        case MotionNotify:
            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);
            break;
        case KeyPress:
        {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)
            {
                running = false;
            }
            break;
        }
        if (handled)
        {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

**Notes:** Columns and rows are 0-based in the API; the drawn row numbers start at 1. The constructor's sample sheet has a formula column and totals — call set_size() to start from a rebuilt grid. A failed formula (bad reference, circular reference, parse failure) marks the cell as an error and evaluates to 0.0; the exact text drawn for an error cell is unclear from the header.

