#!/usr/bin/env python3
"""Generate the 85 XFrame momentum indicators (widgets/components/Ind<Name>/).
Spec table + templates. Regenerable: run this script, it rewrites all 85
widget dirs, the indicator list, the doc and the smoke harness.
Attribution: XFrame.
"""
import os, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = os.path.dirname(ROOT)
COMP = os.path.join(ROOT, "components")
TOOLS = os.path.join(ROOT, "tools")

HELPER = r"""
namespace ind_{FN}_detail {{
inline double nanv() {{ return std::numeric_limits<double>::quiet_NaN(); }}
inline bool fin(double v) {{ return std::isfinite(v); }}
inline std::vector<double> sma(const std::vector<double>& v, int p) {{
    std::vector<double> o(v.size(), nanv());
    if (p <= 0) return o;
    if (p == 1) return v;
    for (std::size_t i = 0; i < v.size(); ++i) {{
        if (i < (std::size_t)(p - 1)) continue;
        double s = 0.0;
        bool ok = true;
        for (int k = 0; k < p; ++k) {{
            if (!fin(v[i - (std::size_t)k])) ok = false;
            else s += v[i - (std::size_t)k];
        }}
        if (ok) o[i] = s / p;
    }}
    return o;
}}
inline std::vector<double> ema(const std::vector<double>& v, int p) {{
    std::vector<double> o(v.size(), nanv());
    if (p <= 0) return o;
    if (p == 1) return v;
    const double a = 2.0 / (p + 1.0);
    double prev = 0.0; bool have = false; int run = 0; double s = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) {{
        if (!fin(v[i])) {{ run = 0; s = 0.0; have = false; continue; }}
        s += v[i]; ++run;
        if (!have && run >= p) {{ prev = s / p; o[i] = prev; have = true; }}
        else if (have) {{ prev = a * v[i] + (1.0 - a) * prev; o[i] = prev; }}
    }}
    return o;
}}
inline std::vector<double> wilder(const std::vector<double>& v, int p) {{
    std::vector<double> o(v.size(), nanv());
    if (p <= 0) return o;
    const double a = 1.0 / p;
    double prev = 0.0; bool have = false; int run = 0; double s = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) {{
        if (!fin(v[i])) {{ run = 0; s = 0.0; have = false; continue; }}
        s += v[i]; ++run;
        if (!have && run >= p) {{ prev = s / p; o[i] = prev; have = true; }}
        else if (have) {{ prev = a * v[i] + (1.0 - a) * prev; o[i] = prev; }}
    }}
    return o;
}}
inline std::vector<double> stddev(const std::vector<double>& v, int p) {{
    std::vector<double> o(v.size(), nanv());
    if (p <= 1) return o;
    std::vector<double> m = sma(v, p);
    for (std::size_t i = 0; i < v.size(); ++i) {{
        if (!fin(m[i])) continue;
        bool ok = true;
        for (int k = 0; k < p; ++k) if (!fin(v[i - (std::size_t)k])) ok = false;
        if (!ok) continue;
        double s = 0.0;
        for (int k = 0; k < p; ++k) {{ double d = v[i - (std::size_t)k] - m[i]; s += d * d; }}
        o[i] = std::sqrt(s / p);
    }}
    return o;
}}
inline double highest(const std::vector<double>& v, std::size_t i, int p) {{
    double m = -1e300; for (int k = 0; k < p && i >= (std::size_t)k; ++k) m = std::max(m, v[i - (std::size_t)k]); return m;
}}
inline double lowest(const std::vector<double>& v, std::size_t i, int p) {{
    double m = 1e300; for (int k = 0; k < p && i >= (std::size_t)k; ++k) m = std::min(m, v[i - (std::size_t)k]); return m;
}}
inline std::vector<double> rsi_series(const std::vector<double>& c, int p) {{
    const std::size_t n = c.size();
    std::vector<double> o(n, nanv());
    if (n < (std::size_t)(p + 1) || p <= 0) return o;
    std::vector<double> g(n, 0.0), l(n, 0.0);
    for (std::size_t i = 1; i < n; ++i) {{ double d = c[i] - c[i-1]; g[i] = d > 0 ? d : 0.0; l[i] = d < 0 ? -d : 0.0; }}
    std::vector<double> ag = wilder(g, p), al = wilder(l, p);
    for (std::size_t i = 0; i < n; ++i) {{
        if (!fin(ag[i]) || !fin(al[i])) continue;
        double den = ag[i] + al[i];
        o[i] = den == 0.0 ? 50.0 : 100.0 * ag[i] / den;
    }}
    return o;
}}
inline std::vector<double> stoch_k(const std::vector<double>& h, const std::vector<double>& l,
                                    const std::vector<double>& c, int p) {{
    std::vector<double> o(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) {{
        if (i < (std::size_t)(p - 1)) continue;
        double hh = highest(h, i, p), ll = lowest(l, i, p);
        if (hh == ll) {{ o[i] = 50.0; continue; }}
        o[i] = 100.0 * (c[i] - ll) / (hh - ll);
    }}
    return o;
}}
inline double linreg_slope_at(const std::vector<double>& v, std::size_t i, int p) {{
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (int k = 0; k < p; ++k) {{ double x = (double)k, y = v[i - (std::size_t)(p - 1) + (std::size_t)k]; sx += x; sy += y; sxx += x*x; sxy += x*y; }}
    double den = (double)p * sxx - sx * sx;
    return den == 0.0 ? 0.0 : ((double)p * sxy - sx * sy) / den;
}}
inline double linreg_value_at(const std::vector<double>& v, std::size_t i, int p) {{
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (int k = 0; k < p; ++k) {{ double x = (double)k, y = v[i - (std::size_t)(p - 1) + (std::size_t)k]; sx += x; sy += y; sxx += x*x; sxy += x*y; }}
    double den = (double)p * sxx - sx * sx;
    if (den == 0.0) return sy / p;
    double slope = ((double)p * sxy - sx * sy) / den;
    double inter = (sy - slope * sx) / p;
    return inter + slope * (p - 1);
}}
inline double linreg_r2_at(const std::vector<double>& v, std::size_t i, int p) {{
    double mean = 0; for (int k = 0; k < p; ++k) mean += v[i - (std::size_t)(p - 1) + (std::size_t)k]; mean /= p;
    double ss_tot = 0, ss_res = 0;
    double slope = linreg_slope_at(v, i, p);
    double inter = mean - slope * (p - 1) / 2.0;
    for (int k = 0; k < p; ++k) {{ double y = v[i - (std::size_t)(p - 1) + (std::size_t)k]; double fit = inter + slope * k; ss_tot += (y - mean)*(y - mean); ss_res += (y - fit)*(y - fit); }}
    return ss_tot == 0.0 ? 0.0 : 1.0 - ss_res / ss_tot;
}}
inline std::vector<double> super_smoother(const std::vector<double>& v, int p) {{
    std::vector<double> o(v.size(), nanv());
    if (p < 2) return v;
    const double pi = 3.141592653589793;
    const double a1 = std::exp(-1.414 * pi / p);
    const double b1 = 2.0 * a1 * std::cos(1.414 * pi / p);
    const double c2 = b1, c3 = -a1 * a1, c1 = 1.0 - c2 - c3;
    for (std::size_t i = 0; i < v.size(); ++i) {{
        if (!fin(v[i])) continue;
        double pm1 = (i >= 1 && fin(v[i-1])) ? v[i-1] : v[i];
        double o1 = (i >= 1 && fin(o[i-1])) ? o[i-1] : 0.0;
        double o2 = (i >= 2 && fin(o[i-2])) ? o[i-2] : 0.0;
        if (i < 2) {{ o[i] = v[i]; continue; }}
        o[i] = c1 * (v[i] + pm1) / 2.0 + c2 * o1 + c3 * o2;
    }}
    return o;
}}
}}  // namespace ind_{FN}_detail
"""

S = []
def add(name, fn, kind, style, ctor, formula, body, call):
    S.append(dict(name=name, fn=fn, kind=kind, style=style, ctor=ctor,
                  formula=formula, body=body, call=call))

add("RSI", "ind_rsi", "single", "pane", "int period = 14",
    "Wilder RSI: 100 * avg gain / (avg gain + avg loss), gains/losses Wilder-smoothed over period.",
    """using namespace ind_ind_rsi_detail;
    return rsi_series(closes, period);""",
    "ind_rsi(xframe::series_closes(s), 14)")
add("ConnorsRSI", "ind_connors_rsi", "single", "pane", "int period = 3",
    "Connors RSI: average of RSI(3) of closes, RSI(2) of the up/down streak, and the 100-bar percent rank of the 1-day return.",
    """using namespace ind_ind_connors_rsi_detail;
    const std::size_t n = s.size();
    std::vector<double> out(n, nanv());
    std::vector<double> closes = series_closes(s);
    std::vector<double> r1 = rsi_series(closes, 3);
    std::vector<double> streak(n, 0.0);
    for (std::size_t i = 1; i < n; ++i) {
        if (closes[i] > closes[i-1]) streak[i] = streak[i-1] >= 0 ? streak[i-1] + 1 : 1;
        else if (closes[i] < closes[i-1]) streak[i] = streak[i-1] <= 0 ? streak[i-1] - 1 : -1;
        else streak[i] = 0;
    }
    std::vector<double> r2 = rsi_series(streak, 2);
    std::vector<double> roc(n, nanv());
    for (std::size_t i = 1; i < n; ++i) roc[i] = 100.0 * (closes[i] - closes[i-1]) / closes[i-1];
    for (std::size_t i = 0; i < n; ++i) {
        if (!fin(r1[i]) || !fin(r2[i]) || i < 100) continue;
        int cnt = 0;
        for (int k = 0; k < 100; ++k) if (fin(roc[i - (std::size_t)k]) && roc[i - (std::size_t)k] <= roc[i]) ++cnt;
        out[i] = (r1[i] + r2[i] + cnt) / 3.0;
    }
    return out;""",
    "ind_connors_rsi(s)")
add("StochRSI", "ind_stoch_rsi", "single", "pane", "int period = 14",
    "Stochastic RSI: 100 * (RSI - lowest RSI over period) / (highest RSI - lowest RSI over period).",
    """using namespace ind_ind_stoch_rsi_detail;
    std::vector<double> rsi = rsi_series(closes, period);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) {
        if (i < (std::size_t)(period - 1) || !fin(rsi[i])) continue;
        double hi = highest(rsi, i, period), lo = lowest(rsi, i, period);
        out[i] = hi == lo ? 50.0 : 100.0 * (rsi[i] - lo) / (hi - lo);
    }
    return out;""",
    "ind_stoch_rsi(xframe::series_closes(s), 14)")
add("CutlerRSI", "ind_cutler_rsi", "single", "pane", "int period = 14",
    "Cutler RSI: same as RSI but gains/losses averaged with a simple moving average instead of Wilder smoothing.",
    """using namespace ind_ind_cutler_rsi_detail;
    const std::size_t n = closes.size();
    std::vector<double> out(n, nanv());
    if (n < (std::size_t)(period + 1) || period <= 0) return out;
    std::vector<double> g(n, 0.0), l(n, 0.0);
    for (std::size_t i = 1; i < n; ++i) { double d = closes[i] - closes[i-1]; g[i] = d > 0 ? d : 0.0; l[i] = d < 0 ? -d : 0.0; }
    std::vector<double> ag = sma(g, period), al = sma(l, period);
    for (std::size_t i = 0; i < n; ++i) {
        if (!fin(ag[i]) || !fin(al[i])) continue;
        double den = ag[i] + al[i];
        out[i] = den == 0.0 ? 50.0 : 100.0 * ag[i] / den;
    }
    return out;""",
    "ind_cutler_rsi(xframe::series_closes(s), 14)")
add("LaguerreRSI", "ind_laguerre_rsi", "single", "pane", "double gamma = 0.8",
    "Laguerre RSI (Ehlers): price passed through a 4-pole Laguerre filter; RSI-style ratio of up-moves (CU) to up+down moves (CD) of the filter outputs.",
    """const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    double g = gamma;
    std::vector<double> L0(n, 0.0), L1(n, 0.0), L2(n, 0.0), L3(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double p = closes[i];
        double pL0 = i ? L0[i-1] : p, pL1 = i ? L1[i-1] : 0.0, pL2 = i ? L2[i-1] : 0.0, pL3 = i ? L3[i-1] : 0.0;
        L0[i] = (1.0 - g) * p + g * pL0;
        L1[i] = -g * L0[i] + pL0 + g * pL1;
        L2[i] = -g * L1[i] + pL1 + g * pL2;
        L3[i] = -g * L2[i] + pL2 + g * pL3;
        if (i == 0) continue;
        double cu = 0.0, cd = 0.0;
        cu += L0[i] > L1[i] ? L0[i] - L1[i] : 0.0; cd += L0[i] < L1[i] ? L1[i] - L0[i] : 0.0;
        cu += L1[i] > L2[i] ? L1[i] - L2[i] : 0.0; cd += L1[i] < L2[i] ? L2[i] - L1[i] : 0.0;
        cu += L2[i] > L3[i] ? L2[i] - L3[i] : 0.0; cd += L2[i] < L3[i] ? L3[i] - L2[i] : 0.0;
        out[i] = (cu + cd) == 0.0 ? 50.0 : 100.0 * cu / (cu + cd);
    }
    return out;""",
    "ind_laguerre_rsi(xframe::series_closes(s), 0.8)")
