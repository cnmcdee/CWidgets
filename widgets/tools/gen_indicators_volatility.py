#!/usr/bin/env python3
# gen_indicators_volatility.py -- regenerable generator for the XFrame Stock
# Market VOLATILITY + VOLUME + OTHER indicator widgets (80 widgets).
# Attribution: XFrame. No network, no live data: seeded synthetic data only.
# Usage: python3 gen_indicators_volatility.py   (run from /opt/cwidgets)
import json, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
COMP = os.path.join(ROOT, "widgets", "components")
TOOLS = os.path.join(ROOT, "widgets", "tools")

HELPERS = r'''
namespace detail_%(L)s {
inline double nanv() { return std::numeric_limits<double>::quiet_NaN(); }
inline bool fin(double v) { return std::isfinite(v); }
inline std::vector<double> sma(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (p <= 1) return v;
    double s = 0.0;
    for (size_t i = 0; i < v.size(); ++i) {
        s += v[i];
        if (i >= (size_t)p) s -= v[i - (size_t)p];
        if (i >= (size_t)(p - 1)) o[i] = s / p;
    }
    return o;
}
inline std::vector<double> ema(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (v.empty()) return o;
    double k = 2.0 / (p + 1), e = v[0];
    for (size_t i = 0; i < v.size(); ++i) {
        e = (i == 0) ? v[0] : e + k * (v[i] - e);
        if (i >= (size_t)(p - 1)) o[i] = e;
    }
    return o;
}
inline std::vector<double> ema_wilder(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    if (v.empty()) return o;
    double s = 0.0; double e = 0.0;
    for (size_t i = 0; i < v.size(); ++i) {
        s += v[i];
        if (i == (size_t)(p - 1)) e = s / p;
        else if (i >= (size_t)p) e = e + (v[i] - e) / p;
        if (i >= (size_t)(p - 1)) o[i] = e;
    }
    return o;
}
inline std::vector<double> stddev(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    std::vector<double> m = sma(v, p);
    std::vector<double> sq(v.size());
    for (size_t i = 0; i < v.size(); ++i) sq[i] = v[i] * v[i];
    std::vector<double> ms = sma(sq, p);
    for (size_t i = 0; i < v.size(); ++i)
        if (fin(m[i])) { double var = ms[i] - m[i] * m[i]; o[i] = std::sqrt(var > 0 ? var : 0.0); }
    return o;
}
inline std::vector<double> tr(const OHLCVSeries& s) {
    std::vector<double> o(s.size(), nanv());
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 0) o[i] = s[i].high - s[i].low;
        else o[i] = std::max({s[i].high - s[i].low, std::fabs(s[i].high - s[i - 1].close),
                              std::fabs(s[i].low - s[i - 1].close)});
    }
    return o;
}
inline std::vector<double> atr(const OHLCVSeries& s, int p) {
    return ema_wilder(tr(s), p);
}
inline std::vector<double> hh(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    for (size_t i = 0; i < v.size(); ++i) {
        if (i < (size_t)(p - 1)) continue;
        double m = v[i];
        for (int k = 1; k < p; ++k) m = std::max(m, v[i - (size_t)k]);
        o[i] = m;
    }
    return o;
}
inline std::vector<double> ll(const std::vector<double>& v, int p) {
    std::vector<double> o(v.size(), nanv());
    for (size_t i = 0; i < v.size(); ++i) {
        if (i < (size_t)(p - 1)) continue;
        double m = v[i];
        for (int k = 1; k < p; ++k) m = std::min(m, v[i - (size_t)k]);
        o[i] = m;
    }
    return o;
}
inline std::vector<double> logret(const std::vector<double>& c) {
    std::vector<double> o(c.size(), 0.0);
    for (size_t i = 1; i < c.size(); ++i)
        if (c[i] > 0 && c[i - 1] > 0) o[i] = std::log(c[i] / c[i - 1]);
    return o;
}
}  // namespace detail_%(L)s
'''

def S(name, cat, style, func, params, ret, outputs, body, formula, warmup):
    return dict(name=name, cat=cat, style=style, func=func, params=params,
                ret=ret, outputs=outputs, body=body, formula=formula, warmup=warmup)

SPEC = []
# ---------------- VOLATILITY (28) ----------------
SPEC.append(S("BollingerBands", "volatility", "overlay", "ind_bollinger_bands",
    "int period = 20, double mult = 2.0", "BollingerBandsResult",
    [("upper", "Upper", 200, 30, 30), ("middle", "Middle", 0, 100, 200), ("lower", "Lower", 200, 30, 30)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, period);
    std::vector<double> sd = stddev(c, period);
    BollingerBandsResult r;
    r.upper.assign(series.size(), nanv()); r.middle = m; r.lower.assign(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(m[i])) { r.upper[i] = m[i] + mult * sd[i]; r.lower[i] = m[i] - mult * sd[i]; }
    return r;''',
    "Middle = SMA(close, 20); Upper = Middle + 2 * StdDev(close, 20); Lower = Middle - 2 * StdDev(close, 20).", 19))
SPEC.append(S("BollingerPercentB", "volatility", "pane", "ind_bollinger_percent_b",
    "int period = 20, double mult = 2.0", "vec",
    [("", "%B", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, period);
    std::vector<double> sd = stddev(c, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(m[i])) {
            double up = m[i] + mult * sd[i], lo = m[i] - mult * sd[i];
            if (up != lo) o[i] = (series[i].close - lo) / (up - lo);
        }
    return o;''',
    "%B = (close - Lower Band) / (Upper Band - Lower Band); 1.0 at the upper band, 0.0 at the lower band.", 19))
