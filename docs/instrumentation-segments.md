# Instrumentation widgets — higher-segment alphanumeric LED displays (Group G)

Twelve widgets in `widgets/components/`, one per directory. Each directory
contains exactly `<Name>.h`, `main_<Name>.cpp`, `CMakeLists.txt`, and
`core/xframe_core.h` (byte-identical to `widgets/core/xframe_core.h`).
Attribution: XFrame.

Group G extends the Instrumentation section (see `docs/instrumentation.md`
for the plan): the higher-segment displays that can roughly show
alphanumeric text — 14-segment in 1–6 digit variants, and 16-segment in
1–6 digit variants. The honest word is "roughly": a segment display is
not a dot matrix, and the tables below say exactly which characters each
type shows well, which it only approximates, and which it cannot show at
all. Standard XFrame hand-drawn style, no photorealism. Demos use
deterministic strings ("HELLO", "ABC123", "42.5").

## Common API (all twelve)

Constructor (all widgets):

```cpp
Name(Display* display, int screen, Window window, GC gc,
     SStyle_Manager* styles, int x, int y, int width, int height);
```

Key methods, identical on every widget:

- `void set_value(const std::string& v)` / `std::string get_value() const` — the displayed string; normalised to uppercase, truncated to the digit count, characters with no approximation dropped, `.`/`:` become a decimal-point dot after the previous digit.
- `std::string shown_text() const` — the string actually shown, padded with spaces to the digit count.
- `int cell_count() const`, `char char_at(int i) const`.
- `static bool segment_on(char ch, const std::string& seg)` — is that segment lit for that character?
- `static bool renders_clean(char ch)` / `renders_approx(char ch)` / `renders_blank(char ch)`; `static std::string clean_repertoire()` / `approx_repertoire()` / `cannot_repertoire()`.
- `static constexpr int kDigits`, `static constexpr int kSegments` (14 or 16), `static const char* default_value()`.
- `void draw()` — redraws the widget.
- `bool on_button_press(int x, int y, unsigned int button)` — click focuses the widget and fires `on_click`; `bool on_button_release(...)`, `bool on_motion(int x, int y)` — hover tracking.
- `bool on_key(KeySym key, const char* str)` — only when focused: Up/Right steps the last displayed character (digit wraps 9→0, letter wraps Z→A), Down/Left steps it down, Home restores the default demo string, Return/Space fires `on_click`.
- `std::function<void()> on_click`, `void set_bounds(...)`, `width()`, `height()`, `contains(x, y)`, `set_visible`/`get_visible`, `set_enabled`/`get_enabled`, `set_style_name`, `set_text`/`get_text`, `style()`.

Build one widget (from the repo root, on the server):

```
cmake -S widgets/components/InstSeg14Display4 -B /tmp/b-seg14-4 -DCMAKE_CXX_FLAGS="-Wall -Wextra"
cmake --build /tmp/b-seg14-4   # 0 warnings
```

Run the whole group (build + harness + demos + doc examples):

```
python3 widgets/tests/test_inst_group_g.py   # ALL PASS, 12/12 widgets
```

## InstSeg14Display1


14-segment alphanumeric LED display, 1 digit. The classic
14-segment font: the seven bars plus two half middle bars, two centre
verticals and four diagonals. Shows A–Z, 0–9 and common punctuation
roughly — M and W are poor (no centre peak), and K, Q, R, V, Y and Z
are approximations. Demo string: `A`.


