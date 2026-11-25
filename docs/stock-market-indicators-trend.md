# Stock Market indicators — trend (85)
Trend indicators for the XFrame Stock Market section. Every compute function returns a vector the same length as its input; leading warm-up bars are NaN until enough bars exist. Demos and tests run on deterministic seeded synthetic data (`xframe::make_seeded_series`), never live market data. Attribution: XFrame.
## SMA (`IndSMA`)

**Formula:** arithmetic mean of the last period closes.

**API:** `std::vector<double> ind_sma(const std::vector<double>& closes, int period)`; widget `IndSMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_sma(xframe::series_closes(series), 14);
```

## EMA (`IndEMA`)

**Formula:** exponentially weighted mean, k=2/(period+1), seeded with the first SMA.

**API:** `std::vector<double> ind_ema(const std::vector<double>& closes, int period)`; widget `IndEMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndEMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ema(xframe::series_closes(series), 14);
```

## WMA (`IndWMA`)

**Formula:** linearly weighted mean, weights 1..period with the newest bar heaviest.

**API:** `std::vector<double> ind_wma(const std::vector<double>& closes, int period)`; widget `IndWMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndWMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_wma(xframe::series_closes(series), 14);
```

## DEMA (`IndDEMA`)

**Formula:** double exponential moving average, 2*EMA - EMA(EMA).

**API:** `std::vector<double> ind_dema(const std::vector<double>& closes, int period)`; widget `IndDEMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndDEMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_dema(xframe::series_closes(series), 14);
```

## TEMA (`IndTEMA`)

**Formula:** triple exponential moving average, 3*EMA - 3*EMA(EMA) + EMA(EMA(EMA)).

**API:** `std::vector<double> ind_tema(const std::vector<double>& closes, int period)`; widget `IndTEMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndTEMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_tema(xframe::series_closes(series), 14);
```

## TRIMA (`IndTRIMA`)

**Formula:** triangular moving average, SMA of the SMA over the same period.

**API:** `std::vector<double> ind_trima(const std::vector<double>& closes, int period)`; widget `IndTRIMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndTRIMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_trima(xframe::series_closes(series), 14);
```

## KAMA (`IndKAMA`)

**Formula:** Kaufman adaptive moving average, smoothing constant from the efficiency ratio.

**API:** `std::vector<double> ind_kama(const std::vector<double>& closes, int period, int fast, int slow)`; widget `IndKAMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10, int fast = 2, int slow = 30)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndKAMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_kama(xframe::series_closes(series), 10, 2, 30);
```

## HMA (`IndHMA`)

**Formula:** Hull moving average, WMA of 2*WMA(period/2) - WMA(period) with window sqrt(period).

**API:** `std::vector<double> ind_hma(const std::vector<double>& closes, int period)`; widget `IndHMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 16)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndHMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_hma(xframe::series_closes(series), 16);
```

## VWMA (`IndVWMA`)

**Formula:** volume-weighted moving average, sum(close*volume)/sum(volume).

**API:** `std::vector<double> ind_vwma(const OHLCVSeries& series, int period)`; widget `IndVWMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndVWMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_vwma(series, 20);
```

## ALMA (`IndALMA`)

**Formula:** Arnaud Legoux moving average, Gaussian-weighted mean with offset 0.85 and sigma 6.

**API:** `std::vector<double> ind_alma(const std::vector<double>& closes, int period, double offset, double sigma)`; widget `IndALMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 9, double offset = 0.85, double sigma = 6.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndALMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_alma(xframe::series_closes(series), 9, 0.85, 6.0);
```

## ZLEMA (`IndZLEMA`)

**Formula:** zero-lag exponential moving average, EMA of price plus price minus price lag bars ago.

**API:** `std::vector<double> ind_zlema(const std::vector<double>& closes, int period)`; widget `IndZLEMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndZLEMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_zlema(xframe::series_closes(series), 14);
```

## LSMA (`IndLSMA`)

**Formula:** least-squares moving average, endpoint value of the linear-regression line over the window.

