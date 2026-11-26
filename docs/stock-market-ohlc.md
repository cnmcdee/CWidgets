# Stock Market widgets — OHLC / price charts

Eight widgets in `widgets/components/`, one per directory. Each directory
contains exactly `<Name>.h`, `main_<Name>.cpp`, `CMakeLists.txt`, and
`core/xframe_core.h` (byte-identical to `widgets/core/xframe_core.h`).
Attribution: XFrame.

All eight render from a shared `xframe::OHLCVSeries`
(`widgets/components/StockData.h`, included as `#include "../StockData.h"`).
Demos use `xframe::make_seeded_series(250)` — deterministic synthetic data,
no network, no live market data. Verified: same `(n, seed)` gives identical
bars, a different seed gives different bars, every bar satisfies
`high >= max(open, close)` and `low <= min(open, close)`.

## Common API (all eight)

Constructor (all widgets):

```cpp
Name(Display* display, int screen, Window window, GC gc,
     SStyle_Manager* styles, int x, int y, int width, int height,
     const xframe::OHLCVSeries& series = xframe::make_seeded_series(250));
```

Key methods, identical on every widget:

- `void set_series(const xframe::OHLCVSeries& s)` — replace the data; resets
  the view to show all items. Derived data (Heikin-Ashi candles, Renko
  bricks, BaselineChart's default baseline) is rebuilt here.
- `const xframe::OHLCVSeries& series() const` — the raw series.
- `int item_count()` — number of plotted items (bars for most widgets;
  Heikin-Ashi candles for StockHeikinAshi; bricks for StockRenko).
- `void draw()` — axes + 1/2/5 grid, series, index axis, title with last
  close, crosshair + readout when the mouse has moved over the plot.
- `bool on_button_press(int x, int y, unsigned int button)` — button 1 starts
  a drag-pan and focuses the widget; wheel (4/5) zooms around the cursor;
  any press also moves the crosshair to the click point.
- `bool on_button_release(int x, int y, unsigned int button)`,
  `bool on_motion(int x, int y)` — crosshair follows the mouse; drag pans.
- `bool on_key(KeySym key, const char* str)` — only when focused:
  Left/Right pan 10% of the view, `+`/`-` zoom, `Home`/`r` reset view.
- `void set_y_range(double lo, double hi)` / `void set_auto_y(bool on)` —
  manual Y range or auto range (default) with 6% padding.
- `void set_bounds(...)`, `width()`, `height()`, `contains(x, y)`,
  `set_visible`/`get_visible`, `set_enabled`/`get_enabled`,
  `set_style_name`, `set_text`/`get_text`, `style()`.
- `double get_view_first()`, `double get_view_count()`,
  `void reset_view()`, `void zoom_at(double factor, double frac_x)`.

Working example (same shape for every widget; here the candlestick one):

```cpp
#include "StockCandlestickChart.h"
StockCandlestickChart w(display, screen, window, gc, &styles,
                        20, 20, 720, 480,
                        xframe::make_seeded_series(250));
w.draw();
// ... event loop forwards ButtonPress/ButtonRelease/MotionNotify/KeyPress
// to w.on_button_press / on_button_release / on_motion / on_key,
// redrawing after any handled event, as in main_StockCandlestickChart.cpp.
```

Build one widget (from the repo root, on the server):

```
cmake -S widgets/components/StockCandlestickChart -B /tmp/b-candle -DCMAKE_CXX_FLAGS="-Wall -Wextra"
cmake --build /tmp/b-candle   # 0 warnings
```

Tests: `python3 widgets/tests/test_stock_ohlc.py` builds all eight with
`-Wall -Wextra`, checks StockData.h determinism with a standalone compile,
then runs each demo under Xvfb `:91` and checks build / draw (non-blank
screenshot) / hover (crosshair) / click / key (process survives) /
clean exit via `WM_DELETE_WINDOW`. Result: 8/8 widgets fully PASS,
0 warnings, driver exit 0.

## StockCandlestickChart

Draws one filled candle per bar: vertical wick from low to high, body from
open to close, green when `close >= open`, red otherwise. Body width is 60%
of the per-item slot. Y range from visible lows/highs.

Unclear from the header: a doji (`open == close`) is drawn green (the
`close >= open` branch) with a 1-pixel-tall body; the header does not say
dojis should be special-cased, so they are not.

## StockOHLCBarChart

Classic OHLC bar per bar: vertical high–low line, left tick at the open,
right tick at the close, half-slot-wide ticks (35% of the item slot).
Colour is green when `close >= open`, red otherwise. Y range from visible
lows/highs.

Unclear from the header: tick length is a fixed fraction of the slot width,
so ticks shrink as more bars are shown; there is no minimum tick length
beyond 1 pixel.

## StockLineChart

Closes joined by a single 2-pixel polyline (blue). Y range from visible
closes only, so wicks do not affect the scale. Crosshair readout shows the
close at the hovered index.

Unclear from the header: with a single bar there is no line (needs 2+
points); the widget then shows only axes, grid and title.

## StockAreaChart

Closes as a line (as in StockLineChart) with the area between the line and
the bottom of the plot filled light blue. Y range from visible closes only.

Unclear from the header: the fill goes to the plot bottom, not to zero or to
the Y minimum — on a zoomed view the filled region is bounded by the visible
plot rectangle.

## StockHeikinAshi

Computes Heikin-Ashi candles from the series and draws them as candles
(same rendering as StockCandlestickChart):

- `ha_close = (open + high + low + close) / 4`
- `ha_open = (prev ha_open + prev ha_close) / 2`; for the first bar,
  `ha_open = (open + close) / 2`
- `ha_high = max(high, ha_open, ha_close)`,
  `ha_low = min(low, ha_open, ha_close)`

Extra API: `const xframe::OHLCVSeries& heikin_ashi_series() const`,
`int heikin_ashi_count()`. The derived candles are rebuilt by the
constructor and by `set_series()`. Y range, item count and crosshair readout
all use the Heikin-Ashi candles, not the raw bars.

Unclear from the header: the first-bar `ha_open` seed `(open + close) / 2`
is a convention choice, not something the header derives — other
implementations seed it with the raw open. The volume and timestamp of each
derived candle are copied from the raw bar.

## StockRenko

Builds Renko bricks from closes. Extra API:

- `void set_brick_size(double s)` — brick height in price units (ignored
  when `s <= 0`); rebuilds the bricks and resets the view. Default `1.0`.
- `double brick_size() const`, `int brick_count()`.

Bricks: the anchor is `floor(first close / brick_size) * brick_size`; each
time the close crosses the running brick boundary a brick is added (green
for up, red for down), and the anchor advances one brick. Bricks are drawn
as filled rectangles 80% of the item slot, with a thin outline. Y range,
item count and the x axis all use bricks, so the x axis is brick index, not
time — consecutive bricks need not be consecutive days.

Unclear from the header: the first bar never produces a brick (it only sets
the anchor), and a large single-bar jump produces several bricks at once;
there is no cap on bricks per bar. With the default brick size 1.0 and the
seeded 250-bar series the brick count is in the tens.

## StockHollowCandles

Candles as in StockCandlestickChart, except the body is hollow
(outline only) when `close > open` and filled when `close <= open`.
The wick is always drawn. Colour is green for hollow (up) bodies, red for
filled (down) bodies.

Unclear from the header: a doji (`open == close`) counts as down/filled
(the `close > open` test is strict); the header does not discuss dojis.

## StockBaselineChart

Closes as a line with the area between the line and a baseline value filled
green where the line is above the baseline and red where it is below; the
baseline itself is drawn as a grey horizontal line. Extra API:
`void set_baseline(double b)`, `double baseline() const`. Default baseline
is the first bar's close (or 100.0 for an empty series); `set_series()`
resets it to the new series' first close. Y range includes the baseline, so
the baseline line is always visible.

Unclear from the header: the fill is computed per segment and split at
baseline crossings, so a segment that crosses the baseline is filled with
two triangles; the exact split point is linear interpolation between the
two closes. No smoothing is applied.

## Per-widget examples

One working example per widget (same shape as the shared example
above, with the widget's own class name).

### StockCandlestickChart

- **Example:**
```cpp
StockCandlestickChart w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```

### StockOHLCBarChart

- **Example:**
```cpp
StockOHLCBarChart w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```

### StockLineChart

- **Example:**
```cpp
StockLineChart w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```

### StockAreaChart

- **Example:**
```cpp
StockAreaChart w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```

### StockHeikinAshi

- **Example:**
```cpp
StockHeikinAshi w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```

### StockRenko

- **Example:**
```cpp
StockRenko w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```

### StockHollowCandles

- **Example:**
```cpp
StockHollowCandles w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```

### StockBaselineChart

- **Example:**
```cpp
StockBaselineChart w(display, screen, window, gc, &styles, 20, 20, 720, 480, xframe::make_seeded_series(250));
w.draw();
```
