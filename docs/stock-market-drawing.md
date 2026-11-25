# Stock Market — Drawing / Annotation Widgets

XFrame Stock Market section, drawing group (14 widgets). Every widget in
this group takes an `xframe::OHLCVSeries` (see
`widgets/components/StockData.h`, seeded synthetic data only — no network,
no live market data), draws a light candlestick background from it, then
draws its annotation in a strong colour. All are header-only classes in
`widgets/components/<Name>/<Name>.h`, following the `XGraph` pattern
(`Display*` / `screen_` / `window_` / `GC`, geometry, `draw()`,
`on_button_press` / `on_button_release` / `on_motion` / `on_key`).
Demos: `widgets/components/<Name>/main_<Name>.cpp` (800×600 window,
widget at 20,20,720,480, 60 seeded bars, seed 42), `CMakeLists.txt`
(CMake ≥ 3.20, C++17, X11 only).

Common API (all 14): constructor
`(Display*, int screen, Window, GC, SStyle_Manager*, x, y, width, height)`;
`set_series(const xframe::OHLCVSeries&)` (also resets anchors to seeded
defaults); `set_anchors(int bar1, double price1, int bar2, double price2)`;
`set_anchors3(...)` (3 anchors); `set_anchor(int i, int bar, double price)`;
`anchor(int i)` / `anchor_count()`; `set_color(Srgb)` / `get_color()`;
`add_point(int bar, double price)` / `clear_points()` / `point_count()`
(meaningful only for StockBrush); `set_text(std::string)` /
`get_text()`; `reset_anchors()`; `bar_count()`; `draw()`.
Interaction (all 14): left-click moves the nearest anchor to the clicked
bar/price (StockBrush: click adds a point instead); drag moves the
dragged anchor / extends the brush path; arrow keys shift all anchors
(Left/Right ±1 bar, Up/Down ±2% of the price range); `r` / `Home` resets
anchors. Tests: `widgets/tests/test_stock_drawing.py`
(build/draw/hover/click/key/clean-exit per widget, 14/14 PASS, 0 warnings).

## StockTrendline

Draws a trend line through two anchors, extended across the full plot
width using the slope between them, with filled circles at both anchors
and a `Trend <p0> -> <p1>` label at the second anchor.

API: `set_anchors(b1, p1, b2, p2)` — the two points the line passes through.

Example:
```cpp
StockTrendline w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(5, series[5].close, 25, series[25].close);
w.draw();
```