**API:** `std::vector<double> ind_lsma(const std::vector<double>& closes, int period)`; widget `IndLSMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndLSMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_lsma(xframe::series_closes(series), 14);
```

## SMMA (`IndSMMA`)

**Formula:** smoothed moving average, Wilder smoothing seeded with the first SMA.

**API:** `std::vector<double> ind_smma(const std::vector<double>& closes, int period)`; widget `IndSMMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSMMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_smma(xframe::series_closes(series), 14);
```

## RMA (`IndRMA`)

**Formula:** Wilder relative moving average, EMA with alpha 1/period seeded with the first SMA.

**API:** `std::vector<double> ind_rma(const std::vector<double>& closes, int period)`; widget `IndRMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndRMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_rma(xframe::series_closes(series), 14);
```

## MAMA (`IndMAMA`)

**Formula:** Ehlers MESA adaptive moving average, alpha from the detrended phase, clamped to slow/fast limits.

**API:** `std::vector<double> ind_mama(const std::vector<double>& closes, double fastlimit, double slowlimit)`; widget `IndMAMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, double fastlimit = 0.5, double slowlimit = 0.05)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndMAMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_mama(xframe::series_closes(series), 0.5, 0.05);
```

**Unclear from the header:** Simplified Ehlers MAMA: the detrender/phase stage uses a fixed-period approximation rather than the full adaptive-period measurement, so values track the published MAMA but are not bit-identical to TA-Lib.

## FAMA (`IndFAMA`)

**Formula:** Ehlers following adaptive moving average, slow EMA-style smoothing of MAMA.

**API:** `std::vector<double> ind_fama(const std::vector<double>& closes, double fastlimit, double slowlimit)`; widget `IndFAMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, double fastlimit = 0.5, double slowlimit = 0.05)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndFAMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_fama(xframe::series_closes(series), 0.5, 0.05);
```

**Unclear from the header:** FAMA smooths MAMA with half the MAMA alpha at each bar, as published.

## FRAMA (`IndFRAMA`)

**Formula:** fractal adaptive moving average, alpha from the fractal dimension of the window.

**API:** `std::vector<double> ind_frama(const std::vector<double>& closes, int period)`; widget `IndFRAMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 16)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndFRAMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_frama(xframe::series_closes(series), 16);
```

## VIDYA (`IndVIDYA`)

**Formula:** variable index dynamic average, Chande momentum oscillator scaled EMA alpha.

**API:** `std::vector<double> ind_vidya(const std::vector<double>& closes, int period)`; widget `IndVIDYA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndVIDYA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_vidya(xframe::series_closes(series), 14);
```

## T3 (`IndT3`)

**Formula:** Tillson T3, triple-smoothed EMA with volume-factor correction, vf 0.7.

**API:** `std::vector<double> ind_t3(const std::vector<double>& closes, int period, double vf)`; widget `IndT3(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 5, double vf = 0.7)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndT3.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_t3(xframe::series_closes(series), 5, 0.7);
```

## McGinley (`IndMcGinley`)

**Formula:** McGinley dynamic average, speed adapts with the price ratio to the fourth power.

**API:** `std::vector<double> ind_mcginley(const std::vector<double>& closes, int period)`; widget `IndMcGinley(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndMcGinley.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_mcginley(xframe::series_closes(series), 14);
```

## GaussianMA (`IndGaussianMA`)

**Formula:** Gaussian-weighted moving average, sigma period/6 centred on the window.

**API:** `std::vector<double> ind_gaussian_ma(const std::vector<double>& closes, int period)`; widget `IndGaussianMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndGaussianMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_gaussian_ma(xframe::series_closes(series), 14);
```

## SineWMA (`IndSineWMA`)

**Formula:** sine-weighted moving average, weights follow a half-sine over the window.

**API:** `std::vector<double> ind_sine_wma(const std::vector<double>& closes, int period)`; widget `IndSineWMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSineWMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_sine_wma(xframe::series_closes(series), 14);
```

## WildersMA (`IndWildersMA`)

**Formula:** Wilder moving average, Wilder smoothing seeded with the first SMA.

