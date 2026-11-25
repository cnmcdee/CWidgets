# Stock Market indicators: momentum (85)

Momentum indicators of the XFrame Stock Market section. Each entry: name, formula, API, a short working example, and honest notes where the header does not settle a behaviour.

Shared conventions: output length == input length; leading warm-up bars are NaN until enough bars exist. All demos and tests use `xframe::make_seeded_series(250)` (deterministic synthetic data, no network, no live market data).

## RSI

- Formula: Wilder RSI: 100 * avg gain / (avg gain + avg loss), gains/losses Wilder-smoothed over period.
- Compute: `std::vector<double> ind_rsi(const std::vector<double>& closes, int period = 14)`
- Widget: `IndRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_rsi(xframe::series_closes(series));
IndRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ConnorsRSI

- Formula: Connors RSI: average of RSI(3) of closes, RSI(2) of the up/down streak, and the 100-bar percent rank of the 1-day return.
- Compute: `std::vector<double> ind_connors_rsi(const xframe::OHLCVSeries& s, int period = 3)`
- Widget: `IndConnorsRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndConnorsRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_connors_rsi(series);
IndConnorsRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## StochRSI

- Formula: Stochastic RSI: 100 * (RSI - lowest RSI over period) / (highest RSI - lowest RSI over period).
- Compute: `std::vector<double> ind_stoch_rsi(const std::vector<double>& closes, int period = 14)`
- Widget: `IndStochRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndStochRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_stoch_rsi(xframe::series_closes(series));
IndStochRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## CutlerRSI

- Formula: Cutler RSI: same as RSI but gains/losses averaged with a simple moving average instead of Wilder smoothing.
- Compute: `std::vector<double> ind_cutler_rsi(const std::vector<double>& closes, int period = 14)`
- Widget: `IndCutlerRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndCutlerRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_cutler_rsi(xframe::series_closes(series));
IndCutlerRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## LaguerreRSI

- Formula: Laguerre RSI (Ehlers): price passed through a 4-pole Laguerre filter; RSI-style ratio of up-moves (CU) to up+down moves (CD) of the filter outputs.
- Compute: `std::vector<double> ind_laguerre_rsi(const std::vector<double>& closes, double gamma = 0.8)`
- Widget: `IndLaguerreRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, double gamma = 0.8)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndLaguerreRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_laguerre_rsi(xframe::series_closes(series));
IndLaguerreRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## AdaptiveRSI

- Formula: Adaptive RSI: Wilder RSI whose smoothing period adapts per bar to the Kaufman efficiency ratio over 10 bars (efficient market -> short period, choppy -> long period, 5..30).
- Compute: `std::vector<double> ind_adaptive_rsi(const std::vector<double>& closes, int period = 14)`
- Widget: `IndAdaptiveRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndAdaptiveRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_adaptive_rsi(xframe::series_closes(series));
IndAdaptiveRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## MACD

- Formula: MACD (Appel): MACD line = EMA(fast) - EMA(slow); signal = EMA(signal) of the MACD line; histogram = MACD - signal.
- Compute: `IndMACDResult ind_macd(const std::vector<double>& closes, int fast = 12, int slow = 26, int signal = 9)`
- Widget: `IndMACD(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 12, int slow = 26, int signal = 9)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndMACD.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_macd(xframe::series_closes(series));
IndMACD w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## MACDHistogram

- Formula: MACD histogram: EMA(fast) - EMA(slow) minus its EMA(signal) signal line; the histogram alone.
- Compute: `std::vector<double> ind_macd_histogram(const std::vector<double>& closes, int fast = 12, int slow = 26, int signal = 9)`
- Widget: `IndMACDHistogram(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 12, int slow = 26, int signal = 9)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndMACDHistogram.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_macd_histogram(xframe::series_closes(series));
IndMACDHistogram w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ZeroLagMACD

- Formula: Zero-lag MACD: MACD line built from zero-lag EMAs (EMA of price + (price - price lagged by (period-1)/2)); this widget plots the zero-lag MACD line.
- Compute: `std::vector<double> ind_zero_lag_macd(const std::vector<double>& closes, int fast = 12, int slow = 26, int signal = 9)`
- Widget: `IndZeroLagMACD(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 12, int slow = 26, int signal = 9)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndZeroLagMACD.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_zero_lag_macd(xframe::series_closes(series));
IndZeroLagMACD w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## MACDofRSI

- Formula: MACD of RSI: the RSI series is computed first, then a MACD line (EMA(fast) - EMA(slow)) is taken of the RSI values.
- Compute: `std::vector<double> ind_macd_of_rsi(const std::vector<double>& closes, int rsi_period = 14, int fast = 12, int slow = 26)`
- Widget: `IndMACDofRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int rsi_period = 14, int fast = 12, int slow = 26)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndMACDofRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_macd_of_rsi(xframe::series_closes(series));
IndMACDofRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## StochasticFast

