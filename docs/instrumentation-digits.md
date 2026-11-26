# Instrumentation widgets — LED digit displays + analog needles (Group F)

Ten widgets in `widgets/components/`, one per directory. Each directory
contains exactly `<Name>.h`, `main_<Name>.cpp`, `CMakeLists.txt`, and
`core/xframe_core.h` (byte-identical to `widgets/core/xframe_core.h`).
Attribution: XFrame.

Group F extends the Instrumentation section (see `docs/instrumentation.md`
for the 44-widget plan): the old 7-segment LED digit displays in 1–6 digit
variants, plus four analog needle meters. Standard XFrame hand-drawn style,
no photorealism. Demos use `xframe::inst_demo_value(min, max, seed)` from
`widgets/components/InstData.h` (included as `#include "../InstData.h"`).

## Common API (all ten)

Constructor (all widgets):

```cpp
Name(Display* display, int screen, Window window, GC gc,
     SStyle_Manager* styles, int x, int y, int width, int height);
```

Key methods, identical on every widget:

- `void set_value(double v)` / `double get_value() const` — settable and
  readable; values clamp to the displayable range.
- `void draw()` — redraws the widget.
- `bool on_button_press(int x, int y, unsigned int button)` — click focuses
  the widget and fires `on_click`; `bool on_button_release(...)`,
  `bool on_motion(int x, int y)` — hover tracking.
- `bool on_key(KeySym key, const char* str)` — only when focused: Up/Right
  steps the value up, Down/Left steps it down, Home resets to the minimum.
- `std::function<void()> on_click` — called on click (and Return/Space).
- `void set_bounds(...)`, `width()`, `height()`, `contains(x, y)`,
  `set_visible`/`get_visible`, `set_enabled`/`get_enabled`,
  `set_style_name`, `set_text`/`get_text`, `style()`.

LED digit displays also have:

- `double max_value() const` — largest displayable value.
- `std::string shown_digits() const` — the digits actually shown,
  zero-padded to the digit count (decimal point not included).
- `int digit_at(int i) const`, `static bool segment_on(int digit, char seg)`,
  `int decimal_point_after() const` — decimal point sits after that digit
  index (-1 when there is none).
- `void set_decimal_places(int dp)` / `int get_decimal_places() const`
  (default 1 for 2–6 digits, 0 for 1 digit).

Analog needles also have:

- `void set_range(double lo, double hi)` / `get_min()` / `get_max()`.
- `double needle_angle_deg() const` — needle angle in degrees from
  12 o'clock (round dials) or from vertical (edgewise / VU), clockwise positive.

Build one widget (from the repo root, on the server):

```
cmake -S widgets/components/InstLedDigits4 -B /tmp/b-digits4 -DCMAKE_CXX_FLAGS="-Wall -Wextra"
cmake --build /tmp/b-digits4   # 0 warnings
```

Tests: `python3 widgets/tests/test_inst_group_f.py` builds all ten with
`-Wall -Wextra`, runs the headless harness (clamping, segment map, needle
angles, pixel movement), runs each demo under Xvfb `:147` (draw / click /
key / clean exit), and compiles every code example in this document.
Result: 10/10 widgets fully PASS, 0 warnings, driver exit 0.

## InstLedDigits1

1-digit vintage red 7-segment LED display. Shows the value zero-padded to 1 digit(s), in the style of old LED counters and clocks: dark bezel, bright red lit segments, faint unlit segments.
Range 0..9, whole digits only.
Up/Down keys step by 1.

Working example:

```cpp
InstLedDigits1 w(display, screen, window, gc, &styles, 20, 20, 90, 110);
w.set_value(7);
w.draw();
```

## InstLedDigits2

2-digit vintage red 7-segment LED display. Shows the value zero-padded to 2 digit(s) with one decimal place before the last digit, in the style of old LED counters and clocks: dark bezel, bright red lit segments, faint unlit segments.
Range 0..9.9, one decimal place (none for the 1-digit display).
Up/Down keys step by 0.1.

Working example:

```cpp
InstLedDigits2 w(display, screen, window, gc, &styles, 20, 20, 140, 110);
w.set_value(42.5);
w.draw();
```

## InstLedDigits3