**API:** `std::vector<double> ind_wilders_ma(const std::vector<double>& closes, int period)`; widget `IndWildersMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndWildersMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_wilders_ma(xframe::series_closes(series), 14);
```

**Unclear from the header:** Wilder's smoothing recursion is identical to SMMA; the two widgets differ only in name and attribution.

## MedianMA (`IndMedianMA`)

**Formula:** median moving average, median of the last period closes.

**API:** `std::vector<double> ind_median_ma(const std::vector<double>& closes, int period)`; widget `IndMedianMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndMedianMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_median_ma(xframe::series_closes(series), 14);
```

## TSMA (`IndTSMA`)

**Formula:** triple-smoothed exponential moving average, EMA applied three times.

**API:** `std::vector<double> ind_tsma(const std::vector<double>& closes, int period)`; widget `IndTSMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndTSMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_tsma(xframe::series_closes(series), 14);
```

## TenkanSen (`IndTenkanSen`)

**Formula:** Ichimoku conversion line, midpoint of the highest high and lowest low over 9 bars.

**API:** `std::vector<double> ind_tenkan_sen(const OHLCVSeries& series, int period)`; widget `IndTenkanSen(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 9)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndTenkanSen.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_tenkan_sen(series, 9);
```

## KijunSen (`IndKijunSen`)

**Formula:** Ichimoku base line, midpoint of the highest high and lowest low over 26 bars.

**API:** `std::vector<double> ind_kijun_sen(const OHLCVSeries& series, int period)`; widget `IndKijunSen(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 26)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndKijunSen.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_kijun_sen(series, 26);
```

## SenkouSpanA (`IndSenkouSpanA`)

**Formula:** Ichimoku leading span A, average of Tenkan-sen and Kijun-sen.

**API:** `std::vector<double> ind_senkou_span_a(const OHLCVSeries& series, int tenkan, int kijun)`; widget `IndSenkouSpanA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int tenkan = 9, int kijun = 26)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSenkouSpanA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_senkou_span_a(series, 9, 26);
```

**Unclear from the header:** In live Ichimoku charts Span A is plotted 26 bars ahead; this widget returns the unshifted value at each bar and documents the shift.

## SenkouSpanB (`IndSenkouSpanB`)

**Formula:** Ichimoku leading span B, midpoint of the highest high and lowest low over 52 bars.

**API:** `std::vector<double> ind_senkou_span_b(const OHLCVSeries& series, int period)`; widget `IndSenkouSpanB(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 52)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSenkouSpanB.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_senkou_span_b(series, 52);
```

**Unclear from the header:** In live Ichimoku charts Span B is plotted 26 bars ahead; this widget returns the unshifted value at each bar and documents the shift.

## ChikouSpan (`IndChikouSpan`)

**Formula:** Ichimoku lagging span, close shifted back 26 bars.

**API:** `std::vector<double> ind_chikou_span(const std::vector<double>& closes, int period)`; widget `IndChikouSpan(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 26)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndChikouSpan.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_chikou_span(xframe::series_closes(series), 26);
```

**Unclear from the header:** Trailing bars are NaN by construction (the shift runs past the end of the series).

## PSAR (`IndPSAR`)

**Formula:** Wilder parabolic SAR, accelerating stop-and-reverse dots.

**API:** `std::vector<double> ind_psar(const OHLCVSeries& series, double step, double maxaf)`; widget `IndPSAR(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, double step = 0.02, double maxaf = 0.2)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndPSAR.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_psar(series, 0.02, 0.2);
```

## Supertrend (`IndSupertrend`)

**Formula:** Supertrend line, ATR trailing band that flips with the trend.

**API:** `std::vector<double> ind_supertrend(const OHLCVSeries& series, int period, double mult)`; widget `IndSupertrend(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10, double mult = 3.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSupertrend.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_supertrend(series, 10, 3.0);
```

## ADX (`IndADX`)

**Formula:** Wilder average directional index, smoothed DX over the period.

**API:** `std::vector<double> ind_adx(const OHLCVSeries& series, int period)`; widget `IndADX(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndADX.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_adx(series, 14);
```