add("AdaptiveRSI", "ind_adaptive_rsi", "single", "pane", "int period = 14",
    "Adaptive RSI: Wilder RSI whose smoothing period adapts per bar to the Kaufman efficiency ratio over 10 bars (efficient market -> short period, choppy -> long period, 5..30).",
    """using namespace ind_ind_adaptive_rsi_detail;
    const std::size_t n = closes.size();
    std::vector<double> out(n, nanv());
    if (n < 12) return out;
    double ag = 0.0, al = 0.0; bool have = false;
    for (std::size_t i = 1; i < n; ++i) {
        double d = closes[i] - closes[i-1];
        double g = d > 0 ? d : 0.0, l = d < 0 ? -d : 0.0;
        double eff = 0.0;
        if (i >= 10) {
            double dir = std::abs(closes[i] - closes[i-10]), vol = 0.0;
            for (std::size_t k = i - 9; k <= i; ++k) vol += std::abs(closes[k] - closes[k-1]);
            eff = vol == 0.0 ? 0.0 : dir / vol;
        }
        int p = (int)std::round(5.0 + (1.0 - eff) * 25.0);
        if (p < 5) p = 5;
        if (p > 30) p = 30;
        double a = 1.0 / p;
        if (!have) {
            if (i >= (std::size_t)period) {
                double sg = 0.0, sl = 0.0;
                for (std::size_t k = i - (std::size_t)period + 1; k <= i; ++k) { double dd = closes[k] - closes[k-1]; sg += dd > 0 ? dd : 0.0; sl += dd < 0 ? -dd : 0.0; }
                ag = sg / period; al = sl / period; have = true;
            } else continue;
        } else { ag = a * g + (1.0 - a) * ag; al = a * l + (1.0 - a) * al; }
        double den = ag + al;
        out[i] = den == 0.0 ? 50.0 : 100.0 * ag / den;
    }
    return out;""",
    "ind_adaptive_rsi(xframe::series_closes(s), 14)")
add("MACD", "ind_macd", "macd", "pane", "int fast = 12, int slow = 26, int signal = 9",
    "MACD (Appel): MACD line = EMA(fast) - EMA(slow); signal = EMA(signal) of the MACD line; histogram = MACD - signal.",
    """using namespace ind_ind_macd_detail;
    std::vector<double> ef = ema(closes, fast), es = ema(closes, slow);
    IndMACDResult r;
    r.macd.assign(closes.size(), nanv());
    r.signal.assign(closes.size(), nanv());
    r.histogram.assign(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ef[i]) && fin(es[i])) r.macd[i] = ef[i] - es[i];
    std::vector<double> sig = ema(r.macd, signal);
    r.signal = sig;
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(r.macd[i]) && fin(sig[i])) r.histogram[i] = r.macd[i] - sig[i];
    return r;""",
    "ind_macd(xframe::series_closes(s), 12, 26, 9)")
add("MACDHistogram", "ind_macd_histogram", "single", "pane", "int fast = 12, int slow = 26, int signal = 9",
    "MACD histogram: EMA(fast) - EMA(slow) minus its EMA(signal) signal line; the histogram alone.",
    """using namespace ind_ind_macd_histogram_detail;
    std::vector<double> ef = ema(closes, fast), es = ema(closes, slow);
    std::vector<double> macd(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ef[i]) && fin(es[i])) macd[i] = ef[i] - es[i];
    std::vector<double> sig = ema(macd, signal);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(macd[i]) && fin(sig[i])) out[i] = macd[i] - sig[i];
    return out;""",
    "ind_macd_histogram(xframe::series_closes(s), 12, 26, 9)")
add("ZeroLagMACD", "ind_zero_lag_macd", "single", "pane", "int fast = 12, int slow = 26, int signal = 9",
    "Zero-lag MACD: MACD line built from zero-lag EMAs (EMA of price + (price - price lagged by (period-1)/2)); this widget plots the zero-lag MACD line.",
    """using namespace ind_ind_zero_lag_macd_detail;
    auto zlema = [&](const std::vector<double>& v, int p) {
        int lag = (p - 1) / 2;
        std::vector<double> adj(v.size(), nanv());
        for (std::size_t i = 0; i < v.size(); ++i) {
            if (!fin(v[i])) continue;
            adj[i] = (i >= (std::size_t)lag && fin(v[i - (std::size_t)lag])) ? v[i] + (v[i] - v[i - (std::size_t)lag]) : v[i];
        }
        return ema(adj, p);
    };
    std::vector<double> ef = zlema(closes, fast), es = zlema(closes, slow);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ef[i]) && fin(es[i])) out[i] = ef[i] - es[i];
    return out;""",
    "ind_zero_lag_macd(xframe::series_closes(s), 12, 26, 9)")
add("MACDofRSI", "ind_macd_of_rsi", "single", "pane", "int rsi_period = 14, int fast = 12, int slow = 26",
    "MACD of RSI: the RSI series is computed first, then a MACD line (EMA(fast) - EMA(slow)) is taken of the RSI values.",
    """using namespace ind_ind_macd_of_rsi_detail;
    std::vector<double> rsi = rsi_series(closes, rsi_period);
    std::vector<double> ef = ema(rsi, fast), es = ema(rsi, slow);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ef[i]) && fin(es[i])) out[i] = ef[i] - es[i];
    return out;""",
    "ind_macd_of_rsi(xframe::series_closes(s), 14, 12, 26)")
add("StochasticFast", "ind_stochastic_fast", "stoch", "pane", "int period = 14, int smooth = 3",
    "Fast stochastic (Lane): %K = 100 * (close - lowest low over period) / (highest high - lowest low); %D = SMA(smooth) of %K.",
    """using namespace ind_ind_stochastic_fast_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    IndStochasticFastResult r;
    r.k = stoch_k(h, l, c, period);
    r.d = sma(r.k, smooth);
    return r;""",
    "ind_stochastic_fast(s, 14, 3)")
add("StochasticSlow", "ind_stochastic_slow", "stoch", "pane", "int period = 14, int smooth = 3",
    "Slow stochastic (Lane): slow %K = SMA(smooth) of fast %K; slow %D = SMA(smooth) of slow %K.",
    """using namespace ind_ind_stochastic_slow_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> fk = stoch_k(h, l, c, period);
    IndStochasticSlowResult r;
    r.k = sma(fk, smooth);
    r.d = sma(r.k, smooth);
    return r;""",
    "ind_stochastic_slow(s, 14, 3)")
add("StochasticFull", "ind_stochastic_full", "stoch", "pane", "int period = 14, int smooth_k = 3, int smooth_d = 3",
    "Full stochastic (Lane): %K = SMA(smooth_k) of raw %K over period; %D = SMA(smooth_d) of %K; all three periods are parameters.",
    """using namespace ind_ind_stochastic_full_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> fk = stoch_k(h, l, c, period);
    IndStochasticFullResult r;
    r.k = sma(fk, smooth_k);
    r.d = sma(r.k, smooth_d);
    return r;""",
    "ind_stochastic_full(s, 14, 3, 3)")
add("KDJ", "ind_kdj", "kdj", "pane", "int period = 9, int smooth = 3",
    "KDJ stochastic: RSV = 100 * (close - lowest low) / (highest high - lowest low); K = 2/3 K(prev) + 1/3 RSV (seeded 50); D = 2/3 D(prev) + 1/3 K; J = 3K - 2D.",
    """std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    const std::size_t n = c.size();
    IndKDJResult r;
    r.k.assign(n, std::numeric_limits<double>::quiet_NaN());
    r.d.assign(n, std::numeric_limits<double>::quiet_NaN());
    r.j.assign(n, std::numeric_limits<double>::quiet_NaN());
    double k = 50.0, d = 50.0;
    for (std::size_t i = 0; i < n; ++i) {
        if (i < (std::size_t)(period - 1)) continue;
        double hi = h[i], lo = l[i];
        for (int kk = 1; kk < period; ++kk) { hi = std::max(hi, h[i - (std::size_t)kk]); lo = std::min(lo, l[i - (std::size_t)kk]); }
        double rsv = hi == lo ? 50.0 : 100.0 * (c[i] - lo) / (hi - lo);
        k = (2.0 / 3.0) * k + (1.0 / 3.0) * rsv;
        d = (2.0 / 3.0) * d + (1.0 / 3.0) * k;
        r.k[i] = k; r.d[i] = d; r.j[i] = 3.0 * k - 2.0 * d;
    }
    return r;""",
    "ind_kdj(s, 9, 3)")
add("WilliamsR", "ind_williams_r", "single", "pane", "int period = 14",
    "Williams %R (Williams): -100 * (highest high over period - close) / (highest high - lowest low over period); 0 to -100.",
    """using namespace ind_ind_williams_r_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) {
        if (i < (std::size_t)(period - 1)) continue;
        double hi = highest(h, i, period), lo = lowest(l, i, period);
        out[i] = hi == lo ? 0.0 : -100.0 * (hi - c[i]) / (hi - lo);
    }
    return out;""",
    "ind_williams_r(s, 14)")
add("SMI", "ind_smi", "single", "pane", "int period = 13",
    "Stochastic Momentum Index (Blau): 100 * double-EMA-smoothed (close - (high+low)/2) / double-EMA-smoothed (high - low).",
    """using namespace ind_ind_smi_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> diff(c.size(), nanv()), rng(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) { diff[i] = c[i] - (h[i] + l[i]) / 2.0; rng[i] = h[i] - l[i]; }
    std::vector<double> num = ema(ema(diff, period), period);
    std::vector<double> den = ema(ema(rng, period), period);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(num[i]) && fin(den[i]) && den[i] != 0.0) out[i] = 100.0 * num[i] / den[i];
    return out;""",
    "ind_smi(s, 13)")
add("DSS", "ind_dss", "single", "pane", "int period = 10, int smooth = 3",
    "Double Smoothed Stochastic (Bressert): raw %K over period, EMA-smoothed twice with period smooth; the twice-smoothed %K is the output.",
    """using namespace ind_ind_dss_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> k = stoch_k(h, l, c, period);
    return ema(ema(k, smooth), smooth);""",
    "ind_dss(s, 10, 3)")
add("CCI", "ind_cci", "single", "pane", "int period = 20",
    "Commodity Channel Index (Lambert): (typical price - SMA(period) of typical price) / (0.015 * mean deviation of typical price).",
    """using namespace ind_ind_cci_detail;
    std::vector<double> tp = series_typical(s);
    std::vector<double> m = sma(tp, period);
    std::vector<double> out(tp.size(), nanv());
    for (std::size_t i = 0; i < tp.size(); ++i) {
        if (!fin(m[i])) continue;
        bool ok = true;
        for (int k = 0; k < period; ++k) if (!fin(tp[i - (std::size_t)k])) ok = false;
        if (!ok) continue;
        double md = 0.0;
        for (int k = 0; k < period; ++k) md += std::abs(tp[i - (std::size_t)k] - m[i]);
        md /= period;
        out[i] = md == 0.0 ? 0.0 : (tp[i] - m[i]) / (0.015 * md);
    }
    return out;""",
    "ind_cci(s, 20)")
add("ROC", "ind_roc", "single", "pane", "int period = 12",
    "Rate of Change: 100 * (close - close[period]) / close[period].",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)period; i < closes.size(); ++i)
        if (closes[i - (std::size_t)period] != 0.0) out[i] = 100.0 * (closes[i] - closes[i - (std::size_t)period]) / closes[i - (std::size_t)period];
    return out;""",
    "ind_roc(xframe::series_closes(s), 12)")
add("ROCR", "ind_rocr", "single", "pane", "int period = 12",
    "Rate of Change Ratio (TA-Lib ROCR): close / close[period].",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)period; i < closes.size(); ++i)
        if (closes[i - (std::size_t)period] != 0.0) out[i] = closes[i] / closes[i - (std::size_t)period];
    return out;""",
    "ind_rocr(xframe::series_closes(s), 12)")
