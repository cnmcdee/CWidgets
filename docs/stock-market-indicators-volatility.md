# Stock Market indicators — volatility, volume and other (80 widgets)

XFrame Stock Market section, volatility + volume + other group. Every indicator
is a real named indicator with a standard formula (TA-Lib / StockCharts /
Investopedia or equivalent). All compute functions take an `xframe::OHLCVSeries`
and return one value per input bar; leading warm-up bars are NaN until enough
bars exist. All demos and tests run on deterministic seeded synthetic data
(`xframe::make_seeded_series`); no network, no live market data, no API keys.
Attribution: XFrame.

Regenerable: `widgets/tools/gen_indicators_volatility.py` emits all 80 widget
directories from its spec table; the audited list is
`widgets/tools/stock_indicators_volatility.txt` (80 lines).
Tests: `widgets/tests/test_stock_indicators_volatility.py` (build / draw /
click / key / exit / smoke per indicator, cmake build per indicator, hand-computed
spot checks for Bollinger Bands and ATR).

## Output convention (applies to every entry below)

Output length == input length. Leading warm-up bars are NaN. After the warm-up,
every value is finite on the seeded series. Multi-output indicators return a small
struct with one `std::vector<double>` per output line (field names listed below).

## IndBollingerBands

- Category: volatility; draw style: overlay
- Formula: Middle = SMA(close, 20); Upper = Middle + 2 * StdDev(close, 20); Lower = Middle - 2 * StdDev(close, 20).
- Compute: `BollingerBandsResult ind_bollinger_bands(const OHLCVSeries& series, int period = 20, double mult = 2.0)`
- Multi-output struct `BollingerBandsResult` with fields: `upper`, `middle`, `lower`
- Constructor: `IndBollingerBands(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_bollinger_bands(series);  // one value per bar, NaN warm-up
  xframe::IndBollingerBands widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndBollingerPercentB

- Category: volatility; draw style: pane
- Formula: %B = (close - Lower Band) / (Upper Band - Lower Band); 1.0 at the upper band, 0.0 at the lower band.
- Compute: `std::vector<double> ind_bollinger_percent_b(const OHLCVSeries& series, int period = 20, double mult = 2.0)`
- Constructor: `IndBollingerPercentB(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_bollinger_percent_b(series);  // one value per bar, NaN warm-up
  xframe::IndBollingerPercentB widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndBollingerBandwidth

- Category: volatility; draw style: pane
- Formula: Bandwidth = (Upper Band - Lower Band) / Middle Band.
- Compute: `std::vector<double> ind_bollinger_bandwidth(const OHLCVSeries& series, int period = 20, double mult = 2.0)`
- Constructor: `IndBollingerBandwidth(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_bollinger_bandwidth(series);  // one value per bar, NaN warm-up
  xframe::IndBollingerBandwidth widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndATR

- Category: volatility; draw style: pane
- Formula: True Range = max(high-low, |high-prev close|, |low-prev close|); ATR = Wilder-smoothed (1/14) moving average of True Range.
- Compute: `std::vector<double> ind_atr(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndATR(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_atr(series);  // one value per bar, NaN warm-up
  xframe::IndATR widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndNATR

- Category: volatility; draw style: pane
- Formula: NATR = 100 * ATR(14) / close.
- Compute: `std::vector<double> ind_natr(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndNATR(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_natr(series);  // one value per bar, NaN warm-up
  xframe::IndNATR widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndTrueRange

- Category: volatility; draw style: pane
- Formula: True Range = max(high - low, |high - previous close|, |low - previous close|); first bar is high - low.
- Compute: `std::vector<double> ind_true_range(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndTrueRange(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_true_range(series);  // one value per bar, NaN warm-up
  xframe::IndTrueRange widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndKeltnerChannels

- Category: volatility; draw style: overlay
- Formula: Middle = EMA(typical price, 20); Upper = Middle + 2 * ATR(20); Lower = Middle - 2 * ATR(20). Typical price = (high + low + close) / 3.
- Compute: `KeltnerChannelsResult ind_keltner_channels(const OHLCVSeries& series, int period = 20, double mult = 2.0)`
- Multi-output struct `KeltnerChannelsResult` with fields: `upper`, `middle`, `lower`
- Constructor: `IndKeltnerChannels(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_keltner_channels(series);  // one value per bar, NaN warm-up
  xframe::IndKeltnerChannels widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndDonchianChannels

- Category: volatility; draw style: overlay
- Formula: Upper = highest high of the last 20 bars; Lower = lowest low of the last 20 bars; Middle = (Upper + Lower) / 2.
- Compute: `DonchianChannelsResult ind_donchian_channels(const OHLCVSeries& series, int period = 20)`
- Multi-output struct `DonchianChannelsResult` with fields: `upper`, `middle`, `lower`
- Constructor: `IndDonchianChannels(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_donchian_channels(series);  // one value per bar, NaN warm-up
  xframe::IndDonchianChannels widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndChaikinVolatility

- Category: volatility; draw style: pane
- Formula: Chaikin Volatility = 100 * (EMA(high-low, 10) - EMA(high-low, 10) 10 bars ago) / EMA(high-low, 10) 10 bars ago.
- Compute: `std::vector<double> ind_chaikin_volatility(const OHLCVSeries& series, int period = 10)`
- Constructor: `IndChaikinVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_chaikin_volatility(series);  // one value per bar, NaN warm-up
  xframe::IndChaikinVolatility widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the percent change is measured against the EMA value exactly `period` bars ago, not against a second EMA.

## IndHistoricalVolatility

- Category: volatility; draw style: pane
- Formula: Historical Volatility = 100 * StdDev(log(close / previous close), 20) * sqrt(252) (annualised, percent).
- Compute: `std::vector<double> ind_historical_volatility(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndHistoricalVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 20 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_historical_volatility(series);  // one value per bar, NaN warm-up
  xframe::IndHistoricalVolatility widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndParkinsonVolatility

- Category: volatility; draw style: pane
- Formula: Parkinson volatility = 100 * sqrt( (1 / (4 ln 2 * n)) * sum(ln(high/low)^2) ) * sqrt(252), over 20 bars.
- Compute: `std::vector<double> ind_parkinson_volatility(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndParkinsonVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_parkinson_volatility(series);  // one value per bar, NaN warm-up
  xframe::IndParkinsonVolatility widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndGarmanKlassVolatility

- Category: volatility; draw style: pane
- Formula: Garman-Klass: per bar 0.5*ln(H/L)^2 - (2 ln 2 - 1)*ln(C/O)^2, averaged over 20 bars, square-rooted, annualised by sqrt(252), in percent.
- Compute: `std::vector<double> ind_garman_klass_volatility(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndGarmanKlassVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_garman_klass_volatility(series);  // one value per bar, NaN warm-up
  xframe::IndGarmanKlassVolatility widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndRogersSatchellVolatility

- Category: volatility; draw style: pane
- Formula: Rogers-Satchell: per bar ln(H/C)*ln(H/O) + ln(L/C)*ln(L/O), averaged over 20 bars, square-rooted, annualised by sqrt(252), in percent.
- Compute: `std::vector<double> ind_rogers_satchell_volatility(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndRogersSatchellVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_rogers_satchell_volatility(series);  // one value per bar, NaN warm-up
  xframe::IndRogersSatchellVolatility widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndYangZhangVolatility

- Category: volatility; draw style: pane
- Formula: Yang-Zhang: overnight variance + k * open-to-close variance + (1-k) * Rogers-Satchell variance, k = 0.34 / (1.34 + (n+1)/(n-1)), square-rooted, annualised by sqrt(252), in percent.
- Compute: `std::vector<double> ind_yang_zhang_volatility(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndYangZhangVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 20 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_yang_zhang_volatility(series);  // one value per bar, NaN warm-up
  xframe::IndYangZhangVolatility widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the overnight variance uses log(open / previous close); the first bar of the series contributes no overnight term.

## IndStdDev

- Category: volatility; draw style: pane
- Formula: StdDev = population standard deviation of close over the last 20 bars.
- Compute: `std::vector<double> ind_stddev(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndStdDev(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_stddev(series);  // one value per bar, NaN warm-up
  xframe::IndStdDev widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVariance

- Category: volatility; draw style: pane
- Formula: Variance = population variance (StdDev squared) of close over the last 20 bars.
- Compute: `std::vector<double> ind_variance(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndVariance(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_variance(series);  // one value per bar, NaN warm-up
  xframe::IndVariance widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndChandelierExit

- Category: volatility; draw style: overlay
- Formula: Long Exit = highest high of the last 22 bars - 3 * ATR(22); Short Exit = lowest low of the last 22 bars + 3 * ATR(22).
- Compute: `ChandelierExitResult ind_chandelier_exit(const OHLCVSeries& series, int period = 22, double mult = 3.0)`
- Multi-output struct `ChandelierExitResult` with fields: `long_exit`, `short_exit`
- Constructor: `IndChandelierExit(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 21 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_chandelier_exit(series);  // one value per bar, NaN warm-up
  xframe::IndChandelierExit widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVolatilityStop

- Category: volatility; draw style: overlay
- Formula: Wilder Volatility Stop: a trailing stop that ratchets up (long) at close - 3*ATR and flips to a short stop at close + 3*ATR when price closes through it.
- Compute: `std::vector<double> ind_volatility_stop(const OHLCVSeries& series, int period = 14, double mult = 3.0)`
- Constructor: `IndVolatilityStop(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volatility_stop(series);  // one value per bar, NaN warm-up
  xframe::IndVolatilityStop widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the first stop is seeded at close - mult*ATR on the first finite ATR bar; the long/short state starts long.

## IndMassIndex

- Category: volatility; draw style: pane
- Formula: Mass Index = 25-bar sum of (high - low) / EMA(high - low, 9).
- Compute: `std::vector<double> ind_mass_index(const OHLCVSeries& series, int period = 25)`
- Constructor: `IndMassIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 32 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_mass_index(series);  // one value per bar, NaN warm-up
  xframe::IndMassIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndUlcerIndex

- Category: volatility; draw style: pane
- Formula: Ulcer Index = sqrt(mean of squared percentage drawdowns from the rolling 14-bar highest close).
- Compute: `std::vector<double> ind_ulcer_index(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndUlcerIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_ulcer_index(series);  // one value per bar, NaN warm-up
  xframe::IndUlcerIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndRelativeVolatilityIndex

- Category: volatility; draw style: pane
- Formula: Relative Volatility Index: Wilder-smoothed standard deviation of up closes vs down closes, combined RSI-style as 100 * up / (up + down) over 10 bars.
- Compute: `std::vector<double> ind_relative_volatility_index(const OHLCVSeries& series, int period = 10)`
- Constructor: `IndRelativeVolatilityIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_relative_volatility_index(series);  // one value per bar, NaN warm-up
  xframe::IndRelativeVolatilityIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndTTMSqueeze

- Category: volatility; draw style: pane
- Formula: TTM Squeeze: the squeeze is ON (1) when the Bollinger Bands (20, 2 StdDev) lie inside the Keltner Channels (20, 1.5 ATR); ratio = Bollinger width / Keltner width.
- Compute: `TTMSqueezeResult ind_ttm_squeeze(const OHLCVSeries& series, int period = 20, double mult = 2.0)`
- Multi-output struct `TTMSqueezeResult` with fields: `squeeze`, `ratio`
- Constructor: `IndTTMSqueeze(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_ttm_squeeze(series);  // one value per bar, NaN warm-up
  xframe::IndTTMSqueeze widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the Keltner width in the squeeze test uses a 1.5 ATR multiplier while the Bollinger width uses the 2.0 StdDev multiplier parameter.

## IndGopalakrishnanRangeIndex

- Category: volatility; draw style: pane
- Formula: Gopalakrishnan Range Index (GAPO) = 100 * mean of ln(high / low) over the last 20 bars.
- Compute: `std::vector<double> ind_gopalakrishnan_range_index(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndGopalakrishnanRangeIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_gopalakrishnan_range_index(series);  // one value per bar, NaN warm-up
  xframe::IndGopalakrishnanRangeIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndWilderVolatilitySystem

- Category: volatility; draw style: overlay
- Formula: Wilder Volatility System stop: long stop trails the highest high minus 3*ATR and flips to a short stop at the lowest low plus 3*ATR on a close through the stop.
- Compute: `std::vector<double> ind_wilder_volatility_system(const OHLCVSeries& series, int period = 14, double mult = 3.0)`
- Constructor: `IndWilderVolatilitySystem(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_wilder_volatility_system(series);  // one value per bar, NaN warm-up
  xframe::IndWilderVolatilitySystem widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the long stop trails the highest high minus mult*ATR (not the close), and flips on a close through the stop.

## IndVolatilityRatio

- Category: volatility; draw style: pane
- Formula: Volatility Ratio = True Range / SMA(True Range, 14).
- Compute: `std::vector<double> ind_volatility_ratio(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndVolatilityRatio(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volatility_ratio(series);  // one value per bar, NaN warm-up
  xframe::IndVolatilityRatio widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndCloseVolatility

- Category: volatility; draw style: pane
- Formula: Close Volatility = 100 * StdDev(log(close / previous close), 20), unannualised, in percent.
- Compute: `std::vector<double> ind_close_volatility(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndCloseVolatility(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 20 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_close_volatility(series);  // one value per bar, NaN warm-up
  xframe::IndCloseVolatility widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndHighLowRange

- Category: volatility; draw style: pane
- Formula: High-Low Range = 100 * (highest high - lowest low of the last 20 bars) / close.
- Compute: `std::vector<double> ind_high_low_range(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndHighLowRange(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_high_low_range(series);  // one value per bar, NaN warm-up
  xframe::IndHighLowRange widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndKeltnerBandwidth

- Category: volatility; draw style: pane
- Formula: Keltner Bandwidth = (Upper Channel - Lower Channel) / Middle Channel, with channels at EMA(typical, 20) +/- 2 * ATR(20).
- Compute: `std::vector<double> ind_keltner_bandwidth(const OHLCVSeries& series, int period = 20, double mult = 2.0)`
- Constructor: `IndKeltnerBandwidth(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_keltner_bandwidth(series);  // one value per bar, NaN warm-up
  xframe::IndKeltnerBandwidth widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndOBV

- Category: volume; draw style: pane
- Formula: On-Balance Volume: cumulative volume, adding the bar volume when close rises, subtracting it when close falls, unchanged on an unchanged close.
- Compute: `std::vector<double> ind_obv(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndOBV(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_obv(series);  // one value per bar, NaN warm-up
  xframe::IndOBV widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndAccumulationDistributionLine

- Category: volume; draw style: pane
- Formula: Accumulation/Distribution Line: cumulative Close Location Value * volume, where CLV = ((close - low) - (high - close)) / (high - low).
- Compute: `std::vector<double> ind_accumulation_distribution_line(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndAccumulationDistributionLine(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_accumulation_distribution_line(series);  // one value per bar, NaN warm-up
  xframe::IndAccumulationDistributionLine widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndChaikinMoneyFlow

- Category: volume; draw style: pane
- Formula: Chaikin Money Flow = sum(CLV * volume, 20) / sum(volume, 20).
- Compute: `std::vector<double> ind_chaikin_money_flow(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndChaikinMoneyFlow(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_chaikin_money_flow(series);  // one value per bar, NaN warm-up
  xframe::IndChaikinMoneyFlow widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndMoneyFlowIndex

- Category: volume; draw style: pane
- Formula: Money Flow Index: raw money flow = typical price * volume; positive flow when typical price rises, negative when it falls; MFI = 100 - 100 / (1 + positive flow / negative flow) over 14 bars.
- Compute: `std::vector<double> ind_money_flow_index(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndMoneyFlowIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 14 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_money_flow_index(series);  // one value per bar, NaN warm-up
  xframe::IndMoneyFlowIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: a bar whose typical price is unchanged contributes its money flow to neither the positive nor the negative sum.

## IndVolumeOscillator

- Category: volume; draw style: pane
- Formula: Volume Oscillator = 100 * (EMA(volume, 5) - EMA(volume, 10)) / EMA(volume, 10).
- Compute: `std::vector<double> ind_volume_oscillator(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndVolumeOscillator(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 9 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volume_oscillator(series);  // one value per bar, NaN warm-up
  xframe::IndVolumeOscillator widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndPercentageVolumeOscillator

- Category: volume; draw style: pane
- Formula: Percentage Volume Oscillator = 100 * (EMA(volume, 12) - EMA(volume, 26)) / EMA(volume, 26).
- Compute: `std::vector<double> ind_percentage_volume_oscillator(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndPercentageVolumeOscillator(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 25 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_percentage_volume_oscillator(series);  // one value per bar, NaN warm-up
  xframe::IndPercentageVolumeOscillator widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVWAP

- Category: volume; draw style: overlay
- Formula: VWAP = cumulative sum(typical price * volume) / cumulative sum(volume) from the first bar; typical price = (high + low + close) / 3.
- Compute: `std::vector<double> ind_vwap(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndVWAP(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_vwap(series);  // one value per bar, NaN warm-up
  xframe::IndVWAP widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVWAPBands

- Category: volume; draw style: overlay
- Formula: VWAP Bands: VWAP plus/minus 1 volume-weighted standard deviation of typical price, computed cumulatively from the first bar.
- Compute: `VWAPBandsResult ind_vwap_bands(const OHLCVSeries& series, int period = 1, double mult = 1.0)`
- Multi-output struct `VWAPBandsResult` with fields: `upper`, `vwap`, `lower`
- Constructor: `IndVWAPBands(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_vwap_bands(series);  // one value per bar, NaN warm-up
  xframe::IndVWAPBands widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndAnchoredVWAP

- Category: volume; draw style: overlay
- Formula: Anchored VWAP = cumulative sum(typical price * volume) / cumulative sum(volume), anchored at bar 50 (the period parameter) instead of the first bar.
- Compute: `std::vector<double> ind_anchored_vwap(const OHLCVSeries& series, int period = 50)`
- Constructor: `IndAnchoredVWAP(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 49 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_anchored_vwap(series);  // one value per bar, NaN warm-up
  xframe::IndAnchoredVWAP widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the anchor is bar (period - 1), i.e. bar 50 by default; there is no calendar-date anchor.

## IndChaikinOscillator

- Category: volume; draw style: pane
- Formula: Chaikin Oscillator = EMA(Accumulation/Distribution Line, 3) - EMA(Accumulation/Distribution Line, 10).
- Compute: `std::vector<double> ind_chaikin_oscillator(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndChaikinOscillator(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 9 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_chaikin_oscillator(series);  // one value per bar, NaN warm-up
  xframe::IndChaikinOscillator widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndEaseOfMovement

- Category: volume; draw style: pane
- Formula: Ease of Movement: distance moved = midpoint change; box ratio = (volume / 1,000,000) / (high - low); EMV = SMA(distance moved / box ratio, 14).
- Compute: `std::vector<double> ind_ease_of_movement(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndEaseOfMovement(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 14 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_ease_of_movement(series);  // one value per bar, NaN warm-up
  xframe::IndEaseOfMovement widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the box ratio divides volume by 1,000,000 before dividing by the high-low range.

## IndVolumeRateOfChange

- Category: volume; draw style: pane
- Formula: Volume Rate of Change = 100 * (volume - volume 14 bars ago) / volume 14 bars ago.
- Compute: `std::vector<double> ind_volume_rate_of_change(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndVolumeRateOfChange(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 14 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volume_rate_of_change(series);  // one value per bar, NaN warm-up
  xframe::IndVolumeRateOfChange widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVolumePriceTrend

- Category: volume; draw style: pane
- Formula: Volume Price Trend: cumulative sum of volume * (close - previous close) / previous close.
- Compute: `std::vector<double> ind_volume_price_trend(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndVolumePriceTrend(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volume_price_trend(series);  // one value per bar, NaN warm-up
  xframe::IndVolumePriceTrend widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndNegativeVolumeIndex

- Category: volume; draw style: pane
- Formula: Negative Volume Index: starts at 1000; only changes on bars whose volume fell versus the previous bar, by the close-to-close price ratio.
- Compute: `std::vector<double> ind_negative_volume_index(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndNegativeVolumeIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_negative_volume_index(series);  // one value per bar, NaN warm-up
  xframe::IndNegativeVolumeIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndPositiveVolumeIndex

- Category: volume; draw style: pane
- Formula: Positive Volume Index: starts at 1000; only changes on bars whose volume rose versus the previous bar, by the close-to-close price ratio.
- Compute: `std::vector<double> ind_positive_volume_index(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndPositiveVolumeIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_positive_volume_index(series);  // one value per bar, NaN warm-up
  xframe::IndPositiveVolumeIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndDemandIndex

- Category: volume; draw style: pane
- Formula: Demand Index: buying pressure = positive CLV*volume, selling pressure = negative CLV*volume; index = 100 * (SMA(buying,14) - SMA(selling,14)) / (SMA(buying,14) + SMA(selling,14)).
- Compute: `std::vector<double> ind_demand_index(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndDemandIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_demand_index(series);  // one value per bar, NaN warm-up
  xframe::IndDemandIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndKlingerVolumeOscillator

- Category: volume; draw style: pane
- Formula: Klinger Volume Oscillator: volume force = trend * volume * |2*dm/cm - 1|, where dm is the high-low range change and cm the cumulative measurement; KVO = EMA(volume force, 34) - EMA(volume force, 55); signal = EMA(KVO, period) (13 by default).
- Compute: `KlingerVolumeOscillatorResult ind_klinger_volume_oscillator(const OHLCVSeries& series, int period = 13)`
- Multi-output struct `KlingerVolumeOscillatorResult` with fields: `kvo`, `signal`
- Constructor: `IndKlingerVolumeOscillator(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 54 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_klinger_volume_oscillator(series);  // one value per bar, NaN warm-up
  xframe::IndKlingerVolumeOscillator widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the signal line is EMA(KVO, period) (default 13) computed over a zero-filled KVO and re-masked to NaN where KVO itself is NaN.

## IndTwiggsMoneyFlow

- Category: volume; draw style: pane
- Formula: Twiggs Money Flow: like Chaikin Money Flow but the true high/low are extended by the previous close; EMA(AD volume, 21) / EMA(volume, 21).
- Compute: `std::vector<double> ind_twiggs_money_flow(const OHLCVSeries& series, int period = 21)`
- Constructor: `IndTwiggsMoneyFlow(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 20 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_twiggs_money_flow(series);  // one value per bar, NaN warm-up
  xframe::IndTwiggsMoneyFlow widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVolumeWeightedMovingAverage

- Category: volume; draw style: overlay
- Formula: Volume Weighted Moving Average = sum(close * volume, 20) / sum(volume, 20).
- Compute: `std::vector<double> ind_volume_weighted_moving_average(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndVolumeWeightedMovingAverage(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volume_weighted_moving_average(series);  // one value per bar, NaN warm-up
  xframe::IndVolumeWeightedMovingAverage widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVolumeZoneOscillator

- Category: volume; draw style: pane
- Formula: Volume Zone Oscillator: R = +1 on up closes, -1 on down closes; VZO = 100 * (EMA(up volume,14) - EMA(down volume,14)) / (EMA(up volume,14) + EMA(down volume,14)).
- Compute: `std::vector<double> ind_volume_zone_oscillator(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndVolumeZoneOscillator(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volume_zone_oscillator(series);  // one value per bar, NaN warm-up
  xframe::IndVolumeZoneOscillator widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndRelativeVolume

- Category: volume; draw style: pane
- Formula: Relative Volume = volume / SMA(volume, 20).
- Compute: `std::vector<double> ind_relative_volume(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndRelativeVolume(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_relative_volume(series);  // one value per bar, NaN warm-up
  xframe::IndRelativeVolume widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVolumeAverage

- Category: volume; draw style: pane
- Formula: Volume Average = SMA(volume, 20).
- Compute: `std::vector<double> ind_volume_average(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndVolumeAverage(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volume_average(series);  // one value per bar, NaN warm-up
  xframe::IndVolumeAverage widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndMoneyFlowVolume

- Category: volume; draw style: pane
- Formula: Money Flow Volume = typical price * volume per bar; typical price = (high + low + close) / 3.
- Compute: `std::vector<double> ind_money_flow_volume(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndMoneyFlowVolume(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_money_flow_volume(series);  // one value per bar, NaN warm-up
  xframe::IndMoneyFlowVolume widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndVolumeDelta

- Category: volume; draw style: pane
- Formula: Volume Delta per bar = Close Location Value * volume; CLV = ((close - low) - (high - close)) / (high - low).
- Compute: `std::vector<double> ind_volume_delta(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndVolumeDelta(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_volume_delta(series);  // one value per bar, NaN warm-up
  xframe::IndVolumeDelta widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: this is the signed-volume proxy CLV * volume per bar, not exchange-reported buy/sell volume.

## IndCumulativeVolumeDelta

- Category: volume; draw style: pane
- Formula: Cumulative Volume Delta = running sum of per-bar Volume Delta (CLV * volume).
- Compute: `std::vector<double> ind_cumulative_volume_delta(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndCumulativeVolumeDelta(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_cumulative_volume_delta(series);  // one value per bar, NaN warm-up
  xframe::IndCumulativeVolumeDelta widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndUpDownVolumeRatio

- Category: volume; draw style: pane
- Formula: Up/Down Volume Ratio = SMA(up-close volume, 14) / SMA(down-close volume, 14).
- Compute: `std::vector<double> ind_up_down_volume_ratio(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndUpDownVolumeRatio(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_up_down_volume_ratio(series);  // one value per bar, NaN warm-up
  xframe::IndUpDownVolumeRatio widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndIntradayIntensityIndex

- Category: volume; draw style: pane
- Formula: Intraday Intensity Index (Elder): per bar ((2*close - high - low) / (high - low)) * volume, summed over the last 21 bars.
- Compute: `std::vector<double> ind_intraday_intensity_index(const OHLCVSeries& series, int period = 21)`
- Constructor: `IndIntradayIntensityIndex(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 20 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_intraday_intensity_index(series);  // one value per bar, NaN warm-up
  xframe::IndIntradayIntensityIndex widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndHurstExponent

- Category: other; draw style: pane
- Formula: Hurst Exponent (rescaled range): H = log(R/S) / log(n/2), where R is the range of the cumulative deviations from the 50-bar mean close and S is the standard deviation of close.
- Compute: `std::vector<double> ind_hurst_exponent(const OHLCVSeries& series, int period = 50)`
- Constructor: `IndHurstExponent(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 49 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_hurst_exponent(series);  // one value per bar, NaN warm-up
  xframe::IndHurstExponent widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: single-window rescaled-range estimate H = log(R/S) / log(n/2); not a multi-window regression slope.

## IndFractalDimension

- Category: other; draw style: pane
- Formula: Fractal Dimension D = 2 - Hurst Exponent, with the Hurst Exponent computed by the rescaled-range method over 50 bars. NOTE: this widget recomputes the Hurst estimate internally; it is the standard D = 2 - H relation, not an independent box-count.
- Compute: `std::vector<double> ind_fractal_dimension(const OHLCVSeries& series, int period = 50)`
- Constructor: `IndFractalDimension(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 49 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_fractal_dimension(series);  // one value per bar, NaN warm-up
  xframe::IndFractalDimension widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the fractal dimension here is the standard D = 2 - H relation applied to the rescaled-range Hurst estimate, not an independent box-counting dimension.

## IndEfficiencyRatio

- Category: other; draw style: pane
- Formula: Kaufman Efficiency Ratio = |close - close 10 bars ago| / sum of |daily close changes| over 10 bars.
- Compute: `std::vector<double> ind_efficiency_ratio(const OHLCVSeries& series, int period = 10)`
- Constructor: `IndEfficiencyRatio(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 10 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_efficiency_ratio(series);  // one value per bar, NaN warm-up
  xframe::IndEfficiencyRatio widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the denominator sums absolute close-to-close changes over the window including the current bar's change.

## IndLinearRegressionSlope

- Category: other; draw style: pane
- Formula: Linear Regression Slope: least-squares slope of close against bar index over the last 14 bars.
- Compute: `std::vector<double> ind_linear_regression_slope(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndLinearRegressionSlope(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_linear_regression_slope(series);  // one value per bar, NaN warm-up
  xframe::IndLinearRegressionSlope widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndLinearRegressionIntercept

- Category: other; draw style: pane
- Formula: Linear Regression Intercept: least-squares intercept of close against bar index over the last 14 bars (value of the fitted line at bar index 0 of the window).
- Compute: `std::vector<double> ind_linear_regression_intercept(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndLinearRegressionIntercept(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_linear_regression_intercept(series);  // one value per bar, NaN warm-up
  xframe::IndLinearRegressionIntercept widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndRSquared

- Category: other; draw style: pane
- Formula: R-Squared: 1 - SSE/SST of the least-squares line of close against bar index over the last 14 bars.
- Compute: `std::vector<double> ind_r_squared(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndRSquared(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_r_squared(series);  // one value per bar, NaN warm-up
  xframe::IndRSquared widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndStandardError

- Category: other; draw style: pane
- Formula: Standard Error of the linear regression of close against bar index over the last 14 bars: sqrt(SSE / (n - 2)).
- Compute: `std::vector<double> ind_standard_error(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndStandardError(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_standard_error(series);  // one value per bar, NaN warm-up
  xframe::IndStandardError widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndLinearRegressionForecast

- Category: other; draw style: pane
- Formula: Linear Regression Forecast: the fitted line value at the last bar of each 14-bar window (intercept + slope * (n - 1)).
- Compute: `std::vector<double> ind_linear_regression_forecast(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndLinearRegressionForecast(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 13 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_linear_regression_forecast(series);  // one value per bar, NaN warm-up
  xframe::IndLinearRegressionForecast widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndZScore

- Category: other; draw style: pane
- Formula: Z-Score = (close - SMA(close, 20)) / StdDev(close, 20).
- Compute: `std::vector<double> ind_z_score(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndZScore(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_z_score(series);  // one value per bar, NaN warm-up
  xframe::IndZScore widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndPercentileRank

- Category: other; draw style: pane
- Formula: Percentile Rank = 100 * (number of closes in the last 20 bars below the current close) / 19.
- Compute: `std::vector<double> ind_percentile_rank(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndPercentileRank(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_percentile_rank(series);  // one value per bar, NaN warm-up
  xframe::IndPercentileRank widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the denominator is period - 1 (19), so a close above every prior bar in the window scores 100.

## IndMedianPrice

- Category: other; draw style: overlay
- Formula: Median Price = rolling median of close over the last 20 bars.
- Compute: `std::vector<double> ind_median_price(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndMedianPrice(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_median_price(series);  // one value per bar, NaN warm-up
  xframe::IndMedianPrice widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndTypicalPrice

- Category: other; draw style: overlay
- Formula: Typical Price = (high + low + close) / 3 per bar.
- Compute: `std::vector<double> ind_typical_price(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndTypicalPrice(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_typical_price(series);  // one value per bar, NaN warm-up
  xframe::IndTypicalPrice widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndWeightedClose

- Category: other; draw style: overlay
- Formula: Weighted Close = (high + low + 2 * close) / 4 per bar.
- Compute: `std::vector<double> ind_weighted_close(const OHLCVSeries& series, int period = 1)`
- Constructor: `IndWeightedClose(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 0 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_weighted_close(series);  // one value per bar, NaN warm-up
  xframe::IndWeightedClose widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```

## IndSkewness

- Category: other; draw style: pane
- Formula: Skewness = mean of ((close - SMA(close,20)) / StdDev(close,20))^3 over the last 20 bars (population moment).
- Compute: `std::vector<double> ind_skewness(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndSkewness(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_skewness(series);  // one value per bar, NaN warm-up
  xframe::IndSkewness widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: population moment (divided by n); the Fisher adjusted (sample) skewness is not used.

## IndKurtosis

- Category: other; draw style: pane
- Formula: Excess Kurtosis = mean of ((close - SMA(close,20)) / StdDev(close,20))^4 over the last 20 bars, minus 3.
- Compute: `std::vector<double> ind_kurtosis(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndKurtosis(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_kurtosis(series);  // one value per bar, NaN warm-up
  xframe::IndKurtosis widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: excess kurtosis, population moment minus 3.

## IndAutocorrelation

- Category: other; draw style: pane
- Formula: Lag-1 Autocorrelation of log returns over the last 20 bars: sum((r_t - mean)(r_{t-1} - mean)) / sum((r_t - mean)^2).
- Compute: `std::vector<double> ind_autocorrelation(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndAutocorrelation(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 20 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_autocorrelation(series);  // one value per bar, NaN warm-up
  xframe::IndAutocorrelation widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: lag-1 autocorrelation of log returns; the numerator sum runs over window pairs only.

## IndShannonEntropy

- Category: other; draw style: pane
- Formula: Shannon Entropy (bits) of the up / down / flat daily-close sign distribution over the last 20 bars: -sum(p * log2(p)).
- Compute: `std::vector<double> ind_shannon_entropy(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndShannonEntropy(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 20 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_shannon_entropy(series);  // one value per bar, NaN warm-up
  xframe::IndShannonEntropy widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: entropy is over the three-state (up / down / flat) daily-close sign distribution, not over return magnitudes.

## IndUpDownRatio

- Category: other; draw style: pane
- Formula: Up/Down Ratio = (number of up closes in the last 14 bars) / (number of down closes in the last 14 bars).
- Compute: `std::vector<double> ind_up_down_ratio(const OHLCVSeries& series, int period = 14)`
- Constructor: `IndUpDownRatio(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 14 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_up_down_ratio(series);  // one value per bar, NaN warm-up
  xframe::IndUpDownRatio widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: when there are no down closes in the window the value is the up count itself (not infinity).

## IndStandardPivotPoints

- Category: other; draw style: overlay
- Formula: Standard Pivot Points from the previous bar: Pivot = (H+L+C)/3; R1 = 2*Pivot - Low; S1 = 2*Pivot - High.
- Compute: `StandardPivotPointsResult ind_standard_pivot_points(const OHLCVSeries& series, int period = 1)`
- Multi-output struct `StandardPivotPointsResult` with fields: `pivot`, `r1`, `s1`
- Constructor: `IndStandardPivotPoints(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 1 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_standard_pivot_points(series);  // one value per bar, NaN warm-up
  xframe::IndStandardPivotPoints widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: levels for bar i are computed from bar i-1 only (previous-bar pivots); R2/R3/S2/S3 are not computed.

## IndFibonacciPivotPoints

- Category: other; draw style: overlay
- Formula: Fibonacci Pivot Points from the previous bar: Pivot = (H+L+C)/3; R1 = Pivot + 0.382*(H-L); S1 = Pivot - 0.382*(H-L).
- Compute: `FibonacciPivotPointsResult ind_fibonacci_pivot_points(const OHLCVSeries& series, int period = 1)`
- Multi-output struct `FibonacciPivotPointsResult` with fields: `pivot`, `r1`, `s1`
- Constructor: `IndFibonacciPivotPoints(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 1 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_fibonacci_pivot_points(series);  // one value per bar, NaN warm-up
  xframe::IndFibonacciPivotPoints widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: only the 0.382 extension is provided (R1/S1); 0.618 / 1.0 extensions are not computed.

## IndCamarillaPivotPoints

- Category: other; draw style: overlay
- Formula: Camarilla Pivot Points from the previous bar: Pivot = (H+L+C)/3; R1 = Close + (H-L)*1.1/12; S1 = Close - (H-L)*1.1/12.
- Compute: `CamarillaPivotPointsResult ind_camarilla_pivot_points(const OHLCVSeries& series, int period = 1)`
- Multi-output struct `CamarillaPivotPointsResult` with fields: `pivot`, `r1`, `s1`
- Constructor: `IndCamarillaPivotPoints(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 1 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_camarilla_pivot_points(series);  // one value per bar, NaN warm-up
  xframe::IndCamarillaPivotPoints widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: only R1/S1 (the 1.1/12 levels) are provided; R2-R4/S2-S4 are not computed.

## IndWoodiePivotPoints

- Category: other; draw style: overlay
- Formula: Woodie Pivot Points from the previous bar: Pivot = (H+L+2*C)/4; R1 = 2*Pivot - Low; S1 = 2*Pivot - High.
- Compute: `WoodiePivotPointsResult ind_woodie_pivot_points(const OHLCVSeries& series, int period = 1)`
- Multi-output struct `WoodiePivotPointsResult` with fields: `pivot`, `r1`, `s1`
- Constructor: `IndWoodiePivotPoints(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 1 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_woodie_pivot_points(series);  // one value per bar, NaN warm-up
  xframe::IndWoodiePivotPoints widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: the pivot weights the close twice, (H + L + 2*C) / 4, per the Woodie definition.

## IndDominantCyclePeriod

- Category: other; draw style: pane
- Formula: Dominant Cycle Period (zero-crossing estimate): count sign changes of (close - SMA(close,10)) over the last 50 bars; period = 2 * 50 / crossings.
- Compute: `std::vector<double> ind_dominant_cycle_period(const OHLCVSeries& series, int period = 50)`
- Constructor: `IndDominantCyclePeriod(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 49 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_dominant_cycle_period(series);  // one value per bar, NaN warm-up
  xframe::IndDominantCyclePeriod widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: this is a zero-crossing estimate of the dominant cycle, not a Hilbert-transform or FFT period measurement.

## IndCorrelation

- Category: other; draw style: pane
- Formula: Correlation: Pearson correlation between bar index and close over the last 20 bars.
- Compute: `std::vector<double> ind_correlation(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndCorrelation(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_correlation(series);  // one value per bar, NaN warm-up
  xframe::IndCorrelation widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: Pearson correlation of close against bar index (a trend-linearity measure), not against another symbol.

## IndCovariance

- Category: other; draw style: pane
- Formula: Covariance: population covariance between close and volume over the last 20 bars.
- Compute: `std::vector<double> ind_covariance(const OHLCVSeries& series, int period = 20)`
- Constructor: `IndCovariance(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles, int x, int y, int width, int height, const OHLCVSeries& series = xframe::make_seeded_series(250), int period = <default>, double mult = <default>)`
- Key methods: `draw()`, `on_button_press(x, y, button)` (wheel zooms, drag pans), `on_button_release(x, y, button)`, `on_motion(x, y)` (crosshair + drag), `on_key(key, str)` (Left/Right pan, +/- zoom, r/Home reset), `line_count()`, `line_values(i)`, `set_bounds()`, `contains()`.
- Warm-up: first 19 bar(s) are NaN.
- Example:
  ```cpp
  xframe::OHLCVSeries series = xframe::make_seeded_series(250);
  auto values = xframe::ind_covariance(series);  // one value per bar, NaN warm-up
  xframe::IndCovariance widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
  widget.draw();
  ```
- Unclear from the header: population covariance (divided by n, not n-1) between close and volume.