- Formula: Fast stochastic (Lane): %K = 100 * (close - lowest low over period) / (highest high - lowest low); %D = SMA(smooth) of %K.
- Compute: `IndStochasticFastResult ind_stochastic_fast(const xframe::OHLCVSeries& s, int period = 14, int smooth = 3)`
- Widget: `IndStochasticFast(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14, int smooth = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndStochasticFast.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_stochastic_fast(series);
IndStochasticFast w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## StochasticSlow

- Formula: Slow stochastic (Lane): slow %K = SMA(smooth) of fast %K; slow %D = SMA(smooth) of slow %K.
- Compute: `IndStochasticSlowResult ind_stochastic_slow(const xframe::OHLCVSeries& s, int period = 14, int smooth = 3)`
- Widget: `IndStochasticSlow(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14, int smooth = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndStochasticSlow.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_stochastic_slow(series);
IndStochasticSlow w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## StochasticFull

- Formula: Full stochastic (Lane): %K = SMA(smooth_k) of raw %K over period; %D = SMA(smooth_d) of %K; all three periods are parameters.
- Compute: `IndStochasticFullResult ind_stochastic_full(const xframe::OHLCVSeries& s, int period = 14, int smooth_k = 3, int smooth_d = 3)`
- Widget: `IndStochasticFull(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14, int smooth_k = 3, int smooth_d = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndStochasticFull.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_stochastic_full(series);
IndStochasticFull w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## KDJ

- Formula: KDJ stochastic: RSV = 100 * (close - lowest low) / (highest high - lowest low); K = 2/3 K(prev) + 1/3 RSV (seeded 50); D = 2/3 D(prev) + 1/3 K; J = 3K - 2D.
- Compute: `IndKDJResult ind_kdj(const xframe::OHLCVSeries& s, int period = 9, int smooth = 3)`
- Widget: `IndKDJ(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 9, int smooth = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndKDJ.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_kdj(series);
IndKDJ w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## WilliamsR

- Formula: Williams %R (Williams): -100 * (highest high over period - close) / (highest high - lowest low over period); 0 to -100.
- Compute: `std::vector<double> ind_williams_r(const xframe::OHLCVSeries& s, int period = 14)`
- Widget: `IndWilliamsR(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndWilliamsR.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_williams_r(series);
IndWilliamsR w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SMI

- Formula: Stochastic Momentum Index (Blau): 100 * double-EMA-smoothed (close - (high+low)/2) / double-EMA-smoothed (high - low).
- Compute: `std::vector<double> ind_smi(const xframe::OHLCVSeries& s, int period = 13)`
- Widget: `IndSMI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 13)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndSMI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_smi(series);
IndSMI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## DSS

- Formula: Double Smoothed Stochastic (Bressert): raw %K over period, EMA-smoothed twice with period smooth; the twice-smoothed %K is the output.
- Compute: `std::vector<double> ind_dss(const xframe::OHLCVSeries& s, int period = 10, int smooth = 3)`
- Widget: `IndDSS(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10, int smooth = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndDSS.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_dss(series);
IndDSS w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## CCI

- Formula: Commodity Channel Index (Lambert): (typical price - SMA(period) of typical price) / (0.015 * mean deviation of typical price).
- Compute: `std::vector<double> ind_cci(const xframe::OHLCVSeries& s, int period = 20)`
- Widget: `IndCCI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndCCI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_cci(series);
IndCCI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ROC

- Formula: Rate of Change: 100 * (close - close[period]) / close[period].
- Compute: `std::vector<double> ind_roc(const std::vector<double>& closes, int period = 12)`
- Widget: `IndROC(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 12)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndROC.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_roc(xframe::series_closes(series));
IndROC w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ROCR

- Formula: Rate of Change Ratio (TA-Lib ROCR): close / close[period].
- Compute: `std::vector<double> ind_rocr(const std::vector<double>& closes, int period = 12)`
- Widget: `IndROCR(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 12)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndROCR.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_rocr(xframe::series_closes(series));
IndROCR w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## Momentum

- Formula: Momentum: close - close[period].
- Compute: `std::vector<double> ind_momentum(const std::vector<double>& closes, int period = 10)`
- Widget: `IndMomentum(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndMomentum.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_momentum(xframe::series_closes(series));
IndMomentum w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## PPO

- Formula: Percentage Price Oscillator: 100 * (EMA(fast) - EMA(slow)) / EMA(slow).
- Compute: `std::vector<double> ind_ppo(const std::vector<double>& closes, int fast = 12, int slow = 26)`
- Widget: `IndPPO(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 12, int slow = 26)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndPPO.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_ppo(xframe::series_closes(series));
IndPPO w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## APO

- Formula: Absolute Price Oscillator: EMA(fast) - EMA(slow) in price units.
- Compute: `std::vector<double> ind_apo(const std::vector<double>& closes, int fast = 12, int slow = 26)`
- Widget: `IndAPO(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 12, int slow = 26)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndAPO.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_apo(xframe::series_closes(series));
IndAPO w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## AwesomeOscillator

- Formula: Awesome Oscillator (Bill Williams): SMA(fast) of median price - SMA(slow) of median price; median price = (high + low) / 2.
- Compute: `std::vector<double> ind_awesome_oscillator(const xframe::OHLCVSeries& s, int fast = 5, int slow = 34)`
- Widget: `IndAwesomeOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 5, int slow = 34)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndAwesomeOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_awesome_oscillator(series);
IndAwesomeOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## AcceleratorOscillator

