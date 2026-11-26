# Stock Market — Comparative Plotting & Volume Widgets

Part of the XFrame Stock Market section (10th palette section). All widgets
are header-only X11 classes in `widgets/components/<Name>/`, following the
`XGraph` pattern (Display\*/screen/window/GC, `draw()`, `on_button_press`,
`on_button_release`, `on_motion`, `on_key`). All demos run on deterministic
seeded synthetic data from `widgets/components/StockData.h`
(`xframe::make_seeded_series(n, seed)`); there is no network, no live market
data, and no API key. Comparative demos use different seeds as different
symbols. Attribution: XFrame.

Common behaviour (all 15): left-drag pans the visible window, mouse wheel
zooms around the cursor, `+`/`-` zoom, arrow keys pan, `r`/`Home` resets the
view, moving the mouse shows a crosshair. `on_view_change(first, count)` fires
on pan/zoom/reset. Build: `cmake -S widgets/components/<Name> -B build`
then `cmake --build build` (C++17, X11 only, `-Wall -Wextra`, 0 warnings).

Test driver: `widgets/tests/test_stock_compare_volume.py` — rebuilds every
demo, runs it under Xvfb, screenshots the default draw, exercises
hover/click/key, and exits cleanly via `WM_DELETE_WINDOW`. Result:
**15/15 widgets, 75/75 cells PASS** (build/draw/click/key/exit each).

---

## Comparative plotting (2+ series each)

### StockOverlayChart
- **Draws/computes:** close-price lines for several symbols overlaid on one
  shared price axis, with a colour legend and a 1/2/5 nice-step grid.
- **API:** `StockOverlayChart(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int w, int h)`;
  `add_series(name, series)` (auto colour) or `add_series(name, series, Srgb)`;
  `clear_series()`, `series_count()`, `item_count()`, `close_at(series, i)`.
- **Example:**
  ```cpp
  StockOverlayChart w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  w.add_series("SYM-B", xframe::make_seeded_series(60, 7));
  w.draw();
  ```
- **Unclear from the header:** symbols with very different price levels share
  one axis, so a low-priced symbol can look flat next to a high-priced one;
  use `StockRelativePerformance` or `StockIndexedComparison` for that case.

### StockMultiSymbolChart
- **Draws/computes:** small-multiple panels — one close-price panel per
  symbol, stacked vertically, each with its own auto y-range.
- **API:** same constructor; `add_series(name, series[, colour])`,
  `clear_series()`, `series_count()`, `item_count()`,
  `panel_close_at(series, i)`.
- **Example:**
  ```cpp
  StockMultiSymbolChart w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  w.add_series("SYM-B", xframe::make_seeded_series(60, 7));
  w.draw();
  ```
- **Unclear from the header:** panels split the plot height evenly (the last
  panel takes any remainder); there is no per-panel y-axis label, only grid
  lines, so exact values are read from the crosshair position, not the panel.

### StockRelativePerformance
- **Draws/computes:** each symbol rebased to 100 at its first close
  (`close / first_close * 100`); lines show percentage performance vs the
  start, with a horizontal line at 100.
- **API:** same constructor; `add_series(...)`, `rebased(series)` (vector),
  `performance_at(series, i)` (rebased value minus 100).
- **Example:**
  ```cpp
  StockRelativePerformance w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  double pct = w.performance_at(0, 59);  // % move of SYM-A over the window
  ```
- **Unclear from the header:** the base is always the first bar in the
  series as supplied, not the first *visible* bar after panning/zooming.

### StockSpreadChart
- **Draws/computes:** the price difference `A.close - B.close` (first two
  series, truncated to the shorter length) as a line around a zero line.
- **API:** same constructor; `add_series(...)`, `spread()` (vector),
  `spread_at(i)`, `spread_label()` (`"A - B"`).
- **Example:**
  ```cpp
  StockSpreadChart w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  w.add_series("SYM-B", xframe::make_seeded_series(60, 7));
  double s = w.spread_at(10);
  w.draw();
  ```
- **Unclear from the header:** only the first two series are used; a third
  series is drawn nowhere and ignored by `spread()`. No z-score or
  normalisation is applied — the spread is in raw price units.

### StockRatioChart
- **Draws/computes:** the price ratio `A.close / B.close` (first two series,
  truncated to the shorter length) as a line.