add("Momentum", "ind_momentum", "single", "pane", "int period = 10",
    "Momentum: close - close[period].",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)period; i < closes.size(); ++i) out[i] = closes[i] - closes[i - (std::size_t)period];
    return out;""",
    "ind_momentum(xframe::series_closes(s), 10)")
add("PPO", "ind_ppo", "single", "pane", "int fast = 12, int slow = 26",
    "Percentage Price Oscillator: 100 * (EMA(fast) - EMA(slow)) / EMA(slow).",
    """using namespace ind_ind_ppo_detail;
    std::vector<double> ef = ema(closes, fast), es = ema(closes, slow);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ef[i]) && fin(es[i]) && es[i] != 0.0) out[i] = 100.0 * (ef[i] - es[i]) / es[i];
    return out;""",
    "ind_ppo(xframe::series_closes(s), 12, 26)")
add("APO", "ind_apo", "single", "pane", "int fast = 12, int slow = 26",
    "Absolute Price Oscillator: EMA(fast) - EMA(slow) in price units.",
    """using namespace ind_ind_apo_detail;
    std::vector<double> ef = ema(closes, fast), es = ema(closes, slow);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ef[i]) && fin(es[i])) out[i] = ef[i] - es[i];
    return out;""",
    "ind_apo(xframe::series_closes(s), 12, 26)")
add("AwesomeOscillator", "ind_awesome_oscillator", "single", "pane", "int fast = 5, int slow = 34",
    "Awesome Oscillator (Bill Williams): SMA(fast) of median price - SMA(slow) of median price; median price = (high + low) / 2.",
    """using namespace ind_ind_awesome_oscillator_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s);
    std::vector<double> med(h.size(), nanv());
    for (std::size_t i = 0; i < h.size(); ++i) med[i] = (h[i] + l[i]) / 2.0;
    std::vector<double> f = sma(med, fast), sl = sma(med, slow);
    std::vector<double> out(med.size(), nanv());
    for (std::size_t i = 0; i < med.size(); ++i) if (fin(f[i]) && fin(sl[i])) out[i] = f[i] - sl[i];
    return out;""",
    "ind_awesome_oscillator(s, 5, 34)")
add("AcceleratorOscillator", "ind_accelerator_oscillator", "single", "pane", "int fast = 5, int slow = 34, int smooth = 5",
    "Accelerator Oscillator (Bill Williams): Awesome Oscillator - SMA(smooth) of the Awesome Oscillator.",
    """using namespace ind_ind_accelerator_oscillator_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s);
    std::vector<double> med(h.size(), nanv());
    for (std::size_t i = 0; i < h.size(); ++i) med[i] = (h[i] + l[i]) / 2.0;
    std::vector<double> f = sma(med, fast), sl = sma(med, slow);
    std::vector<double> ao(med.size(), nanv());
    for (std::size_t i = 0; i < med.size(); ++i) if (fin(f[i]) && fin(sl[i])) ao[i] = f[i] - sl[i];
    std::vector<double> sm = sma(ao, smooth);
    std::vector<double> out(med.size(), nanv());
    for (std::size_t i = 0; i < med.size(); ++i) if (fin(ao[i]) && fin(sm[i])) out[i] = ao[i] - sm[i];
    return out;""",
    "ind_accelerator_oscillator(s, 5, 34, 5)")
add("ChandeMomentumOscillator", "ind_chande_momentum_oscillator", "single", "pane", "int period = 20",
    "Chande Momentum Oscillator: 100 * (sum of up closes - sum of down closes) / (sum of up closes + sum of down closes) over period.",
    """const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)period; i < n; ++i) {
        double up = 0.0, dn = 0.0;
        for (std::size_t k = i - (std::size_t)period + 1; k <= i; ++k) {
            double d = closes[k] - closes[k-1];
            if (d > 0) up += d; else dn -= d;
        }
        out[i] = (up + dn) == 0.0 ? 0.0 : 100.0 * (up - dn) / (up + dn);
    }
    return out;""",
    "ind_chande_momentum_oscillator(xframe::series_closes(s), 20)")
add("TSI", "ind_tsi", "single", "pane", "int long_period = 25, int short_period = 13",
    "True Strength Index (Blau): 100 * double-EMA-smoothed momentum / double-EMA-smoothed absolute momentum; momentum = close - close[1].",
    """using namespace ind_ind_tsi_detail;
    std::vector<double> mom(closes.size(), nanv()), absmom(closes.size(), nanv());
    for (std::size_t i = 1; i < closes.size(); ++i) { mom[i] = closes[i] - closes[i-1]; absmom[i] = std::abs(mom[i]); }
    std::vector<double> num = ema(ema(mom, long_period), short_period);
    std::vector<double> den = ema(ema(absmom, long_period), short_period);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(num[i]) && fin(den[i]) && den[i] != 0.0) out[i] = 100.0 * num[i] / den[i];
    return out;""",
    "ind_tsi(xframe::series_closes(s), 25, 13)")
add("UltimateOscillator", "ind_ultimate_oscillator", "single", "pane", "int p1 = 7, int p2 = 14, int p3 = 28",
    "Ultimate Oscillator (Larry Williams): buying pressure = close - min(low, prev close); true range = max(high, prev close) - min(low, prev close); 100 * (4*avg7 + 2*avg14 + avg28) / 7.",
    """const std::size_t n = s.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    std::vector<double> bp(n, 0.0), tr(n, 0.0);
    for (std::size_t i = 1; i < n; ++i) {
        double lo = std::min(s[i].low, s[i-1].close);
        double hi = std::max(s[i].high, s[i-1].close);
        bp[i] = s[i].close - lo;
        tr[i] = hi - lo;
    }
    auto avg = [&](int p, std::size_t i) {
        double sb = 0.0, st = 0.0;
        for (int k = 0; k < p; ++k) { sb += bp[i - (std::size_t)k]; st += tr[i - (std::size_t)k]; }
        return st == 0.0 ? 0.0 : sb / st;
    };
    for (std::size_t i = (std::size_t)p3; i < n; ++i)
        out[i] = 100.0 * (4.0 * avg(p1, i) + 2.0 * avg(p2, i) + avg(p3, i)) / 7.0;
    return out;""",
    "ind_ultimate_oscillator(s, 7, 14, 28)")
add("KST", "ind_kst", "single", "pane", "int dummy = 0",
    "Know Sure Thing (Pring): weighted sum of four smoothed rates of change: 1*SMA10(ROC10) + 2*SMA10(ROC15) + 3*SMA10(ROC20) + 4*SMA9(ROC30).",
    """using namespace ind_ind_kst_detail;
    auto roc = [&](int p) {
        std::vector<double> o(closes.size(), nanv());
        for (std::size_t i = (std::size_t)p; i < closes.size(); ++i)
            if (closes[i - (std::size_t)p] != 0.0) o[i] = 100.0 * (closes[i] - closes[i - (std::size_t)p]) / closes[i - (std::size_t)p];
        return o;
    };
    std::vector<double> a = sma(roc(10), 10), b = sma(roc(15), 10), c = sma(roc(20), 10), d = sma(roc(30), 9);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i)
        if (fin(a[i]) && fin(b[i]) && fin(c[i]) && fin(d[i])) out[i] = a[i] + 2.0 * b[i] + 3.0 * c[i] + 4.0 * d[i];
    return out;""",
    "ind_kst(xframe::series_closes(s))")
add("FisherTransform", "ind_fisher_transform", "single", "pane", "int period = 10",
    "Fisher Transform (Ehlers): price normalized to [-1, 1] over period, then Fisher transform 0.5 * ln((1+x)/(1-x)), half-weighted with the previous value.",
    """using namespace ind_ind_fisher_transform_detail;
    std::vector<double> out(closes.size(), nanv());
    double prev = 0.0;
    for (std::size_t i = 0; i < closes.size(); ++i) {
        if (i < (std::size_t)(period - 1)) continue;
        double hi = highest(closes, i, period), lo = lowest(closes, i, period);
        double x = hi == lo ? 0.0 : 2.0 * (closes[i] - lo) / (hi - lo) - 1.0;
        if (x > 0.999) x = 0.999;
        if (x < -0.999) x = -0.999;
        double f = 0.5 * std::log((1.0 + x) / (1.0 - x)) + 0.5 * prev;
        out[i] = f; prev = f;
    }
    return out;""",
    "ind_fisher_transform(xframe::series_closes(s), 10)")
add("InverseFisherTransform", "ind_inverse_fisher_transform", "single", "pane", "int period = 14",
    "Inverse Fisher Transform (Ehlers): RSI(period) normalized to [-1, 1], then the inverse Fisher (exp(2x)-1)/(exp(2x)+1), rescaled to 0..100.",
    """using namespace ind_ind_inverse_fisher_transform_detail;
    std::vector<double> rsi = rsi_series(closes, period);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) {
        if (!fin(rsi[i])) continue;
        double x = rsi[i] / 50.0 - 1.0;
        if (x > 0.999) x = 0.999;
        if (x < -0.999) x = -0.999;
        double e = std::exp(2.0 * x);
        out[i] = 50.0 * ((e - 1.0) / (e + 1.0) + 1.0);
    }
    return out;""",
    "ind_inverse_fisher_transform(xframe::series_closes(s), 14)")
add("ErgodicOscillator", "ind_ergodic_oscillator", "single", "pane", "int long_period = 5, int short_period = 3",
    "Ergodic Oscillator (Blau): the True Strength Index with short smoothing periods (5, 3); 100 * double-EMA-smoothed momentum / double-EMA-smoothed absolute momentum.",
    """using namespace ind_ind_ergodic_oscillator_detail;
    std::vector<double> mom(closes.size(), nanv()), absmom(closes.size(), nanv());
    for (std::size_t i = 1; i < closes.size(); ++i) { mom[i] = closes[i] - closes[i-1]; absmom[i] = std::abs(mom[i]); }
    std::vector<double> num = ema(ema(mom, long_period), short_period);
    std::vector<double> den = ema(ema(absmom, long_period), short_period);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(num[i]) && fin(den[i]) && den[i] != 0.0) out[i] = 100.0 * num[i] / den[i];
    return out;""",
    "ind_ergodic_oscillator(xframe::series_closes(s), 5, 3)")
add("SchaffTrendCycle", "ind_schaff_trend_cycle", "single", "pane", "int fast = 23, int slow = 50, int cycle = 10",
    "Schaff Trend Cycle (Schaff): stochastic of the MACD line (EMA(fast) - EMA(slow)) over cycle bars, EMA-smoothed over cycle bars, scaled 0..100.",
    """using namespace ind_ind_schaff_trend_cycle_detail;
    std::vector<double> ef = ema(closes, fast), es = ema(closes, slow);
    std::vector<double> macd(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ef[i]) && fin(es[i])) macd[i] = ef[i] - es[i];
    std::vector<double> st(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) {
        if (i < (std::size_t)(cycle - 1) || !fin(macd[i])) continue;
        double hi = highest(macd, i, cycle), lo = lowest(macd, i, cycle);
        st[i] = hi == lo ? 50.0 : 100.0 * (macd[i] - lo) / (hi - lo);
    }
    return ema(st, cycle);""",
    "ind_schaff_trend_cycle(xframe::series_closes(s), 23, 50, 10)")
add("RelativeVigorIndex", "ind_relative_vigor_index", "single", "pane", "int period = 10",
    "Relative Vigor Index (Ehlers): SMA(period) of (close - open) / SMA(period) of (high - low); -1 to 1.",
    """using namespace ind_ind_relative_vigor_index_detail;
    std::vector<double> o = series_opens(s), c = series_closes(s), h = series_highs(s), l = series_lows(s);
    std::vector<double> num(c.size(), nanv()), den(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) { num[i] = c[i] - o[i]; den[i] = h[i] - l[i]; }
    std::vector<double> sn = sma(num, period), sd = sma(den, period);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(sn[i]) && fin(sd[i]) && sd[i] != 0.0) out[i] = sn[i] / sd[i];
    return out;""",
    "ind_relative_vigor_index(s, 10)")
add("ElderForceIndex", "ind_elder_force_index", "single", "pane", "int period = 13",
    "Elder Force Index: EMA(period) of (close - prev close) * volume.",
    """using namespace ind_ind_elder_force_index_detail;
    std::vector<double> c = series_closes(s), v = series_volumes(s);
    std::vector<double> f(c.size(), nanv());
    for (std::size_t i = 1; i < c.size(); ++i) f[i] = (c[i] - c[i-1]) * v[i];
    return ema(f, period);""",
    "ind_elder_force_index(s, 13)")
add("ElderImpulse", "ind_elder_impulse", "single", "overlay", "int ema_period = 13, int fast = 12, int slow = 26",
    "Elder Impulse System: +1 when EMA(13) is rising and the MACD histogram is rising; -1 when both are falling; 0 otherwise.",
    """using namespace ind_ind_elder_impulse_detail;
    std::vector<double> c = series_closes(s);
    std::vector<double> e = ema(c, ema_period);
    std::vector<double> ef = ema(c, fast), es = ema(c, slow);
    std::vector<double> macd(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(ef[i]) && fin(es[i])) macd[i] = ef[i] - es[i];
    std::vector<double> hist = ema(macd, 9);
    std::vector<double> sig(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(macd[i]) && fin(hist[i])) sig[i] = macd[i] - hist[i];
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 2; i < c.size(); ++i) {
        if (!fin(e[i]) || !fin(e[i-1]) || !fin(sig[i]) || !fin(sig[i-1])) continue;
        bool up = e[i] > e[i-1] && sig[i] > sig[i-1];
        bool dn = e[i] < e[i-1] && sig[i] < sig[i-1];
        out[i] = up ? 1.0 : (dn ? -1.0 : 0.0);
    }
    return out;""",
    "ind_elder_impulse(s, 13, 12, 26)")
add("BullPower", "ind_bull_power", "single", "pane", "int period = 13",
    "Elder Bull Power: high - EMA(period) of closes.",
    """using namespace ind_ind_bull_power_detail;
    std::vector<double> h = series_highs(s), c = series_closes(s);
    std::vector<double> e = ema(c, period);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(e[i])) out[i] = h[i] - e[i];
    return out;""",
    "ind_bull_power(s, 13)")
add("BearPower", "ind_bear_power", "single", "pane", "int period = 13",
    "Elder Bear Power: low - EMA(period) of closes.",
    """using namespace ind_ind_bear_power_detail;
    std::vector<double> l = series_lows(s), c = series_closes(s);
    std::vector<double> e = ema(c, period);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(e[i])) out[i] = l[i] - e[i];
    return out;""",
    "ind_bear_power(s, 13)")
add("CyberCycle", "ind_cyber_cycle", "single", "pane", "int dummy = 0",
    "Ehlers Cyber Cycle: price smoothed over 4 bars, then the cycle recursion with alpha 0.07: cycle = (1-a/2)^2 * (smooth - 2*smooth[1] + smooth[2]) + 2*(1-a)*cycle[1] - (1-a)^2 * cycle[2].",
    """const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    const double a = 0.07;
    std::vector<double> sm(n, 0.0), cy(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double c0 = closes[i], c1 = i >= 1 ? closes[i-1] : c0, c2 = i >= 2 ? closes[i-2] : c0, c3 = i >= 3 ? closes[i-3] : c0;
        sm[i] = (c0 + 2.0 * c1 + 2.0 * c2 + c3) / 6.0;
        double s1 = i >= 1 ? sm[i-1] : sm[i], s2 = i >= 2 ? sm[i-2] : sm[i];
        double y1 = i >= 1 ? cy[i-1] : 0.0, y2 = i >= 2 ? cy[i-2] : 0.0;
        cy[i] = (1.0 - a / 2.0) * (1.0 - a / 2.0) * (sm[i] - 2.0 * s1 + s2) + 2.0 * (1.0 - a) * y1 - (1.0 - a) * (1.0 - a) * y2;
        if (i >= 7) out[i] = cy[i];
    }
    return out;""",
    "ind_cyber_cycle(xframe::series_closes(s))")
add("CenterOfGravity", "ind_center_of_gravity", "single", "pane", "int period = 10",
    "Ehlers Center of Gravity: negative weighted average of price over period (weights 1..period, most recent heaviest), divided by the sum of prices; oscillates around zero with almost no lag.",
    """const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period - 1); i < n; ++i) {
        double num = 0.0, den = 0.0;
        for (int k = 0; k < period; ++k) {
            double w = (double)(k + 1);
            num += w * closes[i - (std::size_t)k];
            den += closes[i - (std::size_t)k];
        }
        out[i] = den == 0.0 ? 0.0 : -num / den;
    }
    return out;""",
    "ind_center_of_gravity(xframe::series_closes(s), 10)")
add("SineWave", "ind_sine_wave", "sine", "pane", "int dummy = 0",
    "Ehlers Sine Wave: sine and lead-sine lines derived from the Cyber Cycle quadrature (cycle and its one-bar change normalized by their combined magnitude). Unclear from the header: this is a simplified quadrature from the Cyber Cycle, not a full Hilbert transform.",
    """const std::size_t n = closes.size();
    IndSineWaveResult r;
    r.sine.assign(n, std::numeric_limits<double>::quiet_NaN());
    r.lead_sine.assign(n, std::numeric_limits<double>::quiet_NaN());
    const double a = 0.07;
    std::vector<double> sm(n, 0.0), cy(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double c0 = closes[i], c1 = i >= 1 ? closes[i-1] : c0, c2 = i >= 2 ? closes[i-2] : c0, c3 = i >= 3 ? closes[i-3] : c0;
        sm[i] = (c0 + 2.0 * c1 + 2.0 * c2 + c3) / 6.0;
        double s1 = i >= 1 ? sm[i-1] : sm[i], s2 = i >= 2 ? sm[i-2] : sm[i];
        double y1 = i >= 1 ? cy[i-1] : 0.0, y2 = i >= 2 ? cy[i-2] : 0.0;
        cy[i] = (1.0 - a / 2.0) * (1.0 - a / 2.0) * (sm[i] - 2.0 * s1 + s2) + 2.0 * (1.0 - a) * y1 - (1.0 - a) * (1.0 - a) * y2;
    }
    for (std::size_t i = 7; i < n; ++i) {
        double q = cy[i] - cy[i-1];
        double mag = std::sqrt(cy[i] * cy[i] + q * q);
        if (mag == 0.0) continue;
        r.sine[i] = cy[i] / mag;
        r.lead_sine[i] = q / mag;
    }
    return r;""",
    "ind_sine_wave(xframe::series_closes(s))")
add("DominantCycle", "ind_dominant_cycle", "single", "pane", "int dummy = 0",
    "Ehlers Dominant Cycle period: estimated from zero crossings of the Cyber Cycle over the last 50 bars (period = 100 / crossings, clamped 10..40). Unclear from the header: this is a zero-crossing estimate, not the MESA phasor measurement.",
    """const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    const double a = 0.07;
    std::vector<double> sm(n, 0.0), cy(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double c0 = closes[i], c1 = i >= 1 ? closes[i-1] : c0, c2 = i >= 2 ? closes[i-2] : c0, c3 = i >= 3 ? closes[i-3] : c0;
        sm[i] = (c0 + 2.0 * c1 + 2.0 * c2 + c3) / 6.0;
        double s1 = i >= 1 ? sm[i-1] : sm[i], s2 = i >= 2 ? sm[i-2] : sm[i];
        double y1 = i >= 1 ? cy[i-1] : 0.0, y2 = i >= 2 ? cy[i-2] : 0.0;
        cy[i] = (1.0 - a / 2.0) * (1.0 - a / 2.0) * (sm[i] - 2.0 * s1 + s2) + 2.0 * (1.0 - a) * y1 - (1.0 - a) * (1.0 - a) * y2;
    }
    for (std::size_t i = 50; i < n; ++i) {
        int cross = 0;
        for (std::size_t k = i - 49; k <= i; ++k) if ((cy[k] >= 0.0) != (cy[k-1] >= 0.0)) ++cross;
        double p = cross == 0 ? 20.0 : 100.0 / cross;
        if (p < 10.0) p = 10.0;
        if (p > 40.0) p = 40.0;
        out[i] = p;
    }
    return out;""",
    "ind_dominant_cycle(xframe::series_closes(s))")
add("ChandeForecastOscillator", "ind_chande_forecast_oscillator", "single", "pane", "int period = 5",
    "Chande Forecast Oscillator: 100 * (close - linear-regression fitted value over period) / close.",
    """using namespace ind_ind_chande_forecast_oscillator_detail;
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = (std::size_t)(period - 1); i < closes.size(); ++i) {
        if (closes[i] == 0.0) continue;
        out[i] = 100.0 * (closes[i] - linreg_value_at(closes, i, period)) / closes[i];
    }
    return out;""",
    "ind_chande_forecast_oscillator(xframe::series_closes(s), 5)")
add("CoppockCurve", "ind_coppock_curve", "single", "pane", "int roc1 = 14, int roc2 = 11, int wma = 10",
    "Coppock Curve: WMA(wma) of (ROC(roc1) + ROC(roc2)).",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    auto roc = [&](int p) {
        std::vector<double> o(closes.size(), std::numeric_limits<double>::quiet_NaN());
        for (std::size_t i = (std::size_t)p; i < closes.size(); ++i)
            if (closes[i - (std::size_t)p] != 0.0) o[i] = 100.0 * (closes[i] - closes[i - (std::size_t)p]) / closes[i - (std::size_t)p];
        return o;
    };
    std::vector<double> a = roc(roc1), b = roc(roc2);
    std::vector<double> sum(closes.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = 0; i < closes.size(); ++i) if (std::isfinite(a[i]) && std::isfinite(b[i])) sum[i] = a[i] + b[i];
    double wsum = (double)wma * (wma + 1) / 2.0;
    for (std::size_t i = (std::size_t)(wma - 1); i < closes.size(); ++i) {
        double s = 0.0; bool ok = true;
        for (int k = 0; k < wma; ++k) { if (!std::isfinite(sum[i - (std::size_t)k])) ok = false; s += (wma - k) * sum[i - (std::size_t)k]; }
        if (ok) out[i] = s / wsum;
    }
    return out;""",
    "ind_coppock_curve(xframe::series_closes(s), 14, 11, 10)")
