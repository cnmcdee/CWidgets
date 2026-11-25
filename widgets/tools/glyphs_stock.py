#!/usr/bin/env python3
"""Unique 16x16 icons for the Stock Market section (287 widgets).

Covers the 8 OHLC price charts, 15 comparative/volume widgets, 14
drawing/annotation tools, and the top-250 indicators (85 trend, 85
momentum, 80 volatility).  Every glyph is drawn with PIL ImageDraw in
the 16x16 box, in the same base language as tools/gen_icons.py (dark
ink INK + MID on white) plus a per-widget colour: each icon's main
colour is a distinct hue from a golden-angle wheel keyed by its index
in the list below, and its geometry is seeded from the widget name, so
all 287 are visibly different from each other and from the 194 icons
that came before.

Motifs by family: candles/bars for price charts, bars/lines for
comparative and volume widgets, literal shapes for drawing tools,
moving-average lines for trend indicators, oscillator waves for
momentum indicators, bands/range bars for volatility indicators.

The 4 trend indicators whose component directories are not on disk yet
(IndAroonOscillator, IndGatorOscillator, IndQStick, IndTRIX) are still
listed here and still get icons: this module iterates its own DRAW
dict, not the component directories.

Usage: python3 widgets/tools/glyphs_stock.py   (run from the repo root)
Regenerates ONLY these 287 .ico files into widgets/icons/.
"""
import colorsys
import hashlib
import os
import random
import sys
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from sections import SECTION_COLORS, section_of  # noqa: E402

INK = (30, 30, 30)
MID = (140, 140, 140)
LIGHT = (226, 228, 232)
WHITE = (255, 255, 255)
GREEN = (22, 163, 74)
RED = (220, 38, 38)