## PlusDI (`IndPlusDI`)

**Formula:** Wilder positive directional indicator, smoothed +DM over smoothed true range.

**API:** `std::vector<double> ind_plus_di(const OHLCVSeries& series, int period)`; widget `IndPlusDI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndPlusDI.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_plus_di(series, 14);
```

## MinusDI (`IndMinusDI`)

**Formula:** Wilder negative directional indicator, smoothed -DM over smoothed true range.

**API:** `std::vector<double> ind_minus_di(const OHLCVSeries& series, int period)`; widget `IndMinusDI(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndMinusDI.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_minus_di(series, 14);
```

## DX (`IndDX`)

**Formula:** directional movement index, 100*|+DI - -DI| / (+DI + -DI).

**API:** `std::vector<double> ind_dx(const OHLCVSeries& series, int period)`; widget `IndDX(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndDX.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_dx(series, 14);
```

## ADXR (`IndADXR`)

**Formula:** average directional movement index rating, average of ADX now and ADX period bars ago.

**API:** `std::vector<double> ind_adxr(const OHLCVSeries& series, int period)`; widget `IndADXR(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndADXR.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_adxr(series, 14);
```

## AroonUp (`IndAroonUp`)

**Formula:** Aroon up, 100*(period - bars since the highest high)/period.

**API:** `std::vector<double> ind_aroon_up(const OHLCVSeries& series, int period)`; widget `IndAroonUp(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndAroonUp.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_aroon_up(series, 14);
```

## AroonDown (`IndAroonDown`)

**Formula:** Aroon down, 100*(period - bars since the lowest low)/period.

**API:** `std::vector<double> ind_aroon_down(const OHLCVSeries& series, int period)`; widget `IndAroonDown(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndAroonDown.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_aroon_down(series, 14);
```

## AroonOscillator (`IndAroonOscillator`)

**Formula:** Aroon oscillator, Aroon up minus Aroon down.

**API:** `std::vector<double> ind_aroon_osc(const OHLCVSeries& series, int period)`; widget `IndAroonOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndAroonOscillator.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_aroon_osc(series, 14);
```

## Vortex (`IndVortex`)

**Formula:** Vortex indicator, summed positive/negative vortex movement over summed true range.

**API:** `VortexResult ind_vortex(const OHLCVSeries& series, int period)`; widget `IndVortex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndVortex.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_vortex(series, 14).plus;
```

## AlligatorJaw (`IndAlligatorJaw`)

**Formula:** Alligator jaw, 13-bar SMMA of the median price shifted 8 bars ahead.

**API:** `std::vector<double> ind_alligator_jaw(const std::vector<double>& closes, int period, int shift)`; widget `IndAlligatorJaw(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 13, int shift = 8)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndAlligatorJaw.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_alligator_jaw(xframe::series_closes(series), 13, 8);
```

**Unclear from the header:** The published Alligator smooths the median price (high+low)/2; this widget smooths closes and documents the difference.

## AlligatorTeeth (`IndAlligatorTeeth`)

**Formula:** Alligator teeth, 8-bar SMMA shifted 5 bars ahead.

**API:** `std::vector<double> ind_alligator_teeth(const std::vector<double>& closes, int period, int shift)`; widget `IndAlligatorTeeth(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 8, int shift = 5)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndAlligatorTeeth.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_alligator_teeth(xframe::series_closes(series), 8, 5);
```

**Unclear from the header:** The published Alligator smooths the median price (high+low)/2; this widget smooths closes and documents the difference.

## AlligatorLips (`IndAlligatorLips`)

**Formula:** Alligator lips, 5-bar SMMA shifted 3 bars ahead.

**API:** `std::vector<double> ind_alligator_lips(const std::vector<double>& closes, int period, int shift)`; widget `IndAlligatorLips(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 5, int shift = 3)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndAlligatorLips.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_alligator_lips(xframe::series_closes(series), 5, 3);
```

**Unclear from the header:** The published Alligator smooths the median price (high+low)/2; this widget smooths closes and documents the difference.