add("Kairi", "ind_kairi", "single", "pane", "int period = 25",
    "Kairi Relative Index: 100 * (close - SMA(period)) / SMA(period).",
    """using namespace ind_ind_kairi_detail;
    std::vector<double> m = sma(closes, period);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(m[i]) && m[i] != 0.0) out[i] = 100.0 * (closes[i] - m[i]) / m[i];
    return out;""",
    "ind_kairi(xframe::series_closes(s), 25)")
add("DetrendedPriceOscillator", "ind_dpo", "single", "pane", "int period = 20",
    "Detrended Price Oscillator: close - SMA(period) displaced back by period/2 + 1 bars.",
    """using namespace ind_ind_dpo_detail;
    std::vector<double> m = sma(closes, period);
    int shift = period / 2 + 1;
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = (std::size_t)shift; i < closes.size(); ++i) {
        std::size_t j = i - (std::size_t)shift;
        if (fin(m[j])) out[i] = closes[i] - m[j];
    }
    return out;""",
    "ind_dpo(xframe::series_closes(s), 20)")
add("DeMarker", "ind_demarker", "single", "pane", "int period = 14",
    "DeMarker (DeMark): DeMax = high - prev high when it exceeds prev low - low, else 0; DeMin likewise for lows; DeM = 100 * SMA(DeMax) / (SMA(DeMax) + SMA(DeMin)).",
    """using namespace ind_ind_demarker_detail;
    const std::size_t n = s.size();
    std::vector<double> dmax(n, 0.0), dmin(n, 0.0);
    for (std::size_t i = 1; i < n; ++i) {
        double up = s[i].high - s[i-1].high, dn = s[i-1].low - s[i].low;
        dmax[i] = (up > 0.0 && up > dn) ? up : 0.0;
        dmin[i] = (dn > 0.0 && dn > up) ? dn : 0.0;
    }
    std::vector<double> amax = sma(dmax, period), amin = sma(dmin, period);
    std::vector<double> out(n, nanv());
    for (std::size_t i = 0; i < n; ++i) {
        if (!fin(amax[i]) || !fin(amin[i])) continue;
        double den = amax[i] + amin[i];
        out[i] = den == 0.0 ? 50.0 : 100.0 * amax[i] / den;
    }
    return out;""",
    "ind_demarker(s, 14)")
add("BalanceOfPower", "ind_balance_of_power", "single", "pane", "int period = 14",
    "Balance of Power: SMA(period) of (close - open) / (high - low).",
    """using namespace ind_ind_balance_of_power_detail;
    std::vector<double> o = series_opens(s), c = series_closes(s), h = series_highs(s), l = series_lows(s);
    std::vector<double> bop(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) bop[i] = (h[i] == l[i]) ? 0.0 : (c[i] - o[i]) / (h[i] - l[i]);
    return sma(bop, period);""",
    "ind_balance_of_power(s, 14)")
add("RelativeMomentumIndex", "ind_relative_momentum_index", "single", "pane", "int period = 20, int momentum = 5",
    "Relative Momentum Index (RMI): RSI computed on momentum-period changes (close vs close[momentum]) instead of one-bar changes, Wilder-smoothed over period.",
    """using namespace ind_ind_relative_momentum_index_detail;
    const std::size_t n = closes.size();
    std::vector<double> out(n, nanv());
    if (n < (std::size_t)(period + momentum)) return out;
    std::vector<double> g(n, 0.0), l(n, 0.0);
    for (std::size_t i = (std::size_t)momentum; i < n; ++i) { double d = closes[i] - closes[i - (std::size_t)momentum]; g[i] = d > 0 ? d : 0.0; l[i] = d < 0 ? -d : 0.0; }
    std::vector<double> ag = wilder(g, period), al = wilder(l, period);
    for (std::size_t i = 0; i < n; ++i) {
        if (!fin(ag[i]) || !fin(al[i])) continue;
        double den = ag[i] + al[i];
        out[i] = den == 0.0 ? 50.0 : 100.0 * ag[i] / den;
    }
    return out;""",
    "ind_relative_momentum_index(xframe::series_closes(s), 20, 5)")
add("IntradayMomentumIndex", "ind_intraday_momentum_index", "single", "pane", "int period = 14",
    "Intraday Momentum Index (Chande): 100 * sum of up-day (close - open) gains / (sum of gains + sum of down-day (open - close) losses) over period.",
    """const std::size_t n = s.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period - 1); i < n; ++i) {
        double up = 0.0, dn = 0.0;
        for (int k = 0; k < period; ++k) {
            double d = s[i - (std::size_t)k].close - s[i - (std::size_t)k].open;
            if (d > 0) up += d; else dn -= d;
        }
        out[i] = (up + dn) == 0.0 ? 50.0 : 100.0 * up / (up + dn);
    }
    return out;""",
    "ind_intraday_momentum_index(s, 14)")
add("DynamicMomentumIndex", "ind_dynamic_momentum_index", "single", "pane", "int period = 14",
    "Dynamic Momentum Index (Chande): RSI whose period adapts to relative volatility (stddev over 5 bars / stddev over 14 bars), clamped to 5..30 bars.",
    """using namespace ind_ind_dynamic_momentum_index_detail;
    const std::size_t n = closes.size();
    std::vector<double> out(n, nanv());
    if (n < 15) return out;
    std::vector<double> sd5 = stddev(closes, 5), sd14 = stddev(closes, 14);
    double ag = 0.0, al = 0.0; bool have = false;
    for (std::size_t i = 1; i < n; ++i) {
        double d = closes[i] - closes[i-1];
        double g = d > 0 ? d : 0.0, l = d < 0 ? -d : 0.0;
        double ratio = 1.0;
        if (fin(sd5[i]) && fin(sd14[i]) && sd14[i] != 0.0) ratio = sd5[i] / sd14[i];
        int p = (int)std::round(period * ratio);
        if (p < 5) p = 5;
        if (p > 30) p = 30;
        double a = 1.0 / p;
        if (!have) {
            if (i >= (std::size_t)period) {
                double sg = 0.0, sl = 0.0;
                for (std::size_t k = i - (std::size_t)period + 1; k <= i; ++k) { double dd = closes[k] - closes[k-1]; sg += dd > 0 ? dd : 0.0; sl += dd < 0 ? -dd : 0.0; }
                ag = sg / period; al = sl / period; have = true;
            } else continue;
        } else { ag = a * g + (1.0 - a) * ag; al = a * l + (1.0 - a) * al; }
        double den = ag + al;
        out[i] = den == 0.0 ? 50.0 : 100.0 * ag / den;
    }
    return out;""",
    "ind_dynamic_momentum_index(xframe::series_closes(s), 14)")