TREND = ('IndSMA', 'IndEMA', 'IndWMA', 'IndDEMA', 'IndTEMA', 'IndTRIMA', 'IndKAMA', 'IndHMA', 'IndVWMA', 'IndALMA', 'IndZLEMA', 'IndLSMA', 'IndSMMA', 'IndRMA', 'IndMAMA', 'IndFAMA', 'IndFRAMA', 'IndVIDYA', 'IndT3', 'IndMcGinley', 'IndGaussianMA', 'IndSineWMA', 'IndWildersMA', 'IndMedianMA', 'IndTSMA', 'IndTenkanSen', 'IndKijunSen', 'IndSenkouSpanA', 'IndSenkouSpanB', 'IndChikouSpan', 'IndPSAR', 'IndSupertrend', 'IndADX', 'IndPlusDI', 'IndMinusDI', 'IndDX', 'IndADXR', 'IndAroonUp', 'IndAroonDown', 'IndAroonOscillator', 'IndVortex', 'IndAlligatorJaw', 'IndAlligatorTeeth', 'IndAlligatorLips', 'IndGatorOscillator', 'IndTRIX', 'IndDPO', 'IndQStick', 'IndElderRayBull', 'IndElderRayBear', 'IndLinReg', 'IndLinRegSlope', 'IndLinRegAngle', 'IndLinRegIntercept', 'IndTSF', 'IndDonchianUpper', 'IndDonchianMiddle', 'IndDonchianLower', 'IndChandelierExitLong', 'IndChandelierExitShort', 'IndChandeKrollStop', 'IndHalfTrend', 'IndSSLChannel', 'IndUTBot', 'IndPivotPoint', 'IndMAEnvelopeUpper', 'IndMAEnvelopeLower', 'IndTrendIntensityIndex', 'IndVHF', 'IndRWIHigh', 'IndRWILow', 'IndEhlersTrendline', 'IndEhlersDecycler', 'IndGuppyMMA', 'IndZigZag', 'IndFractalUp', 'IndFractalDown', 'IndSwingIndex', 'IndAccumulativeSwingIndex', 'IndFractalDimensionIndex', 'IndRegressionChannel', 'IndMovingAverageRibbon', 'IndEhlersSuperSmoother', 'IndVWEMA', 'IndElderSafeZone')
MOMENTUM = ('IndRSI', 'IndConnorsRSI', 'IndStochRSI', 'IndCutlerRSI', 'IndLaguerreRSI', 'IndAdaptiveRSI', 'IndMACD', 'IndMACDHistogram', 'IndZeroLagMACD', 'IndMACDofRSI', 'IndStochasticFast', 'IndStochasticSlow', 'IndStochasticFull', 'IndKDJ', 'IndWilliamsR', 'IndSMI', 'IndDSS', 'IndCCI', 'IndROC', 'IndROCR', 'IndMomentum', 'IndPPO', 'IndAPO', 'IndAwesomeOscillator', 'IndAcceleratorOscillator', 'IndChandeMomentumOscillator', 'IndTSI', 'IndUltimateOscillator', 'IndKST', 'IndFisherTransform', 'IndInverseFisherTransform', 'IndErgodicOscillator', 'IndSchaffTrendCycle', 'IndRelativeVigorIndex', 'IndElderForceIndex', 'IndElderImpulse', 'IndBullPower', 'IndBearPower', 'IndCyberCycle', 'IndCenterOfGravity', 'IndSineWave', 'IndDominantCycle', 'IndChandeForecastOscillator', 'IndCoppockCurve', 'IndKairi', 'IndDetrendedPriceOscillator', 'IndDeMarker', 'IndBalanceOfPower', 'IndRelativeMomentumIndex', 'IndIntradayMomentumIndex', 'IndDynamicMomentumIndex', 'IndWaveTrend', 'IndPrettyGoodOscillator', 'IndInertia', 'IndRangeExpansionIndex', 'IndTDSequential', 'IndDoubleStochastic', 'IndPriceMomentumOscillator', 'IndSwenlinTradingOscillator', 'IndRSX', 'IndJurikVelocity', 'IndNormalizedROC', 'IndLaguerreFilter', 'IndSuperSmoother', 'IndRainbowOscillator', 'IndLinearRegressionOscillator', 'IndPercentRank', 'IndSpearmanRank', 'IndCorrelationCoefficient', 'IndDecycler', 'IndTrendFlex', 'IndRoofingFilter', 'IndReflex', 'IndEvenBetterSineWave', 'IndStochasticPop', 'IndGannSwingOscillator', 'IndSpecialK', 'IndAdaptiveCyberCycle', 'IndDisparityIndex', 'IndSmoothedROC', 'IndWilliamsAccumulationDistribution', 'IndChoppinessIndex', 'IndGannHiLoActivator', 'IndJurikMovingAverage', 'IndEhlersSmoothedRSI')
VOLATILITY = ('IndBollingerBands', 'IndBollingerPercentB', 'IndBollingerBandwidth', 'IndATR', 'IndNATR', 'IndTrueRange', 'IndKeltnerChannels', 'IndDonchianChannels', 'IndChaikinVolatility', 'IndHistoricalVolatility', 'IndParkinsonVolatility', 'IndGarmanKlassVolatility', 'IndRogersSatchellVolatility', 'IndYangZhangVolatility', 'IndStdDev', 'IndVariance', 'IndChandelierExit', 'IndVolatilityStop', 'IndMassIndex', 'IndUlcerIndex', 'IndRelativeVolatilityIndex', 'IndTTMSqueeze', 'IndGopalakrishnanRangeIndex', 'IndWilderVolatilitySystem', 'IndVolatilityRatio', 'IndCloseVolatility', 'IndHighLowRange', 'IndKeltnerBandwidth', 'IndOBV', 'IndAccumulationDistributionLine', 'IndChaikinMoneyFlow', 'IndMoneyFlowIndex', 'IndVolumeOscillator', 'IndPercentageVolumeOscillator', 'IndVWAP', 'IndVWAPBands', 'IndAnchoredVWAP', 'IndChaikinOscillator', 'IndEaseOfMovement', 'IndVolumeRateOfChange', 'IndVolumePriceTrend', 'IndNegativeVolumeIndex', 'IndPositiveVolumeIndex', 'IndDemandIndex', 'IndKlingerVolumeOscillator', 'IndTwiggsMoneyFlow', 'IndVolumeWeightedMovingAverage', 'IndVolumeZoneOscillator', 'IndRelativeVolume', 'IndVolumeAverage', 'IndMoneyFlowVolume', 'IndVolumeDelta', 'IndCumulativeVolumeDelta', 'IndUpDownVolumeRatio', 'IndIntradayIntensityIndex', 'IndHurstExponent', 'IndFractalDimension', 'IndEfficiencyRatio', 'IndLinearRegressionSlope', 'IndLinearRegressionIntercept', 'IndRSquared', 'IndStandardError', 'IndLinearRegressionForecast', 'IndZScore', 'IndPercentileRank', 'IndMedianPrice', 'IndTypicalPrice', 'IndWeightedClose', 'IndSkewness', 'IndKurtosis', 'IndAutocorrelation', 'IndShannonEntropy', 'IndUpDownRatio', 'IndStandardPivotPoints', 'IndFibonacciPivotPoints', 'IndCamarillaPivotPoints', 'IndWoodiePivotPoints', 'IndDominantCyclePeriod', 'IndCorrelation', 'IndCovariance')
TREND_SET = frozenset(TREND)
MOMENTUM_SET = frozenset(MOMENTUM)
VOLATILITY_SET = frozenset(VOLATILITY)