- Formula: Accelerator Oscillator (Bill Williams): Awesome Oscillator - SMA(smooth) of the Awesome Oscillator.
- Compute: `std::vector<double> ind_accelerator_oscillator(const xframe::OHLCVSeries& s, int fast = 5, int slow = 34, int smooth = 5)`
- Widget: `IndAcceleratorOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 5, int slow = 34, int smooth = 5)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndAcceleratorOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_accelerator_oscillator(series);
IndAcceleratorOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ChandeMomentumOscillator

- Formula: Chande Momentum Oscillator: 100 * (sum of up closes - sum of down closes) / (sum of up closes + sum of down closes) over period.
- Compute: `std::vector<double> ind_chande_momentum_oscillator(const std::vector<double>& closes, int period = 20)`
- Widget: `IndChandeMomentumOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndChandeMomentumOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_chande_momentum_oscillator(xframe::series_closes(series));
IndChandeMomentumOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## TSI

- Formula: True Strength Index (Blau): 100 * double-EMA-smoothed momentum / double-EMA-smoothed absolute momentum; momentum = close - close[1].
- Compute: `std::vector<double> ind_tsi(const std::vector<double>& closes, int long_period = 25, int short_period = 13)`
- Widget: `IndTSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int long_period = 25, int short_period = 13)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndTSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_tsi(xframe::series_closes(series));
IndTSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## UltimateOscillator

- Formula: Ultimate Oscillator (Larry Williams): buying pressure = close - min(low, prev close); true range = max(high, prev close) - min(low, prev close); 100 * (4*avg7 + 2*avg14 + avg28) / 7.
- Compute: `std::vector<double> ind_ultimate_oscillator(const xframe::OHLCVSeries& s, int p1 = 7, int p2 = 14, int p3 = 28)`
- Widget: `IndUltimateOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int p1 = 7, int p2 = 14, int p3 = 28)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndUltimateOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_ultimate_oscillator(series);
IndUltimateOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## KST