add("WaveTrend", "ind_wave_trend", "single", "pane", "int esa = 10, int d = 21",
    "WaveTrend Oscillator: ESA = EMA(esa) of typical price; D = EMA(esa) of |typical - ESA|; CI = (typical - ESA) / (0.015 * D); WT1 = EMA(d) of CI. This widget plots WT1.",
    """using namespace ind_ind_wave_trend_detail;
    std::vector<double> tp = series_typical(s);
    std::vector<double> esa_v = ema(tp, esa);
    std::vector<double> dev(tp.size(), nanv());
    for (std::size_t i = 0; i < tp.size(); ++i) if (fin(esa_v[i])) dev[i] = std::abs(tp[i] - esa_v[i]);
    std::vector<double> dv = ema(dev, esa);
    std::vector<double> ci(tp.size(), nanv());
    for (std::size_t i = 0; i < tp.size(); ++i) if (fin(esa_v[i]) && fin(dv[i]) && dv[i] != 0.0) ci[i] = (tp[i] - esa_v[i]) / (0.015 * dv[i]);
    return ema(ci, d);""",
    "ind_wave_trend(s, 10, 21)")
add("PrettyGoodOscillator", "ind_pretty_good_oscillator", "single", "pane", "int period = 14",
    "Pretty Good Oscillator (Mark Johnson): (close - SMA(period)) / EMA(period) of true range, where true range = max(high-low, |high-prev close|, |low-prev close|).",
    """using namespace ind_ind_pretty_good_oscillator_detail;
    std::vector<double> c = series_closes(s), h = series_highs(s), l = series_lows(s);
    std::vector<double> tr(c.size(), nanv());
    for (std::size_t i = 1; i < c.size(); ++i) tr[i] = std::max(h[i] - l[i], std::max(std::abs(h[i] - c[i-1]), std::abs(l[i] - c[i-1])));
    std::vector<double> m = sma(c, period), atr = ema(tr, period);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(m[i]) && fin(atr[i]) && atr[i] != 0.0) out[i] = (c[i] - m[i]) / atr[i];
    return out;""",
    "ind_pretty_good_oscillator(s, 14)")
add("Inertia", "ind_inertia", "single", "pane", "int period = 20",
    "Inertia (Chande): SMA(period) of RSI(period); a smoothed RSI.",
    """using namespace ind_ind_inertia_detail;
    std::vector<double> rsi = rsi_series(closes, period);
    return sma(rsi, period);""",
    "ind_inertia(xframe::series_closes(s), 20)")
add("RangeExpansionIndex", "ind_range_expansion_index", "single", "pane", "int period = 8",
    "Range Expansion Index (DeMark): over period bars, sum of (close - close[2]) for bars whose high breaks above high[2] or low breaks below low[2], divided by the sum of |(close - close[2])| for the same bars, times 100.",
    """const std::size_t n = s.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period + 1); i < n; ++i) {
        double num = 0.0, den = 0.0;
        for (int k = 0; k < period; ++k) {
            std::size_t j = i - (std::size_t)k;
            if (j < 2) continue;
            if (s[j].high >= s[j-2].high || s[j].low <= s[j-2].low) {
                double d = s[j].close - s[j-2].close;
                num += d; den += std::abs(d);
            }
        }
        out[i] = den == 0.0 ? 0.0 : 100.0 * num / den;
    }
    return out;""",
    "ind_range_expansion_index(s, 8)")
add("TDSequential", "ind_td_sequential", "single", "pane", "int dummy = 0",
    "TD Sequential setup (DeMark): counts consecutive closes below close[4] as a buy setup (+1..+9) or above close[4] as a sell setup (-1..-9); the count resets when the condition flips.",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    int count = 0;
    for (std::size_t i = 0; i < closes.size(); ++i) {
        if (i < 4) { out[i] = 0.0; continue; }
        if (closes[i] < closes[i-4]) count = count > 0 ? count + 1 : 1;
        else if (closes[i] > closes[i-4]) count = count < 0 ? count - 1 : -1;
        else count = 0;
        if (count > 9) count = 9;
        if (count < -9) count = -9;
        out[i] = (double)count;
    }
    return out;""",
    "ind_td_sequential(xframe::series_closes(s))")
add("DoubleStochastic", "ind_double_stochastic", "single", "pane", "int period = 14, int short_period = 5, int smooth = 3",
    "Double Stochastic: average of the %K over period and the %K over short_period, SMA-smoothed over smooth bars.",
    """using namespace ind_ind_double_stochastic_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> k1 = stoch_k(h, l, c, period), k2 = stoch_k(h, l, c, short_period);
    std::vector<double> avg(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(k1[i]) && fin(k2[i])) avg[i] = (k1[i] + k2[i]) / 2.0;
    return sma(avg, smooth);""",
    "ind_double_stochastic(s, 14, 5, 3)")
add("PriceMomentumOscillator", "ind_price_momentum_oscillator", "single", "pane", "int slow = 35, int fast = 20",
    "Price Momentum Oscillator (Swenlin): one-bar ROC in percent, EMA-smoothed over slow bars then over fast bars, times 10.",
    """using namespace ind_ind_price_momentum_oscillator_detail;
    std::vector<double> roc(closes.size(), nanv());
    for (std::size_t i = 1; i < closes.size(); ++i) if (closes[i-1] != 0.0) roc[i] = 100.0 * (closes[i] - closes[i-1]) / closes[i-1];
    std::vector<double> s1 = ema(roc, slow);
    std::vector<double> s2 = ema(s1, fast);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(s2[i])) out[i] = 10.0 * s2[i];
    return out;""",
    "ind_price_momentum_oscillator(xframe::series_closes(s), 35, 20)")
add("SwenlinTradingOscillator", "ind_swenlin_trading_oscillator", "single", "pane", "int slow = 35, int fast = 20, int signal = 10",
    "Swenlin Trading Oscillator: the Price Momentum Oscillator minus its EMA(signal) signal line.",
    """using namespace ind_ind_swenlin_trading_oscillator_detail;
    std::vector<double> roc(closes.size(), nanv());
    for (std::size_t i = 1; i < closes.size(); ++i) if (closes[i-1] != 0.0) roc[i] = 100.0 * (closes[i] - closes[i-1]) / closes[i-1];
    std::vector<double> s1 = ema(roc, slow);
    std::vector<double> s2 = ema(s1, fast);
    std::vector<double> pmo(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(s2[i])) pmo[i] = 10.0 * s2[i];
    std::vector<double> sig = ema(pmo, signal);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(pmo[i]) && fin(sig[i])) out[i] = pmo[i] - sig[i];
    return out;""",
    "ind_swenlin_trading_oscillator(xframe::series_closes(s), 35, 20, 10)")
add("RSX", "ind_rsx", "single", "pane", "int period = 14",
    "RSX (Jurik): RSI whose average gains and losses are triple-EMA-smoothed instead of Wilder-smoothed. Unclear from the header: the Jurik smoothing is proprietary; this uses triple EMA as the published approximation.",
    """using namespace ind_ind_rsx_detail;
    const std::size_t n = closes.size();
    std::vector<double> out(n, nanv());
    if (n < (std::size_t)(period + 1)) return out;
    std::vector<double> g(n, 0.0), l(n, 0.0);
    for (std::size_t i = 1; i < n; ++i) { double d = closes[i] - closes[i-1]; g[i] = d > 0 ? d : 0.0; l[i] = d < 0 ? -d : 0.0; }
    std::vector<double> ag = ema(ema(ema(g, period), period), period);
    std::vector<double> al = ema(ema(ema(l, period), period), period);
    for (std::size_t i = 0; i < n; ++i) {
        if (!fin(ag[i]) || !fin(al[i])) continue;
        double den = ag[i] + al[i];
        out[i] = den == 0.0 ? 50.0 : 100.0 * ag[i] / den;
    }
    return out;""",
    "ind_rsx(xframe::series_closes(s), 14)")
add("JurikVelocity", "ind_jurik_velocity", "single", "pane", "int period = 14",
    "Jurik Velocity: one-bar ROC in percent, triple-EMA-smoothed over period. Unclear from the header: Jurik Velocity is proprietary; this uses triple EMA as the published approximation.",
    """using namespace ind_ind_jurik_velocity_detail;
    std::vector<double> roc(closes.size(), nanv());
    for (std::size_t i = 1; i < closes.size(); ++i) if (closes[i-1] != 0.0) roc[i] = 100.0 * (closes[i] - closes[i-1]) / closes[i-1];
    return ema(ema(ema(roc, period), period), period);""",
    "ind_jurik_velocity(xframe::series_closes(s), 14)")
add("NormalizedROC", "ind_normalized_roc", "single", "pane", "int period = 12, int norm = 20",
    "Normalized ROC: ROC(period) divided by the rolling standard deviation of ROC(period) over norm bars.",
    """using namespace ind_ind_normalized_roc_detail;
    std::vector<double> roc(closes.size(), nanv());
    for (std::size_t i = (std::size_t)period; i < closes.size(); ++i)
        if (closes[i - (std::size_t)period] != 0.0) roc[i] = 100.0 * (closes[i] - closes[i - (std::size_t)period]) / closes[i - (std::size_t)period];
    std::vector<double> sd = stddev(roc, norm);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(roc[i]) && fin(sd[i]) && sd[i] != 0.0) out[i] = roc[i] / sd[i];
    return out;""",
    "ind_normalized_roc(xframe::series_closes(s), 12, 20)")
add("LaguerreFilter", "ind_laguerre_filter", "single", "overlay", "double gamma = 0.8",
    "Laguerre Filter (Ehlers): the first output (L0) of a 4-pole Laguerre filter on price; a low-lag smoothed price line drawn over the candles.",
    """const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    double g = gamma;
    std::vector<double> L0(n, 0.0), L1(n, 0.0), L2(n, 0.0), L3(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double p = closes[i];
        double pL0 = i ? L0[i-1] : p, pL1 = i ? L1[i-1] : 0.0, pL2 = i ? L2[i-1] : 0.0, pL3 = i ? L3[i-1] : 0.0;
        L0[i] = (1.0 - g) * p + g * pL0;
        L1[i] = -g * L0[i] + pL0 + g * pL1;
        L2[i] = -g * L1[i] + pL1 + g * pL2;
        L3[i] = -g * L2[i] + pL2 + g * pL3;
        out[i] = L0[i];
    }
    return out;""",
    "ind_laguerre_filter(xframe::series_closes(s), 0.8)")
add("SuperSmoother", "ind_super_smoother", "single", "overlay", "int period = 10",
    "Ehlers SuperSmoother: a 2-pole low-pass filter with coefficients from the period (a1 = exp(-1.414*pi/period)); a smoothed price line drawn over the candles.",
    """using namespace ind_ind_super_smoother_detail;
    return super_smoother(closes, period);""",
    "ind_super_smoother(xframe::series_closes(s), 10)")
add("RainbowOscillator", "ind_rainbow_oscillator", "single", "pane", "int max_period = 10",
    "Rainbow Oscillator: 100 * (close - rainbow average) / rainbow average, where the rainbow average is the mean of SMA(2)..SMA(max_period).",
    """using namespace ind_ind_rainbow_oscillator_detail;
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) {
        if (i < (std::size_t)(max_period - 1)) continue;
        double sum = 0.0; int cnt = 0;
        for (int p = 2; p <= max_period; ++p) {
            double s = 0.0;
            for (int k = 0; k < p; ++k) s += closes[i - (std::size_t)k];
            sum += s / p; ++cnt;
        }
        double rb = sum / cnt;
        if (rb != 0.0) out[i] = 100.0 * (closes[i] - rb) / rb;
    }
    return out;""",
    "ind_rainbow_oscillator(xframe::series_closes(s), 10)")
add("LinearRegressionOscillator", "ind_linear_regression_oscillator", "single", "pane", "int period = 14",
    "Linear Regression Oscillator: close minus the linear-regression fitted value over period bars.",
    """using namespace ind_ind_linear_regression_oscillator_detail;
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = (std::size_t)(period - 1); i < closes.size(); ++i)
        out[i] = closes[i] - linreg_value_at(closes, i, period);
    return out;""",
    "ind_linear_regression_oscillator(xframe::series_closes(s), 14)")
add("PercentRank", "ind_percent_rank", "single", "pane", "int period = 14",
    "Percent Rank: 100 * (number of prior period-1 closes below the current close) / (period - 1).",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period - 1); i < closes.size(); ++i) {
        int cnt = 0;
        for (int k = 1; k < period; ++k) if (closes[i - (std::size_t)k] < closes[i]) ++cnt;
        out[i] = 100.0 * cnt / (period - 1);
    }
    return out;""",
    "ind_percent_rank(xframe::series_closes(s), 14)")
