# CWidgets Documentation

**XFrame** — a pure X11/Xlib hand-drawn C++17 widget library. Every widget is
drawn by hand with raw Xlib calls. There is no GTK here, no Qt, no toolkit at
all — despite the `Gtk*` names. The `Gtk*` components recreate the look of
GTK widgets as standalone X11 classes; the `T*` components are the same idea
in Lazarus/Delphi style; plus `XGraph` and `XSpreadsheet`.

Start with the [project README](../README.md) for the repo layout, build
instructions, and test status. This index documents the widget API itself:
the contract every widget shares, then one page per section with every
widget in it.

## Contents

| Section | Widgets | Page |
|---|---|---|
| Windows & Dialogs | 28 | [windows-dialogs.md](windows-dialogs.md) |
| Containers & Layout | 34 | [containers-layout.md](containers-layout.md) |
| Buttons | 24 | [buttons.md](buttons.md) |
| Display | 33 | [display.md](display.md) |
| Text & Entry | 28 | [text-entry.md](text-entry.md) |
| Choosers | 10 | [choosers.md](choosers.md) |
| Views & Lists | 17 | [views-lists.md](views-lists.md) |
| Menus & Toolbars | 16 | [menus-toolbars.md](menus-toolbars.md) |
| Misc / X | 4 | [misc-x.md](misc-x.md) |
| Stock Market | 287 | [stock-market.md](stock-market.md) |
| **Total** | **481** | |

Section assignment is defined in `tools/sections.py` and verified by
`tools/sections.py` itself (run it: `python3 tools/sections.py`) — every
widget belongs to exactly one section.

## The shared widget contract

This is the part that is the same for **all 481 widgets** (verified by
inspection across all 481 headers: every member below exists in every
header). Each section page therefore lists only what is *specific* to each
widget — constructor, widget-specific methods, signals — and does not repeat
this contract.

### Construction

Every widget has the identical constructor:

```cpp
<Name>(Display* display, int screen, Window window, GC gc,
       SStyle_Manager* styles, int x, int y, int width, int height)
```

- `display`, `screen`, `window`, `gc` — the X connection and the window the
  widget draws into, from `XOpenDisplay` / `XCreateSimpleWindow` /
  `DefaultGC`.
- `styles` — an `SStyle_Manager` (from `core/xframe_core.h`) holding the
  named styles. Register at least a `"default"` style with
  `add_style_ex(...)` and activate it with `set_active_style_byName(...)`
  before constructing widgets.
- `x, y, width, height` — the widget's initial bounds in window coordinates.

### Geometry

```cpp
void set_bounds(int x, int y, int width, int height);
int  width();    // x2_ - x1_
int  height();   // y2_ - y1_
bool contains(int x, int y);  // false when not visible
```

### Visibility and enabled state

```cpp
void set_visible(bool v);
bool get_visible();
void set_enabled(bool e);
bool get_enabled();
```

A disabled widget still draws, but greyed out (text colour forced to grey)
and its event handlers refuse input. An invisible widget does not draw and
`contains()` returns false for it.

### Style

```cpp
void   set_style_name(std::string s);  // "" = the manager's active style
SStyle style();                        // styles_->resolve(style_name_)
```

### Drawing

```cpp
void draw();
```

Draws the widget into its window with the resolved style. Returns
immediately when the widget is not visible. Call it after construction and
again after any handled event (the standard demo loop clears the window and
redraws on every handled event).

### Events

Every widget implements the same four handlers. Each returns `true` when it
handled the event (the demo loop then clears and redraws) and `false`
otherwise:

```cpp
bool on_button_press(int x, int y, unsigned int button);   // button 1 = left
bool on_button_release(int x, int y, unsigned int button);
bool on_motion(int x, int y);
bool on_key(KeySym key, const char* str);
```

Typical behaviour: `on_button_press` sets `pressed_`/`focused_` when the
click lands inside the widget; `on_button_release` fires the widget's
callback if the release is still inside; `on_key` fires the callback on
Return/Space when the widget has focus. `on_motion` is a no-op on most
widgets — only a few headers carry hover state, and the test suite found
only 12 of the original 194 demos show a visible hover response. Do not assume a widget
reacts to hover unless its section page says so.

### Signals

Widgets expose their callbacks as **public `std::function` members**, e.g.:

```cpp
std::function<void()> on_click;   // the most common signal (85 widgets)
```

Assign them like any member:

```cpp
widget.on_click = [] { printf("clicked\n"); };
```