- **API:** same constructor; `add_series(...)`, `ratio()` (vector),
  `ratio_at(i)`, `ratio_label()` (`"A / B"`).
- **Example:**
  ```cpp
  StockRatioChart w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  w.add_series("SYM-B", xframe::make_seeded_series(60, 7));
  double r = w.ratio_at(10);
  w.draw();
  ```
- **Unclear from the header:** a zero close in B yields a ratio of 0.0
  rather than an error or a skipped bar; only the first two series are used.

### StockCorrelationView
- **Draws/computes:** top panel — rolling Pearson correlation (default
  window 20 bars) between the closes of two symbols, on a fixed −1..+1
  scale with a zero line; bottom panel — scatter of daily returns
  (`(close-prev)/prev`) of A vs B, with cross axes at zero.
- **API:** same constructor; `add_series(...)`, `set_window(w)` /
  `get_window()`, `correlation()` (full-period, on closes),
  `rolling_correlation()` (vector, 0.0 before the window fills),
  `return_scatter()` (vector of `(retA, retB)` pairs).
- **Example:**
  ```cpp
  StockCorrelationView w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  w.add_series("SYM-B", xframe::make_seeded_series(60, 7));
  double c = w.correlation();
  ```
- **Unclear from the header:** the rolling correlation is computed on raw
  closes, while the scatter uses returns — the two panels deliberately use
  different inputs. The scatter axes are scaled to the largest absolute
  return in either symbol, so the aspect is data-dependent.

### StockIndexedComparison
- **Draws/computes:** all symbols indexed to a common base value (default
  100) at their first close, as lines, with a horizontal line at the base.
- **API:** same constructor; `add_series(...)`, `set_base_value(v)` /
  `base_value()`, `indexed(series)` (vector), `indexed_at(series, i)`.
- **Example:**
  ```cpp
  StockIndexedComparison w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  w.add_series("SYM-B", xframe::make_seeded_series(60, 7));
  w.set_base_value(1000.0);
  w.draw();
  ```
- **Unclear from the header:** this differs from `StockRelativePerformance`
  only in the configurable base value and the absence of the
  `performance_at` helper; the plotted lines are identical when the base
  is 100. Changing the base after construction re-indexes on the next draw.

### StockSectorComparison
- **Draws/computes:** series grouped by sector label; one line per sector,
  the average of that sector's members each indexed to 100 at its first
  close. Legend shows the sector names.
- **API:** same constructor; `add_series(...)`,
  `set_labels(vector<string>)` (one label per series, in order),
  `labels()`, `sector_names()`, `sector_count()`,
  `sector_indexed(sector)` (average indexed vector).
- **Example:**
  ```cpp
  StockSectorComparison w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.add_series("SYM-A", xframe::make_seeded_series(60, 42));
  w.add_series("SYM-B", xframe::make_seeded_series(60, 7));
  w.add_series("SYM-C", xframe::make_seeded_series(60, 99));
  w.set_labels({"Tech", "Tech", "Energy"});
  ```
- **Unclear from the header:** a series with no label (shorter label list)
  is grouped under `"Other"`. The sector line is a simple mean of the
  members' indexed values, not volume- or capitalisation-weighted. The
  "grouped bars" mentioned in the brief are not drawn — the current widget
  draws grouped (per-sector) lines only.

---

## Volume (one OHLCVSeries each)

### StockVolumeBars
- **Draws/computes:** close-price line in the top ~62% of the plot and
  volume bars in the bottom strip, coloured green when `close >= open`,
  red otherwise.
- **API:** same constructor; `set_series(series)`, `set_symbol(name)` /
  `symbol()`, `volume_at(i)`, `max_volume()`.
- **Example:**
  ```cpp
  StockVolumeBars w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.set_series(xframe::make_seeded_series(60, 42));
  ```
- **Unclear from the header:** bar height is scaled to the maximum volume
  in the *whole* series, not the visible window, so zooming does not
  rescale the bars.

### StockVWAP
- **Draws/computes:** close price plus the VWAP line, where VWAP is the
  cumulative `sum(typical * volume) / sum(volume)` with typical price
  `(high + low + close) / 3`, anchored at the first bar of the series.