SPEC.append(S("BollingerBandwidth", "volatility", "pane", "ind_bollinger_bandwidth",
    "int period = 20, double mult = 2.0", "vec",
    [("", "Bandwidth", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, period);
    std::vector<double> sd = stddev(c, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(m[i]) && m[i] != 0.0) o[i] = (2.0 * mult * sd[i]) / m[i];
    return o;''',
    "Bandwidth = (Upper Band - Lower Band) / Middle Band.", 19))
SPEC.append(S("ATR", "volatility", "pane", "ind_atr",
    "int period = 14", "vec",
    [("", "ATR", 200, 120, 0)],
    r'''return atr(series, period);''',
    "True Range = max(high-low, |high-prev close|, |low-prev close|); ATR = Wilder-smoothed (1/14) moving average of True Range.", 13))
SPEC.append(S("NATR", "volatility", "pane", "ind_natr",
    "int period = 14", "vec",
    [("", "NATR", 200, 120, 0)],
    r'''std::vector<double> a = atr(series, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(a[i]) && series[i].close != 0.0) o[i] = 100.0 * a[i] / series[i].close;
    return o;''',
    "NATR = 100 * ATR(14) / close.", 13))
SPEC.append(S("TrueRange", "volatility", "pane", "ind_true_range",
    "int period = 1", "vec",
    [("", "TR", 200, 120, 0)],
    r'''(void)period; return tr(series);''',
    "True Range = max(high - low, |high - previous close|, |low - previous close|); first bar is high - low.", 0))
SPEC.append(S("KeltnerChannels", "volatility", "overlay", "ind_keltner_channels",
    "int period = 20, double mult = 2.0", "KeltnerChannelsResult",
    [("upper", "Upper", 200, 30, 30), ("middle", "Middle", 0, 100, 200), ("lower", "Lower", 200, 30, 30)],
    r'''std::vector<double> tp = series_typical(series);
    std::vector<double> m = ema(tp, period);
    std::vector<double> a = atr(series, period);
    KeltnerChannelsResult r;
    r.upper.assign(series.size(), nanv()); r.middle = m; r.lower.assign(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(m[i]) && fin(a[i])) { r.upper[i] = m[i] + mult * a[i]; r.lower[i] = m[i] - mult * a[i]; }
    return r;''',
    "Middle = EMA(typical price, 20); Upper = Middle + 2 * ATR(20); Lower = Middle - 2 * ATR(20). Typical price = (high + low + close) / 3.", 19))
SPEC.append(S("DonchianChannels", "volatility", "overlay", "ind_donchian_channels",
    "int period = 20", "DonchianChannelsResult",
    [("upper", "Upper", 200, 30, 30), ("middle", "Middle", 0, 100, 200), ("lower", "Lower", 200, 30, 30)],
    r'''std::vector<double> h = series_highs(series), l = series_lows(series);
    DonchianChannelsResult r;
    r.upper = hh(h, period); r.lower = ll(l, period);
    r.middle.assign(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(r.upper[i])) r.middle[i] = (r.upper[i] + r.lower[i]) / 2.0;
    return r;''',
    "Upper = highest high of the last 20 bars; Lower = lowest low of the last 20 bars; Middle = (Upper + Lower) / 2.", 19))
SPEC.append(S("ChaikinVolatility", "volatility", "pane", "ind_chaikin_volatility",
    "int period = 10", "vec",
    [("", "ChaikinVol", 150, 0, 150)],
    r'''std::vector<double> hl(series.size());
    for (size_t i = 0; i < series.size(); ++i) hl[i] = series[i].high - series[i].low;
    std::vector<double> e = ema(hl, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = (size_t)period; i < series.size(); ++i)
        if (fin(e[i]) && fin(e[i - (size_t)period]) && e[i - (size_t)period] != 0.0)
            o[i] = 100.0 * (e[i] - e[i - (size_t)period]) / e[i - (size_t)period];
    return o;''',
    "Chaikin Volatility = 100 * (EMA(high-low, 10) - EMA(high-low, 10) 10 bars ago) / EMA(high-low, 10) 10 bars ago.", 19))
SPEC.append(S("HistoricalVolatility", "volatility", "pane", "ind_historical_volatility",
    "int period = 20", "vec",
    [("", "HistVol", 150, 0, 150)],
    r'''std::vector<double> lr = logret(series_closes(series));
    std::vector<double> sd = stddev(lr, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(sd[i])) o[i] = 100.0 * sd[i] * std::sqrt(252.0);
    return o;''',
    "Historical Volatility = 100 * StdDev(log(close / previous close), 20) * sqrt(252) (annualised, percent).", 20))
SPEC.append(S("ParkinsonVolatility", "volatility", "pane", "ind_parkinson_volatility",
    "int period = 20", "vec",
    [("", "Parkinson", 150, 0, 150)],
    r'''std::vector<double> o(series.size(), nanv());
    const double k = 1.0 / (4.0 * std::log(2.0));
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double s = 0.0; bool ok = true;
        for (int j = 0; j < period; ++j) {
            const OHLCV& b = series[i - (size_t)j];
            if (b.high <= 0 || b.low <= 0) { ok = false; break; }
            double lh = std::log(b.high / b.low); s += lh * lh;
        }
        if (ok) o[i] = 100.0 * std::sqrt(k * s / period) * std::sqrt(252.0);
    }
    return o;''',
    "Parkinson volatility = 100 * sqrt( (1 / (4 ln 2 * n)) * sum(ln(high/low)^2) ) * sqrt(252), over 20 bars.", 19))
SPEC.append(S("GarmanKlassVolatility", "volatility", "pane", "ind_garman_klass_volatility",
    "int period = 20", "vec",
    [("", "GarmanKlass", 150, 0, 150)],
    r'''std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double s = 0.0; bool ok = true;
        for (int j = 0; j < period; ++j) {
            const OHLCV& b = series[i - (size_t)j];
            if (b.high <= 0 || b.low <= 0 || b.open <= 0 || b.close <= 0) { ok = false; break; }
            double lhl = std::log(b.high / b.low), lco = std::log(b.close / b.open);
            s += 0.5 * lhl * lhl - (2.0 * std::log(2.0) - 1.0) * lco * lco;
        }
        if (ok && s > 0) o[i] = 100.0 * std::sqrt(s / period) * std::sqrt(252.0);
        else if (ok) o[i] = 0.0;
    }
    return o;''',
    "Garman-Klass: per bar 0.5*ln(H/L)^2 - (2 ln 2 - 1)*ln(C/O)^2, averaged over 20 bars, square-rooted, annualised by sqrt(252), in percent.", 19))
SPEC.append(S("RogersSatchellVolatility", "volatility", "pane", "ind_rogers_satchell_volatility",
    "int period = 20", "vec",
    [("", "RogersSatchell", 150, 0, 150)],
    r'''std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double s = 0.0; bool ok = true;
        for (int j = 0; j < period; ++j) {
            const OHLCV& b = series[i - (size_t)j];
            if (b.high <= 0 || b.low <= 0 || b.open <= 0 || b.close <= 0) { ok = false; break; }
            s += std::log(b.high / b.close) * std::log(b.high / b.open)
               + std::log(b.low / b.close) * std::log(b.low / b.open);
        }
        if (ok && s > 0) o[i] = 100.0 * std::sqrt(s / period) * std::sqrt(252.0);
        else if (ok) o[i] = 0.0;
    }
    return o;''',
    "Rogers-Satchell: per bar ln(H/C)*ln(H/O) + ln(L/C)*ln(L/O), averaged over 20 bars, square-rooted, annualised by sqrt(252), in percent.", 19))
SPEC.append(S("YangZhangVolatility", "volatility", "pane", "ind_yang_zhang_volatility",
    "int period = 20", "vec",
    [("", "YangZhang", 150, 0, 150)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o = series_opens(series);
    std::vector<double> lr_oc(series.size(), nanv()), lr_co(series.size(), nanv());
    for (size_t i = 1; i < series.size(); ++i) {
        if (o[i] > 0 && c[i - 1] > 0) lr_oc[i] = std::log(o[i] / c[i - 1]);
        if (c[i] > 0 && o[i] > 0) lr_co[i] = std::log(c[i] / o[i]);
    }
    std::vector<double> out(series.size(), nanv());
    const double k = 0.34 / (1.34 + (period + 1.0) / (period - 1.0));
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)period) continue;
        double so = 0.0, sc = 0.0, mo = 0.0, mc = 0.0; int n = 0;
        for (int j = 0; j < period; ++j) {
            size_t idx = i - (size_t)j;
            if (!fin(lr_oc[idx]) || !fin(lr_co[idx])) continue;
            mo += lr_oc[idx]; mc += lr_co[idx]; ++n;
        }
        if (n < period) continue;
        mo /= n; mc /= n;
        for (int j = 0; j < period; ++j) {
            size_t idx = i - (size_t)j;
            so += (lr_oc[idx] - mo) * (lr_oc[idx] - mo);
            sc += (lr_co[idx] - mc) * (lr_co[idx] - mc);
        }
        double rs = 0.0; bool ok = true;
        for (int j = 0; j < period; ++j) {
            const OHLCV& b = series[i - (size_t)j];
            if (b.high <= 0 || b.low <= 0 || b.open <= 0 || b.close <= 0) { ok = false; break; }
            rs += std::log(b.high / b.close) * std::log(b.high / b.open)
                + std::log(b.low / b.close) * std::log(b.low / b.open);
        }
        if (!ok) continue;
        double var = so / (period - 1) + k * sc / (period - 1) + (1.0 - k) * rs / period;
        out[i] = var > 0 ? 100.0 * std::sqrt(var) * std::sqrt(252.0) : 0.0;
    }
    return out;''',
    "Yang-Zhang: overnight variance + k * open-to-close variance + (1-k) * Rogers-Satchell variance, k = 0.34 / (1.34 + (n+1)/(n-1)), square-rooted, annualised by sqrt(252), in percent.", 20))
SPEC.append(S("StdDev", "volatility", "pane", "ind_stddev",
    "int period = 20", "vec",
    [("", "StdDev", 0, 100, 200)],
    r'''return stddev(series_closes(series), period);''',
    "StdDev = population standard deviation of close over the last 20 bars.", 19))
SPEC.append(S("Variance", "volatility", "pane", "ind_variance",
    "int period = 20", "vec",
    [("", "Variance", 0, 100, 200)],
    r'''std::vector<double> sd = stddev(series_closes(series), period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) if (fin(sd[i])) o[i] = sd[i] * sd[i];
    return o;''',
    "Variance = population variance (StdDev squared) of close over the last 20 bars.", 19))
SPEC.append(S("ChandelierExit", "volatility", "overlay", "ind_chandelier_exit",
    "int period = 22, double mult = 3.0", "ChandelierExitResult",
    [("long_exit", "Long Exit", 0, 150, 0), ("short_exit", "Short Exit", 200, 30, 30)],
    r'''std::vector<double> h = series_highs(series), l = series_lows(series);
    std::vector<double> a = atr(series, period);
    ChandelierExitResult r;
    r.long_exit = hh(h, period); r.short_exit = ll(l, period);
    for (size_t i = 0; i < series.size(); ++i) {
        if (fin(r.long_exit[i]) && fin(a[i])) r.long_exit[i] = r.long_exit[i] - mult * a[i];
        if (fin(r.short_exit[i]) && fin(a[i])) r.short_exit[i] = r.short_exit[i] + mult * a[i];
    }
    return r;''',
    "Long Exit = highest high of the last 22 bars - 3 * ATR(22); Short Exit = lowest low of the last 22 bars + 3 * ATR(22).", 21))
SPEC.append(S("VolatilityStop", "volatility", "overlay", "ind_volatility_stop",
    "int period = 14, double mult = 3.0", "vec",
    [("", "VolStop", 200, 30, 30)],
    r'''std::vector<double> a = atr(series, period);
    std::vector<double> o(series.size(), nanv());
    double stop = nanv(); bool is_long = true;
    for (size_t i = 0; i < series.size(); ++i) {
        if (!fin(a[i])) continue;
        if (!fin(stop)) { stop = series[i].close - mult * a[i]; is_long = true; o[i] = stop; continue; }
        if (is_long) {
            stop = std::max(stop, series[i].close - mult * a[i]);
            if (series[i].close < stop) { is_long = false; stop = series[i].close + mult * a[i]; }
        } else {
            stop = std::min(stop, series[i].close + mult * a[i]);
            if (series[i].close > stop) { is_long = true; stop = series[i].close - mult * a[i]; }
        }
        o[i] = stop;
    }
    return o;''',
    "Wilder Volatility Stop: a trailing stop that ratchets up (long) at close - 3*ATR and flips to a short stop at close + 3*ATR when price closes through it.", 13))
SPEC.append(S("MassIndex", "volatility", "pane", "ind_mass_index",
    "int period = 25", "vec",
    [("", "MassIndex", 150, 0, 150)],
    r'''std::vector<double> hl(series.size());
    for (size_t i = 0; i < series.size(); ++i) hl[i] = series[i].high - series[i].low;
    std::vector<double> e = ema(hl, 9);
    std::vector<double> ratio(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(e[i]) && e[i] != 0.0) ratio[i] = hl[i] / e[i];
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double s = 0.0; bool ok = true;
        for (int j = 0; j < period; ++j) { if (!fin(ratio[i - (size_t)j])) { ok = false; break; } s += ratio[i - (size_t)j]; }
        if (ok) o[i] = s;
    }
    return o;''',
    "Mass Index = 25-bar sum of (high - low) / EMA(high - low, 9).", 32))
SPEC.append(S("UlcerIndex", "volatility", "pane", "ind_ulcer_index",
    "int period = 14", "vec",
    [("", "UlcerIndex", 200, 0, 0)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double mx = c[i]; for (int j = 1; j < period; ++j) mx = std::max(mx, c[i - (size_t)j]);
        double s = 0.0;
        for (int j = 0; j < period; ++j) {
            double dd = 100.0 * (c[i - (size_t)j] - mx) / mx; s += dd * dd;
        }
        o[i] = std::sqrt(s / period);
    }
    return o;''',
    "Ulcer Index = sqrt(mean of squared percentage drawdowns from the rolling 14-bar highest close).", 13))
SPEC.append(S("RelativeVolatilityIndex", "volatility", "pane", "ind_relative_volatility_index",
    "int period = 10", "vec",
    [("", "RVI", 150, 0, 150)],
    r'''std::vector<double> sd = stddev(series_closes(series), period);
    std::vector<double> up(series.size(), 0.0), dn(series.size(), 0.0);
    for (size_t i = 1; i < series.size(); ++i) {
        if (!fin(sd[i])) continue;
        if (series[i].close > series[i - 1].close) up[i] = sd[i]; else if (series[i].close < series[i - 1].close) dn[i] = sd[i];
    }
    std::vector<double> eu = ema_wilder(up, period), ed = ema_wilder(dn, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(eu[i]) && fin(ed[i]) && (eu[i] + ed[i]) != 0.0) o[i] = 100.0 * eu[i] / (eu[i] + ed[i]);
    return o;''',
    "Relative Volatility Index: Wilder-smoothed standard deviation of up closes vs down closes, combined RSI-style as 100 * up / (up + down) over 10 bars.", 19))
SPEC.append(S("TTMSqueeze", "volatility", "pane", "ind_ttm_squeeze",
    "int period = 20, double mult = 2.0", "TTMSqueezeResult",
    [("squeeze", "Squeeze", 200, 30, 30), ("ratio", "BB/KC ratio", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, period), sd = stddev(c, period), a = atr(series, period);
    TTMSqueezeResult r;
    r.squeeze.assign(series.size(), nanv()); r.ratio.assign(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (!fin(m[i]) || !fin(a[i])) continue;
        double bbw = 2.0 * mult * sd[i];
        double kcw = 2.0 * 1.5 * a[i];
        r.ratio[i] = kcw != 0.0 ? bbw / kcw : nanv();
        r.squeeze[i] = (bbw < kcw) ? 1.0 : 0.0;
    }
    return r;''',
    "TTM Squeeze: the squeeze is ON (1) when the Bollinger Bands (20, 2 StdDev) lie inside the Keltner Channels (20, 1.5 ATR); ratio = Bollinger width / Keltner width.", 19))