CHARTS = ("StockCandlestickChart", "StockOHLCBarChart", "StockAreaChart",
          "StockLineChart", "StockHeikinAshi", "StockHollowCandles",
          "StockRenko", "StockBaselineChart")
COMPARE = ("StockVolumeBars", "StockVolumeCandles", "StockVolumeProfile",
           "StockVWAP", "StockOBVChart", "StockMoneyFlowChart",
           "StockAccumulationDistribution", "StockCorrelationView",
           "StockOverlayChart", "StockMultiSymbolChart",
           "StockIndexedComparison", "StockRelativePerformance",
           "StockRatioChart", "StockSectorComparison", "StockSpreadChart")
DRAWING = ("StockTrendline", "StockHorizontalLine", "StockVerticalLine",
           "StockRectangle", "StockEllipse", "StockArrow",
           "StockFibRetracement", "StockFibExtension", "StockPitchfork",
           "StockParallelChannel", "StockBrush", "StockTextAnnotation",
           "StockPriceLabel", "StockMeasureTool")
ALL_STOCK = (list(CHARTS) + list(COMPARE) + list(DRAWING)
             + list(TREND) + list(MOMENTUM) + list(VOLATILITY))
assert len(ALL_STOCK) == len(set(ALL_STOCK)) == 287

# ---------------------------------------------------------------- helpers
# d = ImageDraw, rng = random.Random seeded from the widget name,
# c1/c2 = this widget's two distinct hues, a = section accent (teal).

def _rng(name):
    return random.Random(int(hashlib.sha256(name.encode()).hexdigest()[:12], 16))


def _hue(idx, off=0.0, sat=0.72, val=0.82):
    r, g, b = colorsys.hsv_to_rgb(((idx * 0.618033988749895) + off) % 1.0, sat, val)
    return (int(r * 255), int(g * 255), int(b * 255))