## GatorOscillator (`IndGatorOscillator`)

**Formula:** Gator oscillator, absolute jaw-teeth gap above zero and teeth-lips gap below zero.

**API:** `GatorResult ind_gator(const std::vector<double>& closes)`; widget `IndGatorOscillator(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndGatorOscillator.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_gator(xframe::series_closes(series)).upper;
```

## TRIX (`IndTRIX`)

**Formula:** TRIX, 1-bar rate of change of the triple-smoothed EMA, in percent.

**API:** `std::vector<double> ind_trix(const std::vector<double>& closes, int period)`; widget `IndTRIX(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndTRIX.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_trix(xframe::series_closes(series), 14);
```

## DPO (`IndDPO`)

**Formula:** detrended price oscillator, close minus the SMA shifted back period/2+1 bars.

**API:** `std::vector<double> ind_dpo(const std::vector<double>& closes, int period)`; widget `IndDPO(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndDPO.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_dpo(xframe::series_closes(series), 20);
```

## QStick (`IndQStick`)

**Formula:** QStick, SMA of close minus open over the period.

**API:** `std::vector<double> ind_qstick(const OHLCVSeries& series, int period)`; widget `IndQStick(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndQStick.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_qstick(series, 14);
```

## ElderRayBull (`IndElderRayBull`)

**Formula:** Elder Ray bull power, high minus the EMA of closes.

**API:** `std::vector<double> ind_elder_ray_bull(const OHLCVSeries& series, int period)`; widget `IndElderRayBull(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 13)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndElderRayBull.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_elder_ray_bull(series, 13);
```

## ElderRayBear (`IndElderRayBear`)

**Formula:** Elder Ray bear power, low minus the EMA of closes.

**API:** `std::vector<double> ind_elder_ray_bear(const OHLCVSeries& series, int period)`; widget `IndElderRayBear(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 13)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndElderRayBear.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_elder_ray_bear(series, 13);
```

## LinReg (`IndLinReg`)

**Formula:** linear regression line, endpoint value of the least-squares fit over the window.

**API:** `std::vector<double> ind_linreg(const std::vector<double>& closes, int period)`; widget `IndLinReg(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndLinReg.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_linreg(xframe::series_closes(series), 14);
```

## LinRegSlope (`IndLinRegSlope`)

**Formula:** linear regression slope, price change per bar of the least-squares fit.

**API:** `std::vector<double> ind_linreg_slope(const std::vector<double>& closes, int period)`; widget `IndLinRegSlope(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndLinRegSlope.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_linreg_slope(xframe::series_closes(series), 14);
```

## LinRegAngle (`IndLinRegAngle`)

**Formula:** linear regression angle, arctangent of the slope in degrees.

**API:** `std::vector<double> ind_linreg_angle(const std::vector<double>& closes, int period)`; widget `IndLinRegAngle(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndLinRegAngle.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_linreg_angle(xframe::series_closes(series), 14);
```

## LinRegIntercept (`IndLinRegIntercept`)

**Formula:** linear regression intercept, fitted value at the start of the window.

**API:** `std::vector<double> ind_linreg_intercept(const std::vector<double>& closes, int period)`; widget `IndLinRegIntercept(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndLinRegIntercept.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_linreg_intercept(xframe::series_closes(series), 14);
```

## TSF (`IndTSF`)

**Formula:** time series forecast, linear-regression line extrapolated one bar ahead.

**API:** `std::vector<double> ind_tsf(const std::vector<double>& closes, int period)`; widget `IndTSF(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndTSF.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_tsf(xframe::series_closes(series), 14);
```

## DonchianUpper (`IndDonchianUpper`)

**Formula:** Donchian channel upper band, highest high over the period.

**API:** `std::vector<double> ind_donchian_upper(const OHLCVSeries& series, int period)`; widget `IndDonchianUpper(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndDonchianUpper.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_donchian_upper(series, 20);
```

## DonchianMiddle (`IndDonchianMiddle`)

**Formula:** Donchian channel middle line, average of the upper and lower bands.