3-digit vintage red 7-segment LED display. Shows the value zero-padded to 3 digit(s) with one decimal place before the last digit, in the style of old LED counters and clocks: dark bezel, bright red lit segments, faint unlit segments.
Range 0..99.9, one decimal place (none for the 1-digit display).
Up/Down keys step by 0.1.

Working example:

```cpp
InstLedDigits3 w(display, screen, window, gc, &styles, 20, 20, 190, 110);
w.set_value(123.4);
w.draw();
```

## InstLedDigits4

4-digit vintage red 7-segment LED display. Shows the value zero-padded to 4 digit(s) with one decimal place before the last digit, in the style of old LED counters and clocks: dark bezel, bright red lit segments, faint unlit segments.
Range 0..999.9, one decimal place (none for the 1-digit display).
Up/Down keys step by 0.1.

Working example:

```cpp
InstLedDigits4 w(display, screen, window, gc, &styles, 20, 20, 240, 110);
w.set_value(123.4);
w.draw();
```

## InstLedDigits5

5-digit vintage red 7-segment LED display. Shows the value zero-padded to 5 digit(s) with one decimal place before the last digit, in the style of old LED counters and clocks: dark bezel, bright red lit segments, faint unlit segments.
Range 0..9999.9, one decimal place (none for the 1-digit display).
Up/Down keys step by 0.1.

Working example:

```cpp
InstLedDigits5 w(display, screen, window, gc, &styles, 20, 20, 290, 110);
w.set_value(1234.5);
w.draw();
```

## InstLedDigits6

6-digit vintage red 7-segment LED display. Shows the value zero-padded to 6 digit(s) with one decimal place before the last digit, in the style of old LED counters and clocks: dark bezel, bright red lit segments, faint unlit segments.
Range 0..99999.9, one decimal place (none for the 1-digit display).
Up/Down keys step by 0.1.

Working example:

```cpp
InstLedDigits6 w(display, screen, window, gc, &styles, 20, 20, 340, 110);
w.set_value(12345.6);
w.draw();
```

## InstNeedleTapered

Standard tapered pointer on a round dial: the needle is wide at the hub and narrows to the tip, sweeping -135..+135 degrees over the range (default 0..100). Cream dial, major ticks every 20, red zone over the top 10%.
Range 0.0..100.0 by default; `set_range` changes it.

Working example:

```cpp
InstNeedleTapered w(display, screen, window, gc, &styles, 20, 20, 420, 380);
w.set_value(xframe::inst_demo_value(0.0, 100.0, 42));
w.draw();
```

## InstNeedleKnifeEdge

Knife-edge pointer on a round dial: a hair-thin pointer with a pointed tip and a short tail, sweeping -120..+120 degrees. White dial with a parallax mirror arc strip under the scale, ticks every 5, red zone over the top 10%.
Range 0.0..100.0 by default; `set_range` changes it.

Working example:

```cpp
InstNeedleKnifeEdge w(display, screen, window, gc, &styles, 20, 20, 420, 380);
w.set_value(xframe::inst_demo_value(0.0, 100.0, 42));
w.draw();
```

## InstNeedleEdgewise

Edgewise horizontal meter: a flat horizontal scale with the needle pivoting below it and sweeping left-right (-50..+50 degrees from vertical) across the scale. Ticks every 10, labels every 20, red zone over the rightmost 10%.
Range 0.0..100.0 by default; `set_range` changes it.

Working example:

```cpp
InstNeedleEdgewise w(display, screen, window, gc, &styles, 20, 20, 560, 280);
w.set_value(xframe::inst_demo_value(0.0, 100.0, 42));
w.draw();
```

## InstNeedleVu

VU-style needle on a VU meter scale: -20..+3 VU with the standard nonlinear VU spacing (-20, -10, -7, -5, -3, -1, 0, +1, +2, +3), red arc above 0 VU, cream face, thin black needle sweeping -45..+45 degrees.
Range -20.0..3.0 by default; `set_range` changes it.

Working example:

```cpp
InstNeedleVu w(display, screen, window, gc, &styles, 20, 20, 480, 380);
w.set_value(xframe::inst_demo_value(-20.0, 3.0, 42));
w.draw();
```