SPEC.append(S("GopalakrishnanRangeIndex", "volatility", "pane", "ind_gopalakrishnan_range_index",
    "int period = 20", "vec",
    [("", "GAPO", 150, 0, 150)],
    r'''std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double s = 0.0; bool ok = true;
        for (int j = 0; j < period; ++j) {
            const OHLCV& b = series[i - (size_t)j];
            if (b.high <= 0 || b.low <= 0) { ok = false; break; }
            s += std::log(b.high / b.low);
        }
        if (ok) o[i] = 100.0 * s / period;
    }
    return o;''',
    "Gopalakrishnan Range Index (GAPO) = 100 * mean of ln(high / low) over the last 20 bars.", 19))
SPEC.append(S("WilderVolatilitySystem", "volatility", "overlay", "ind_wilder_volatility_system",
    "int period = 14, double mult = 3.0", "vec",
    [("", "VolSystem", 200, 30, 30)],
    r'''std::vector<double> a = atr(series, period);
    std::vector<double> o(series.size(), nanv());
    double stop = nanv(); bool is_long = true;
    for (size_t i = 0; i < series.size(); ++i) {
        if (!fin(a[i])) continue;
        if (!fin(stop)) { stop = series[i].high - mult * a[i]; is_long = true; o[i] = stop; continue; }
        if (is_long) {
            double cand = series[i].high - mult * a[i];
            stop = std::max(stop, cand);
            if (series[i].close < stop) { is_long = false; stop = series[i].low + mult * a[i]; }
        } else {
            double cand = series[i].low + mult * a[i];
            stop = std::min(stop, cand);
            if (series[i].close > stop) { is_long = true; stop = series[i].high - mult * a[i]; }
        }
        o[i] = stop;
    }
    return o;''',
    "Wilder Volatility System stop: long stop trails the highest high minus 3*ATR and flips to a short stop at the lowest low plus 3*ATR on a close through the stop.", 13))
SPEC.append(S("VolatilityRatio", "volatility", "pane", "ind_volatility_ratio",
    "int period = 14", "vec",
    [("", "VolRatio", 150, 0, 150)],
    r'''std::vector<double> t = tr(series);
    std::vector<double> mt = sma(t, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(t[i]) && fin(mt[i]) && mt[i] != 0.0) o[i] = t[i] / mt[i];
    return o;''',
    "Volatility Ratio = True Range / SMA(True Range, 14).", 13))
SPEC.append(S("CloseVolatility", "volatility", "pane", "ind_close_volatility",
    "int period = 20", "vec",
    [("", "CloseVol", 150, 0, 150)],
    r'''std::vector<double> lr = logret(series_closes(series));
    std::vector<double> sd = stddev(lr, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(sd[i])) o[i] = 100.0 * sd[i];
    return o;''',
    "Close Volatility = 100 * StdDev(log(close / previous close), 20), unannualised, in percent.", 20))
SPEC.append(S("HighLowRange", "volatility", "pane", "ind_high_low_range",
    "int period = 20", "vec",
    [("", "HLRange", 150, 0, 150)],
    r'''std::vector<double> h = series_highs(series), l = series_lows(series), c = series_closes(series);
    std::vector<double> hhv = hh(h, period), llv = ll(l, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(hhv[i]) && c[i] != 0.0) o[i] = 100.0 * (hhv[i] - llv[i]) / c[i];
    return o;''',
    "High-Low Range = 100 * (highest high - lowest low of the last 20 bars) / close.", 19))
SPEC.append(S("KeltnerBandwidth", "volatility", "pane", "ind_keltner_bandwidth",
    "int period = 20, double mult = 2.0", "vec",
    [("", "KeltnerBW", 0, 100, 200)],
    r'''std::vector<double> tp = series_typical(series);
    std::vector<double> m = ema(tp, period);
    std::vector<double> a = atr(series, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(m[i]) && fin(a[i]) && m[i] != 0.0) o[i] = (4.0 * mult * a[i]) / m[i];
    return o;''',
    "Keltner Bandwidth = (Upper Channel - Lower Channel) / Middle Channel, with channels at EMA(typical, 20) +/- 2 * ATR(20).", 19))
# ---------------- VOLUME (27) ----------------
SPEC.append(S("OBV", "volume", "pane", "ind_obv",
    "int period = 1", "vec",
    [("", "OBV", 0, 100, 200)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    double acc = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i > 0) {
            if (series[i].close > series[i - 1].close) acc += series[i].volume;
            else if (series[i].close < series[i - 1].close) acc -= series[i].volume;
        }
        o[i] = acc;
    }
    return o;''',
    "On-Balance Volume: cumulative volume, adding the bar volume when close rises, subtracting it when close falls, unchanged on an unchanged close.", 0))
SPEC.append(S("AccumulationDistributionLine", "volume", "pane", "ind_accumulation_distribution_line",
    "int period = 1", "vec",
    [("", "ADL", 0, 150, 0)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    double acc = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        double range = series[i].high - series[i].low;
        double clv = (range != 0.0) ? ((series[i].close - series[i].low) - (series[i].high - series[i].close)) / range : 0.0;
        acc += clv * series[i].volume;
        o[i] = acc;
    }
    return o;''',
    "Accumulation/Distribution Line: cumulative Close Location Value * volume, where CLV = ((close - low) - (high - close)) / (high - low).", 0))
SPEC.append(S("ChaikinMoneyFlow", "volume", "pane", "ind_chaikin_money_flow",
    "int period = 20", "vec",
    [("", "CMF", 0, 150, 0)],
    r'''std::vector<double> mf(series.size(), 0.0), vol(series.size(), 0.0);
    for (size_t i = 0; i < series.size(); ++i) {
        double range = series[i].high - series[i].low;
        double clv = (range != 0.0) ? ((series[i].close - series[i].low) - (series[i].high - series[i].close)) / range : 0.0;
        mf[i] = clv * series[i].volume; vol[i] = series[i].volume;
    }
    std::vector<double> sm = sma(mf, period), sv = sma(vol, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(sm[i]) && sv[i] != 0.0) o[i] = sm[i] / sv[i];
    return o;''',
    "Chaikin Money Flow = sum(CLV * volume, 20) / sum(volume, 20).", 19))
SPEC.append(S("MoneyFlowIndex", "volume", "pane", "ind_money_flow_index",
    "int period = 14", "vec",
    [("", "MFI", 150, 0, 150)],
    r'''std::vector<double> tp = series_typical(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)period) continue;
        double pos = 0.0, neg = 0.0;
        for (int j = 0; j < period; ++j) {
            size_t idx = i - (size_t)j;
            double mf = tp[idx] * series[idx].volume;
            if (idx > 0 && tp[idx] > tp[idx - 1]) pos += mf; else if (idx > 0 && tp[idx] < tp[idx - 1]) neg += mf;
        }
        if (neg == 0.0) o[i] = 100.0;
        else { double mr = pos / neg; o[i] = 100.0 - (100.0 / (1.0 + mr)); }
    }
    return o;''',
    "Money Flow Index: raw money flow = typical price * volume; positive flow when typical price rises, negative when it falls; MFI = 100 - 100 / (1 + positive flow / negative flow) over 14 bars.", 14))
SPEC.append(S("VolumeOscillator", "volume", "pane", "ind_volume_oscillator",
    "int period = 14", "vec",
    [("", "VolOsc", 0, 100, 200)],
    r'''(void)period;
    std::vector<double> v = series_volumes(series);
    std::vector<double> fast = ema(v, 5), slow = ema(v, 10);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(fast[i]) && fin(slow[i]) && slow[i] != 0.0) o[i] = 100.0 * (fast[i] - slow[i]) / slow[i];
    return o;''',
    "Volume Oscillator = 100 * (EMA(volume, 5) - EMA(volume, 10)) / EMA(volume, 10).", 9))
SPEC.append(S("PercentageVolumeOscillator", "volume", "pane", "ind_percentage_volume_oscillator",
    "int period = 14", "vec",
    [("", "PVO", 0, 100, 200)],
    r'''(void)period;
    std::vector<double> v = series_volumes(series);
    std::vector<double> fast = ema(v, 12), slow = ema(v, 26);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(fast[i]) && fin(slow[i]) && slow[i] != 0.0) o[i] = 100.0 * (fast[i] - slow[i]) / slow[i];
    return o;''',
    "Percentage Volume Oscillator = 100 * (EMA(volume, 12) - EMA(volume, 26)) / EMA(volume, 26).", 25))
SPEC.append(S("VWAP", "volume", "overlay", "ind_vwap",
    "int period = 1", "vec",
    [("", "VWAP", 200, 120, 0)],
    r'''(void)period;
    std::vector<double> tp = series_typical(series);
    std::vector<double> o(series.size(), nanv());
    double pv = 0.0, vv = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        pv += tp[i] * series[i].volume; vv += series[i].volume;
        if (vv != 0.0) o[i] = pv / vv;
    }
    return o;''',
    "VWAP = cumulative sum(typical price * volume) / cumulative sum(volume) from the first bar; typical price = (high + low + close) / 3.", 0))