**API:** `std::vector<double> ind_donchian_middle(const OHLCVSeries& series, int period)`; widget `IndDonchianMiddle(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndDonchianMiddle.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_donchian_middle(series, 20);
```

## DonchianLower (`IndDonchianLower`)

**Formula:** Donchian channel lower band, lowest low over the period.

**API:** `std::vector<double> ind_donchian_lower(const OHLCVSeries& series, int period)`; widget `IndDonchianLower(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndDonchianLower.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_donchian_lower(series, 20);
```

## ChandelierExitLong (`IndChandelierExitLong`)

**Formula:** Chandelier exit for longs, highest high over the period minus mult times ATR.

**API:** `std::vector<double> ind_chandelier_long(const OHLCVSeries& series, int period, double mult)`; widget `IndChandelierExitLong(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 22, double mult = 3.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndChandelierExitLong.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_chandelier_long(series, 22, 3.0);
```

## ChandelierExitShort (`IndChandelierExitShort`)

**Formula:** Chandelier exit for shorts, lowest low over the period plus mult times ATR.

**API:** `std::vector<double> ind_chandelier_short(const OHLCVSeries& series, int period, double mult)`; widget `IndChandelierExitShort(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 22, double mult = 3.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndChandelierExitShort.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_chandelier_short(series, 22, 3.0);
```

## ChandeKrollStop (`IndChandeKrollStop`)

**Formula:** Chande-Kroll stop for longs, highest high over the period minus mult times ATR.

**API:** `std::vector<double> ind_chande_kroll_stop(const OHLCVSeries& series, int period, double mult)`; widget `IndChandeKrollStop(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10, double mult = 1.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndChandeKrollStop.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_chande_kroll_stop(series, 10, 1.0);
```

**Unclear from the header:** The published Chande-Kroll stop carries a second, wider stop; this widget implements the primary (first) stop only.

## HalfTrend (`IndHalfTrend`)

**Formula:** HalfTrend line, ATR channel-deviation bands that flip with the trend.

**API:** `std::vector<double> ind_half_trend(const OHLCVSeries& series, int amplitude, double channelDeviation)`; widget `IndHalfTrend(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int amplitude = 2, double channelDeviation = 2.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndHalfTrend.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_half_trend(series, 2, 2.0);
```

**Unclear from the header:** Simplified port of the published HalfTrend logic (amplitude extrema plus ATR channel-deviation bands); the exact ATR period and band update rules vary by implementation.

## SSLChannel (`IndSSLChannel`)

**Formula:** SSL channel line, SMA of lows when close is above the SMA of highs, else SMA of highs.

**API:** `std::vector<double> ind_ssl_channel(const OHLCVSeries& series, int period)`; widget `IndSSLChannel(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSSLChannel.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ssl_channel(series, 10);
```

## UTBot (`IndUTBot`)

**Formula:** UT Bot trailing stop, ATR trailing stop that flips sides with the trend.

**API:** `std::vector<double> ind_ut_bot(const OHLCVSeries& series, int period, double mult)`; widget `IndUTBot(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10, double mult = 1.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndUTBot.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ut_bot(series, 10, 1.0);
```

## PivotPoint (`IndPivotPoint`)

**Formula:** classic floor-trader pivot point, (prior high + prior low + prior close)/3.

**API:** `std::vector<double> ind_pivot_point(const OHLCVSeries& series)`; widget `IndPivotPoint(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndPivotPoint.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_pivot_point(series);
```

## MAEnvelopeUpper (`IndMAEnvelopeUpper`)

**Formula:** moving average envelope upper band, SMA shifted up by pct percent.

**API:** `std::vector<double> ind_ma_envelope_upper(const std::vector<double>& closes, int period, double pct)`; widget `IndMAEnvelopeUpper(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20, double pct = 2.5)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndMAEnvelopeUpper.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ma_envelope_upper(xframe::series_closes(series), 20, 2.5);
```

## MAEnvelopeLower (`IndMAEnvelopeLower`)

**Formula:** moving average envelope lower band, SMA shifted down by pct percent.

