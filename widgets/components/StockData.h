// StockData.h — shared OHLCV series type and deterministic seeded synthetic
// data for the XFrame Stock Market widget section.
//
// No network, no live market data, no API keys: demos and tests run on
// synthetic data generated here, so results are reproducible.
// Attribution: XFrame.
#pragma once

#include <cmath>
#include <cstdint>
#include <vector>

namespace xframe {

// One OHLCV bar. timestamp is seconds since the epoch (daily bars).
struct OHLCV {
    double open = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    double volume = 0.0;
    std::int64_t timestamp = 0;
};

using OHLCVSeries = std::vector<OHLCV>;

namespace detail {
// xorshift64*: small, fast, deterministic on every platform.
inline std::uint64_t stock_next(std::uint64_t &state) {
    std::uint64_t x = state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    state = x;
    return x * 0x2545F4914F6CDD1DULL;
}
// Uniform double in [0, 1).
inline double stock_uniform(std::uint64_t &state) {
    return (stock_next(state) >> 11) * (1.0 / 9007199254740992.0);
}
}  // namespace detail

// Deterministic seeded synthetic OHLCV series: a random walk starting at
// 100.0, daily bars from 2026-01-01 UTC. Same (n, seed) gives the identical
// series on every run and every platform.
inline OHLCVSeries make_seeded_series(int n, unsigned seed = 42) {
    OHLCVSeries out;
    if (n <= 0) return out;
    out.reserve(static_cast<std::size_t>(n));
    std::uint64_t state = 0x9E3779B97F4A7C15ULL ^
        (static_cast<std::uint64_t>(seed) * 0xBF58476D1CE4E5B9ULL);
    if (state == 0) state = 0x2545F4914F6CDD1DULL;
    const std::int64_t day0 = 1767225600;  // 2026-01-01 00:00:00 UTC
    double prev_close = 100.0;
    for (int i = 0; i < n; ++i) {
        const double open = prev_close;
        const double shock = (detail::stock_uniform(state) - 0.5) * 2.4;
        double close = open + 0.02 + shock;
        if (!(close > 1.0)) close = 1.0 + detail::stock_uniform(state);
        const double high = (open > close ? open : close) +
            detail::stock_uniform(state) * 0.9;
        const double low = (open < close ? open : close) -
            detail::stock_uniform(state) * 0.9;
        const double volume =
            500000.0 + detail::stock_uniform(state) * 1500000.0;
        out.push_back(OHLCV{open, high, low, close, volume,
                            day0 + static_cast<std::int64_t>(i) * 86400});
        prev_close = close;
    }
    return out;
}

inline std::vector<double> series_opens(const OHLCVSeries &s) {
    std::vector<double> v;
    v.reserve(s.size());
    for (const auto &b : s) v.push_back(b.open);
    return v;
}

inline std::vector<double> series_highs(const OHLCVSeries &s) {
    std::vector<double> v;
    v.reserve(s.size());
    for (const auto &b : s) v.push_back(b.high);
    return v;
}

inline std::vector<double> series_lows(const OHLCVSeries &s) {
    std::vector<double> v;
    v.reserve(s.size());
    for (const auto &b : s) v.push_back(b.low);
    return v;
}

inline std::vector<double> series_closes(const OHLCVSeries &s) {
    std::vector<double> v;
    v.reserve(s.size());
    for (const auto &b : s) v.push_back(b.close);
    return v;
}

inline std::vector<double> series_volumes(const OHLCVSeries &s) {
    std::vector<double> v;
    v.reserve(s.size());
    for (const auto &b : s) v.push_back(b.volume);
    return v;
}

// Typical price (high + low + close) / 3, one value per bar.
inline std::vector<double> series_typical(const OHLCVSeries &s) {
    std::vector<double> v;
    v.reserve(s.size());
    for (const auto &b : s) v.push_back((b.high + b.low + b.close) / 3.0);
    return v;
}

}  // namespace xframe