Repertoire (14-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L O P S T U X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K M N Q R V W Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

M and W have no centre peak on 14 segments, so they read as rough approximations; K, Q, R, V, Y and Z need diagonals the font only hints at.


Example:

```cpp
InstSeg14Display1 w(display, screen, window, gc, &styles, 20, 20, 90, 110);
w.set_value("A");
w.draw();
```


## InstSeg14Display2


14-segment alphanumeric LED display, 2 digits. The classic
14-segment font: the seven bars plus two half middle bars, two centre
verticals and four diagonals. Shows A–Z, 0–9 and common punctuation
roughly — M and W are poor (no centre peak), and K, Q, R, V, Y and Z
are approximations. Demo string: `42`.


Repertoire (14-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L O P S T U X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K M N Q R V W Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

M and W have no centre peak on 14 segments, so they read as rough approximations; K, Q, R, V, Y and Z need diagonals the font only hints at.


Example:

```cpp
InstSeg14Display2 w(display, screen, window, gc, &styles, 20, 20, 140, 110);
w.set_value("42");
w.draw();
```


## InstSeg14Display3


14-segment alphanumeric LED display, 3 digits. The classic
14-segment font: the seven bars plus two half middle bars, two centre
verticals and four diagonals. Shows A–Z, 0–9 and common punctuation
roughly — M and W are poor (no centre peak), and K, Q, R, V, Y and Z
are approximations. Demo string: `ABC`.


Repertoire (14-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L O P S T U X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K M N Q R V W Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

M and W have no centre peak on 14 segments, so they read as rough approximations; K, Q, R, V, Y and Z need diagonals the font only hints at.


Example:

```cpp
InstSeg14Display3 w(display, screen, window, gc, &styles, 20, 20, 190, 110);
w.set_value("ABC");
w.draw();
```


## InstSeg14Display4


14-segment alphanumeric LED display, 4 digits. The classic
14-segment font: the seven bars plus two half middle bars, two centre
verticals and four diagonals. Shows A–Z, 0–9 and common punctuation
roughly — M and W are poor (no centre peak), and K, Q, R, V, Y and Z
are approximations. Demo string: `HELL`.


Repertoire (14-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L O P S T U X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K M N Q R V W Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

M and W have no centre peak on 14 segments, so they read as rough approximations; K, Q, R, V, Y and Z need diagonals the font only hints at.


Example:

```cpp
InstSeg14Display4 w(display, screen, window, gc, &styles, 20, 20, 240, 110);
w.set_value("HELL");
w.draw();
```


## InstSeg14Display5


14-segment alphanumeric LED display, 5 digits. The classic
14-segment font: the seven bars plus two half middle bars, two centre
verticals and four diagonals. Shows A–Z, 0–9 and common punctuation
roughly — M and W are poor (no centre peak), and K, Q, R, V, Y and Z
are approximations. Demo string: `HELLO`.


Repertoire (14-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L O P S T U X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K M N Q R V W Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

M and W have no centre peak on 14 segments, so they read as rough approximations; K, Q, R, V, Y and Z need diagonals the font only hints at.


Example:

```cpp
InstSeg14Display5 w(display, screen, window, gc, &styles, 20, 20, 290, 110);
w.set_value("HELLO");
w.draw();
```


## InstSeg14Display6


14-segment alphanumeric LED display, 6 digits. The classic
14-segment font: the seven bars plus two half middle bars, two centre
verticals and four diagonals. Shows A–Z, 0–9 and common punctuation
roughly — M and W are poor (no centre peak), and K, Q, R, V, Y and Z
are approximations. Demo string: `ABC123`.


Repertoire (14-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L O P S T U X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K M N Q R V W Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

M and W have no centre peak on 14 segments, so they read as rough approximations; K, Q, R, V, Y and Z need diagonals the font only hints at.


Example:

```cpp
InstSeg14Display6 w(display, screen, window, gc, &styles, 20, 20, 340, 110);
w.set_value("ABC123");
w.draw();
```


## InstSeg16Display1


16-segment alphanumeric LED display, 1 digit. The 14-segment
font with the top and bottom bars split in two, which — together with
the centre verticals — gives M and W a real middle peak and handles
more punctuation better. K, Q, R, V, Y and Z are still approximations.
Demo string: `M`.


Repertoire (16-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L M O P S T U W X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K N Q R V Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

The split top/bottom bars plus the centre verticals give M and W a real middle peak, so they read cleanly here. K, Q, R, V, Y and Z are still only approximate.


Example:

```cpp
InstSeg16Display1 w(display, screen, window, gc, &styles, 20, 20, 90, 110);
w.set_value("M");
w.draw();
```


## InstSeg16Display2


16-segment alphanumeric LED display, 2 digits. The 14-segment
font with the top and bottom bars split in two, which — together with
the centre verticals — gives M and W a real middle peak and handles
more punctuation better. K, Q, R, V, Y and Z are still approximations.
Demo string: `MW`.


Repertoire (16-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L M O P S T U W X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K N Q R V Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

The split top/bottom bars plus the centre verticals give M and W a real middle peak, so they read cleanly here. K, Q, R, V, Y and Z are still only approximate.


Example:

```cpp
InstSeg16Display2 w(display, screen, window, gc, &styles, 20, 20, 140, 110);
w.set_value("MW");
w.draw();
```


## InstSeg16Display3


16-segment alphanumeric LED display, 3 digits. The 14-segment
font with the top and bottom bars split in two, which — together with
the centre verticals — gives M and W a real middle peak and handles
more punctuation better. K, Q, R, V, Y and Z are still approximations.
Demo string: `MWM`.


Repertoire (16-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L M O P S T U W X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K N Q R V Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

The split top/bottom bars plus the centre verticals give M and W a real middle peak, so they read cleanly here. K, Q, R, V, Y and Z are still only approximate.


Example:

```cpp
InstSeg16Display3 w(display, screen, window, gc, &styles, 20, 20, 190, 110);
w.set_value("MWM");
w.draw();
```


## InstSeg16Display4


16-segment alphanumeric LED display, 4 digits. The 14-segment
font with the top and bottom bars split in two, which — together with
the centre verticals — gives M and W a real middle peak and handles
more punctuation better. K, Q, R, V, Y and Z are still approximations.
Demo string: `MWMW`.


Repertoire (16-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L M O P S T U W X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K N Q R V Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

The split top/bottom bars plus the centre verticals give M and W a real middle peak, so they read cleanly here. K, Q, R, V, Y and Z are still only approximate.


Example:

```cpp
InstSeg16Display4 w(display, screen, window, gc, &styles, 20, 20, 240, 110);
w.set_value("MWMW");
w.draw();
```


## InstSeg16Display5


16-segment alphanumeric LED display, 5 digits. The 14-segment
font with the top and bottom bars split in two, which — together with
the centre verticals — gives M and W a real middle peak and handles
more punctuation better. K, Q, R, V, Y and Z are still approximations.
Demo string: `MWMWM`.


Repertoire (16-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L M O P S T U W X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K N Q R V Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

The split top/bottom bars plus the centre verticals give M and W a real middle peak, so they read cleanly here. K, Q, R, V, Y and Z are still only approximate.


Example:

```cpp
InstSeg16Display5 w(display, screen, window, gc, &styles, 20, 20, 290, 110);
w.set_value("MWMWM");
w.draw();
```


## InstSeg16Display6


16-segment alphanumeric LED display, 6 digits. The 14-segment
font with the top and bottom bars split in two, which — together with
the centre verticals — gives M and W a real middle peak and handles
more punctuation better. K, Q, R, V, Y and Z are still approximations.
Demo string: `MWMWMW`.


Repertoire (16-segment):


| Class | Characters | Notes |
|---|---|---|
| Clean | 0–9, A B C D E F G H I J L M O P S T U W X, - _ = + / | Render distinctly; verified pixel-distinct in the harness, except the look-alikes below. |
| Approximate | K N Q R V Y Z ' " : | Renders something recognisable, but only roughly. |
| Cannot show | ; , ? ! @ # $ % & * ( ) [ ] { } < > \ | ^ ` ~ | Renders blank (no lit segments). |
| Look-alikes | 0/O, 5/S, 6/G, J/U | Each pair renders identically on segment displays. |

The split top/bottom bars plus the centre verticals give M and W a real middle peak, so they read cleanly here. K, Q, R, V, Y and Z are still only approximate.


Example:

```cpp
InstSeg16Display6 w(display, screen, window, gc, &styles, 20, 20, 340, 110);
w.set_value("MWMWMW");
w.draw();
```