add("SpearmanRank", "ind_spearman_rank", "single", "pane", "int period = 14",
    "Spearman Rank Correlation: rank correlation between closes and bar order over period bars; -1 to 1.",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period - 1); i < closes.size(); ++i) {
        double mx = 0.0, my = 0.0;
        std::vector<double> rx((std::size_t)period), ry((std::size_t)period);
        for (int k = 0; k < period; ++k) {
            double v = closes[i - (std::size_t)(period - 1) + (std::size_t)k];
            int r = 1;
            for (int j = 0; j < period; ++j) if (closes[i - (std::size_t)(period - 1) + (std::size_t)j] < v) ++r;
            rx[(std::size_t)k] = (double)r; ry[(std::size_t)k] = (double)(k + 1);
            mx += rx[(std::size_t)k]; my += ry[(std::size_t)k];
        }
        mx /= period; my /= period;
        double sxy = 0.0, sxx = 0.0, syy = 0.0;
        for (int k = 0; k < period; ++k) { sxy += (rx[(std::size_t)k] - mx) * (ry[(std::size_t)k] - my); sxx += (rx[(std::size_t)k] - mx) * (rx[(std::size_t)k] - mx); syy += (ry[(std::size_t)k] - my) * (ry[(std::size_t)k] - my); }
        out[i] = (sxx == 0.0 || syy == 0.0) ? 0.0 : sxy / std::sqrt(sxx * syy);
    }
    return out;""",
    "ind_spearman_rank(xframe::series_closes(s), 14)")
add("CorrelationCoefficient", "ind_correlation_coefficient", "single", "pane", "int period = 14",
    "Correlation Coefficient: Pearson correlation between closes and bar order over period bars; -1 to 1.",
    """std::vector<double> out(closes.size(), std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period - 1); i < closes.size(); ++i) {
        double mx = 0.0, my = 0.0;
        for (int k = 0; k < period; ++k) { mx += closes[i - (std::size_t)(period - 1) + (std::size_t)k]; my += (double)(k + 1); }
        mx /= period; my /= period;
        double sxy = 0.0, sxx = 0.0, syy = 0.0;
        for (int k = 0; k < period; ++k) {
            double dx = closes[i - (std::size_t)(period - 1) + (std::size_t)k] - mx;
            double dy = (double)(k + 1) - my;
            sxy += dx * dy; sxx += dx * dx; syy += dy * dy;
        }
        out[i] = (sxx == 0.0 || syy == 0.0) ? 0.0 : sxy / std::sqrt(sxx * syy);
    }
    return out;""",
    "ind_correlation_coefficient(xframe::series_closes(s), 14)")
add("Decycler", "ind_decycler", "single", "overlay", "int period = 10",
    "Ehlers Decycler: price high-pass filtered (price minus its SMA over 48 bars) and then SuperSmoothed over period; a low-lag smoothed price line drawn over the candles. Unclear from the header: the high-pass here uses an SMA subtraction rather than the textbook 2-pole high-pass.",
    """using namespace ind_ind_decycler_detail;
    std::vector<double> hp(closes.size(), nanv());
    std::vector<double> m48 = sma(closes, 48);
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(m48[i])) hp[i] = closes[i] - m48[i];
    std::vector<double> ss = super_smoother(hp, period);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(ss[i]) && fin(m48[i])) out[i] = ss[i] + m48[i];
    return out;""",
    "ind_decycler(xframe::series_closes(s), 10)")
add("TrendFlex", "ind_trend_flex", "single", "pane", "int period = 10",
    "Ehlers TrendFlex: the 10-bar price change (close - close[10]), SuperSmoothed over period. Unclear from the header: this is the simplified TrendFlex form (smoothed momentum), not the full roofing-filter version.",
    """using namespace ind_ind_trend_flex_detail;
    std::vector<double> d(closes.size(), nanv());
    for (std::size_t i = 10; i < closes.size(); ++i) d[i] = closes[i] - closes[i-10];
    return super_smoother(d, period);""",
    "ind_trend_flex(xframe::series_closes(s), 10)")
add("RoofingFilter", "ind_roofing_filter", "single", "pane", "int period = 10, int hp_period = 48",
    "Ehlers Roofing Filter: price high-pass filtered (price minus its SMA over hp_period bars), then SuperSmoothed over period.",
    """using namespace ind_ind_roofing_filter_detail;
    std::vector<double> hp(closes.size(), nanv());
    std::vector<double> m = sma(closes, hp_period);
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(m[i])) hp[i] = closes[i] - m[i];
    return super_smoother(hp, period);""",
    "ind_roofing_filter(xframe::series_closes(s), 10, 48)")
add("Reflex", "ind_reflex", "single", "pane", "int period = 10",
    "Ehlers Reflex: the 10-bar price change (close - close[10]), SuperSmoothed over period and normalized by the SMA(10) of the high-low range.",
    """using namespace ind_ind_reflex_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> d(c.size(), nanv());
    for (std::size_t i = 10; i < c.size(); ++i) d[i] = c[i] - c[i-10];
    std::vector<double> ss = super_smoother(d, period);
    std::vector<double> rng(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) rng[i] = h[i] - l[i];
    std::vector<double> m = sma(rng, period);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 0; i < c.size(); ++i) if (fin(ss[i]) && fin(m[i]) && m[i] != 0.0) out[i] = ss[i] / m[i];
    return out;""",
    "ind_reflex(s, 10)")
add("EvenBetterSineWave", "ind_even_better_sinewave", "sine", "pane", "int period = 10, int hp_period = 48",
    "Ehlers Even Better SineWave: sine and lead-sine lines from the Roofing Filter output and its one-bar change, normalized by their combined magnitude. Unclear from the header: quadrature is taken from the roofing filter, not a Hilbert transform.",
    """using namespace ind_ind_even_better_sinewave_detail;
    std::vector<double> hp(closes.size(), nanv());
    std::vector<double> m = sma(closes, hp_period);
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(m[i])) hp[i] = closes[i] - m[i];
    std::vector<double> rf = super_smoother(hp, period);
    const std::size_t n = closes.size();
    IndEvenBetterSineWaveResult r;
    r.sine.assign(n, nanv());
    r.lead_sine.assign(n, nanv());
    for (std::size_t i = 1; i < n; ++i) {
        if (!fin(rf[i]) || !fin(rf[i-1])) continue;
        double q = rf[i] - rf[i-1];
        double mag = std::sqrt(rf[i] * rf[i] + q * q);
        if (mag == 0.0) continue;
        r.sine[i] = rf[i] / mag;
        r.lead_sine[i] = q / mag;
    }
    return r;""",
    "ind_even_better_sinewave(xframe::series_closes(s), 10, 48)")
add("StochasticPop", "ind_stochastic_pop", "single", "pane", "int period = 14",
    "Stochastic Pop (Bernstein): +1 on the bar the fast stochastic %K crosses above 80, -1 on the bar it crosses below 20, 0 otherwise; a pop signal series.",
    """using namespace ind_ind_stochastic_pop_detail;
    std::vector<double> h = series_highs(s), l = series_lows(s), c = series_closes(s);
    std::vector<double> k = stoch_k(h, l, c, period);
    std::vector<double> out(c.size(), nanv());
    for (std::size_t i = 1; i < c.size(); ++i) {
        if (!fin(k[i]) || !fin(k[i-1])) continue;
        if (k[i-1] <= 80.0 && k[i] > 80.0) out[i] = 1.0;
        else if (k[i-1] >= 20.0 && k[i] < 20.0) out[i] = -1.0;
        else out[i] = 0.0;
    }
    return out;""",
    "ind_stochastic_pop(s, 14)")
add("GannSwingOscillator", "ind_gann_swing_oscillator", "single", "pane", "int dummy = 0",
    "Gann Swing Oscillator: swing state, +1 on an up-swing bar (higher high and higher low than the previous bar), -1 on a down-swing bar (lower high and lower low), otherwise the previous state is carried; 0 before the first swing.",
    """const std::size_t n = s.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    double state = 0.0;
    for (std::size_t i = 1; i < n; ++i) {
        bool up = s[i].high > s[i-1].high && s[i].low > s[i-1].low;
        bool dn = s[i].high < s[i-1].high && s[i].low < s[i-1].low;
        if (up) state = 1.0;
        else if (dn) state = -1.0;
        out[i] = state;
    }
    return out;""",
    "ind_gann_swing_oscillator(s)")
add("SpecialK", "ind_special_k", "single", "pane", "int dummy = 0",
    "Special K (Pring): 1*SMA10(ROC10) + 2*SMA10(ROC15) + 3*SMA10(ROC20) + 4*SMA15(ROC30). Differs from KST only in the smoothing of the 30-bar ROC (SMA15 here, SMA9 in KST).",
    """using namespace ind_ind_special_k_detail;
    auto roc = [&](int p) {
        std::vector<double> o(closes.size(), nanv());
        for (std::size_t i = (std::size_t)p; i < closes.size(); ++i)
            if (closes[i - (std::size_t)p] != 0.0) o[i] = 100.0 * (closes[i] - closes[i - (std::size_t)p]) / closes[i - (std::size_t)p];
        return o;
    };
    std::vector<double> a = sma(roc(10), 10), b = sma(roc(15), 10), c = sma(roc(20), 10), d = sma(roc(30), 15);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i)
        if (fin(a[i]) && fin(b[i]) && fin(c[i]) && fin(d[i])) out[i] = a[i] + 2.0 * b[i] + 3.0 * c[i] + 4.0 * d[i];
    return out;""",
    "ind_special_k(xframe::series_closes(s))")
add("AdaptiveCyberCycle", "ind_adaptive_cyber_cycle", "single", "pane", "int dummy = 0",
    "Ehlers Adaptive Cyber Cycle: the Cyber Cycle recursion with alpha adapted per bar from the dominant-cycle period estimate (zero-crossing estimate over 50 bars): alpha = clamp(0.07 * 20 / period, 0.02, 0.20). Unclear from the header: the period estimate is a zero-crossing estimate, not the MESA phasor measurement.",
    """const std::size_t n = closes.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    std::vector<double> sm(n, 0.0), cy(n, 0.0);
    std::vector<double> cyc(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double c0 = closes[i], c1 = i >= 1 ? closes[i-1] : c0, c2 = i >= 2 ? closes[i-2] : c0, c3 = i >= 3 ? closes[i-3] : c0;
        sm[i] = (c0 + 2.0 * c1 + 2.0 * c2 + c3) / 6.0;
    }
    for (std::size_t i = 0; i < n; ++i) {
        double a = 0.07;
        if (i >= 50) {
            int cross = 0;
            for (std::size_t k = i - 49; k < i; ++k) if ((cyc[k] >= 0.0) != (cyc[k-1] >= 0.0)) ++cross;
            double p = cross == 0 ? 20.0 : 98.0 / cross;
            if (p < 10.0) p = 10.0;
            if (p > 40.0) p = 40.0;
            a = 0.07 * 20.0 / p;
            if (a < 0.02) a = 0.02;
            if (a > 0.20) a = 0.20;
        }
        double s1 = i >= 1 ? sm[i-1] : sm[i], s2 = i >= 2 ? sm[i-2] : sm[i];
        double y1 = i >= 1 ? cy[i-1] : 0.0, y2 = i >= 2 ? cy[i-2] : 0.0;
        cy[i] = (1.0 - a / 2.0) * (1.0 - a / 2.0) * (sm[i] - 2.0 * s1 + s2) + 2.0 * (1.0 - a) * y1 - (1.0 - a) * (1.0 - a) * y2;
        cyc[i] = cy[i];
        if (i >= 7) out[i] = cy[i];
    }
    return out;""",
    "ind_adaptive_cyber_cycle(xframe::series_closes(s))")

# ---------------- emission ----------------
import re

add("DisparityIndex", "ind_disparity_index", "single", "pane", "int period = 20",
    "Disparity Index: 100 * (close - SMA(period)) / SMA(period); 0 when price sits on its moving average, positive above it, negative below it.",
    """using namespace ind_ind_disparity_index_detail;
    std::vector<double> m = sma(closes, period);
    std::vector<double> out(closes.size(), nanv());
    for (std::size_t i = 0; i < closes.size(); ++i) if (fin(m[i]) && m[i] != 0.0) out[i] = 100.0 * (closes[i] - m[i]) / m[i];
    return out;""",
    "ind_disparity_index(xframe::series_closes(s), 20)")
add("SmoothedROC", "ind_smoothed_roc", "single", "pane", "int period = 12, int smooth = 25",
    "Smoothed Rate of Change (Schutzman): EMA(smooth) of ROC(period), where ROC(period) = 100 * (close - close[period]) / close[period].",
    """using namespace ind_ind_smoothed_roc_detail;
    std::vector<double> roc(closes.size(), nanv());
    for (std::size_t i = (std::size_t)period; i < closes.size(); ++i) if (closes[i - (std::size_t)period] != 0.0) roc[i] = 100.0 * (closes[i] - closes[i - (std::size_t)period]) / closes[i - (std::size_t)period];
    return ema(roc, smooth);""",
    "ind_smoothed_roc(xframe::series_closes(s), 12, 25)")
add("WilliamsAccumulationDistribution", "ind_williams_ad", "single", "pane", "int dummy = 0",
    "Williams Accumulation/Distribution (Larry Williams): cumulative; when close rises, add close - min(low, prev close); when close falls, subtract max(high, prev close) - close; unchanged on an unchanged close.",
    """const std::size_t n = s.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    double run = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        if (i == 0) { out[i] = 0.0; continue; }
        double C = s[i].close, C1 = s[i-1].close, H = s[i].high, L = s[i].low;
        if (C > C1) run += C - std::min(L, C1);
        else if (C < C1) run -= std::max(H, C1) - C;
        out[i] = run;
    }
    return out;""",
    "ind_williams_ad(s)")
add("ChoppinessIndex", "ind_choppiness_index", "single", "pane", "int period = 14",
    "Choppiness Index (Ehlers): 100 * log10(sum of true range over period / (highest high - lowest low over period)) / log10(period); above about 61.8 the market is choppy, below about 38.2 it is trending.",
    """const std::size_t n = s.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = (std::size_t)(period - 1); i < n; ++i) {
        double tr = 0.0, hh = -1e300, ll = 1e300;
        for (int k = 0; k < period; ++k) {
            std::size_t j = i - (std::size_t)k;
            double h = s[j].high, l = s[j].low;
            double t = h - l;
            if (j > 0) { t = std::max(t, std::abs(h - s[j-1].close)); t = std::max(t, std::abs(l - s[j-1].close)); }
            tr += t;
            hh = std::max(hh, h); ll = std::min(ll, l);
        }
        double range = hh - ll;
        if (range <= 0.0 || tr <= 0.0) continue;
        out[i] = 100.0 * std::log10(tr / range) / std::log10((double)period);
    }
    return out;""",
    "ind_choppiness_index(s, 14)")
add("GannHiLoActivator", "ind_gann_hilo_activator", "single", "overlay", "int period = 3",
    "Gann Hi-Lo Activator (Gann): in an uptrend the line is the lowest low of the previous 3 bars, in a downtrend the highest high of the previous 3 bars; the trend flips when the close crosses the line.",
    """const std::size_t n = s.size();
    std::vector<double> out(n, std::numeric_limits<double>::quiet_NaN());
    bool up = true, have = false;
    double act = 0.0;
    for (std::size_t i = (std::size_t)period; i < n; ++i) {
        double hi = -1e300, lo = 1e300;
        for (int k = 1; k <= period; ++k) {
            hi = std::max(hi, s[i - (std::size_t)k].high);
            lo = std::min(lo, s[i - (std::size_t)k].low);
        }
        if (!have) { up = true; act = lo; have = true; }
        if (up) {
            if (s[i].close < act) { up = false; act = hi; }
            else act = lo;
        } else {
            if (s[i].close > act) { up = true; act = lo; }
            else act = hi;
        }
        out[i] = act;
    }
    return out;""",
    "ind_gann_hilo_activator(s, 3)")
add("JurikMovingAverage", "ind_jurik_moving_average", "single", "overlay", "int period = 20",
    "Jurik Moving Average: an adaptive smoothed price line. Unclear from the header: the Jurik Moving Average is proprietary; this uses triple EMA as the published approximation.",
    """using namespace ind_ind_jurik_moving_average_detail;
    return ema(ema(ema(closes, period), period), period);""",
    "ind_jurik_moving_average(xframe::series_closes(s), 20)")
add("EhlersSmoothedRSI", "ind_ehlers_smoothed_rsi", "single", "pane", "int period = 14",
    "Ehlers Smoothed RSI: Wilder RSI(period) of closes, then SuperSmoothed over period. Unclear from the header: Ehlers smoothed RSI applies his SuperSmoother to the RSI series; this widget SuperSmooths the RSI output.",
    """using namespace ind_ind_ehlers_smoothed_rsi_detail;
    std::vector<double> rsi = rsi_series(closes, period);
    return super_smoother(rsi, period);""",
    "ind_ehlers_smoothed_rsi(xframe::series_closes(s), 14)")

def param_names(ctor):
    names = []
    for part in ctor.split(","):
        part = part.strip()
        if not part:
            continue
        part = part.split("=")[0].strip()
        names.append(part.split()[-1])
    return names

def is_ohlc(spec):
    return spec["call"].startswith(spec["fn"] + "(s")

def signature(spec):
    fn = spec["fn"]
    params = spec["ctor"]
    if is_ohlc(spec):
        head = "const xframe::OHLCVSeries& s"
    else:
        head = "const std::vector<double>& closes"
    if params:
        return head + ", " + params
    return head

STRUCTS = {
    "macd": "struct Ind{NAME}Result { std::vector<double> macd; std::vector<double> signal; std::vector<double> histogram; };\n",
    "stoch": "struct Ind{NAME}Result { std::vector<double> k; std::vector<double> d; };\n",
    "kdj": "struct Ind{NAME}Result { std::vector<double> k; std::vector<double> d; std::vector<double> j; };\n",
    "gator": "struct Ind{NAME}Result { std::vector<double> upper; std::vector<double> lower; };\n",
    "sine": "struct Ind{NAME}Result { std::vector<double> sine; std::vector<double> lead_sine; };\n",
}

LINE_INFO = {
    "single": ([("value", "Srgb{0, 100, 200}")], "lines_.push_back({FN}_result);"),
    "macd": ([("macd", "Srgb{0, 100, 200}"), ("signal", "Srgb{200, 30, 30}"), ("histogram", "Srgb{0, 150, 0}")], None),
    "stoch": ([("k", "Srgb{0, 100, 200}"), ("d", "Srgb{200, 30, 30}")], None),
    "kdj": ([("k", "Srgb{0, 100, 200}"), ("d", "Srgb{200, 30, 30}"), ("j", "Srgb{0, 150, 0}")], None),
    "gator": ([("upper", "Srgb{0, 150, 0}"), ("lower", "Srgb{200, 30, 30}")], None),
    "sine": ([("sine", "Srgb{0, 100, 200}"), ("lead_sine", "Srgb{200, 30, 30}")], None),
}

def compute_call(spec):
    fn = spec["fn"]
    names = param_names(spec["ctor"])
    args = ", ".join(names)
    if is_ohlc(spec):
        return "xframe::%s(series%s)" % (fn, (", " + args) if args else "")
    return "xframe::%s(xframe::series_closes(series)%s)" % (fn, (", " + args) if args else "")

def widget_lines_code(spec):
    kind = spec["kind"]
    fn = spec["fn"]
    call = compute_call(spec)
    if kind == "single":
        return ("        std::vector<double> result = %s;\n"
                "        lines_.push_back(result);\n"
                "        line_names_.push_back(\"%s\");\n"
                "        line_colors_.push_back(Srgb{0, 100, 200});\n" % (call, spec["name"]))
    members = {"macd": ["macd", "signal", "histogram"],
               "stoch": ["k", "d"], "kdj": ["k", "d", "j"],
               "gator": ["upper", "lower"], "sine": ["sine", "lead_sine"]}[kind]
    colors = {"macd": "Srgb{0, 100, 200}", "signal": "Srgb{200, 30, 30}",
              "histogram": "Srgb{0, 150, 0}", "k": "Srgb{0, 100, 200}",
              "d": "Srgb{200, 30, 30}", "j": "Srgb{0, 150, 0}",
              "upper": "Srgb{0, 150, 0}", "lower": "Srgb{200, 30, 30}",
              "sine": "Srgb{0, 100, 200}", "lead_sine": "Srgb{200, 30, 30}"}
    code = "        xframe::Ind%sResult result = %s;\n" % (spec["name"], call)
    for m in members:
        code += ("        lines_.push_back(result.%s);\n"
                 "        line_names_.push_back(\"%s\");\n"
                 "        line_colors_.push_back(%s);\n" % (m, m, colors[m]))
    return code

def header_text(spec):
    name = spec["name"]
    fn = spec["fn"]
    helper = HELPER.format(FN=fn)
    struct = STRUCTS.get(spec["kind"], "").replace("{NAME}", name)
    ret = "Ind%sResult" % name if spec["kind"] != "single" else "std::vector<double>"
    body = spec["body"]
    # silence unused params (e.g. dummy, KDJ smooth) explicitly
    for pn in param_names(spec["ctor"]):
        if pn not in body and pn not in ("dummy",):
            body = "(void)%s;\n    " % pn + body
        elif pn == "dummy" and "dummy" not in body:
            body = "(void)dummy;\n    " + body
    sig = signature(spec)
    lines_code = widget_lines_code(spec)
    style = spec["style"]
    return """#pragma once