Signal names and signatures vary by widget — `on_change`, `on_response`,
`on_toggled`, `on_value_changed`, `on_selection_changed`,
`on_color_changed`, `on_page_selected`, ... — with payloads such as
`void(int)`, `void(std::string)`, `void(bool)`, `void(double)`,
`void(Srgb)`, `void(TTreeNode&)`. The exact name and signature for each
widget are listed on its section page. Never guess a signal name: if it is
not in the header, it does not exist.

## Minimal program skeleton

Every demo in `components/<Name>/main_<Name>.cpp` follows this shape
(shown here for `GtkButton` — the section pages trim it further per widget):

```cpp
#include "GtkButton.h"
#include <cstdio>

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
        100, 100, 800, 600, 1,
        BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "GtkButton - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask |
        StructureNotifyMask | ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask);
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDelete, 1);
    XMapWindow(display, window);

    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120},
        Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1,
        "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");

    GtkButton widget(display, screen, window, gc, &styles, 40, 40, 340, 200);
    widget.on_click = [] { printf("clicked\n"); };
    widget.draw();

    bool running = true;
    while (running) {
        XEvent event;
        XNextEvent(display, &event);
        bool handled = false;
        switch (event.type) {
        case Expose:        handled = true; break;
        case ButtonPress:   handled = widget.on_button_press(
                                event.xbutton.x, event.xbutton.y,
                                event.xbutton.button); break;
        case ButtonRelease: handled = widget.on_button_release(
                                event.xbutton.x, event.xbutton.y,
                                event.xbutton.button); break;
        case MotionNotify:  handled = widget.on_motion(
                                event.xmotion.x, event.xmotion.y); break;
        case KeyPress: {
            char buffer[32];
            KeySym key = 0;
            int n = XLookupString(&event.xkey, buffer, sizeof(buffer) - 1,
                                  &key, nullptr);
            buffer[n] = 0;
            handled = widget.on_key(key, buffer);
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == wmDelete) running = false;
            break;
        }
        if (handled) {
            XClearWindow(display, window);
            widget.draw();
            XFlush(display);
        }
    }
    XCloseDisplay(display);
    return 0;
}
```

Build a single example (from the repo root, with the header's component
directory on the include path):

```sh
g++ -std=c++17 -I components/GtkButton example.cpp -lX11 -o example
```

Or build the shipped demo instead — `components/<Name>/CMakeLists.txt`
builds it standalone, and the top-level build builds all 481 (see the
README).

## Tools

- **Selector palette** — `palette/` builds `cwidgets_palette` (part of the
  top-level build). A pure X11/XFrame window listing all 481 widgets in
  their 10 sections, each with its 16x16 icon. Click (or Up/Down + Enter)
  to select; the selection is written to the shared selection file
  (`/tmp/cwidgets-selection.txt`) for the layout tool. Self-test:
  481/481 selections OK.
- **Layout window** — `layout/` builds `cwidgets_layout`. Instantiates the
  real widget classes via the generated factory in `common/registry.h`
  (generated by `tools/gen_registry.py` — do not edit by hand). Click to
  place the selected widget, drag to move, Delete to remove, `s` to
  save/load the layout, `q` to quit. Double-click a placed widget for 8
  orange resize handles (corners + edges); drag a handle to resize live;
  Escape dismisses the handles, Delete removes the widget.
- **Icons** — `icons/<Name>.ico`, one real 16x16 ICO per widget,
  purpose-drawn glyphs generated by `tools/gen_icons.py` (70 glyph kinds,
  section accent colours — not downscaled screenshots). Verified by
  `tools/verify_icons.py`: 481/481 parse as 16x16, all non-blank.

## Test status

Tested Nov 26, 2026 on the VPS (Debian 12, g++ 12.2, Xvfb) with the
autonomous driver `test-driver/run_tests.py`: all 194 original demos build with
0 warnings (`-Wall -Wextra`); 193 draw (GtkInvisible is non-visual by
design); 125 show a visible click response (the rest are display-only by
design); all 194 survive keypresses and exit cleanly; zero crashes, zero
hangs. Full table: `test-results/results.md`. Stock Market section: OHLC 8/8,
comparative/volume 15/15, drawing 14/14, trend 85/85, momentum 85/85,
volatility/other 80/80 (see [stock-market.md](stock-market.md)). Palette self-test 481/481;
layout placement 481/481, draw 481/481, all 481 alive at once
(`test-results/palette_layout_results.csv`).

## License

MIT — see [LICENSE](../LICENSE). Copyright (c) 2026 cnmcdee.