Unclear from the header: the line is extrapolated to the plot edges, not
clipped to the segment between the anchors; if both anchors share a bar
the slope is treated as 0 (flat line at the first anchor's price).

## StockHorizontalLine

Draws a full-width horizontal line at one price level, with a filled tag
at the right edge showing `H <price>`.

API: `set_anchors(bar, price, bar, price)` — only the first anchor's
price is drawn; the bar indices are stored but ignored.

Example:
```cpp
StockHorizontalLine w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(0, series[30].close, 0, series[30].close);
w.draw();
```

Unclear from the header: clicking/dragging moves the line to the clicked
price (the bar component of the move has no visible effect); arrow-key
Left/Right therefore also has no visible effect on this widget.

## StockVerticalLine

Draws a full-height vertical line at one bar index, labelled
`Bar <n>` at the top.

API: `set_anchors(bar, price, bar, price)` — only the first anchor's bar
is drawn; the prices are stored but ignored.

Example:
```cpp
StockVerticalLine w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(30, series[30].close, 30, series[30].close);
w.draw();
```

Unclear from the header: the line is drawn at the centre of the bar's
slot; Up/Down arrow keys (price shift) have no visible effect here.

## StockRectangle

Draws a rectangle between two anchors (bar/price corners), pale-blue
fill with a strong-colour outline, plus filled circles at both corners.

API: `set_anchors(b1, p1, b2, p2)` — opposite corners; order does not
matter, the min/max of each axis is used.

Example:
```cpp
StockRectangle w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(10, series[10].low, 30, series[30].high);
w.draw();
```

Unclear from the header: the fill is a fixed pale blue, not the
annotation colour set by `set_color()` (only the outline follows it);
a zero-width or zero-height rectangle still draws a 1 px line.

## StockEllipse

Draws an ellipse inscribed in the rectangle between two anchors,
pale-blue fill with a strong-colour outline.

API: `set_anchors(b1, p1, b2, p2)` — opposite corners of the bounding
box; order does not matter.

Example:
```cpp
StockEllipse w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(10, series[10].low, 30, series[30].high);
w.draw();
```

Unclear from the header: same fixed pale-blue fill as StockRectangle;
the ellipse is axis-aligned only (no rotation parameter).

## StockFibRetracement

Draws horizontal Fibonacci retracement levels between a high and a low
anchor: 0%, 23.6%, 38.2%, 50%, 61.8%, 78.6%, 100%, each labelled at the
right edge. Alternating levels are dashed.

API: `set_anchors(b_high, high, b_low, low)`; `set_high_low(hi, lo)`;
`level_count()` (= 7); `level_ratio(int i)`; `level_price(int i)`.

Example:
```cpp
StockFibRetracement w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_high_low(hi, lo);  // hi/lo from the series range
w.draw();
```

Unclear from the header: high/low are normalised (swapped if inverted),
so the anchor order does not matter; the anchors' bar indices do not
affect where the levels are drawn (levels span the full plot width);
labels can run past the right plot edge on narrow windows.

## StockFibExtension

Draws horizontal Fibonacci extension levels beyond the anchor range:
0%, 38.2%, 61.8%, 100%, 127.2%, 161.8%, each labelled at the right edge.

API: `set_anchors(b_high, high, b_low, low)`; `set_high_low(hi, lo)`;
`level_count()` (= 6); `level_ratio(int i)`; `level_price(int i)`.
`level_price(i)` returns `low + (high - low) * ratio`, so 127.2% and
161.8% lie beyond the high.

Example:
```cpp
StockFibExtension w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_high_low(hi, lo);
w.draw();
```

Unclear from the header: same normalisation and full-width behaviour as
StockFibRetracement; the 0%/38.2%/61.8%/100% levels duplicate the
retracement levels by design (they are the shared part of the scale).

## StockTextAnnotation

Draws a text note in a pale-yellow box with a strong-colour border,
anchored at one bar/price, with a small dot at the anchor point.

API: `set_anchors(bar, price, bar, price)`; `set_text(std::string)` /
`set_annotation_text(std::string)` / `annotation_text()`.

Example:
```cpp
StockTextAnnotation w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(20, series[20].close, 20, series[20].close);
w.set_text("Breakout here");
w.draw();
```

Unclear from the header: `set_text()` and `set_annotation_text()` are
aliases (both set the same text); long text is not wrapped and the box
is not clipped to the plot, so it can overflow the right edge; the box
is drawn to the right and slightly above the anchor.

## StockArrow

Draws a thick arrow from the first anchor to the second, with a filled
triangular arrowhead at the tip.

API: `set_anchors(b1, p1, b2, p2)` — tail and tip.

Example:
```cpp
StockArrow w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(10, series[10].low, 25, series[25].high);
w.draw();
```

Unclear from the header: the arrowhead size is fixed (14 px) regardless
of arrow length or zoom; a zero-length arrow (same anchor twice) draws
only the head.

## StockParallelChannel

Draws three parallel lines: a solid line through the first two anchors,
a dashed mid-line halfway between them, and a second solid line offset
from the first by the third anchor's price.

API: `set_anchors(b1, p1, b2, p2)` plus `set_anchor(2, bar, offset_price)`.

Example:
```cpp
StockParallelChannel w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(5, series[5].close, 25, series[25].close);
w.set_anchor(2, 25, series[5].close + 5.0);
w.draw();
```

Unclear from the header: the offset is interpreted as the parallel
line's price at the *first* anchor's bar (the line shares the slope of
the first two anchors); the third anchor's bar index is stored but does
not move the line. All three lines extend across the full plot width.

## StockPitchfork

Draws an Andrews pitchfork from three pivots: a solid median line from
the first pivot through the midpoint of pivots 2 and 3, extended to the
right edge, plus dashed parallel lines through pivots 2 and 3 with the
same slope.

API: `set_anchors3(b1, p1, b2, p2, b3, p3)` or
`set_pivots(b1, p1, b2, p2, b3, p3)` (aliases).

Example:
```cpp
StockPitchfork w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_pivots(5, series[5].low, 15, series[15].high, 25, series[25].low);
w.draw();
```

Unclear from the header: the lines are only drawn from each pivot to
the right edge (no extension to the left); if the midpoint of pivots 2
and 3 shares the first pivot's bar, the slope division is guarded and
the median line is drawn flat.

## StockBrush

Draws a freehand highlight path through stored (bar, price) points: a
thick polyline with a dot at each point, plus a `Brush: <n> points`
label. Clicking adds a point; dragging extends the path.

API: `add_point(int bar, double price)`; `clear_points()`;
`point_count()`.

Example:
```cpp
StockBrush w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.clear_points();
for (int i = 8; i <= 28; i += 2) w.add_point(i, series[i].close);
w.draw();
```

Unclear from the header: there is no erase/undo — points can only be
removed all at once with `clear_points()`; `set_series()` reseeds a
default path (bars 8–20 step 2); the two anchors are unused by the
drawing (arrow keys move them with no visible effect); a single point
draws only its dot, no line.

## StockPriceLabel

Draws a dashed connector from an anchor bar/price to the right edge,
ending in a filled tag showing the price.

API: `set_anchors(bar, price, bar, price)` — only the first anchor is used.

Example:
```cpp
StockPriceLabel w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(59, series[59].close, 59, series[59].close);
w.draw();
```

Unclear from the header: the tag is drawn over the plot's right margin
and can overlap the axis area on narrow windows; the tag text is the
price only (no symbol or bar label).

## StockMeasureTool

Draws a line between two anchors with a right-angle projection, and a
box showing the measurement: `dP <diff> (<pct>%)  bars <n>`.

API: `set_anchors(b1, p1, b2, p2)`; `price_diff()`; `bar_diff()`;
`pct_diff()` (percent of the first anchor's price).

Example:
```cpp
StockMeasureTool w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_series(xframe::make_seeded_series(60, 42));
w.set_anchors(10, series[10].close, 30, series[30].close);
w.draw();
// w.price_diff(), w.bar_diff(), w.pct_diff()
```

Unclear from the header: differences are signed (reversed anchors give
negative values); `pct_diff()` returns 0 if the first anchor's price is
0; the info box is placed above the higher anchor and clamped into the
plot, so it can overlap the line on short measurements.