def _light(c):
    return ((c[0] + 255) // 2, (c[1] + 255) // 2, (c[2] + 255) // 2)


def _pts(rng, n=4, lo=3, hi=12):
    xs = [1 + round(i * 13 / (n - 1)) for i in range(n)]
    y = rng.randint(lo + 2, hi - 2)
    ys = [y]
    for _ in range(n - 1):
        y = max(lo, min(hi, y + rng.randint(-4, 4)))
        ys.append(y)
    return list(zip(xs, ys))


def _poly(d, pts, c, w=2):
    d.line(pts, fill=c, width=w, joint="curve")


def _candle(d, x, top, bot, btop, bbot, c, hollow=False):
    d.line([x, top, x, bot], fill=INK)
    d.rectangle([x - 1, btop, x + 1, bbot],
                fill=WHITE if hollow else c, outline=c)


# ---------------------------------------------------------------- charts
def m_candles(d, rng, c1, c2, a):
    for x in (3, 8, 12):
        top = rng.randint(1, 4)
        bot = rng.randint(10, 14)
        btop = rng.randint(top, top + 3)
        bbot = rng.randint(max(btop + 2, bot - 4), bot)
        _candle(d, x, top, bot, btop, bbot, c1 if rng.random() < 0.5 else c2)


def m_ohlc(d, rng, c1, c2, a):
    for i, x in enumerate((3, 8, 12)):
        top = rng.randint(2, 5)
        bot = rng.randint(10, 13)
        c = c1 if i % 2 == 0 else c2
        d.line([x, top, x, bot], fill=c, width=2)
        d.line([x - 2, top + 1, x, top + 1], fill=c, width=2)
        d.line([x, bot - 1, x + 2, bot - 1], fill=c, width=2)


def m_area(d, rng, c1, c2, a):
    pts = _pts(rng)
    d.polygon(pts + [(14, 14), (1, 14)], fill=_light(c1))
    _poly(d, pts, c1)


def m_line(d, rng, c1, c2, a):
    pts = _pts(rng, n=5)
    _poly(d, pts, c1)
    for x, y in pts:
        d.ellipse([x - 1, y - 1, x + 1, y + 1], fill=c2)


def m_heikin(d, rng, c1, c2, a):
    for i, x in enumerate((3, 8, 12)):
        top = rng.randint(1, 4)
        bot = rng.randint(10, 14)
        _candle(d, x, top, bot, top + rng.randint(0, 2),
                bot - rng.randint(0, 2), c1 if i % 2 else c2, hollow=(i == 1))


def m_hollow(d, rng, c1, c2, a):
    for i, x in enumerate((3, 8, 12)):
        top = rng.randint(1, 4)
        bot = rng.randint(10, 14)
        _candle(d, x, top, bot, top + 1, bot - 1,
                c1 if i % 2 == 0 else c2, hollow=True)


def m_renko(d, rng, c1, c2, a):
    x, y = 2, 11
    for i in range(3):
        w = rng.randint(3, 4)
        h = rng.randint(2, 3)
        d.rectangle([x, y - h, x + w, y],
                    fill=c1 if i % 2 == 0 else c2, outline=INK)
        x += w
        y -= h
        if x > 10:
            break


def m_baseline(d, rng, c1, c2, a):
    d.line([1, 8, 14, 8], fill=MID)
    pts = _pts(rng, n=5, lo=2, hi=13)
    _poly(d, pts, c1)
    for x, y in pts:
        d.ellipse([x - 1, y - 1, x + 1, y + 1], fill=c2 if y > 8 else GREEN)


# ------------------------------------------------------- comparative/volume
def m_volbars(d, rng, c1, c2, a):
    d.line([1, 13, 14, 13], fill=INK)
    for i, x in enumerate((2, 5, 8, 11, 13)):
        h = rng.randint(3, 11)
        d.rectangle([x, 13 - h, x + 1, 13], fill=c1 if i % 2 == 0 else c2)


def m_volcandles(d, rng, c1, c2, a):
    for i, x in enumerate((4, 11)):
        top = rng.randint(1, 3)
        bot = rng.randint(6, 8)
        _candle(d, x, top, bot, top + 1, bot - 1, c1 if i == 0 else c2)
        h = rng.randint(2, 4)
        d.rectangle([x - 1, 14 - h, x + 1, 14], fill=c1 if i == 0 else c2)


def m_volprofile(d, rng, c1, c2, a):
    for i, y in enumerate((3, 6, 9, 12)):
        ln = rng.randint(3, 11)
        d.rectangle([1, y, 1 + ln, y + 1], fill=c1 if i % 2 == 0 else c2)
    d.line([13, 1, 13, 14], fill=INK)


def m_vwap(d, rng, c1, c2, a):
    y = rng.randint(5, 9)
    d.line([(1, y), (5, y), (5, y - 2), (9, y - 2), (9, y + 2), (13, y + 2)],
           fill=c1, width=2, joint="curve")
    _poly(d, _pts(rng, n=4), c2)


def m_obv(d, rng, c1, c2, a):
    y = rng.randint(9, 12)
    pts = [(1, y)]
    x = 1
    while x < 13:
        nx = min(14, x + rng.randint(2, 4))
        pts.append((nx, y))
        y = max(2, min(13, y + rng.randint(-3, 2)))
        pts.append((nx, y))
        x = nx
    d.line(pts, fill=c1, width=2, joint="curve")
    d.ellipse([pts[-1][0] - 1, pts[-1][1] - 1, pts[-1][0] + 1, pts[-1][1] + 1],
              fill=c2)


def m_moneyflow(d, rng, c1, c2, a):
    for i, x in enumerate((3, 7, 11)):
        h = rng.randint(4, 9)
        d.rectangle([x, 13 - h, x + 2, 13], fill=c1 if i != 1 else c2)
    d.polygon([(12, 2), (14, 5), (13, 5), (13, 8), (11, 8), (11, 5), (10, 5)],
              fill=c1)


def m_accdist(d, rng, c1, c2, a):
    pts = _pts(rng, n=5, lo=2, hi=9)
    _poly(d, pts, c1)
    for x in (2, 6, 10, 13):
        d.rectangle([x, 12, x + 1, 13], fill=c2)


def m_corr(d, rng, c1, c2, a):
    d.line([2, 13, 13, 2], fill=MID)
    for _ in range(7):
        x = rng.randint(2, 13)
        y = rng.randint(2, 13)
        d.ellipse([x - 1, y - 1, x + 1, y + 1],
                  fill=c1 if rng.random() < 0.5 else c2)


def m_overlay(d, rng, c1, c2, a):
    _poly(d, _pts(rng, n=4), c2)
    _poly(d, _pts(rng, n=4), c1)


def m_multi(d, rng, c1, c2, a):
    _poly(d, _pts(rng, n=4), c2)
    _poly(d, _pts(rng, n=4), c1)
    _poly(d, _pts(rng, n=4), a)


def m_indexed(d, rng, c1, c2, a):
    for c, dy in ((c1, rng.randint(3, 6)), (c2, rng.randint(7, 10)),
                  (a, rng.randint(11, 13))):
        d.line([2, 13, 13, dy], fill=c, width=2)
    d.ellipse([1, 12, 3, 14], fill=INK)


def m_relperf(d, rng, c1, c2, a):
    d.line([1, 7, 14, 7], fill=MID)
    y1 = rng.randint(3, 5)
    y2 = rng.randint(9, 12)
    d.line([(1, 7), (5, y1), (9, y1), (14, rng.randint(2, 4))],
           fill=c1, width=2, joint="curve")
    d.line([(1, 7), (5, y2), (9, y2), (14, rng.randint(11, 13))],
           fill=c2, width=2, joint="curve")


def m_ratio(d, rng, c1, c2, a):
    p1 = _pts(rng, n=4, lo=2, hi=8)
    p2 = _pts(rng, n=4, lo=9, hi=13)
    d.polygon(p1 + list(reversed(p2)), fill=_light(c2))
    _poly(d, p1, c1)
    _poly(d, p2, c2)


def m_sector(d, rng, c1, c2, a):
    for g, x in enumerate((2, 7, 11)):
        h1 = rng.randint(3, 9)
        h2 = rng.randint(3, 9)
        d.rectangle([x, 13 - h1, x + 1, 13], fill=c1)
        d.rectangle([x + 2, 13 - h2, x + 3, 13], fill=c2 if g % 2 else a)


def m_spread(d, rng, c1, c2, a):
    p1 = _pts(rng, n=4, lo=2, hi=7)
    p2 = _pts(rng, n=4, lo=9, hi=13)
    d.polygon(p1 + list(reversed(p2)), fill=_light(c1))
    _poly(d, p1, c1)
    _poly(d, p2, c2)


# ---------------------------------------------------------------- drawing
def m_trendline(d, rng, c1, c2, a):
    y1 = rng.randint(10, 13)
    y2 = rng.randint(2, 5)
    d.line([2, y1, 13, y2], fill=c1, width=2)
    d.ellipse([1, y1 - 1, 3, y1 + 1], fill=c2)
    d.ellipse([12, y2 - 1, 14, y2 + 1], fill=c2)


def m_hline(d, rng, c1, c2, a):
    y = rng.randint(4, 11)
    d.line([1, y, 14, y], fill=c1, width=2)
    d.line([4, y - 2, 4, y + 2], fill=c2)
    d.line([11, y - 2, 11, y + 2], fill=c2)


def m_vline(d, rng, c1, c2, a):
    x = rng.randint(4, 11)
    d.line([x, 1, x, 14], fill=c1, width=2)
    d.line([x - 2, 4, x + 2, 4], fill=c2)
    d.line([x - 2, 11, x + 2, 11], fill=c2)


def m_rect(d, rng, c1, c2, a):
    x0 = rng.randint(1, 3)
    y0 = rng.randint(2, 4)
    d.rectangle([x0, y0, 15 - x0, 15 - y0], outline=c1, width=2)
    d.line([x0, y0, 15 - x0, 15 - y0], fill=c2)


def m_ellipse(d, rng, c1, c2, a):
    d.ellipse([2, 2, 13, 13], outline=c1, width=2)
    d.ellipse([7, 7, 8, 8], fill=c2)


def m_arrow(d, rng, c1, c2, a):
    y = rng.randint(6, 9)
    d.polygon([(2, y - 1), (10, y - 1), (10, y - 3), (14, y), (10, y + 3),
               (10, y + 1), (2, y + 1)], fill=c1, outline=INK)


def m_fibret(d, rng, c1, c2, a):
    d.line([2, 13, 13, 2], fill=MID)
    for i, f in enumerate((0.236, 0.382, 0.618)):
        y = round(2 + f * 11)
        d.line([2, y, 13, y], fill=(c1, c2, a)[i])


def m_fibext(d, rng, c1, c2, a):
    d.line([2, 12, 9, 5], fill=c1, width=2)
    d.line([9, 5, 12, 8], fill=c2, width=2)
    d.line([12, 8, 14, 3], fill=a, width=2)
    d.point((9, 5), fill=INK)


def m_pitchfork(d, rng, c1, c2, a):
    d.line([2, 13, 7, 7], fill=c1, width=2)
    d.line([7, 7, 13, 2], fill=c2, width=2)
    d.line([7, 7, 13, 7], fill=c1, width=2)
    d.line([7, 7, 13, 12], fill=a, width=2)
    d.ellipse([6, 6, 8, 8], fill=INK)


def m_channel(d, rng, c1, c2, a):
    d.line([2, 12, 13, 4], fill=c1, width=2)
    d.line([2, 8, 13, 1], fill=c2, width=2)
    d.line([2, 10, 13, 2], fill=MID)


def m_brush(d, rng, c1, c2, a):
    d.line([(2, 12), (5, 8), (8, 10), (11, 5), (14, 7)],
           fill=c1, width=3, joint="curve")
    d.ellipse([12, 5, 14, 7], fill=c2)


def m_textann(d, rng, c1, c2, a):
    d.line([7, 3, 4, 11], fill=c1, width=2)
    d.line([7, 3, 10, 11], fill=c1, width=2)
    d.line([5, 8, 9, 8], fill=c1, width=2)
    d.line([2, 13, 13, 13], fill=c2, width=2)


def m_pricelabel(d, rng, c1, c2, a):
    y = rng.randint(5, 8)
    d.polygon([(2, y - 3), (10, y - 3), (10, y + 3), (2, y + 3)], fill=c1)
    d.polygon([(10, y - 3), (14, y), (10, y + 3)], fill=c2)
    d.line([4, y, 8, y], fill=WHITE, width=2)


def m_measure(d, rng, c1, c2, a):
    y = rng.randint(6, 9)
    d.line([2, y, 13, y], fill=c1, width=2)
    for x in (2, 5, 8, 11, 13):
        d.line([x, y - 2, x, y + 2], fill=INK)
    d.polygon([(2, y), (4, y - 2), (4, y + 2)], fill=c2)
    d.polygon([(13, y), (11, y - 2), (11, y + 2)], fill=c2)


# -------------------------------------------------------------- indicators
def m_trend_ma(d, rng, c1, c2, a):
    d.line(_pts(rng, n=5), fill=MID, width=1)
    pts = _pts(rng, n=4)
    _poly(d, pts, c1)
    for x, y in pts:
        d.ellipse([x - 1, y - 1, x + 1, y + 1], fill=c2)


def m_trend_channel(d, rng, c1, c2, a):
    p1 = _pts(rng, n=4, lo=2, hi=8)
    p2 = _pts(rng, n=4, lo=9, hi=13)
    d.polygon(p1 + list(reversed(p2)), fill=_light(c1))
    _poly(d, p1, c1)
    _poly(d, p2, c2)


def m_trend_ribbon(d, rng, c1, c2, a):
    base = _pts(rng, n=4)
    for j, c in enumerate((c1, c2, a)):
        pts = [(x, max(1, min(14, y + (j - 1) * 2))) for x, y in base]
        _poly(d, pts, c)


def m_mom_osc(d, rng, c1, c2, a):
    d.line([1, 8, 14, 8], fill=MID)
    d.line([1, 3, 14, 3], fill=_light(c2))
    d.line([1, 13, 14, 13], fill=_light(c2))
    pts = _pts(rng, n=6, lo=2, hi=14)
    _poly(d, pts, c1)
    d.ellipse([pts[0][0] - 1, pts[0][1] - 1, pts[0][0] + 1, pts[0][1] + 1],
              fill=c2)


def m_mom_hist(d, rng, c1, c2, a):
    d.line([1, 8, 14, 8], fill=INK)
    for i, x in enumerate((2, 5, 8, 11, 13)):
        h = rng.randint(2, 6)
        if rng.random() < 0.5:
            d.rectangle([x, 8 - h, x + 1, 8], fill=c1)
        else:
            d.rectangle([x, 8, x + 1, 8 + h], fill=c2)


def m_mom_band(d, rng, c1, c2, a):
    d.line([1, 4, 14, 4], fill=c2)
    d.line([1, 12, 14, 12], fill=c2)
    _poly(d, _pts(rng, n=5, lo=4, hi=12), c1)


def m_vol_bands(d, rng, c1, c2, a):
    mid = _pts(rng, n=4, lo=6, hi=10)
    up = [(x, max(1, y - rng.randint(3, 5))) for x, y in mid]
    lo = [(x, min(14, y + rng.randint(3, 5))) for x, y in mid]
    d.polygon(up + list(reversed(lo)), fill=_light(c1))
    _poly(d, up, c1)
    _poly(d, lo, c2)
    _poly(d, mid, a)


def m_vol_range(d, rng, c1, c2, a):
    for i, x in enumerate((3, 8, 12)):
        top = rng.randint(2, 5)
        bot = rng.randint(10, 13)
        c = c1 if i % 2 == 0 else c2
        d.line([x, top, x, bot], fill=c, width=2)
        d.line([x - 2, top, x + 2, top], fill=c, width=2)
        d.line([x - 2, bot, x + 2, bot], fill=c, width=2)


def m_vol_atr(d, rng, c1, c2, a):
    y = rng.randint(6, 10)
    pts = [(1, y)]
    x = 1
    while x < 13:
        nx = min(14, x + rng.randint(3, 5))
        pts.append((nx, y))
        y = max(3, min(12, y + rng.randint(-2, 2)))
        pts.append((nx, y))
        x = nx
    d.line(pts, fill=c1, width=2, joint="curve")
    for px, py in pts[::2]:
        d.ellipse([px - 1, py - 1, px + 1, py + 1], fill=c2)


# ---------------------------------------------------------------- dispatch
CHART_MOTIFS = {
    "StockCandlestickChart": m_candles, "StockOHLCBarChart": m_ohlc,
    "StockAreaChart": m_area, "StockLineChart": m_line,
    "StockHeikinAshi": m_heikin, "StockHollowCandles": m_hollow,
    "StockRenko": m_renko, "StockBaselineChart": m_baseline,
}
COMPARE_MOTIFS = {
    "StockVolumeBars": m_volbars, "StockVolumeCandles": m_volcandles,
    "StockVolumeProfile": m_volprofile, "StockVWAP": m_vwap,
    "StockOBVChart": m_obv, "StockMoneyFlowChart": m_moneyflow,
    "StockAccumulationDistribution": m_accdist,
    "StockCorrelationView": m_corr, "StockOverlayChart": m_overlay,
    "StockMultiSymbolChart": m_multi, "StockIndexedComparison": m_indexed,
    "StockRelativePerformance": m_relperf, "StockRatioChart": m_ratio,
    "StockSectorComparison": m_sector, "StockSpreadChart": m_spread,
}
DRAWING_MOTIFS = {
    "StockTrendline": m_trendline, "StockHorizontalLine": m_hline,
    "StockVerticalLine": m_vline, "StockRectangle": m_rect,
    "StockEllipse": m_ellipse, "StockArrow": m_arrow,
    "StockFibRetracement": m_fibret, "StockFibExtension": m_fibext,
    "StockPitchfork": m_pitchfork, "StockParallelChannel": m_channel,
    "StockBrush": m_brush, "StockTextAnnotation": m_textann,
    "StockPriceLabel": m_pricelabel, "StockMeasureTool": m_measure,
}
TREND_MOTIFS = (m_trend_ma, m_trend_channel, m_trend_ribbon)
MOMENTUM_MOTIFS = (m_mom_osc, m_mom_hist, m_mom_band)
VOLATILITY_MOTIFS = (m_vol_bands, m_vol_range, m_vol_atr)


def _motif_for(name, rng):
    if name in CHART_MOTIFS:
        return CHART_MOTIFS[name]
    if name in COMPARE_MOTIFS:
        return COMPARE_MOTIFS[name]
    if name in DRAWING_MOTIFS:
        return DRAWING_MOTIFS[name]
    if name in TREND_SET:
        return TREND_MOTIFS[rng.randrange(3)]
    if name in MOMENTUM_SET:
        return MOMENTUM_MOTIFS[rng.randrange(3)]
    if name in VOLATILITY_SET:
        return VOLATILITY_MOTIFS[rng.randrange(3)]
    raise KeyError(name)


def _make(name, idx):
    def draw(d, a):
        rng = _rng(name)
        motif = _motif_for(name, rng)
        motif(d, rng, _hue(idx), _hue(idx, off=0.45, sat=0.65, val=0.75), a)
    return draw


DRAW = {}
for _i, _n in enumerate(ALL_STOCK):
    DRAW[_n] = _make(_n, _i)


def main():
    outdir = os.path.join(ROOT, "icons")
    os.makedirs(outdir, exist_ok=True)
    for name in sorted(DRAW):
        accent = SECTION_COLORS[section_of(name)]
        img = Image.new("RGB", (16, 16), WHITE)
        d = ImageDraw.Draw(img)
        DRAW[name](d, accent)
        path = os.path.join(outdir, name + ".ico")
        # BMP-format ICO (32bpp, uncompressed), exactly like gen_icons.py.
        img.save(path, format="ICO", sizes=[(16, 16)], bitmap_format="bmp")
    print(f"icons written: {len(DRAW)} in {outdir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