**API:** `std::vector<double> ind_ma_envelope_lower(const std::vector<double>& closes, int period, double pct)`; widget `IndMAEnvelopeLower(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20, double pct = 2.5)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndMAEnvelopeLower.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ma_envelope_lower(xframe::series_closes(series), 20, 2.5);
```

## TrendIntensityIndex (`IndTrendIntensityIndex`)

**Formula:** trend intensity index, share of SMA deviation that lies above the SMA, 0-100.

**API:** `std::vector<double> ind_trend_intensity(const std::vector<double>& closes, int period)`; widget `IndTrendIntensityIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 30)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndTrendIntensityIndex.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_trend_intensity(xframe::series_closes(series), 30);
```

## VHF (`IndVHF`)

**Formula:** vertical horizontal filter, price range over the period divided by the sum of absolute daily changes.

**API:** `std::vector<double> ind_vhf(const OHLCVSeries& series, int period)`; widget `IndVHF(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 28)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndVHF.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_vhf(series, 28);
```

## RWIHigh (`IndRWIHigh`)

**Formula:** random walk index high, (high - low period bars ago) / (ATR * sqrt(period)).

**API:** `std::vector<double> ind_rwi_high(const OHLCVSeries& series, int period)`; widget `IndRWIHigh(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndRWIHigh.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_rwi_high(series, 14);
```

## RWILow (`IndRWILow`)

**Formula:** random walk index low, (high period bars ago - low) / (ATR * sqrt(period)).

**API:** `std::vector<double> ind_rwi_low(const OHLCVSeries& series, int period)`; widget `IndRWILow(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndRWILow.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_rwi_low(series, 14);
```

## EhlersTrendline (`IndEhlersTrendline`)

**Formula:** Ehlers instantaneous trendline, alpha-weighted two-pole low-pass filter.

**API:** `std::vector<double> ind_ehlers_trendline(const std::vector<double>& closes, double alpha)`; widget `IndEhlersTrendline(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, double alpha = 0.07)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndEhlersTrendline.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ehlers_trendline(xframe::series_closes(series), 0.07);
```

## EhlersDecycler (`IndEhlersDecycler`)

**Formula:** Ehlers decycler, high-pass cycle removal plus first-order smoothing.

**API:** `std::vector<double> ind_ehlers_decycler(const std::vector<double>& closes, int period)`; widget `IndEhlersDecycler(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndEhlersDecycler.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ehlers_decycler(xframe::series_closes(series), 10);
```

**Unclear from the header:** Simplified Ehlers Decycler: the high-pass stage keeps only the current two-pole term and the published second-order recursion is reduced; values track the published decycler but are not bit-identical.

## GuppyMMA (`IndGuppyMMA`)

**Formula:** Guppy multiple moving average, 12 EMAs in a fast group (3-15) and a slow group (30-60).

**API:** `GuppyResult ind_guppy_mma(const std::vector<double>& closes)`; widget `IndGuppyMMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndGuppyMMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_guppy_mma(xframe::series_closes(series)).lines[0];
```

## ZigZag (`IndZigZag`)

**Formula:** ZigZag, pivot points where price reverses by at least deviation percent.

**API:** `std::vector<double> ind_zigzag(const OHLCVSeries& series, double deviation)`; widget `IndZigZag(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, double deviation = 5.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndZigZag.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_zigzag(series, 5.0);
```

**Unclear from the header:** Only pivot bars carry values; every other bar is NaN by design. The last leg repaints as new bars arrive.

## FractalUp (`IndFractalUp`)

**Formula:** Bill Williams fractal up, high greater than the highs of the two bars either side.

**API:** `std::vector<double> ind_fractal_up(const OHLCVSeries& series)`; widget `IndFractalUp(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndFractalUp.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_fractal_up(series);
```

**Unclear from the header:** Only fractal bars carry values; uses two bars of lookahead, so the signal confirms two bars late.

## FractalDown (`IndFractalDown`)

**Formula:** Bill Williams fractal down, low lower than the lows of the two bars either side.