SPEC.append(S("VWAPBands", "volume", "overlay", "ind_vwap_bands",
    "int period = 1, double mult = 1.0", "VWAPBandsResult",
    [("upper", "Upper", 200, 30, 30), ("vwap", "VWAP", 200, 120, 0), ("lower", "Lower", 200, 30, 30)],
    r'''(void)period;
    std::vector<double> tp = series_typical(series);
    VWAPBandsResult r;
    r.upper.assign(series.size(), nanv()); r.vwap.assign(series.size(), nanv()); r.lower.assign(series.size(), nanv());
    double pv = 0.0, vv = 0.0, pv2 = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        pv += tp[i] * series[i].volume; vv += series[i].volume; pv2 += tp[i] * tp[i] * series[i].volume;
        if (vv == 0.0) continue;
        double mean = pv / vv;
        double var = pv2 / vv - mean * mean;
        double sd = std::sqrt(var > 0 ? var : 0.0);
        r.vwap[i] = mean; r.upper[i] = mean + mult * sd; r.lower[i] = mean - mult * sd;
    }
    return r;''',
    "VWAP Bands: VWAP plus/minus 1 volume-weighted standard deviation of typical price, computed cumulatively from the first bar.", 0))
SPEC.append(S("AnchoredVWAP", "volume", "overlay", "ind_anchored_vwap",
    "int period = 50", "vec",
    [("", "AnchoredVWAP", 200, 120, 0)],
    r'''std::vector<double> tp = series_typical(series);
    std::vector<double> o(series.size(), nanv());
    size_t anchor = (size_t)std::max(0, period - 1);
    if (anchor >= series.size()) anchor = series.size() > 0 ? series.size() - 1 : 0;
    double pv = 0.0, vv = 0.0;
    for (size_t i = anchor; i < series.size(); ++i) {
        pv += tp[i] * series[i].volume; vv += series[i].volume;
        if (vv != 0.0) o[i] = pv / vv;
    }
    return o;''',
    "Anchored VWAP = cumulative sum(typical price * volume) / cumulative sum(volume), anchored at bar 50 (the period parameter) instead of the first bar.", 49))
SPEC.append(S("ChaikinOscillator", "volume", "pane", "ind_chaikin_oscillator",
    "int period = 14", "vec",
    [("", "ChaikinOsc", 0, 150, 0)],
    r'''(void)period;
    std::vector<double> adl(series.size(), 0.0);
    double acc = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        double range = series[i].high - series[i].low;
        double clv = (range != 0.0) ? ((series[i].close - series[i].low) - (series[i].high - series[i].close)) / range : 0.0;
        acc += clv * series[i].volume; adl[i] = acc;
    }
    std::vector<double> fast = ema(adl, 3), slow = ema(adl, 10);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(fast[i]) && fin(slow[i])) o[i] = fast[i] - slow[i];
    return o;''',
    "Chaikin Oscillator = EMA(Accumulation/Distribution Line, 3) - EMA(Accumulation/Distribution Line, 10).", 9))
SPEC.append(S("EaseOfMovement", "volume", "pane", "ind_ease_of_movement",
    "int period = 14", "vec",
    [("", "EMV", 0, 150, 0)],
    r'''std::vector<double> raw(series.size(), 0.0);
    for (size_t i = 1; i < series.size(); ++i) {
        double mid = (series[i].high + series[i].low) / 2.0;
        double pmid = (series[i - 1].high + series[i - 1].low) / 2.0;
        double box = series[i].volume / 1000000.0 / (series[i].high - series[i].low);
        if (box != 0.0) raw[i] = (mid - pmid) / box;
    }
    return sma(raw, period);''',
    "Ease of Movement: distance moved = midpoint change; box ratio = (volume / 1,000,000) / (high - low); EMV = SMA(distance moved / box ratio, 14).", 14))
SPEC.append(S("VolumeRateOfChange", "volume", "pane", "ind_volume_rate_of_change",
    "int period = 14", "vec",
    [("", "VROC", 0, 100, 200)],
    r'''std::vector<double> v = series_volumes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = (size_t)period; i < series.size(); ++i)
        if (v[i - (size_t)period] != 0.0) o[i] = 100.0 * (v[i] - v[i - (size_t)period]) / v[i - (size_t)period];
    return o;''',
    "Volume Rate of Change = 100 * (volume - volume 14 bars ago) / volume 14 bars ago.", 14))
SPEC.append(S("VolumePriceTrend", "volume", "pane", "ind_volume_price_trend",
    "int period = 1", "vec",
    [("", "VPT", 0, 100, 200)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    double acc = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i > 0 && series[i - 1].close != 0.0)
            acc += series[i].volume * (series[i].close - series[i - 1].close) / series[i - 1].close;
        o[i] = acc;
    }
    return o;''',
    "Volume Price Trend: cumulative sum of volume * (close - previous close) / previous close.", 0))
SPEC.append(S("NegativeVolumeIndex", "volume", "pane", "ind_negative_volume_index",
    "int period = 1", "vec",
    [("", "NVI", 0, 150, 0)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    double acc = 1000.0;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i > 0 && series[i].volume < series[i - 1].volume && series[i - 1].close != 0.0)
            acc *= series[i].close / series[i - 1].close;
        o[i] = acc;
    }
    return o;''',
    "Negative Volume Index: starts at 1000; only changes on bars whose volume fell versus the previous bar, by the close-to-close price ratio.", 0))
SPEC.append(S("PositiveVolumeIndex", "volume", "pane", "ind_positive_volume_index",
    "int period = 1", "vec",
    [("", "PVI", 0, 150, 0)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    double acc = 1000.0;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i > 0 && series[i].volume > series[i - 1].volume && series[i - 1].close != 0.0)
            acc *= series[i].close / series[i - 1].close;
        o[i] = acc;
    }
    return o;''',
    "Positive Volume Index: starts at 1000; only changes on bars whose volume rose versus the previous bar, by the close-to-close price ratio.", 0))
SPEC.append(S("DemandIndex", "volume", "pane", "ind_demand_index",
    "int period = 14", "vec",
    [("", "DemandIndex", 0, 100, 200)],
    r'''std::vector<double> bp(series.size(), 0.0), sp(series.size(), 0.0);
    for (size_t i = 0; i < series.size(); ++i) {
        double range = series[i].high - series[i].low;
        double clv = (range != 0.0) ? ((series[i].close - series[i].low) - (series[i].high - series[i].close)) / range : 0.0;
        double mf = clv * series[i].volume;
        if (mf >= 0) bp[i] = mf; else sp[i] = -mf;
    }
    std::vector<double> sbp = sma(bp, period), ssp = sma(sp, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        double tot = sbp[i] + ssp[i];
        if (fin(sbp[i]) && tot != 0.0) o[i] = 100.0 * (sbp[i] - ssp[i]) / tot;
    }
    return o;''',
    "Demand Index: buying pressure = positive CLV*volume, selling pressure = negative CLV*volume; index = 100 * (SMA(buying,14) - SMA(selling,14)) / (SMA(buying,14) + SMA(selling,14)).", 13))
SPEC.append(S("KlingerVolumeOscillator", "volume", "pane", "ind_klinger_volume_oscillator",
    "int period = 14", "KlingerVolumeOscillatorResult",
    [("kvo", "KVO", 0, 100, 200), ("signal", "Signal", 200, 120, 0)],
    r'''std::vector<double> vf(series.size(), 0.0);
    for (size_t i = 0; i < series.size(); ++i) {
        double trend = 0.0;
        if (i > 0) trend = (series[i].high + series[i].low + series[i].close > series[i - 1].high + series[i - 1].low + series[i - 1].close) ? 1.0 : -1.0;
        double dm = (i > 0) ? ((series[i].high - series[i].low) - (series[i - 1].high - series[i - 1].low)) : 0.0;
        double cm = (i > 0) ? (std::fabs(series[i].high - series[i - 1].low) + std::fabs(series[i].low - series[i - 1].high) + std::fabs(series[i].high - series[i - 1].close) + std::fabs(series[i].low - series[i - 1].close)) : 1.0;
        double vf_raw = (cm != 0.0) ? trend * series[i].volume * std::fabs(2.0 * dm / cm - 1.0) : 0.0;
        vf[i] = vf_raw;
    }
    KlingerVolumeOscillatorResult r;
    r.kvo.assign(series.size(), nanv()); r.signal.assign(series.size(), nanv());
    std::vector<double> fast = ema(vf, 34), slow = ema(vf, 55);
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(fast[i]) && fin(slow[i])) r.kvo[i] = fast[i] - slow[i];
    { std::vector<double> kv = r.kvo;
      for (size_t i = 0; i < kv.size(); ++i) if (!fin(kv[i])) kv[i] = 0.0;
      r.signal = ema(kv, 13);
      for (size_t i = 0; i < series.size(); ++i) if (!fin(r.kvo[i])) r.signal[i] = nanv(); }
    return r;''',
    "Klinger Volume Oscillator: volume force = trend * volume * |2*dm/cm - 1|, where dm is the high-low range change and cm the cumulative measurement; KVO = EMA(volume force, 34) - EMA(volume force, 55); signal = EMA(KVO, 13).",
    54))
SPEC.append(S("TwiggsMoneyFlow", "volume", "pane", "ind_twiggs_money_flow",
    "int period = 21", "vec",
    [("", "TwiggsMF", 0, 150, 0)],
    r'''std::vector<double> th(series.size()), tl(series.size());
    for (size_t i = 0; i < series.size(); ++i) {
        th[i] = series[i].high; tl[i] = series[i].low;
        if (i > 0) { th[i] = std::max(th[i], series[i - 1].close); tl[i] = std::min(tl[i], series[i - 1].close); }
    }
    std::vector<double> adv(series.size(), 0.0), vol(series.size(), 0.0);
    for (size_t i = 0; i < series.size(); ++i) {
        double range = th[i] - tl[i];
        double clv = (range != 0.0) ? ((series[i].close - tl[i]) - (th[i] - series[i].close)) / range : 0.0;
        adv[i] = clv * series[i].volume; vol[i] = series[i].volume;
    }
    std::vector<double> sa = ema(adv, period), sv = ema(vol, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(sa[i]) && fin(sv[i]) && sv[i] != 0.0) o[i] = sa[i] / sv[i];
    return o;''',
    "Twiggs Money Flow: like Chaikin Money Flow but the true high/low are extended by the previous close; EMA(AD volume, 21) / EMA(volume, 21).", 20))
SPEC.append(S("VolumeWeightedMovingAverage", "volume", "overlay", "ind_volume_weighted_moving_average",
    "int period = 20", "vec",
    [("", "VWMA", 200, 120, 0)],
    r'''std::vector<double> c = series_closes(series), v = series_volumes(series);
    std::vector<double> cv(series.size());
    for (size_t i = 0; i < series.size(); ++i) cv[i] = c[i] * v[i];
    std::vector<double> sc = sma(cv, period), sv = sma(v, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(sc[i]) && sv[i] != 0.0) o[i] = sc[i] / sv[i];
    return o;''',
    "Volume Weighted Moving Average = sum(close * volume, 20) / sum(volume, 20).", 19))
SPEC.append(S("VolumeZoneOscillator", "volume", "pane", "ind_volume_zone_oscillator",
    "int period = 14", "vec",
    [("", "VZO", 0, 100, 200)],
    r'''std::vector<double> r(series.size(), 0.0), vp(series.size(), 0.0), vn(series.size(), 0.0);
    for (size_t i = 0; i < series.size(); ++i) {
        if (i > 0 && series[i].close > series[i - 1].close) r[i] = 1.0;
        else if (i > 0 && series[i].close < series[i - 1].close) r[i] = -1.0;
        double v = series[i].volume;
        if (r[i] > 0) vp[i] = v; else if (r[i] < 0) vn[i] = v;
    }
    std::vector<double> svp = ema(vp, period), svn = ema(vn, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        double tot = svp[i] + svn[i];
        if (fin(svp[i]) && fin(svn[i]) && tot != 0.0) o[i] = 100.0 * (svp[i] - svn[i]) / tot;
    }
    return o;''',
    "Volume Zone Oscillator: R = +1 on up closes, -1 on down closes; VZO = 100 * (EMA(up volume,14) - EMA(down volume,14)) / (EMA(up volume,14) + EMA(down volume,14)).", 13))
SPEC.append(S("RelativeVolume", "volume", "pane", "ind_relative_volume",
    "int period = 20", "vec",
    [("", "RVOL", 0, 100, 200)],
    r'''std::vector<double> v = series_volumes(series);
    std::vector<double> mv = sma(v, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(mv[i]) && mv[i] != 0.0) o[i] = v[i] / mv[i];
    return o;''',
    "Relative Volume = volume / SMA(volume, 20).", 19))
SPEC.append(S("VolumeAverage", "volume", "pane", "ind_volume_average",
    "int period = 20", "vec",
    [("", "VolAvg", 0, 100, 200)],
    r'''return sma(series_volumes(series), period);''',
    "Volume Average = SMA(volume, 20).", 19))
SPEC.append(S("MoneyFlowVolume", "volume", "pane", "ind_money_flow_volume",
    "int period = 1", "vec",
    [("", "MFVolume", 0, 150, 0)],
    r'''(void)period;
    std::vector<double> tp = series_typical(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) o[i] = tp[i] * series[i].volume;
    return o;''',
    "Money Flow Volume = typical price * volume per bar; typical price = (high + low + close) / 3.", 0))
SPEC.append(S("VolumeDelta", "volume", "pane", "ind_volume_delta",
    "int period = 1", "vec",
    [("", "VolDelta", 0, 150, 0)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        double range = series[i].high - series[i].low;
        double clv = (range != 0.0) ? ((series[i].close - series[i].low) - (series[i].high - series[i].close)) / range : 0.0;
        o[i] = clv * series[i].volume;
    }
    return o;''',
    "Volume Delta per bar = Close Location Value * volume; CLV = ((close - low) - (high - close)) / (high - low).", 0))