- Formula: Know Sure Thing (Pring): weighted sum of four smoothed rates of change: 1*SMA10(ROC10) + 2*SMA10(ROC15) + 3*SMA10(ROC20) + 4*SMA9(ROC30).
- Compute: `std::vector<double> ind_kst(const std::vector<double>& closes, int dummy = 0)`
- Widget: `IndKST(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndKST.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_kst(xframe::series_closes(series));
IndKST w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## FisherTransform

- Formula: Fisher Transform (Ehlers): price normalized to [-1, 1] over period, then Fisher transform 0.5 * ln((1+x)/(1-x)), half-weighted with the previous value.
- Compute: `std::vector<double> ind_fisher_transform(const std::vector<double>& closes, int period = 10)`
- Widget: `IndFisherTransform(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndFisherTransform.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_fisher_transform(xframe::series_closes(series));
IndFisherTransform w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## InverseFisherTransform

- Formula: Inverse Fisher Transform (Ehlers): RSI(period) normalized to [-1, 1], then the inverse Fisher (exp(2x)-1)/(exp(2x)+1), rescaled to 0..100.
- Compute: `std::vector<double> ind_inverse_fisher_transform(const std::vector<double>& closes, int period = 14)`
- Widget: `IndInverseFisherTransform(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndInverseFisherTransform.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_inverse_fisher_transform(xframe::series_closes(series));
IndInverseFisherTransform w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ErgodicOscillator

- Formula: Ergodic Oscillator (Blau): the True Strength Index with short smoothing periods (5, 3); 100 * double-EMA-smoothed momentum / double-EMA-smoothed absolute momentum.
- Compute: `std::vector<double> ind_ergodic_oscillator(const std::vector<double>& closes, int long_period = 5, int short_period = 3)`
- Widget: `IndErgodicOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int long_period = 5, int short_period = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndErgodicOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_ergodic_oscillator(xframe::series_closes(series));
IndErgodicOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SchaffTrendCycle

- Formula: Schaff Trend Cycle (Schaff): stochastic of the MACD line (EMA(fast) - EMA(slow)) over cycle bars, EMA-smoothed over cycle bars, scaled 0..100.
- Compute: `std::vector<double> ind_schaff_trend_cycle(const std::vector<double>& closes, int fast = 23, int slow = 50, int cycle = 10)`
- Widget: `IndSchaffTrendCycle(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int fast = 23, int slow = 50, int cycle = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndSchaffTrendCycle.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_schaff_trend_cycle(xframe::series_closes(series));
IndSchaffTrendCycle w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## RelativeVigorIndex

- Formula: Relative Vigor Index (Ehlers): SMA(period) of (close - open) / SMA(period) of (high - low); -1 to 1.
- Compute: `std::vector<double> ind_relative_vigor_index(const xframe::OHLCVSeries& s, int period = 10)`
- Widget: `IndRelativeVigorIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndRelativeVigorIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_relative_vigor_index(series);
IndRelativeVigorIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ElderForceIndex

- Formula: Elder Force Index: EMA(period) of (close - prev close) * volume.
- Compute: `std::vector<double> ind_elder_force_index(const xframe::OHLCVSeries& s, int period = 13)`
- Widget: `IndElderForceIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 13)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndElderForceIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_elder_force_index(series);
IndElderForceIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ElderImpulse

- Formula: Elder Impulse System: +1 when EMA(13) is rising and the MACD histogram is rising; -1 when both are falling; 0 otherwise.
- Compute: `std::vector<double> ind_elder_impulse(const xframe::OHLCVSeries& s, int ema_period = 13, int fast = 12, int slow = 26)`
- Widget: `IndElderImpulse(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int ema_period = 13, int fast = 12, int slow = 26)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: overlay.
- Example:

```cpp
#include "IndElderImpulse.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_elder_impulse(series);
IndElderImpulse w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## BullPower

- Formula: Elder Bull Power: high - EMA(period) of closes.
- Compute: `std::vector<double> ind_bull_power(const xframe::OHLCVSeries& s, int period = 13)`
- Widget: `IndBullPower(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 13)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndBullPower.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_bull_power(series);
IndBullPower w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## BearPower

- Formula: Elder Bear Power: low - EMA(period) of closes.
- Compute: `std::vector<double> ind_bear_power(const xframe::OHLCVSeries& s, int period = 13)`
- Widget: `IndBearPower(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 13)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndBearPower.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_bear_power(series);
IndBearPower w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## CyberCycle

- Formula: Ehlers Cyber Cycle: price smoothed over 4 bars, then the cycle recursion with alpha 0.07: cycle = (1-a/2)^2 * (smooth - 2*smooth[1] + smooth[2]) + 2*(1-a)*cycle[1] - (1-a)^2 * cycle[2].
- Compute: `std::vector<double> ind_cyber_cycle(const std::vector<double>& closes, int dummy = 0)`
- Widget: `IndCyberCycle(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndCyberCycle.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_cyber_cycle(xframe::series_closes(series));
IndCyberCycle w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## CenterOfGravity

- Formula: Ehlers Center of Gravity: negative weighted average of price over period (weights 1..period, most recent heaviest), divided by the sum of prices; oscillates around zero with almost no lag.
- Compute: `std::vector<double> ind_center_of_gravity(const std::vector<double>& closes, int period = 10)`
- Widget: `IndCenterOfGravity(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndCenterOfGravity.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_center_of_gravity(xframe::series_closes(series));
IndCenterOfGravity w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SineWave

- Formula: Ehlers Sine Wave: sine and lead-sine lines derived from the Cyber Cycle quadrature (cycle and its one-bar change normalized by their combined magnitude). Unclear from the header: this is a simplified quadrature from the Cyber Cycle, not a full Hilbert transform.
- Compute: `IndSineWaveResult ind_sine_wave(const std::vector<double>& closes, int dummy = 0)`
- Widget: `IndSineWave(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndSineWave.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_sine_wave(xframe::series_closes(series));
IndSineWave w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## DominantCycle

- Formula: Ehlers Dominant Cycle period: estimated from zero crossings of the Cyber Cycle over the last 50 bars (period = 100 / crossings, clamped 10..40). Unclear from the header: this is a zero-crossing estimate, not the MESA phasor measurement.
- Compute: `std::vector<double> ind_dominant_cycle(const std::vector<double>& closes, int dummy = 0)`
- Widget: `IndDominantCycle(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndDominantCycle.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_dominant_cycle(xframe::series_closes(series));
IndDominantCycle w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## ChandeForecastOscillator

- Formula: Chande Forecast Oscillator: 100 * (close - linear-regression fitted value over period) / close.
- Compute: `std::vector<double> ind_chande_forecast_oscillator(const std::vector<double>& closes, int period = 5)`
- Widget: `IndChandeForecastOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 5)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndChandeForecastOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_chande_forecast_oscillator(xframe::series_closes(series));
IndChandeForecastOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## CoppockCurve

- Formula: Coppock Curve: WMA(wma) of (ROC(roc1) + ROC(roc2)).
- Compute: `std::vector<double> ind_coppock_curve(const std::vector<double>& closes, int roc1 = 14, int roc2 = 11, int wma = 10)`
- Widget: `IndCoppockCurve(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int roc1 = 14, int roc2 = 11, int wma = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndCoppockCurve.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_coppock_curve(xframe::series_closes(series));
IndCoppockCurve w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## Kairi

- Formula: Kairi Relative Index: 100 * (close - SMA(period)) / SMA(period).
- Compute: `std::vector<double> ind_kairi(const std::vector<double>& closes, int period = 25)`
- Widget: `IndKairi(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 25)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndKairi.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_kairi(xframe::series_closes(series));
IndKairi w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## DetrendedPriceOscillator

- Formula: Detrended Price Oscillator: close - SMA(period) displaced back by period/2 + 1 bars.
- Compute: `std::vector<double> ind_dpo(const std::vector<double>& closes, int period = 20)`
- Widget: `IndDetrendedPriceOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndDetrendedPriceOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_dpo(xframe::series_closes(series));
IndDetrendedPriceOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## DeMarker

- Formula: DeMarker (DeMark): DeMax = high - prev high when it exceeds prev low - low, else 0; DeMin likewise for lows; DeM = 100 * SMA(DeMax) / (SMA(DeMax) + SMA(DeMin)).
- Compute: `std::vector<double> ind_demarker(const xframe::OHLCVSeries& s, int period = 14)`
- Widget: `IndDeMarker(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndDeMarker.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_demarker(series);
IndDeMarker w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## BalanceOfPower

- Formula: Balance of Power: SMA(period) of (close - open) / (high - low).
- Compute: `std::vector<double> ind_balance_of_power(const xframe::OHLCVSeries& s, int period = 14)`
- Widget: `IndBalanceOfPower(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndBalanceOfPower.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_balance_of_power(series);
IndBalanceOfPower w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## RelativeMomentumIndex

- Formula: Relative Momentum Index (RMI): RSI computed on momentum-period changes (close vs close[momentum]) instead of one-bar changes, Wilder-smoothed over period.
- Compute: `std::vector<double> ind_relative_momentum_index(const std::vector<double>& closes, int period = 20, int momentum = 5)`
- Widget: `IndRelativeMomentumIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 20, int momentum = 5)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndRelativeMomentumIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_relative_momentum_index(xframe::series_closes(series));
IndRelativeMomentumIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## IntradayMomentumIndex

- Formula: Intraday Momentum Index (Chande): 100 * sum of up-day (close - open) gains / (sum of gains + sum of down-day (open - close) losses) over period.
- Compute: `std::vector<double> ind_intraday_momentum_index(const xframe::OHLCVSeries& s, int period = 14)`
- Widget: `IndIntradayMomentumIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndIntradayMomentumIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_intraday_momentum_index(series);
IndIntradayMomentumIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## DynamicMomentumIndex

- Formula: Dynamic Momentum Index (Chande): RSI whose period adapts to relative volatility (stddev over 5 bars / stddev over 14 bars), clamped to 5..30 bars.
- Compute: `std::vector<double> ind_dynamic_momentum_index(const std::vector<double>& closes, int period = 14)`
- Widget: `IndDynamicMomentumIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndDynamicMomentumIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_dynamic_momentum_index(xframe::series_closes(series));
IndDynamicMomentumIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## WaveTrend

- Formula: WaveTrend Oscillator: ESA = EMA(esa) of typical price; D = EMA(esa) of |typical - ESA|; CI = (typical - ESA) / (0.015 * D); WT1 = EMA(d) of CI. This widget plots WT1.
- Compute: `std::vector<double> ind_wave_trend(const xframe::OHLCVSeries& s, int esa = 10, int d = 21)`
- Widget: `IndWaveTrend(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int esa = 10, int d = 21)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndWaveTrend.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_wave_trend(series);
IndWaveTrend w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## PrettyGoodOscillator

- Formula: Pretty Good Oscillator (Mark Johnson): (close - SMA(period)) / EMA(period) of true range, where true range = max(high-low, |high-prev close|, |low-prev close|).
- Compute: `std::vector<double> ind_pretty_good_oscillator(const xframe::OHLCVSeries& s, int period = 14)`
- Widget: `IndPrettyGoodOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndPrettyGoodOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_pretty_good_oscillator(series);
IndPrettyGoodOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## Inertia

- Formula: Inertia (Chande): SMA(period) of RSI(period); a smoothed RSI.
- Compute: `std::vector<double> ind_inertia(const std::vector<double>& closes, int period = 20)`
- Widget: `IndInertia(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndInertia.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_inertia(xframe::series_closes(series));
IndInertia w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## RangeExpansionIndex

- Formula: Range Expansion Index (DeMark): over period bars, sum of (close - close[2]) for bars whose high breaks above high[2] or low breaks below low[2], divided by the sum of |(close - close[2])| for the same bars, times 100.
- Compute: `std::vector<double> ind_range_expansion_index(const xframe::OHLCVSeries& s, int period = 8)`
- Widget: `IndRangeExpansionIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 8)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndRangeExpansionIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_range_expansion_index(series);
IndRangeExpansionIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## TDSequential

- Formula: TD Sequential setup (DeMark): counts consecutive closes below close[4] as a buy setup (+1..+9) or above close[4] as a sell setup (-1..-9); the count resets when the condition flips.
- Compute: `std::vector<double> ind_td_sequential(const std::vector<double>& closes, int dummy = 0)`
- Widget: `IndTDSequential(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndTDSequential.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_td_sequential(xframe::series_closes(series));
IndTDSequential w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## DoubleStochastic

- Formula: Double Stochastic: average of the %K over period and the %K over short_period, SMA-smoothed over smooth bars.
- Compute: `std::vector<double> ind_double_stochastic(const xframe::OHLCVSeries& s, int period = 14, int short_period = 5, int smooth = 3)`
- Widget: `IndDoubleStochastic(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14, int short_period = 5, int smooth = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndDoubleStochastic.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_double_stochastic(series);
IndDoubleStochastic w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## PriceMomentumOscillator

- Formula: Price Momentum Oscillator (Swenlin): one-bar ROC in percent, EMA-smoothed over slow bars then over fast bars, times 10.
- Compute: `std::vector<double> ind_price_momentum_oscillator(const std::vector<double>& closes, int slow = 35, int fast = 20)`
- Widget: `IndPriceMomentumOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int slow = 35, int fast = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndPriceMomentumOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_price_momentum_oscillator(xframe::series_closes(series));
IndPriceMomentumOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SwenlinTradingOscillator

- Formula: Swenlin Trading Oscillator: the Price Momentum Oscillator minus its EMA(signal) signal line.
- Compute: `std::vector<double> ind_swenlin_trading_oscillator(const std::vector<double>& closes, int slow = 35, int fast = 20, int signal = 10)`
- Widget: `IndSwenlinTradingOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int slow = 35, int fast = 20, int signal = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndSwenlinTradingOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_swenlin_trading_oscillator(xframe::series_closes(series));
IndSwenlinTradingOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## RSX

- Formula: RSX (Jurik): RSI whose average gains and losses are triple-EMA-smoothed instead of Wilder-smoothed. Unclear from the header: the Jurik smoothing is proprietary; this uses triple EMA as the published approximation.
- Compute: `std::vector<double> ind_rsx(const std::vector<double>& closes, int period = 14)`
- Widget: `IndRSX(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndRSX.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_rsx(xframe::series_closes(series));
IndRSX w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## JurikVelocity

- Formula: Jurik Velocity: one-bar ROC in percent, triple-EMA-smoothed over period. Unclear from the header: Jurik Velocity is proprietary; this uses triple EMA as the published approximation.
- Compute: `std::vector<double> ind_jurik_velocity(const std::vector<double>& closes, int period = 14)`
- Widget: `IndJurikVelocity(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndJurikVelocity.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_jurik_velocity(xframe::series_closes(series));
IndJurikVelocity w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## NormalizedROC

- Formula: Normalized ROC: ROC(period) divided by the rolling standard deviation of ROC(period) over norm bars.
- Compute: `std::vector<double> ind_normalized_roc(const std::vector<double>& closes, int period = 12, int norm = 20)`
- Widget: `IndNormalizedROC(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 12, int norm = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndNormalizedROC.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_normalized_roc(xframe::series_closes(series));
IndNormalizedROC w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## LaguerreFilter

- Formula: Laguerre Filter (Ehlers): the first output (L0) of a 4-pole Laguerre filter on price; a low-lag smoothed price line drawn over the candles.
- Compute: `std::vector<double> ind_laguerre_filter(const std::vector<double>& closes, double gamma = 0.8)`
- Widget: `IndLaguerreFilter(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, double gamma = 0.8)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: overlay.
- Example:

```cpp
#include "IndLaguerreFilter.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_laguerre_filter(xframe::series_closes(series));
IndLaguerreFilter w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SuperSmoother

- Formula: Ehlers SuperSmoother: a 2-pole low-pass filter with coefficients from the period (a1 = exp(-1.414*pi/period)); a smoothed price line drawn over the candles.
- Compute: `std::vector<double> ind_super_smoother(const std::vector<double>& closes, int period = 10)`
- Widget: `IndSuperSmoother(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: overlay.
- Example:

```cpp
#include "IndSuperSmoother.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_super_smoother(xframe::series_closes(series));
IndSuperSmoother w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## RainbowOscillator

- Formula: Rainbow Oscillator: 100 * (close - rainbow average) / rainbow average, where the rainbow average is the mean of SMA(2)..SMA(max_period).
- Compute: `std::vector<double> ind_rainbow_oscillator(const std::vector<double>& closes, int max_period = 10)`
- Widget: `IndRainbowOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int max_period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndRainbowOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_rainbow_oscillator(xframe::series_closes(series));
IndRainbowOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## LinearRegressionOscillator

- Formula: Linear Regression Oscillator: close minus the linear-regression fitted value over period bars.
- Compute: `std::vector<double> ind_linear_regression_oscillator(const std::vector<double>& closes, int period = 14)`
- Widget: `IndLinearRegressionOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndLinearRegressionOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_linear_regression_oscillator(xframe::series_closes(series));
IndLinearRegressionOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## PercentRank

- Formula: Percent Rank: 100 * (number of prior period-1 closes below the current close) / (period - 1).
- Compute: `std::vector<double> ind_percent_rank(const std::vector<double>& closes, int period = 14)`
- Widget: `IndPercentRank(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndPercentRank.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_percent_rank(xframe::series_closes(series));
IndPercentRank w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SpearmanRank

- Formula: Spearman Rank Correlation: rank correlation between closes and bar order over period bars; -1 to 1.
- Compute: `std::vector<double> ind_spearman_rank(const std::vector<double>& closes, int period = 14)`
- Widget: `IndSpearmanRank(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndSpearmanRank.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_spearman_rank(xframe::series_closes(series));
IndSpearmanRank w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## CorrelationCoefficient

- Formula: Correlation Coefficient: Pearson correlation between closes and bar order over period bars; -1 to 1.
- Compute: `std::vector<double> ind_correlation_coefficient(const std::vector<double>& closes, int period = 14)`
- Widget: `IndCorrelationCoefficient(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndCorrelationCoefficient.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_correlation_coefficient(xframe::series_closes(series));
IndCorrelationCoefficient w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## Decycler

- Formula: Ehlers Decycler: price high-pass filtered (price minus its SMA over 48 bars) and then SuperSmoothed over period; a low-lag smoothed price line drawn over the candles. Unclear from the header: the high-pass here uses an SMA subtraction rather than the textbook 2-pole high-pass.
- Compute: `std::vector<double> ind_decycler(const std::vector<double>& closes, int period = 10)`
- Widget: `IndDecycler(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: overlay.
- Example:

```cpp
#include "IndDecycler.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_decycler(xframe::series_closes(series));
IndDecycler w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## TrendFlex

- Formula: Ehlers TrendFlex: the 10-bar price change (close - close[10]), SuperSmoothed over period. Unclear from the header: this is the simplified TrendFlex form (smoothed momentum), not the full roofing-filter version.
- Compute: `std::vector<double> ind_trend_flex(const std::vector<double>& closes, int period = 10)`
- Widget: `IndTrendFlex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndTrendFlex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_trend_flex(xframe::series_closes(series));
IndTrendFlex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## RoofingFilter

- Formula: Ehlers Roofing Filter: price high-pass filtered (price minus its SMA over hp_period bars), then SuperSmoothed over period.
- Compute: `std::vector<double> ind_roofing_filter(const std::vector<double>& closes, int period = 10, int hp_period = 48)`
- Widget: `IndRoofingFilter(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10, int hp_period = 48)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndRoofingFilter.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_roofing_filter(xframe::series_closes(series));
IndRoofingFilter w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## Reflex

- Formula: Ehlers Reflex: the 10-bar price change (close - close[10]), SuperSmoothed over period and normalized by the SMA(10) of the high-low range.
- Compute: `std::vector<double> ind_reflex(const xframe::OHLCVSeries& s, int period = 10)`
- Widget: `IndReflex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndReflex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_reflex(series);
IndReflex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## EvenBetterSineWave

- Formula: Ehlers Even Better SineWave: sine and lead-sine lines from the Roofing Filter output and its one-bar change, normalized by their combined magnitude. Unclear from the header: quadrature is taken from the roofing filter, not a Hilbert transform.
- Compute: `IndEvenBetterSineWaveResult ind_even_better_sinewave(const std::vector<double>& closes, int period = 10, int hp_period = 48)`
- Widget: `IndEvenBetterSineWave(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 10, int hp_period = 48)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndEvenBetterSineWave.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_even_better_sinewave(xframe::series_closes(series));
IndEvenBetterSineWave w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## StochasticPop

- Formula: Stochastic Pop (Bernstein): +1 on the bar the fast stochastic %K crosses above 80, -1 on the bar it crosses below 20, 0 otherwise; a pop signal series.
- Compute: `std::vector<double> ind_stochastic_pop(const xframe::OHLCVSeries& s, int period = 14)`
- Widget: `IndStochasticPop(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndStochasticPop.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_stochastic_pop(series);
IndStochasticPop w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## GannSwingOscillator

- Formula: Gann Swing Oscillator: swing state, +1 on an up-swing bar (higher high and higher low than the previous bar), -1 on a down-swing bar (lower high and lower low), otherwise the previous state is carried; 0 before the first swing.
- Compute: `std::vector<double> ind_gann_swing_oscillator(const xframe::OHLCVSeries& s, int dummy = 0)`
- Widget: `IndGannSwingOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndGannSwingOscillator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_gann_swing_oscillator(series);
IndGannSwingOscillator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SpecialK

- Formula: Special K (Pring): 1*SMA10(ROC10) + 2*SMA10(ROC15) + 3*SMA10(ROC20) + 4*SMA15(ROC30). Differs from KST only in the smoothing of the 30-bar ROC (SMA15 here, SMA9 in KST).
- Compute: `std::vector<double> ind_special_k(const std::vector<double>& closes, int dummy = 0)`
- Widget: `IndSpecialK(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndSpecialK.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_special_k(xframe::series_closes(series));
IndSpecialK w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## AdaptiveCyberCycle

- Formula: Ehlers Adaptive Cyber Cycle: the Cyber Cycle recursion with alpha adapted per bar from the dominant-cycle period estimate (zero-crossing estimate over 50 bars): alpha = clamp(0.07 * 20 / period, 0.02, 0.20). Unclear from the header: the period estimate is a zero-crossing estimate, not the MESA phasor measurement.
- Compute: `std::vector<double> ind_adaptive_cyber_cycle(const std::vector<double>& closes, int dummy = 0)`
- Widget: `IndAdaptiveCyberCycle(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndAdaptiveCyberCycle.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_adaptive_cyber_cycle(xframe::series_closes(series));
IndAdaptiveCyberCycle w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## DisparityIndex

- Formula: Disparity Index: 100 * (close - SMA(period)) / SMA(period); 0 when price sits on its moving average, positive above it, negative below it.
- Compute: `std::vector<double> ind_disparity_index(const std::vector<double>& closes, int period = 20)`
- Widget: `IndDisparityIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndDisparityIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_disparity_index(xframe::series_closes(series));
IndDisparityIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## SmoothedROC

- Formula: Smoothed Rate of Change (Schutzman): EMA(smooth) of ROC(period), where ROC(period) = 100 * (close - close[period]) / close[period].
- Compute: `std::vector<double> ind_smoothed_roc(const std::vector<double>& closes, int period = 12, int smooth = 25)`
- Widget: `IndSmoothedROC(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 12, int smooth = 25)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndSmoothedROC.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_smoothed_roc(xframe::series_closes(series));
IndSmoothedROC w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## WilliamsAccumulationDistribution

- Formula: Williams Accumulation/Distribution (Larry Williams): cumulative; when close rises, add close - min(low, prev close); when close falls, subtract max(high, prev close) - close; unchanged on an unchanged close.
- Compute: `std::vector<double> ind_williams_ad(const xframe::OHLCVSeries& s, int dummy = 0)`
- Widget: `IndWilliamsAccumulationDistribution(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int dummy = 0)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndWilliamsAccumulationDistribution.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_williams_ad(series);
IndWilliamsAccumulationDistribution w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## ChoppinessIndex

- Formula: Choppiness Index (Ehlers): 100 * log10(sum of true range over period / (highest high - lowest low over period)) / log10(period); above about 61.8 the market is choppy, below about 38.2 it is trending.
- Compute: `std::vector<double> ind_choppiness_index(const xframe::OHLCVSeries& s, int period = 14)`
- Widget: `IndChoppinessIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndChoppinessIndex.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_choppiness_index(series);
IndChoppinessIndex w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## GannHiLoActivator

- Formula: Gann Hi-Lo Activator (Gann): in an uptrend the line is the lowest low of the previous 3 bars, in a downtrend the highest high of the previous 3 bars; the trend flips when the close crosses the line.
- Compute: `std::vector<double> ind_gann_hilo_activator(const xframe::OHLCVSeries& s, int period = 3)`
- Widget: `IndGannHiLoActivator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 3)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: overlay.
- Example:

```cpp
#include "IndGannHiLoActivator.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_gann_hilo_activator(series);
IndGannHiLoActivator w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```

## JurikMovingAverage

- Formula: Jurik Moving Average: an adaptive smoothed price line. Unclear from the header: the Jurik Moving Average is proprietary; this uses triple EMA as the published approximation.
- Compute: `std::vector<double> ind_jurik_moving_average(const std::vector<double>& closes, int period = 20)`
- Widget: `IndJurikMovingAverage(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 20)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: overlay.
- Example:

```cpp
#include "IndJurikMovingAverage.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_jurik_moving_average(xframe::series_closes(series));
IndJurikMovingAverage w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.

## EhlersSmoothedRSI

- Formula: Ehlers Smoothed RSI: Wilder RSI(period) of closes, then SuperSmoothed over period. Unclear from the header: Ehlers smoothed RSI applies his SuperSmoother to the RSI series; this widget SuperSmooths the RSI output.
- Compute: `std::vector<double> ind_ehlers_smoothed_rsi(const std::vector<double>& closes, int period = 14)`
- Widget: `IndEhlersSmoothedRSI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, int period = 14)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.
- Draw style: pane.
- Example:

```cpp
#include "IndEhlersSmoothedRSI.h"
auto series = xframe::make_seeded_series(250);
auto out = xframe::ind_ehlers_smoothed_rsi(xframe::series_closes(series));
IndEhlersSmoothedRSI w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
w.draw();
```
- Unclear from the header: see the formula note above; the implementation choice is documented there.