// Ind%(name)s.h -- %(name)s momentum indicator widget (XFrame Stock Market section).
// Formula: %(formula)s
// Output: output length == input length; leading warm-up bars are NaN until enough bars exist.
// Draw style: %(style)s.
// Attribution: XFrame.

#include "core/xframe_core.h"
#include "../StockData.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <string>
#include <vector>

namespace xframe {
%(helper)s
%(struct)s%(ret)s %(fn)s(%(sig)s)
{
    %(body)s
}

}  // namespace xframe

// Ind%(name)s widget: follows the XGraph pattern (Display*/screen_/window_/GC,
// geometry, on_click-style interaction). Constructor takes an OHLCVSeries plus
// indicator parameters and draws the indicator (%(style)s style).
class Ind%(name)s
{
private:
    Display* display_;
    int screen_;
    Window window_;
    GC gc_;
    SStyle_Manager* styles_;
    int x1_, y1_, x2_, y2_;
    std::string name_;
    std::string text_;
    std::string style_name_;
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    xframe::OHLCVSeries series_;
    std::vector<std::vector<double> > lines_;
    std::vector<std::string> line_names_;
    std::vector<Srgb> line_colors_;
    double view_first_ = 0.0;
    double view_count_ = 0.0;
    bool cross_valid_ = false;
    int cross_x_ = 0;
    int cross_y_ = 0;

public:
    Ind%(name)s(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
            int x, int y, int width, int height, const xframe::OHLCVSeries& series, %(ctor)s)
    {
        display_ = display;
        screen_ = screen;
        window_ = window;
        gc_ = gc;
        styles_ = styles;
        x1_ = x;
        y1_ = y;
        x2_ = x + width;
        y2_ = y + height;
        name_ = "Ind%(name)s";
        text_ = "%(name)s";
        series_ = series;
%(lines)s
        view_count_ = (double)item_count();
    }

    ~Ind%(name)s() {}

    void set_bounds(int x, int y, int width, int height) { x1_ = x; y1_ = y; x2_ = x + width; y2_ = y + height; }
    int width() { return x2_ - x1_; }
    int height() { return y2_ - y1_; }
    bool contains(int x, int y) { return visible_ && x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_; }
    void set_visible(bool v) { visible_ = v; }
    bool get_visible() { return visible_; }
    void set_enabled(bool e) { enabled_ = e; }
    bool get_enabled() { return enabled_; }
    void set_style_name(std::string s) { style_name_ = s; }
    void set_text(std::string t) { text_ = t; }
    std::string get_text() { return text_; }
    SStyle style() { return styles_->resolve(style_name_); }
    int item_count() { return (int)series_.size(); }
    int line_count() { return (int)lines_.size(); }
    const std::vector<double>& line_values(int i) { return lines_.at((std::size_t)i); }
    const std::vector<std::vector<double> >& values() { return lines_; }

    void reset_view() { view_first_ = 0.0; view_count_ = (double)item_count(); }
    double get_view_first() { return view_first_; }
    double get_view_count() { return view_count_ > 0 ? view_count_ : (double)item_count(); }