**API:** `std::vector<double> ind_fractal_down(const OHLCVSeries& series)`; widget `IndFractalDown(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndFractalDown.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_fractal_down(series);
```

**Unclear from the header:** Only fractal bars carry values; uses two bars of lookahead, so the signal confirms two bars late.

## SwingIndex (`IndSwingIndex`)

**Formula:** Wilder swing index, real price move scaled by the limit move.

**API:** `std::vector<double> ind_swing_index(const OHLCVSeries& series, double limitMove)`; widget `IndSwingIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, double limitMove = 1.0)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndSwingIndex.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_swing_index(series, 1.0);
```

## AccumulativeSwingIndex (`IndAccumulativeSwingIndex`)

**Formula:** Wilder accumulative swing index, running sum of the swing index.

**API:** `std::vector<double> ind_accumulative_swing_index(const OHLCVSeries& series, double limitMove)`; widget `IndAccumulativeSwingIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, double limitMove = 1.0)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndAccumulativeSwingIndex.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_accumulative_swing_index(series, 1.0);
```

## FractalDimensionIndex (`IndFractalDimensionIndex`)

**Formula:** fractal dimension index, 1 + (ln(sum of absolute changes) - ln 2) / ln(period).

**API:** `std::vector<double> ind_fractal_dimension(const std::vector<double>& closes, int period)`; widget `IndFractalDimensionIndex(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 30)`; draw style: pane (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndFractalDimensionIndex.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_fractal_dimension(xframe::series_closes(series), 30);
```

**Unclear from the header:** The FDI formula has several published variants; this widget uses the common ln form and documents it.

## RegressionChannel (`IndRegressionChannel`)

**Formula:** linear regression channel, regression line plus/minus mult times the residual standard deviation.

**API:** `RegChannelResult ind_regression_channel(const std::vector<double>& closes, int period, double mult)`; widget `IndRegressionChannel(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 20, double mult = 2.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndRegressionChannel.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_regression_channel(xframe::series_closes(series), 20, 2.0).upper;
```

## MovingAverageRibbon (`IndMovingAverageRibbon`)

**Formula:** moving average ribbon, six SMAs (5, 10, 20, 30, 50, 100) plotted together.

**API:** `RibbonResult ind_ma_ribbon(const std::vector<double>& closes)`; widget `IndMovingAverageRibbon(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndMovingAverageRibbon.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ma_ribbon(xframe::series_closes(series)).lines[0];
```

## EhlersSuperSmoother (`IndEhlersSuperSmoother`)

**Formula:** Ehlers SuperSmoother, two-pole low-pass filter with minimal lag.

**API:** `std::vector<double> ind_ehlers_super_smoother(const std::vector<double>& closes, int period)`; widget `IndEhlersSuperSmoother(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndEhlersSuperSmoother.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_ehlers_super_smoother(xframe::series_closes(series), 10);
```

## VWEMA (`IndVWEMA`)

**Formula:** volume-weighted exponential moving average, EMA of close times volume over EMA of volume.

**API:** `std::vector<double> ind_vwema(const OHLCVSeries& series, int period)`; widget `IndVWEMA(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 14)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndVWEMA.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_vwema(series, 14);
```

## ElderSafeZone (`IndElderSafeZone`)

**Formula:** Elder SafeZone long stop, lowest low over the period minus mult times ATR.

**API:** `std::vector<double> ind_elder_safe_zone(const OHLCVSeries& series, int period, double mult)`; widget `IndElderSafeZone(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const OHLCVSeries& series, int period = 10, double mult = 2.0)`; draw style: overlay (overlay draws over a light candle background, pane draws its own subplot with an axis). Key methods: `draw()`, `on_click(x, y)`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line(i)`, `line_count()`, `is_pane()`.

**Example:**

```cpp
#include "IndElderSafeZone.h"
auto series = xframe::make_seeded_series(250, 42);
auto values = xframe::ind_elder_safe_zone(series, 10, 2.0);
```

**Unclear from the header:** Simplified Elder SafeZone: the published SafeZone subtracts the average downside penetration rather than a fixed ATR multiple.