SPEC.append(S("CumulativeVolumeDelta", "volume", "pane", "ind_cumulative_volume_delta",
    "int period = 1", "vec",
    [("", "CumVolDelta", 0, 150, 0)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    double acc = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        double range = series[i].high - series[i].low;
        double clv = (range != 0.0) ? ((series[i].close - series[i].low) - (series[i].high - series[i].close)) / range : 0.0;
        acc += clv * series[i].volume;
        o[i] = acc;
    }
    return o;''',
    "Cumulative Volume Delta = running sum of per-bar Volume Delta (CLV * volume).", 0))
SPEC.append(S("UpDownVolumeRatio", "volume", "pane", "ind_up_down_volume_ratio",
    "int period = 14", "vec",
    [("", "UpDownVol", 0, 100, 200)],
    r'''std::vector<double> up(series.size(), 0.0), dn(series.size(), 0.0);
    for (size_t i = 0; i < series.size(); ++i) {
        if (i > 0 && series[i].close > series[i - 1].close) up[i] = series[i].volume;
        else if (i > 0 && series[i].close < series[i - 1].close) dn[i] = series[i].volume;
    }
    std::vector<double> su = sma(up, period), sd = sma(dn, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(su[i]) && fin(sd[i]) && sd[i] != 0.0) o[i] = su[i] / sd[i];
    return o;''',
    "Up/Down Volume Ratio = SMA(up-close volume, 14) / SMA(down-close volume, 14).", 13))
SPEC.append(S("IntradayIntensityIndex", "volume", "pane", "ind_intraday_intensity_index",
    "int period = 21", "vec",
    [("", "IntradayIntensity", 0, 150, 0)],
    r'''std::vector<double> ii(series.size(), 0.0);
    for (size_t i = 0; i < series.size(); ++i) {
        double range = series[i].high - series[i].low;
        if (range != 0.0) ii[i] = ((2.0 * series[i].close - series[i].high - series[i].low) / range) * series[i].volume;
    }
    std::vector<double> o(series.size(), nanv());
    double s = 0.0;
    for (size_t i = 0; i < series.size(); ++i) {
        s += ii[i];
        if (i >= (size_t)period) s -= ii[i - (size_t)period];
        if (i >= (size_t)(period - 1)) o[i] = s;
    }
    return o;''',
    "Intraday Intensity Index (Elder): per bar ((2*close - high - low) / (high - low)) * volume, summed over the last 21 bars.", 20))
# ---------------- OTHER (25) ----------------
SPEC.append(S("HurstExponent", "other", "pane", "ind_hurst_exponent",
    "int period = 50", "vec",
    [("", "Hurst", 150, 0, 150)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double mean = 0.0;
        for (int j = 0; j < period; ++j) mean += c[i - (size_t)j];
        mean /= period;
        double mx = -1e300, mn = 1e300, s = 0.0, dev = 0.0;
        for (int j = 0; j < period; ++j) {
            dev += c[i - (size_t)j] - mean;
            mx = std::max(mx, dev); mn = std::min(mn, dev);
            double d = c[i - (size_t)j] - mean; s += d * d;
        }
        double sd = std::sqrt(s / period);
        double rs = (sd > 0) ? (mx - mn) / sd : 0.0;
        if (rs > 0) o[i] = std::log(rs) / std::log(period / 2.0);
    }
    return o;''',
    "Hurst Exponent (rescaled range): H = log(R/S) / log(n/2), where R is the range of the cumulative deviations from the 50-bar mean close and S is the standard deviation of close.", 49))
SPEC.append(S("FractalDimension", "other", "pane", "ind_fractal_dimension",
    "int period = 50", "vec",
    [("", "FractalDim", 150, 0, 150)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double mean = 0.0;
        for (int j = 0; j < period; ++j) mean += c[i - (size_t)j];
        mean /= period;
        double mx = -1e300, mn = 1e300, sq = 0.0, dev = 0.0;
        for (int j = 0; j < period; ++j) {
            dev += c[i - (size_t)j] - mean;
            mx = std::max(mx, dev); mn = std::min(mn, dev);
            double d = c[i - (size_t)j] - mean; sq += d * d;
        }
        double sd = std::sqrt(sq / period);
        double rs = (sd > 0) ? (mx - mn) / sd : 0.0;
        if (rs > 0) o[i] = 2.0 - std::log(rs) / std::log(period / 2.0);
    }
    return o;''',
    "Fractal Dimension D = 2 - Hurst Exponent, with the Hurst Exponent computed by the rescaled-range method over 50 bars. NOTE: this widget recomputes the Hurst estimate internally; it is the standard D = 2 - H relation, not an independent box-count.", 49))
SPEC.append(S("EfficiencyRatio", "other", "pane", "ind_efficiency_ratio",
    "int period = 10", "vec",
    [("", "EffRatio", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)period) continue;
        double net = std::fabs(c[i] - c[i - (size_t)period]);
        double tot = 0.0;
        for (int j = 0; j < period; ++j) tot += std::fabs(c[i - (size_t)j] - c[i - (size_t)j - 1]);
        o[i] = (tot != 0.0) ? net / tot : 0.0;
    }
    return o;''',
    "Kaufman Efficiency Ratio = |close - close 10 bars ago| / sum of |daily close changes| over 10 bars.", 10))