    void draw()
    {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long grid = styles_->get_color_rgb(225, 225, 225);
        unsigned long axis_col = styles_->get_color_rgb(150, 150, 150);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        int px = x1_ + 56, py = y1_ + 6;
        int pw = width() - 64, ph = height() - 32;
        if (pw < 10 || ph < 10) return;
        double lo = 1e300, hi = -1e300;
        int total = item_count();
        int i0 = (int)std::floor(view_first_);
        int i1 = (int)std::ceil(view_first_ + get_view_count());
        if (i0 < 0) i0 = 0;
        if (i1 > total) i1 = total;
%(overlay)s
        for (auto& line : lines_) {
            for (int i = i0; i < i1; ++i) {
                if (i >= 0 && i < (int)line.size() && std::isfinite(line[(std::size_t)i])) {
                    lo = std::min(lo, line[(std::size_t)i]);
                    hi = std::max(hi, line[(std::size_t)i]);
                }
            }
        }
        if (lo > hi) { lo = 0.0; hi = 1.0; }
        if (hi - lo < 1e-12) { hi = lo + 1.0; }
        double pad = (hi - lo) * 0.06 + 1e-9;
        lo -= pad;
        hi += pad;
        auto yat = [&](double v) { return py + (int)((hi - v) / (hi - lo) * ph); };
        auto xat = [&](int i) { return px + (int)(((double)i - view_first_) / get_view_count() * pw); };
        for (int g = 0; g <= 4; ++g) {
            double v = lo + (hi - lo) * g / 4.0;
            int gy = yat(v);
            XSetForeground(display_, gc_, grid);
            XDrawLine(display_, window_, gc_, px, gy, px + pw, gy);
            std::string lab = std::to_string((int)std::round(v));
            xf::draw_text_at(display_, window_, gc_, st, px - 52, gy + 4, lab);
        }
        if (lo < 0.0 && hi > 0.0) {
            XSetForeground(display_, gc_, axis_col);
            XDrawLine(display_, window_, gc_, px, yat(0.0), px + pw, yat(0.0));
        }
        for (std::size_t li = 0; li < lines_.size(); ++li) {
            XSetForeground(display_, gc_, styles_->get_color_Srgb(line_colors_[li]));
            xf::set_line_width(display_, gc_, 2);
            for (int i = i0 + 1; i < i1; ++i) {
                if (i < 1 || i >= (int)lines_[li].size()) continue;
                double a = lines_[li][(std::size_t)(i - 1)], b = lines_[li][(std::size_t)i];
                if (!std::isfinite(a) || !std::isfinite(b)) continue;
                XDrawLine(display_, window_, gc_, xat(i - 1), yat(a), xat(i), yat(b));
            }
            xf::set_line_width(display_, gc_, 1);
        }
        XSetForeground(display_, gc_, axis_col);
        XDrawRectangle(display_, window_, gc_, px, py, pw, ph);
        {
            int lx = px + 8, ly = py + 8;
            for (std::size_t li = 0; li < lines_.size(); ++li) {
                XSetForeground(display_, gc_, styles_->get_color_Srgb(line_colors_[li]));
                XDrawLine(display_, window_, gc_, lx, ly + 4, lx + 18, ly + 4);
                std::string last;
                for (int i = (int)lines_[li].size() - 1; i >= 0; --i) {
                    if (std::isfinite(lines_[li][(std::size_t)i])) { last = std::to_string(lines_[li][(std::size_t)i]); break; }
                }
                xf::draw_text_at(display_, window_, gc_, st, lx + 24, ly + 8, line_names_[li] + (last.empty() ? "" : " " + last));
                ly += 16;
            }
            xf::draw_text_at(display_, window_, gc_, st, px + pw - 120, py + ph + 16, text_);
        }
        if (cross_valid_ && cross_x_ >= px && cross_x_ <= px + pw && cross_y_ >= py && cross_y_ <= py + ph) {
            XSetForeground(display_, gc_, axis_col);
            XDrawLine(display_, window_, gc_, cross_x_, py, cross_x_, py + ph);
            XDrawLine(display_, window_, gc_, px, cross_y_, px + pw, cross_y_);
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }

    bool on_button_press(int x, int y, unsigned int button)
    {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        if (button == 4) { view_count_ = std::max(5.0, get_view_count() * 0.8); return true; }
        if (button == 5) { view_count_ = std::min((double)item_count(), get_view_count() * 1.25); return true; }
        if (button == 1) return true;
        return false;
    }

    bool on_button_release(int x, int y, unsigned int button)
    {
        (void)x; (void)y;
        return button == 1 && focused_;
    }

    bool on_motion(int x, int y)
    {
        cross_valid_ = true;
        cross_x_ = x;
        cross_y_ = y;
        return contains(x, y);
    }

    bool on_key(KeySym key, const char* str)
    {
        (void)str;
        if (!focused_ || !enabled_) return false;
        double total = (double)item_count();
        double count = get_view_count();
        switch (key) {
        case XK_Left:
            view_first_ = std::max(0.0, view_first_ - count * 0.1);
            return true;
        case XK_Right:
            view_first_ = std::min(std::max(0.0, total - count), view_first_ + count * 0.1);
            return true;
        case XK_Home:
        case XK_r:
        case XK_R:
            reset_view();
            return true;
        }
        return false;
    }
};
""" % dict(name=name, fn=fn, helper=helper, struct=struct, ret=ret, sig=sig,
            body=body, style=style, formula=spec["formula"], ctor=spec["ctor"],
            lines=lines_code,
            overlay=OVERLAY_SNIPPET if style == "overlay" else "")

OVERLAY_SNIPPET = """        // Overlay style: light candle background, then the indicator line(s).
        {
            double plo = 1e300, phi = -1e300;
            for (int i = i0; i < i1; ++i) {
                plo = std::min(plo, series_[(std::size_t)i].low);
                phi = std::max(phi, series_[(std::size_t)i].high);
            }
            if (plo <= phi) {
                double prange = phi - plo < 1e-12 ? 1.0 : phi - plo;
                for (int i = i0; i < i1; ++i) {
                    const xframe::OHLCV& b = series_[(std::size_t)i];
                    int cx = px + (int)(((double)i - view_first_ + 0.5) / get_view_count() * pw);
                    int bw = std::max(1, (int)(pw / std::max(1.0, get_view_count()) * 0.6));
                    unsigned long c = styles_->get_color_rgb(180, 180, 180);
                    XSetForeground(display_, gc_, c);
                    int top = py + (int)((phi - std::max(b.open, b.close)) / prange * ph);
                    int bot = py + (int)((phi - std::min(b.open, b.close)) / prange * ph);
                    XFillRectangle(display_, window_, gc_, cx - bw / 2, top, bw, std::max(1, bot - top));
                }
            }
        }
"""

def main_text(spec):
    name = spec["name"]
    return """#include "Ind%(name)s.h"

#include <cstdio>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "Ind%(name)s - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask |
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60},
                        Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    xframe::OHLCVSeries series = xframe::make_seeded_series(250);
    Ind%(name)s widget(display, screen, window, gc, &styles, 20, 20, 760, 520, series);
    widget.draw();
    widget.on_button_press(100, 100, 1);
    widget.on_button_release(100, 100, 1);
    widget.on_motion(200, 200);
    widget.on_key(XK_Right, "");
    widget.on_key(XK_Home, "");
    widget.draw();
    XFlush(display);
    XSync(display, False);
    XCloseDisplay(display);
    return 0;
}
""" % dict(name=name)

def cmake_text(spec):
    name = spec["name"]
    return """cmake_minimum_required(VERSION 3.20)

project(Ind%(name)s LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(Ind%(name)s main_Ind%(name)s.cpp)

target_link_libraries(Ind%(name)s PRIVATE X11::X11)
""" % dict(name=name)

def write_file(path, text):
    with open(path, "w") as f:
        f.write(text)

def emit_all():
    import shutil
    names = [sp["name"] for sp in S]
    assert len(names) == 85, "expected 85 indicators, got %d" % len(names)
    assert len(set(names)) == 85, "duplicate indicator names"
    # Collision guard (Nov 26 ~12:35 NZDT, momentum recovery): never emit a name
    # claimed by the trend or volatility groups. Volatility names are compared
    # with their "Ind" prefix stripped. Refuse (exit non-zero) instead of
    # overwriting another group's dir.
    for lf in ("stock_indicators_trend.txt", "stock_indicators_volatility.txt"):
        lp = os.path.join(TOOLS, lf)
        if os.path.exists(lp):
            other = set()
            for ln in open(lp):
                if ln.strip():
                    n = ln.split("|")[0].strip()
                    if n.startswith("Ind"):
                        n = n[3:]
                    other.add(n)
            bad = sorted(set(names) & other)
            if bad:
                print("COLLISION with %s: %s -- refusing to emit" % (lf, ", ".join(bad)))
                sys.exit(2)
    # Ownership guard: a dir whose header is not ours belongs to another group.
    # Never write into it; skip it and report it instead of overwriting.
    blocked = []
    for spec in S:
        hp = os.path.join(COMP, "Ind" + spec["name"], "Ind" + spec["name"] + ".h")
        if os.path.exists(hp):
            with open(hp) as f:
                if "momentum indicator widget" not in f.read():
                    blocked.append(spec["name"])
    blocked_set = set(blocked)
    core_src = os.path.join(REPO, "widgets", "core", "xframe_core.h")
    with open(core_src, "rb") as f:
        core_bytes = f.read()
    for spec in S:
        if spec["name"] in blocked_set:
            continue
        d = os.path.join(COMP, "Ind" + spec["name"])
        os.makedirs(os.path.join(d, "core"), exist_ok=True)
        write_file(os.path.join(d, "Ind" + spec["name"] + ".h"), header_text(spec))
        write_file(os.path.join(d, "main_Ind" + spec["name"] + ".cpp"), main_text(spec))
        write_file(os.path.join(d, "CMakeLists.txt"), cmake_text(spec))
        with open(os.path.join(d, "core", "xframe_core.h"), "wb") as f:
            f.write(core_bytes)
    # list file
    lines = []
    for spec in S:
        lines.append("%s | momentum | %s" % (spec["name"], spec["formula"]))
    write_file(os.path.join(TOOLS, "stock_indicators_momentum.txt"), "\n".join(lines) + "\n")
    # doc
    doc = ["# Stock Market indicators: momentum (85)\n",
           "\nMomentum indicators of the XFrame Stock Market section. Each entry: name, formula, API, a short working example, and honest notes where the header does not settle a behaviour.\n",
           "\nShared conventions: output length == input length; leading warm-up bars are NaN until enough bars exist. All demos and tests use `xframe::make_seeded_series(250)` (deterministic synthetic data, no network, no live market data).\n"]
    for spec in S:
        ret = "Ind%sResult" % spec["name"] if spec["kind"] != "single" else "std::vector<double>"
        doc.append("\n## %s\n\n" % spec["name"])
        doc.append("- Formula: %s\n" % spec["formula"])
        doc.append("- Compute: `%s %s(%s)`\n" % (ret, spec["fn"], signature(spec)))
        doc.append("- Widget: `Ind%s(Display*, int screen, Window, GC, SStyle_Manager*, int x, int y, int width, int height, const xframe::OHLCVSeries& series, %s)`; methods: `draw()`, `on_button_press(x, y, button)`, `on_button_release(x, y, button)`, `on_motion(x, y)`, `on_key(key, str)`, `line_values(i)`, `values()`, `item_count()`, `line_count()`, `reset_view()`.\n" % (spec["name"], spec["ctor"]))
        doc.append("- Draw style: %s.\n" % spec["style"])
        doc.append("- Example:\n\n```cpp\n#include \"Ind%s.h\"\nauto series = xframe::make_seeded_series(250);\n" % spec["name"])
        if is_ohlc(spec):
            doc.append("auto out = xframe::%s(series);\n" % spec["fn"])
        else:
            doc.append("auto out = xframe::%s(xframe::series_closes(series));\n" % spec["fn"])
        doc.append("Ind%s w(display, screen, window, gc, &styles, 20, 20, 760, 520, series);\nw.draw();\n```\n" % spec["name"])
        if "Unclear from the header" in spec["formula"]:
            doc.append("- Unclear from the header: see the formula note above; the implementation choice is documented there.\n")
        if spec["name"] in blocked_set:
            doc.append("- Blocked: this directory currently holds another group's version; not regenerated by this generator.\n")
    write_file(os.path.join(REPO, "docs", "stock-market-indicators-momentum.md"), "".join(doc))
    # smoke harness
    smoke = ["// Smoke harness for the 85 momentum indicators. Generated by gen_indicators_momentum.py.\n",
             "// Blocked (another group's dirs, not tested here): %s\n" % ", ".join(blocked),
             "#include <cmath>\n#include <cstdio>\n#include <limits>\n#include <vector>\n"]
    for spec in S:
        if spec["name"] in blocked_set:
            continue
        smoke.append("#include \"Ind%s/Ind%s.h\"\n" % (spec["name"], spec["name"]))
    smoke.append("""
static int failures = 0;
static void check_vec(const char* name, const std::vector<double>& v, int n) {
    bool ok = (int)v.size() == n;
    bool seen_finite = false, nan_after_finite = false;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (std::isfinite(v[i])) { seen_finite = true; if (nan_after_finite) ok = false; }
        else if (std::isnan(v[i])) { if (seen_finite) nan_after_finite = true; }
        else ok = false;
    }
    if (!seen_finite) ok = false;
    std::printf("%s %s\\n", ok ? "PASS" : "FAIL", name);
    if (!ok) ++failures;
}
int main() {
    xframe::OHLCVSeries s = xframe::make_seeded_series(250);
""")
    for spec in S:
        if spec["name"] in blocked_set:
            continue
        kind = spec["kind"]
        call = spec["call"]
        if kind == "single":
            smoke.append("    { auto v = xframe::%s; check_vec(\"%s\", v, 250); }\n" % (call, spec["name"]))
        else:
            members = {"macd": ["macd", "signal", "histogram"], "stoch": ["k", "d"],
                       "kdj": ["k", "d", "j"], "gator": ["upper", "lower"],
                       "sine": ["sine", "lead_sine"]}[kind]
            smoke.append("    { auto r = xframe::%s;\n" % call)
            for m in members:
                smoke.append("      check_vec(\"%s.%s\", r.%s, 250);\n" % (spec["name"], m, m))
            smoke.append("    }\n")
    smoke.append("""
    // Hand check 1: RSI on the StockCharts Wilder example data; first RSI(14) is 70.46.
    {
        std::vector<double> c = {44.34, 44.09, 44.15, 43.61, 44.33, 44.83, 45.10, 45.42, 45.84, 46.08,
                                 45.89, 46.03, 45.61, 46.28, 46.28, 46.00, 46.03, 46.41, 46.22, 46.76};
        auto r = xframe::ind_rsi(c, 14);
        bool ok = std::isfinite(r[14]) && std::abs(r[14] - 70.46) < 0.5;
        std::printf("%s HANDCHECK RSI\\n", ok ? "PASS" : "FAIL");
        if (!ok) { ++failures; std::printf("  rsi[14]=%f expected 70.46\\n", r[14]); }
    }
    // Hand check 2: MACD on a constant series is exactly zero everywhere it is defined.
    {
        std::vector<double> c(50, 100.0);
        auto r = xframe::ind_macd(c, 12, 26, 9);
        bool ok = true;
        for (std::size_t i = 0; i < c.size(); ++i) {
            if (std::isfinite(r.macd[i]) && std::abs(r.macd[i]) > 1e-9) ok = false;
            if (std::isfinite(r.signal[i]) && std::abs(r.signal[i]) > 1e-9) ok = false;
            if (std::isfinite(r.histogram[i]) && std::abs(r.histogram[i]) > 1e-9) ok = false;
        }
        std::printf("%s HANDCHECK MACD-CONSTANT\\n", ok ? "PASS" : "FAIL");
        if (!ok) ++failures;
    }
    // Hand check 3: ROC(1) on {100, 110, 121} is 10.0 at index 2.
    {
        std::vector<double> c = {100.0, 110.0, 121.0};
        auto r = xframe::ind_roc(c, 1);
        bool ok = std::isfinite(r[2]) && std::abs(r[2] - 10.0) < 1e-9;
        std::printf("%s HANDCHECK ROC\\n", ok ? "PASS" : "FAIL");
        if (!ok) ++failures;
    }
    std::printf("SMOKE failures=%d\\n", failures);
    return failures ? 1 : 0;
}
""")
    os.makedirs(os.path.join(ROOT, "tests"), exist_ok=True)
    write_file(os.path.join(ROOT, "tests", "smoke_momentum_indicators.cpp"), "".join(smoke))
    print("generated %d indicators" % len(S))
    if blocked:
        print("BLOCKED (another group's dirs, skipped, not overwritten): %s" % ", ".join(blocked))
    return blocked

if __name__ == "__main__":
    sys.exit(2 if emit_all() else 0)