- **API:** same constructor; `set_series(series)`, `vwap()` (vector),
  `vwap_at(i)`.
- **Example:**
  ```cpp
  StockVWAP w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.set_series(xframe::make_seeded_series(60, 42));
  double v = w.vwap_at(59);
  w.draw();
  ```
- **Unclear from the header:** VWAP is session-anchored at bar 0 of the
  supplied series — there is no per-day reset and no way to re-anchor it
  other than supplying a shorter series.

### StockVolumeProfile
- **Draws/computes:** horizontal volume-by-price histogram: volume
  accumulated into 20 price bins (by typical price) between the series low
  and high; the point of control (highest-volume bin) is drawn in red and
  labelled with its price.
- **API:** same constructor; `set_series(series)`, `set_bins(n)` /
  `get_bins()` (clamped 2..100), `profile()` (vector of bin volumes),
  `poc_bin()`, `poc_price()`.
- **Example:**
  ```cpp
  StockVolumeProfile w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.set_series(xframe::make_seeded_series(60, 42));
  double poc = w.poc_price();
  ```
- **Unclear from the header:** a bar's whole volume goes into the single
  bin containing its typical price (no splitting across bins); on a tie,
  the lowest bin wins as POC.

### StockOBVChart
- **Draws/computes:** on-balance volume — cumulative volume, adding the
  bar's volume when `close > open`, subtracting it when `close < open`,
  unchanged on a doji (`close == open`).
- **API:** same constructor; `set_series(series)`, `obv()` (vector),
  `obv_at(i)`.
- **Example:**
  ```cpp
  StockOBVChart w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.set_series(xframe::make_seeded_series(60, 42));
  double o = w.obv_at(59);
  w.draw();
  ```
- **Unclear from the header:** OBV starts at 0 at bar 0 (bar 0's own volume
  is included with its sign); it is not normalised or rebased.

### StockVolumeCandles
- **Draws/computes:** candlesticks (wick = high–low, body = open–close,
  green up / red down) whose body width encodes volume:
  `width fraction = 0.3 + 0.7 * volume / max_volume`, so the heaviest bar
  is full slot width and the lightest is 30%.
- **API:** same constructor; `set_series(series)`,
  `body_width_at(i)` (the 0.3..1.0 fraction), `max_volume()`.
- **Example:**
  ```cpp
  StockVolumeCandles w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.set_series(xframe::make_seeded_series(60, 42));
  double f = w.body_width_at(3);
  w.draw();
  ```
- **Unclear from the header:** the width encodes *relative* volume within
  the supplied series; absolute volume is not shown anywhere.

### StockAccumulationDistribution
- **Draws/computes:** the accumulation/distribution line — cumulative
  money-flow volume, where each bar contributes
  `CLV * volume` and `CLV = ((close-low) - (high-close)) / (high-low)`.
- **API:** same constructor; `set_series(series)`,
  `accumulation_distribution()` (vector), `ad_at(i)`,
  `clv(bar)` (static helper).
- **Example:**
  ```cpp
  StockAccumulationDistribution w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.set_series(xframe::make_seeded_series(60, 42));
  double a = w.ad_at(59);
  w.draw();
  ```
- **Unclear from the header:** a zero-range bar (`high == low`) contributes
  0; the line starts at 0 plus bar 0's contribution, not at bar 0's close.

### StockMoneyFlowChart
- **Draws/computes:** money-flow volume bars — `CLV * typical * volume`
  per bar (same CLV as A/D), drawn as signed bars from a zero line, green
  for positive flow, red for negative.
- **API:** same constructor; `set_series(series)`,
  `money_flow()` (vector, per-bar values, *not* cumulative),
  `money_flow_at(i)`, `total_money_flow()` (sum over bars),
  `clv(bar)` (static helper).
- **Example:**
  ```cpp
  StockMoneyFlowChart w(display, screen, window, gc, &styles, 20, 20, 720, 480);
  w.set_series(xframe::make_seeded_series(60, 42));
  double m = w.total_money_flow();
  w.draw();
  ```
- **Unclear from the header:** unlike `StockAccumulationDistribution`,
  this widget does *not* cumulate — each bar stands alone. The vertical
  scale is money-flow units (price × volume), not volume, so bar heights
  are not comparable with `StockVolumeBars`.