SPEC.append(S("LinearRegressionSlope", "other", "pane", "ind_linear_regression_slope",
    "int period = 14", "vec",
    [("", "LRSlope", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    double sx = period * (period - 1) / 2.0;
    double sxx = 0.0; for (int j = 0; j < period; ++j) sxx += (double)j * j;
    double den = period * sxx - sx * sx;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double sy = 0.0, sxy = 0.0;
        for (int j = 0; j < period; ++j) { double y = c[i - (size_t)(period - 1) + (size_t)j]; sy += y; sxy += j * y; }
        if (den != 0.0) o[i] = (period * sxy - sx * sy) / den;
    }
    return o;''',
    "Linear Regression Slope: least-squares slope of close against bar index over the last 14 bars.", 13))
SPEC.append(S("LinearRegressionIntercept", "other", "pane", "ind_linear_regression_intercept",
    "int period = 14", "vec",
    [("", "LRIntercept", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    double sx = period * (period - 1) / 2.0;
    double sxx = 0.0; for (int j = 0; j < period; ++j) sxx += (double)j * j;
    double den = period * sxx - sx * sx;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double sy = 0.0, sxy = 0.0;
        for (int j = 0; j < period; ++j) { double y = c[i - (size_t)(period - 1) + (size_t)j]; sy += y; sxy += j * y; }
        if (den != 0.0) { double slope = (period * sxy - sx * sy) / den; o[i] = (sy - slope * sx) / period; }
    }
    return o;''',
    "Linear Regression Intercept: least-squares intercept of close against bar index over the last 14 bars (value of the fitted line at bar index 0 of the window).", 13))
SPEC.append(S("RSquared", "other", "pane", "ind_r_squared",
    "int period = 14", "vec",
    [("", "RSquared", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    double sx = period * (period - 1) / 2.0;
    double sxx = 0.0; for (int j = 0; j < period; ++j) sxx += (double)j * j;
    double den = period * sxx - sx * sx;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double sy = 0.0, sxy = 0.0;
        for (int j = 0; j < period; ++j) { double y = c[i - (size_t)(period - 1) + (size_t)j]; sy += y; sxy += j * y; }
        if (den == 0.0) continue;
        double slope = (period * sxy - sx * sy) / den;
        double inter = (sy - slope * sx) / period;
        double mean = sy / period, sst = 0.0, sse = 0.0;
        for (int j = 0; j < period; ++j) {
            double y = c[i - (size_t)(period - 1) + (size_t)j];
            double fit = inter + slope * j;
            sst += (y - mean) * (y - mean); sse += (y - fit) * (y - fit);
        }
        o[i] = (sst != 0.0) ? 1.0 - sse / sst : 1.0;
    }
    return o;''',
    "R-Squared: 1 - SSE/SST of the least-squares line of close against bar index over the last 14 bars.", 13))
SPEC.append(S("StandardError", "other", "pane", "ind_standard_error",
    "int period = 14", "vec",
    [("", "StdError", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    double sx = period * (period - 1) / 2.0;
    double sxx = 0.0; for (int j = 0; j < period; ++j) sxx += (double)j * j;
    double den = period * sxx - sx * sx;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double sy = 0.0, sxy = 0.0;
        for (int j = 0; j < period; ++j) { double y = c[i - (size_t)(period - 1) + (size_t)j]; sy += y; sxy += j * y; }
        if (den == 0.0 || period <= 2) continue;
        double slope = (period * sxy - sx * sy) / den;
        double inter = (sy - slope * sx) / period;
        double sse = 0.0;
        for (int j = 0; j < period; ++j) { double y = c[i - (size_t)(period - 1) + (size_t)j]; double d = y - (inter + slope * j); sse += d * d; }
        o[i] = std::sqrt(sse / (period - 2));
    }
    return o;''',
    "Standard Error of the linear regression of close against bar index over the last 14 bars: sqrt(SSE / (n - 2)).", 13))
SPEC.append(S("LinearRegressionForecast", "other", "pane", "ind_linear_regression_forecast",
    "int period = 14", "vec",
    [("", "LRForecast", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    double sx = period * (period - 1) / 2.0;
    double sxx = 0.0; for (int j = 0; j < period; ++j) sxx += (double)j * j;
    double den = period * sxx - sx * sx;
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double sy = 0.0, sxy = 0.0;
        for (int j = 0; j < period; ++j) { double y = c[i - (size_t)(period - 1) + (size_t)j]; sy += y; sxy += j * y; }
        if (den == 0.0) continue;
        double slope = (period * sxy - sx * sy) / den;
        double inter = (sy - slope * sx) / period;
        o[i] = inter + slope * (period - 1);
    }
    return o;''',
    "Linear Regression Forecast: the fitted line value at the last bar of each 14-bar window (intercept + slope * (n - 1)).", 13))
SPEC.append(S("ZScore", "other", "pane", "ind_z_score",
    "int period = 20", "vec",
    [("", "ZScore", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, period), sd = stddev(c, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i)
        if (fin(m[i]) && sd[i] != 0.0) o[i] = (c[i] - m[i]) / sd[i];
    return o;''',
    "Z-Score = (close - SMA(close, 20)) / StdDev(close, 20).", 19))
SPEC.append(S("PercentileRank", "other", "pane", "ind_percentile_rank",
    "int period = 20", "vec",
    [("", "PctRank", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        int below = 0;
        for (int j = 0; j < period; ++j) if (c[i - (size_t)j] < c[i]) ++below;
        o[i] = 100.0 * below / (period - 1);
    }
    return o;''',
    "Percentile Rank = 100 * (number of closes in the last 20 bars below the current close) / 19.", 19))
SPEC.append(S("MedianPrice", "other", "overlay", "ind_median_price",
    "int period = 20", "vec",
    [("", "Median", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        std::vector<double> w(c.begin() + (long)(i - (size_t)(period - 1)), c.begin() + (long)(i + 1));
        std::sort(w.begin(), w.end());
        o[i] = (period % 2 == 1) ? w[(size_t)(period / 2)] : (w[(size_t)(period / 2 - 1)] + w[(size_t)(period / 2)]) / 2.0;
    }
    return o;''',
    "Median Price = rolling median of close over the last 20 bars.", 19))
SPEC.append(S("TypicalPrice", "other", "overlay", "ind_typical_price",
    "int period = 1", "vec",
    [("", "Typical", 0, 100, 200)],
    r'''(void)period; return series_typical(series);''',
    "Typical Price = (high + low + close) / 3 per bar.", 0))
SPEC.append(S("WeightedClose", "other", "overlay", "ind_weighted_close",
    "int period = 1", "vec",
    [("", "WtdClose", 0, 100, 200)],
    r'''(void)period;
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) o[i] = (series[i].high + series[i].low + 2.0 * series[i].close) / 4.0;
    return o;''',
    "Weighted Close = (high + low + 2 * close) / 4 per bar.", 0))
SPEC.append(S("Skewness", "other", "pane", "ind_skewness",
    "int period = 20", "vec",
    [("", "Skew", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, period), sd = stddev(c, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (!fin(m[i]) || sd[i] == 0.0) continue;
        double s = 0.0;
        for (int j = 0; j < period; ++j) { double z = (c[i - (size_t)j] - m[i]) / sd[i]; s += z * z * z; }
        o[i] = s / period;
    }
    return o;''',
    "Skewness = mean of ((close - SMA(close,20)) / StdDev(close,20))^3 over the last 20 bars (population moment).", 19))
SPEC.append(S("Kurtosis", "other", "pane", "ind_kurtosis",
    "int period = 20", "vec",
    [("", "Kurtosis", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, period), sd = stddev(c, period);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (!fin(m[i]) || sd[i] == 0.0) continue;
        double s = 0.0;
        for (int j = 0; j < period; ++j) { double z = (c[i - (size_t)j] - m[i]) / sd[i]; s += z * z * z * z; }
        o[i] = s / period - 3.0;
    }
    return o;''',
    "Excess Kurtosis = mean of ((close - SMA(close,20)) / StdDev(close,20))^4 over the last 20 bars, minus 3.", 19))
SPEC.append(S("Autocorrelation", "other", "pane", "ind_autocorrelation",
    "int period = 20", "vec",
    [("", "Autocorr", 0, 100, 200)],
    r'''std::vector<double> lr = logret(series_closes(series));
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)period) continue;
        double mean = 0.0; int n = 0;
        for (int j = 0; j < period; ++j) if (fin(lr[i - (size_t)j])) { mean += lr[i - (size_t)j]; ++n; }
        if (n < period) continue;
        mean /= n;
        double num = 0.0, den = 0.0;
        for (int j = 0; j < period; ++j) { double d = lr[i - (size_t)j] - mean; den += d * d; }
        for (int j = 1; j < period; ++j) num += (lr[i - (size_t)j] - mean) * (lr[i - (size_t)j - 1] - mean);
        o[i] = (den != 0.0) ? num / den : 0.0;
    }
    return o;''',
    "Lag-1 Autocorrelation of log returns over the last 20 bars: sum((r_t - mean)(r_{t-1} - mean)) / sum((r_t - mean)^2).", 20))
SPEC.append(S("ShannonEntropy", "other", "pane", "ind_shannon_entropy",
    "int period = 20", "vec",
    [("", "Entropy", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)period) continue;
        int up = 0, dn = 0, fl = 0;
        for (int j = 0; j < period; ++j) {
            double d = c[i - (size_t)j] - c[i - (size_t)j - 1];
            if (d > 0) ++up; else if (d < 0) ++dn; else ++fl;
        }
        double e = 0.0;
        for (int k = 0; k < 3; ++k) {
            double p = (k == 0 ? up : (k == 1 ? dn : fl)) / (double)period;
            if (p > 0) e -= p * std::log2(p);
        }
        o[i] = e;
    }
    return o;''',
    "Shannon Entropy (bits) of the up / down / flat daily-close sign distribution over the last 20 bars: -sum(p * log2(p)).", 20))
SPEC.append(S("UpDownRatio", "other", "pane", "ind_up_down_ratio",
    "int period = 14", "vec",
    [("", "UpDownRatio", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)period) continue;
        int up = 0, dn = 0;
        for (int j = 0; j < period; ++j) {
            if (c[i - (size_t)j] > c[i - (size_t)j - 1]) ++up;
            else if (c[i - (size_t)j] < c[i - (size_t)j - 1]) ++dn;
        }
        o[i] = (dn != 0) ? (double)up / dn : (double)up;
    }
    return o;''',
    "Up/Down Ratio = (number of up closes in the last 14 bars) / (number of down closes in the last 14 bars).", 14))
SPEC.append(S("StandardPivotPoints", "other", "overlay", "ind_standard_pivot_points",
    "int period = 1", "StandardPivotPointsResult",
    [("pivot", "Pivot", 0, 100, 200), ("r1", "R1", 200, 30, 30), ("s1", "S1", 0, 150, 0)],
    r'''(void)period;
    StandardPivotPointsResult r;
    r.pivot.assign(series.size(), nanv()); r.r1.assign(series.size(), nanv()); r.s1.assign(series.size(), nanv());
    for (size_t i = 1; i < series.size(); ++i) {
        double p = (series[i - 1].high + series[i - 1].low + series[i - 1].close) / 3.0;
        r.pivot[i] = p; r.r1[i] = 2.0 * p - series[i - 1].low; r.s1[i] = 2.0 * p - series[i - 1].high;
    }
    return r;''',
    "Standard Pivot Points from the previous bar: Pivot = (H+L+C)/3; R1 = 2*Pivot - Low; S1 = 2*Pivot - High.", 1))
SPEC.append(S("FibonacciPivotPoints", "other", "overlay", "ind_fibonacci_pivot_points",
    "int period = 1", "FibonacciPivotPointsResult",
    [("pivot", "Pivot", 0, 100, 200), ("r1", "R1", 200, 30, 30), ("s1", "S1", 0, 150, 0)],
    r'''(void)period;
    FibonacciPivotPointsResult r;
    r.pivot.assign(series.size(), nanv()); r.r1.assign(series.size(), nanv()); r.s1.assign(series.size(), nanv());
    for (size_t i = 1; i < series.size(); ++i) {
        double range = series[i - 1].high - series[i - 1].low;
        double p = (series[i - 1].high + series[i - 1].low + series[i - 1].close) / 3.0;
        r.pivot[i] = p; r.r1[i] = p + 0.382 * range; r.s1[i] = p - 0.382 * range;
    }
    return r;''',
    "Fibonacci Pivot Points from the previous bar: Pivot = (H+L+C)/3; R1 = Pivot + 0.382*(H-L); S1 = Pivot - 0.382*(H-L).", 1))
SPEC.append(S("CamarillaPivotPoints", "other", "overlay", "ind_camarilla_pivot_points",
    "int period = 1", "CamarillaPivotPointsResult",
    [("pivot", "Pivot", 0, 100, 200), ("r1", "R1", 200, 30, 30), ("s1", "S1", 0, 150, 0)],
    r'''(void)period;
    CamarillaPivotPointsResult r;
    r.pivot.assign(series.size(), nanv()); r.r1.assign(series.size(), nanv()); r.s1.assign(series.size(), nanv());
    for (size_t i = 1; i < series.size(); ++i) {
        double range = series[i - 1].high - series[i - 1].low;
        double p = (series[i - 1].high + series[i - 1].low + series[i - 1].close) / 3.0;
        r.pivot[i] = p; r.r1[i] = series[i - 1].close + range * 1.1 / 12.0; r.s1[i] = series[i - 1].close - range * 1.1 / 12.0;
    }
    return r;''',
    "Camarilla Pivot Points from the previous bar: Pivot = (H+L+C)/3; R1 = Close + (H-L)*1.1/12; S1 = Close - (H-L)*1.1/12.", 1))
SPEC.append(S("WoodiePivotPoints", "other", "overlay", "ind_woodie_pivot_points",
    "int period = 1", "WoodiePivotPointsResult",
    [("pivot", "Pivot", 0, 100, 200), ("r1", "R1", 200, 30, 30), ("s1", "S1", 0, 150, 0)],
    r'''(void)period;
    WoodiePivotPointsResult r;
    r.pivot.assign(series.size(), nanv()); r.r1.assign(series.size(), nanv()); r.s1.assign(series.size(), nanv());
    for (size_t i = 1; i < series.size(); ++i) {
        double p = (series[i - 1].high + series[i - 1].low + 2.0 * series[i - 1].close) / 4.0;
        r.pivot[i] = p; r.r1[i] = 2.0 * p - series[i - 1].low; r.s1[i] = 2.0 * p - series[i - 1].high;
    }
    return r;''',
    "Woodie Pivot Points from the previous bar: Pivot = (H+L+2*C)/4; R1 = 2*Pivot - Low; S1 = 2*Pivot - High.", 1))
SPEC.append(S("DominantCyclePeriod", "other", "pane", "ind_dominant_cycle_period",
    "int period = 50", "vec",
    [("", "CyclePeriod", 150, 0, 150)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> m = sma(c, 10);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1) || !fin(m[i])) continue;
        int crossings = 0; double prev = 0.0; bool have_prev = false;
        for (size_t j = i - (size_t)(period - 1); j <= i; ++j) {
            if (!fin(m[j])) continue;
            double d = c[j] - m[j];
            if (have_prev && ((prev < 0 && d >= 0) || (prev > 0 && d <= 0))) ++crossings;
            prev = d; have_prev = true;
        }
        if (crossings > 0) o[i] = 2.0 * period / crossings;
    }
    return o;''',
    "Dominant Cycle Period (zero-crossing estimate): count sign changes of (close - SMA(close,10)) over the last 50 bars; period = 2 * 50 / crossings.", 49))
SPEC.append(S("Correlation", "other", "pane", "ind_correlation",
    "int period = 20", "vec",
    [("", "Correlation", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double mx = (period - 1) / 2.0, my = 0.0;
        for (int j = 0; j < period; ++j) my += c[i - (size_t)j];
        my /= period;
        double num = 0.0, dx = 0.0, dy = 0.0;
        for (int j = 0; j < period; ++j) {
            double x = j - mx, y = c[i - (size_t)(period - 1) + (size_t)j] - my;
            num += x * y; dx += x * x; dy += y * y;
        }
        double den = std::sqrt(dx * dy);
        o[i] = (den != 0.0) ? num / den : 0.0;
    }
    return o;''',
    "Correlation: Pearson correlation between bar index and close over the last 20 bars.", 19))
SPEC.append(S("Covariance", "other", "pane", "ind_covariance",
    "int period = 20", "vec",
    [("", "Covariance", 0, 100, 200)],
    r'''std::vector<double> c = series_closes(series), v = series_volumes(series);
    std::vector<double> o(series.size(), nanv());
    for (size_t i = 0; i < series.size(); ++i) {
        if (i < (size_t)(period - 1)) continue;
        double mc = 0.0, mv = 0.0;
        for (int j = 0; j < period; ++j) { mc += c[i - (size_t)j]; mv += v[i - (size_t)j]; }
        mc /= period; mv /= period;
        double s = 0.0;
        for (int j = 0; j < period; ++j) s += (c[i - (size_t)j] - mc) * (v[i - (size_t)j] - mv);
        o[i] = s / period;
    }
    return o;''',
    "Covariance: population covariance between close and volume over the last 20 bars.", 19))

# ---------------- emission ----------------
HEADER_TMPL = r'''#pragma once
// %(CLS)s -- %(NAME)s (%(CAT)s indicator).
// Formula: %(FORMULA)s
// Output: one value per input bar; leading warm-up bars are NaN until
// enough bars exist (see the docs entry). Attribution: XFrame.
#include "core/xframe_core.h"
#include "../StockData.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace xframe {
%(HELPERS)s
%(STRUCT)s
%(RET)s %(FUNC)s(const OHLCVSeries& series, %(PARAMS)s) {
    using namespace detail_%(L)s;
    %(BODY)s
}

class %(CLS)s {
private:
    Display* display_;
    int screen_;
    Window window_;
    GC gc_;
    SStyle_Manager* styles_;
    int x1_, y1_, x2_, y2_;
    std::string name_ = "%(CLS)s";
    std::string text_ = "%(NAME)s";
    std::string style_name_;
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    struct Line { std::string name; Srgb color; std::vector<double> v; };
    std::vector<Line> lines_;
    OHLCVSeries series_;
    double view_first_ = 0.0;
    double zoom_ = 1.0;
    bool dragging_ = false;
    int drag_start_x_ = 0;
    double drag_start_first_ = 0.0;
    bool cross_valid_ = false;
    int cross_x_ = 0, cross_y_ = 0;
    int period_;
    double mult_;
public:
    std::function<void(double, double)> on_view_change;
    %(CLS)s(Display* display, int screen, Window window, GC gc, SStyle_Manager* styles,
        int x, int y, int width, int height, const OHLCVSeries& series,
        int period = %(PERIOD)s, double mult = %(MULT)s)
        : display_(display), screen_(screen), window_(window), gc_(gc), styles_(styles),
          x1_(x), y1_(y), x2_(x + width), y2_(y + height), series_(series),
          period_(period), mult_(mult) {
%(LINES)s
    }
    ~%(CLS)s() {}
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
    const std::vector<double>& line_values(int i) { return lines_[i].v; }
    double get_view_first() { return view_first_; }
    double get_zoom() { return zoom_; }
    void reset_view() { view_first_ = 0.0; zoom_ = 1.0; if (on_view_change) on_view_change(0.0, (double)item_count()); }
    int plot_x() { return x1_ + 56; }
    int plot_y() { return y1_ + 6; }
    int plot_w() { return width() - 56 - 8; }
    int plot_h() { return height() - 20 - 12; }
    double visible_count() { double n = (double)item_count(); return n <= 0 ? 1.0 : std::max(5.0, n / zoom_); }
    double item_w() { return plot_w() / visible_count(); }
    int x_center(int i) { return plot_x() + (int)((i - view_first_ + 0.5) * item_w()); }
    int y_at(double v, double lo, double hi) { return plot_y() + (int)((hi - v) / (hi - lo) * plot_h()); }
    static std::string fmt(double v) {
        std::ostringstream oss; oss.precision(2); oss << std::fixed << v;
        std::string s = oss.str();
        while (!s.empty() && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
        return s;
    }
    void draw() {
        if (!visible_) return;
        SStyle st = style();
        unsigned long white = styles_->get_color_rgb(255, 255, 255);
        unsigned long grid = styles_->get_color_rgb(225, 225, 225);
        unsigned long axis_col = styles_->get_color_rgb(150, 150, 150);
        XSetForeground(display_, gc_, white);
        XFillRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        double lo = 1e300, hi = -1e300;
        for (auto& ln : lines_) for (double v : ln.v)
            if (std::isfinite(v)) { lo = std::min(lo, v); hi = std::max(hi, v); }
        if (%(OVERLAY)s) {
            for (auto& b : series_) { lo = std::min(lo, b.low); hi = std::max(hi, b.high); }
        }
        if (lo > hi) { lo = 0.0; hi = 1.0; }
        double pad = (hi - lo) * 0.06 + 1e-9; lo -= pad; hi += pad;
        int px = plot_x(), py = plot_y(), pw = plot_w(), ph = plot_h();
        double step = XGraph_nice_step(hi - lo, 6);
        double start = std::ceil(lo / step) * step;
        for (double v = start; v <= hi; v += step) {
            int gy = y_at(v, lo, hi);
            XSetForeground(display_, gc_, grid);
            XDrawLine(display_, window_, gc_, px, gy, px + pw, gy);
            std::string lab = fmt(v);
            int tw = xf::text_width(st, lab);
            xf::draw_text_at(display_, window_, gc_, st, px - tw - 6, gy + 4, lab);
        }
        if (%(OVERLAY)s) {
            int bw = std::max(1, (int)(item_w() * 0.6));
            for (int i = 0; i < (int)series_.size(); ++i) {
                int cx = x_center(i);
                if (cx < px || cx > px + pw) continue;
                const OHLCV& b = series_[i];
                unsigned long c = styles_->get_color_rgb(b.close >= b.open ? 0 : 200, b.close >= b.open ? 150 : 30, b.close >= b.open ? 0 : 30);
                XSetForeground(display_, gc_, c);
                XDrawLine(display_, window_, gc_, cx, y_at(b.high, lo, hi), cx, y_at(b.low, lo, hi));
                int top = y_at(std::max(b.open, b.close), lo, hi);
                int bot = y_at(std::min(b.open, b.close), lo, hi);
                XFillRectangle(display_, window_, gc_, cx - bw / 2, top, bw, std::max(1, bot - top));
            }
        }
        for (auto& ln : lines_) {
            unsigned long col = styles_->get_color_Srgb(ln.color);
            XSetForeground(display_, gc_, col);
            xf::set_line_width(display_, gc_, 2);
            std::vector<XPoint> pts;
            auto flush = [&]() {
                if (pts.size() > 1) XDrawLines(display_, window_, gc_, pts.data(), (int)pts.size(), CoordModeOrigin);
                pts.clear();
            };
            for (int i = 0; i < (int)ln.v.size(); ++i) {
                if (!std::isfinite(ln.v[i])) { flush(); continue; }
                XPoint p; p.x = (short)x_center(i); p.y = (short)y_at(ln.v[i], lo, hi);
                pts.push_back(p);
            }
            flush();
            xf::set_line_width(display_, gc_, 1);
        }
        XSetForeground(display_, gc_, axis_col);
        XDrawLine(display_, window_, gc_, px, py + ph, px + pw, py + ph);
        XDrawRectangle(display_, window_, gc_, px, py, pw, ph);
        {   int lx = px + 8, ly = py + 8;
            for (auto& ln : lines_) {
                XSetForeground(display_, gc_, styles_->get_color_Srgb(ln.color));
                xf::set_line_width(display_, gc_, 2);
                XDrawLine(display_, window_, gc_, lx, ly + 4, lx + 18, ly + 4);
                xf::set_line_width(display_, gc_, 1);
                std::string last;
                for (int i = (int)ln.v.size() - 1; i >= 0; --i)
                    if (std::isfinite(ln.v[i])) { last = fmt(ln.v[i]); break; }
                xf::draw_text_at(display_, window_, gc_, st, lx + 24, ly + 8, ln.name + (last.empty() ? "" : " " + last));
                ly += 16;
            }
            xf::draw_text_at(display_, window_, gc_, st, lx, ly + 8, text_);
        }
        if (cross_valid_ && cross_x_ >= px && cross_x_ <= px + pw && cross_y_ >= py && cross_y_ <= py + ph) {
            XGCValues gv; gv.line_style = LineOnOffDash;
            XChangeGC(display_, gc_, GCLineStyle, &gv);
            XSetForeground(display_, gc_, axis_col);
            XDrawLine(display_, window_, gc_, cross_x_, py, cross_x_, py + ph);
            XDrawLine(display_, window_, gc_, px, cross_y_, px + pw, cross_y_);
            gv.line_style = LineSolid;
            XChangeGC(display_, gc_, GCLineStyle, &gv);
        }
        XSetForeground(display_, gc_, st.lineColor_);
        XDrawRectangle(display_, window_, gc_, x1_, y1_, width(), height());
        XFlush(display_);
    }
    static double XGraph_nice_step(double range, int target) {
        double raw = range / target;
        double mag = std::pow(10.0, std::floor(std::log10(raw)));
        double norm = raw / mag;
        double step = (norm < 1.5) ? 1.0 : (norm < 3.5) ? 2.0 : (norm < 7.5) ? 5.0 : 10.0;
        return step * mag;
    }
    bool on_button_press(int x, int y, unsigned int button) {
        if (!contains(x, y) || !enabled_) return false;
        focused_ = true;
        double frac = plot_w() > 0 ? std::max(0.0, std::min(1.0, (double)(x - plot_x()) / plot_w())) : 0.0;
        if (button == 4) { zoom_ = std::min(8.0, zoom_ * 1.25); return true; }
        if (button == 5) { zoom_ = std::max(1.0, zoom_ / 1.25); return true; }
        if (button == 1) { dragging_ = true; drag_start_x_ = x; drag_start_first_ = view_first_; return true; }
        (void)frac;
        return false;
    }
    bool on_button_release(int x, int y, unsigned int button) {
        (void)x; (void)y;
        if (button == 1 && dragging_) { dragging_ = false; return true; }
        return false;
    }
    bool on_motion(int x, int y) {
        cross_valid_ = true; cross_x_ = x; cross_y_ = y;
        if (dragging_) {
            double total = (double)item_count(), count = visible_count();
            double delta = -(x - drag_start_x_) / item_w();
            view_first_ = std::max(0.0, std::min(std::max(0.0, total - count), drag_start_first_ + delta));
        }
        return true;
    }
    bool on_key(KeySym key, const char* str) {
        (void)str;
        if (!focused_ || !enabled_) return false;
        double total = (double)item_count(), count = visible_count();
        switch (key) {
        case XK_Left: view_first_ = std::max(0.0, view_first_ - count * 0.1); return true;
        case XK_Right: view_first_ = std::min(std::max(0.0, total - count), view_first_ + count * 0.1); return true;
        case XK_plus: case XK_equal: zoom_ = std::min(8.0, zoom_ * 1.25); return true;
        case XK_minus: zoom_ = std::max(1.0, zoom_ / 1.25); return true;
        case XK_Home: case XK_r: case XK_R: reset_view(); return true;
        }
        return false;
    }
};

}  // namespace xframe
'''

MAIN_TMPL = r'''#include "%(CLS)s.h"

#include <cstdio>
#include <unistd.h>

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1,
        BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "%(CLS)s - XFrame demo");
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
    xframe::%(CLS)s widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
    widget.draw();
    // Exercise the interaction handlers headlessly, then redraw.
    widget.on_button_press(400, 300, 1);
    widget.on_motion(420, 310);
    widget.on_button_release(420, 310, 1);
    widget.on_key(XK_Right, "");
    widget.on_button_press(400, 300, 4);
    widget.draw();
    XFlush(display);
    usleep(150000);
    XCloseDisplay(display);
    return 0;
}
'''

CMAKE_TMPL = r'''cmake_minimum_required(VERSION 3.20)

project(%(CLS)s LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(%(CLS)s main_%(CLS)s.cpp)

target_link_libraries(%(CLS)s PRIVATE X11::X11)
'''

def param_names(params):
    names = []
    for part in params.split(","):
        part = part.strip()
        if not part:
            continue
        left = part.split("=")[0].strip()
        names.append(left.split()[-1])
    return names

def param_default(params, idx, fallback):
    parts = params.split(",")
    if idx >= len(parts):
        return fallback
    if "=" in parts[idx]:
        return parts[idx].split("=", 1)[1].strip()
    return fallback

def emit():
    import shutil, hashlib
    core_src = os.path.join(ROOT, "widgets", "core", "xframe_core.h")
    core_bytes = open(core_src, "rb").read()
    names = set()
    for sp in SPEC:
        assert sp["name"] not in names, "duplicate " + sp["name"]
        names.add(sp["name"])
    assert len(SPEC) == 80, len(SPEC)
    cats = {}
    for sp in SPEC:
        cats[sp["cat"]] = cats.get(sp["cat"], 0) + 1
    # list file
    lines = []
    for sp in SPEC:
        lines.append("Ind%s | %s | %s" % (sp["name"], sp["cat"], sp["formula"]))
    with open(os.path.join(TOOLS, "stock_indicators_volatility.txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    # spec json for the test driver
    jspec = []
    for sp in SPEC:
        jspec.append(dict(name=sp["name"], cls="Ind" + sp["name"], func=sp["func"],
                          ret=sp["ret"], params=sp["params"],
                          primary=(sp["outputs"][0][0] if sp["outputs"] else ""),
                          warmup=sp["warmup"], cat=sp["cat"], style=sp["style"],
                          formula=sp["formula"]))
    with open(os.path.join(TOOLS, "stock_indicators_volatility_spec.json"), "w") as f:
        json.dump(jspec, f, indent=1)
    for sp in SPEC:
        cls = "Ind" + sp["name"]
        L = sp["func"][4:]
        d = os.path.join(COMP, cls)
        os.makedirs(os.path.join(d, "core"), exist_ok=True)
        with open(os.path.join(d, "core", "xframe_core.h"), "wb") as f:
            f.write(core_bytes)
        assert hashlib.md5(core_bytes).hexdigest() == hashlib.md5(
            open(os.path.join(ROOT, "widgets", "core", "xframe_core.h"), "rb").read()).hexdigest()
        if sp["ret"] == "vec":
            struct = ""
            ret = "std::vector<double>"
        else:
            fields = "\n".join("    std::vector<double> %s;" % o[0] for o in sp["outputs"])
            struct = "struct %s {\n%s\n};\n" % (sp["ret"], fields)
            ret = sp["ret"]
        args = ", ".join(param_names(sp["params"]))
        lines_code = ["    auto result = %s(series, %s);" % (sp["func"], args)]
        for (field, label, r, g, b) in sp["outputs"]:
            src = "result" if field == "" else "result." + field
            lines_code.append('    lines_.push_back(Line{"%s", Srgb{%d, %d, %d}, %s});' % (label, r, g, b, src))
        hdr = HEADER_TMPL % dict(
            CLS=cls, NAME=sp["name"], CAT=sp["cat"], FORMULA=sp["formula"],
            HELPERS=HELPERS % {"L": L}, STRUCT=struct, RET=ret, FUNC=sp["func"],
            PARAMS=sp["params"], BODY=sp["body"], L=L,
            OVERLAY="true" if sp["style"] == "overlay" else "false",
            PERIOD=param_default(sp["params"], 0, "20"),
            MULT=param_default(sp["params"], 1, "2.0"),
            LINES="\n".join(lines_code))
        with open(os.path.join(d, cls + ".h"), "w") as f:
            f.write(hdr)
        with open(os.path.join(d, "main_" + cls + ".cpp"), "w") as f:
            f.write(MAIN_TMPL % dict(CLS=cls))
        with open(os.path.join(d, "CMakeLists.txt"), "w") as f:
            f.write(CMAKE_TMPL % dict(CLS=cls))
    print("generated %d widgets: %s" % (len(SPEC), cats))

if __name__ == "__main__":
    emit()
