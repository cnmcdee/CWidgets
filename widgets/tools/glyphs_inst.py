#!/usr/bin/env python3
"""Unique 16x16 icons for the Instrumentation section (54 widgets).

Covers the 8 tank/level widgets, 12 switches, 12 lamps/LEDs, 8 gauges,
4 panel-furniture widgets, 6 old-style 7-segment LED digit displays
(1-6 digits) and 4 analog needle types.  Every glyph is drawn with PIL
ImageDraw in the 16x16 box, in the same base language as
tools/gen_icons.py (dark ink INK + MID on white) plus the amber section
accent and 1-2 bright colours per glyph, so all 54 are visibly different
from each other and from the 481 icons that came before.  No
photorealism, no photo textures: plain hand-drawn widget glyphs.

Motifs by family: tanks with a fill line for level, tilted rockers /
levers for switches, glowing dots for lamps, dials with needles for
gauges, plates/bezels for panel furniture, red 7-segment digits (digit
count = widget) for the LED displays, and four distinct needle shapes
(tapered, knife-edge, edgewise, VU) for needles.

Usage: python3 widgets/tools/glyphs_inst.py   (run from the repo root)
Regenerates ONLY these 54 .ico files into widgets/icons/.
"""
import colorsys
import os
import sys
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from sections import SECTION_COLORS, section_of  # noqa: E402

INK = (30, 30, 30)
MID = (140, 140, 140)
LIGHT = (226, 228, 232)
WHITE = (255, 255, 255)
RED = (220, 38, 38)
GREEN = (22, 163, 74)
BLUE = (37, 99, 235)
AMBER = (245, 158, 11)
YELLOW = (234, 179, 8)
CYAN = (8, 145, 178)
ACCENT = (217, 119, 6)  # amber section accent (fallback until the
# Instrumentation section lands in sections.py)

GROUP_A = ("InstVerticalTankLevel", "InstHorizontalTankLevel",
           "InstSphericalTankLevel", "InstSightGlassLevel",
           "InstLevelColumnGauge", "InstTankLevelAlarms",
           "InstOpenChannelLevel", "InstLevelTransmitter")
GROUP_B = ("InstRockerSwitch", "InstRockerSwitchIlluminated",
           "InstRockerSwitchDouble", "InstToggleSwitch",
           "InstToggleSwitchIlluminated", "InstPushButton",
           "InstPushButtonLatching", "InstEmergencyStop",
           "InstRotarySelector", "InstKeySwitch", "InstSlideSwitch",
           "InstCircuitBreaker")
GROUP_C = ("InstPanelLampRed", "InstPanelLampGreen", "InstPanelLampAmber",
           "InstPanelLampBlue", "InstPanelLampWhite", "InstLedRed",
           "InstLedGreen", "InstLedBlue", "InstRgbStatusLed",
           "InstStackLight", "InstBeacon", "InstLampTestButton")
GROUP_D = ("InstAnalogGauge", "InstPressureGauge", "InstTemperatureGauge",
           "InstBarMeter", "InstVuMeter", "InstDigitalReadout",
           "InstManometer", "InstFlowMeter")
GROUP_E = ("InstPanelPlate", "InstNameplate", "InstBezel",
           "InstControlPanel")
GROUP_F = ("InstLedDigits1", "InstLedDigits2", "InstLedDigits3",
           "InstLedDigits4", "InstLedDigits5", "InstLedDigits6",
           "InstNeedleTapered", "InstNeedleKnifeEdge", "InstNeedleEdgewise",
           "InstNeedleVu")
ALL_INST = (list(GROUP_A) + list(GROUP_B) + list(GROUP_C)
            + list(GROUP_D) + list(GROUP_E) + list(GROUP_F))
assert len(ALL_INST) == len(set(ALL_INST)) == 54


# ---------------------------------------------------------------- helpers
def _hue(idx, off=0.0, sat=0.72, val=0.82):
    # Golden-angle wheel like glyphs_stock, shifted so these hues do not
    # repeat the stock wheel's first 54 entries.
    r, g, b = colorsys.hsv_to_rgb(((idx * 0.618033988749895) + 0.13 + off) % 1.0,
                                  sat, val)
    return (int(r * 255), int(g * 255), int(b * 255))


def _light(c):
    return ((c[0] + 255) // 2, (c[1] + 255) // 2, (c[2] + 255) // 2)


def _dial(d, cx=7, cy=7, r=6):
    d.ellipse([cx - r, cy - r, cx + r, cy + r], outline=INK, fill=WHITE)


# ---------------------------------------------------------------- Group A: tanks / level
def m_vtank(d, c1, c2, a):
    d.rectangle([4, 1, 11, 13], outline=INK, fill=WHITE)
    d.rectangle([5, 7, 10, 12], fill=BLUE)
    d.line([5, 7, 10, 7], fill=c1, width=1)
    d.rectangle([6, 0, 9, 1], fill=c2)  # vent cap
    d.line([11, 11, 14, 11], fill=INK)  # outlet pipe
    d.line([5, 13, 4, 15], fill=INK)
    d.line([10, 13, 11, 15], fill=INK)


def m_htank(d, c1, c2, a):
    d.rounded_rectangle([1, 5, 14, 11], radius=3, outline=INK, fill=WHITE)
    d.rectangle([2, 8, 13, 10], fill=BLUE)
    d.line([2, 8, 13, 8], fill=c1, width=1)
    d.line([7, 2, 7, 5], fill=INK)  # top pipe
    d.rectangle([6, 1, 8, 2], fill=c2)
    d.line([4, 11, 4, 13], fill=INK)
    d.line([11, 11, 11, 13], fill=INK)


def m_sph_tank(d, c1, c2, a):
    d.ellipse([2, 2, 13, 13], outline=INK, fill=WHITE)
    d.pieslice([2, 2, 13, 13], 0, 180, fill=BLUE)  # lower half = liquid
    d.ellipse([2, 2, 13, 13], outline=INK)
    d.line([7, 9, 7, 7], fill=c1, width=1)
    d.line([4, 12, 3, 15], fill=INK)
    d.line([11, 12, 12, 15], fill=INK)
    d.line([7, 0, 7, 2], fill=INK)


def m_sight(d, c1, c2, a):
    d.line([2, 1, 2, 14], fill=INK)  # tank wall
    d.line([2, 4, 6, 4], fill=INK)  # top connector
    d.line([2, 11, 6, 11], fill=INK)  # bottom connector
    d.rectangle([6, 1, 9, 14], outline=INK, fill=WHITE)
    d.rectangle([7, 8, 8, 13], fill=BLUE)
    for y in (3, 6, 9, 12):  # scale ticks
        d.line([10, y, 11, y], fill=MID)
    d.line([10, 9, 11, 9], fill=c1, width=1)


def m_colgauge(d, c1, c2, a):
    d.rectangle([5, 1, 10, 14], outline=INK, fill=WHITE)
    d.rectangle([6, 9, 9, 13], fill=c1)
    d.line([6, 9, 9, 9], fill=a, width=1)
    for y in (2, 5, 8, 11, 14):
        d.line([3, y, 4, y], fill=MID)
    d.line([3, 4, 4, 4], fill=RED)  # high-alarm tick
    d.rectangle([5, 0, 10, 1], fill=c2)


def m_tankalarms(d, c1, c2, a):
    d.rectangle([1, 4, 9, 13], outline=INK, fill=WHITE)
    d.rectangle([2, 9, 8, 12], fill=BLUE)
    d.line([2, 9, 8, 9], fill=c1, width=1)
    d.ellipse([11, 1, 14, 4], fill=RED, outline=INK)  # high alarm
    d.ellipse([11, 7, 14, 10], fill=AMBER, outline=INK)  # low alarm
    d.line([9, 2, 11, 2], fill=INK)
    d.line([9, 8, 11, 8], fill=INK)


def m_openchan(d, c1, c2, a):
    d.line([1, 3, 1, 13], fill=INK)  # channel walls
    d.line([14, 3, 14, 13], fill=INK)
    d.line([1, 13, 14, 13], fill=INK)
    d.rectangle([2, 9, 13, 12], fill=_light(BLUE))
    d.line([(2, 8), (5, 7), (8, 8), (11, 7), (13, 8)], fill=BLUE, width=2)
    d.line([4, 1, 10, 1], fill=c1, width=1)  # flow arrow shaft
    d.polygon([(10, 0), (13, 1), (10, 2)], fill=c1)


def m_txtrans(d, c1, c2, a):
    d.rectangle([1, 5, 10, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 10, 9, 13], fill=BLUE)
    d.line([2, 10, 9, 10], fill=c1, width=1)
    d.rectangle([11, 2, 14, 6], outline=INK, fill=c2)  # transmitter box
    d.line([12, 6, 12, 8], fill=INK)
    d.line([11, 4, 10, 7], fill=INK)
    d.point((12, 4), fill=WHITE)
    d.point((13, 4), fill=WHITE)


# ---------------------------------------------------------------- Group B: switches
def m_rocker(d, c1, c2, a):
    d.rectangle([2, 2, 13, 13], outline=INK, fill=LIGHT)
    d.polygon([(4, 4), (11, 3), (12, 11), (5, 12)], fill=c1, outline=INK)
    d.line([4, 7, 11, 7], fill=INK)


def m_rocker_ill(d, c1, c2, a):
    d.rectangle([2, 2, 13, 13], outline=INK, fill=LIGHT)
    d.polygon([(4, 4), (11, 3), (12, 11), (5, 12)], fill=c1, outline=INK)
    d.ellipse([6, 4, 9, 7], fill=AMBER, outline=INK)  # lit lens


def m_rocker_dbl(d, c1, c2, a):
    d.rectangle([1, 3, 14, 12], outline=INK, fill=LIGHT)
    d.rectangle([2, 4, 6, 11], fill=c1, outline=INK)
    d.rectangle([9, 4, 13, 11], fill=c2, outline=INK)
    d.line([7, 3, 7, 12], fill=INK)


def m_toggle(d, c1, c2, a):
    d.rectangle([4, 10, 11, 13], outline=INK, fill=LIGHT)
    d.line([7, 10, 10, 3], fill=INK, width=2)
    d.ellipse([9, 1, 12, 4], fill=c1, outline=INK)


def m_toggle_ill(d, c1, c2, a):
    d.rectangle([4, 10, 11, 13], outline=INK, fill=LIGHT)
    d.line([7, 10, 10, 3], fill=INK, width=2)
    d.ellipse([9, 1, 12, 4], fill=AMBER, outline=RED)
    d.point((11, 0), fill=AMBER)
    d.point((13, 2), fill=AMBER)


def m_push(d, c1, c2, a):
    d.ellipse([2, 2, 13, 13], outline=INK, fill=LIGHT)
    d.ellipse([4, 4, 11, 11], fill=c1, outline=INK)
    d.point((5, 5), fill=WHITE)
    d.point((6, 5), fill=WHITE)


def m_push_latch(d, c1, c2, a):
    d.rectangle([2, 2, 13, 13], outline=INK, fill=LIGHT)
    d.rectangle([4, 4, 11, 11], fill=c2, outline=INK)
    d.line([4, 4, 11, 4], fill=WHITE)  # pressed-in top edge
    d.rectangle([13, 6, 14, 9], fill=AMBER)  # latch tab
    d.line([5, 7, 10, 7], fill=INK)


def m_estop(d, c1, c2, a):
    d.rectangle([2, 11, 13, 14], fill=YELLOW, outline=INK)  # yellow base
    d.rectangle([6, 8, 9, 11], fill=RED, outline=INK)  # stem
    d.ellipse([4, 3, 11, 8], fill=RED, outline=INK)  # mushroom head
    d.point((5, 4), fill=WHITE)
    d.point((6, 4), fill=WHITE)


def m_rotary(d, c1, c2, a):
    _dial(d)
    d.line([7, 7, 11, 3], fill=c1, width=2)  # pointer
    for x, y in ((7, 2), (12, 7), (7, 12), (2, 7)):
        d.point((x, y), fill=MID)
    d.ellipse([6, 6, 8, 8], fill=INK)


def m_keysw(d, c1, c2, a):
    _dial(d, r=5)
    d.rectangle([5, 0, 9, 3], fill=c1, outline=INK)  # key bow
    d.line([7, 4, 7, 9], fill=INK, width=2)  # key blade / slot
    d.line([7, 9, 9, 9], fill=INK)
    d.point((12, 7), fill=RED)


def m_slide(d, c1, c2, a):
    d.rectangle([1, 6, 14, 9], outline=INK, fill=WHITE)
    d.rectangle([8, 4, 11, 11], fill=c1, outline=INK)
    d.line([3, 5, 3, 5], fill=MID)
    d.point((3, 12), fill=MID)
    d.point((12, 12), fill=MID)
    d.line([9, 6, 10, 6], fill=WHITE)


def m_breaker(d, c1, c2, a):
    d.rectangle([3, 1, 12, 14], outline=INK, fill=WHITE)
    d.line([7, 8, 10, 3], fill=c1, width=2)  # lever
    d.ellipse([9, 2, 11, 4], fill=c1, outline=INK)
    d.rectangle([6, 0, 9, 1], fill=MID)  # terminals
    d.rectangle([6, 14, 9, 15], fill=MID)
    d.ellipse([10, 11, 12, 13], fill=RED)  # trip indicator


# ---------------------------------------------------------------- Group C: lamps / LEDs
def _panel_lamp(d, colour):
    d.ellipse([2, 2, 13, 13], outline=INK, fill=MID)  # bezel
    d.ellipse([4, 4, 11, 11], fill=colour, outline=INK)
    d.point((5, 5), fill=WHITE)
    d.point((6, 5), fill=WHITE)
    d.rectangle([6, 13, 9, 15], fill=MID, outline=INK)  # mount


def m_lamp_red(d, c1, c2, a):
    _panel_lamp(d, RED)


def m_lamp_green(d, c1, c2, a):
    _panel_lamp(d, GREEN)


def m_lamp_amber(d, c1, c2, a):
    _panel_lamp(d, AMBER)


def m_lamp_blue(d, c1, c2, a):
    _panel_lamp(d, BLUE)


def m_lamp_white(d, c1, c2, a):
    d.ellipse([2, 2, 13, 13], outline=INK, fill=MID)
    d.ellipse([4, 4, 11, 11], fill=LIGHT, outline=INK)
    d.ellipse([5, 5, 10, 10], fill=WHITE, outline=c1)
    d.rectangle([6, 13, 9, 15], fill=MID, outline=INK)


def _led(d, colour):
    d.rectangle([5, 4, 10, 9], fill=colour, outline=INK)
    d.point((6, 5), fill=WHITE)
    d.line([6, 9, 6, 13], fill=INK)
    d.line([9, 9, 9, 13], fill=INK)
    d.line([5, 14, 10, 14], fill=MID)


def m_led_red(d, c1, c2, a):
    _led(d, RED)


def m_led_green(d, c1, c2, a):
    _led(d, GREEN)


def m_led_blue(d, c1, c2, a):
    _led(d, BLUE)


def m_rgb_led(d, c1, c2, a):
    d.ellipse([1, 4, 4, 7], fill=RED, outline=INK)
    d.ellipse([6, 4, 9, 7], fill=GREEN, outline=INK)
    d.ellipse([11, 4, 14, 7], fill=BLUE, outline=INK)
    d.line([2, 8, 2, 12], fill=INK)
    d.line([7, 8, 7, 12], fill=INK)
    d.line([12, 8, 12, 12], fill=INK)
    d.rectangle([1, 13, 14, 14], fill=c1)


def m_stacklight(d, c1, c2, a):
    d.rectangle([5, 1, 10, 4], fill=RED, outline=INK)
    d.rectangle([5, 5, 10, 8], fill=AMBER, outline=INK)
    d.rectangle([5, 9, 10, 12], fill=GREEN, outline=INK)
    d.rectangle([4, 13, 11, 14], fill=MID, outline=INK)
    d.line([7, 14, 7, 15], fill=INK)


def m_beacon(d, c1, c2, a):
    d.pieslice([3, 4, 12, 12], 180, 360, fill=AMBER, outline=INK)
    d.rectangle([2, 11, 13, 14], fill=MID, outline=INK)
    d.line([7, 0, 7, 2], fill=AMBER)
    d.line([2, 2, 4, 4], fill=AMBER)
    d.line([13, 2, 11, 4], fill=AMBER)


def m_lamptest(d, c1, c2, a):
    d.ellipse([4, 1, 11, 8], fill=AMBER, outline=INK)
    d.point((5, 2), fill=WHITE)
    d.rectangle([5, 10, 10, 14], fill=c1, outline=INK)
    d.line([7, 9, 7, 10], fill=INK)


# ---------------------------------------------------------------- Group D: gauges
def m_analog(d, c1, c2, a):
    _dial(d)
    d.arc([3, 3, 12, 12], 180, 360, fill=c1, width=1)
    d.line([7, 7, 11, 3], fill=RED, width=2)
    d.line([3, 7, 4, 7], fill=MID)
    d.line([11, 7, 12, 7], fill=MID)
    d.ellipse([6, 6, 8, 8], fill=INK)


def m_pressure(d, c1, c2, a):
    d.ellipse([1, 1, 14, 14], outline=INK, width=2, fill=WHITE)
    d.arc([3, 3, 12, 12], 270, 360, fill=RED, width=2)  # red zone
    d.line([7, 7, 4, 10], fill=c1, width=2)
    d.ellipse([6, 6, 8, 8], fill=INK)
    d.line([7, 14, 7, 15], fill=INK, width=2)


def m_temp(d, c1, c2, a):
    _dial(d, r=5)
    d.arc([3, 3, 12, 12], 180, 270, fill=RED, width=2)
    d.line([7, 7, 10, 4], fill=INK, width=2)
    d.ellipse([6, 11, 9, 14], fill=RED)  # bulb
    d.line([7, 7, 7, 11], fill=RED, width=2)


def m_barmeter(d, c1, c2, a):
    d.rectangle([3, 1, 12, 14], outline=INK, fill=WHITE)
    for y, col in ((12, GREEN), (10, GREEN), (8, AMBER),
                   (6, AMBER), (4, RED), (2, LIGHT)):
        d.rectangle([4, y, 11, y + 1], fill=col, outline=INK)
    d.line([1, 2, 2, 2], fill=MID)
    d.line([1, 8, 2, 8], fill=MID)
    d.line([1, 14, 2, 14], fill=MID)


def m_vumeter(d, c1, c2, a):
    d.rectangle([1, 3, 14, 12], outline=INK, fill=WHITE)
    d.arc([3, 4, 12, 13], 180, 360, fill=c1)
    d.arc([3, 4, 12, 13], 300, 360, fill=RED, width=2)
    d.line([7, 11, 10, 5], fill=RED, width=2)
    d.line([3, 13, 12, 13], fill=MID)
    d.point((7, 11), fill=INK)


def m_digread(d, c1, c2, a):
    d.rectangle([1, 4, 14, 11], outline=INK, fill=(20, 20, 20))
    _seg7(d, 3, 5, 3, 5, "1", GREEN, (10, 40, 20))
    _seg7(d, 7, 5, 3, 5, "2", GREEN, (10, 40, 20))
    _seg7(d, 11, 5, 3, 5, "5", GREEN, (10, 40, 20))
    d.point((6, 9), fill=GREEN)


def m_manometer(d, c1, c2, a):
    d.line([3, 2, 3, 13], fill=INK)
    d.line([5, 2, 5, 12], fill=INK)
    d.line([10, 2, 10, 12], fill=INK)
    d.line([12, 2, 12, 13], fill=INK)
    d.line([3, 13, 12, 13], fill=INK)
    d.line([4, 7, 4, 12], fill=BLUE, width=2)  # liquid, high left
    d.line([11, 10, 11, 12], fill=BLUE, width=2)  # liquid, low right
    d.line([4, 12, 11, 12], fill=BLUE, width=2)
    d.line([1, 4, 2, 4], fill=c1)
    d.line([1, 8, 2, 8], fill=MID)


def m_flowmeter(d, c1, c2, a):
    d.rectangle([1, 6, 14, 9], outline=INK, fill=LIGHT)
    d.line([3, 7, 10, 7], fill=BLUE, width=2)
    d.polygon([(10, 5), (13, 7), (10, 9)], fill=BLUE)
    d.ellipse([6, 2, 9, 5], fill=c1, outline=INK)
    d.line([7, 5, 7, 6], fill=INK)


# ---------------------------------------------------------------- Group E: panel furniture
def m_plate(d, c1, c2, a):
    d.rectangle([1, 2, 14, 13], outline=INK, fill=LIGHT)
    for x, y in ((2, 3), (12, 3), (2, 11), (12, 11)):
        d.ellipse([x, y, x + 1, y + 1], fill=c1)
    d.line([4, 7, 11, 7], fill=MID)
    d.line([4, 9, 8, 9], fill=MID)


def m_nameplate(d, c1, c2, a):
    d.rectangle([2, 5, 13, 10], fill=c1, outline=INK)
    d.line([4, 7, 11, 7], fill=WHITE)
    d.line([4, 8, 9, 8], fill=WHITE)
    d.point((3, 6), fill=INK)
    d.point((12, 6), fill=INK)
    d.line([2, 3, 2, 5], fill=MID)
    d.line([13, 3, 13, 5], fill=MID)


def m_bezel(d, c1, c2, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=MID)
    d.ellipse([4, 4, 11, 11], fill=WHITE, outline=INK)
    d.ellipse([5, 5, 10, 10], outline=c1)
    for x, y in ((2, 2), (13, 2), (2, 13), (13, 13)):
        d.point((x, y), fill=INK)


def m_ctrlpanel(d, c1, c2, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=LIGHT)
    d.ellipse([3, 3, 5, 5], fill=GREEN, outline=INK)  # lamp
    d.rectangle([7, 2, 9, 6], fill=c1, outline=INK)  # switch
    d.ellipse([11, 2, 13, 4], outline=INK)  # knob
    d.rectangle([3, 8, 12, 12], outline=INK, fill=WHITE)  # meter
    d.line([7, 11, 10, 9], fill=RED, width=1)
    d.rectangle([4, 9, 6, 11], fill=RED, outline=INK)  # button


# ---------------------------------------------------------------- Group F: LED digits + needles
SEG = {
    "0": "abcdef", "1": "bc", "2": "abged", "3": "abgcd",
    "4": "fgbc", "5": "afgcd", "6": "afgedc", "7": "abc",
    "8": "abcdefg", "9": "abcdfg",
}


def _seg7(d, x, y, w, h, ch, on, off):
    """One 7-segment digit: (x, y) top-left, w wide, h tall."""
    mid = y + h // 2
    xe = x + w - 1
    ye = y + h - 1
    segs = {
        "a": [(x, y), (xe, y)],
        "b": [(xe, y), (xe, mid)],
        "c": [(xe, mid), (xe, ye)],
        "d": [(x, ye), (xe, ye)],
        "e": [(x, mid), (x, ye)],
        "f": [(x, y), (x, mid)],
        "g": [(x, mid), (xe, mid)],
    }
    wd = 2 if w >= 5 else 1
    for s, pts in segs.items():
        d.line(pts, fill=on if s in SEG[ch] else off, width=wd)


def _led_digits(d, value, c1, c2, a):
    d.rectangle([1, 3, 14, 12], outline=INK, fill=(25, 25, 25))
    n = len(value)
    if n == 1:
        w, h, ys, xs = 6, 8, 4, [5]
    elif n == 2:
        w, h, ys, xs = 5, 8, 4, [2, 8]
    elif n == 3:
        w, h, ys, xs = 3, 8, 4, [2, 6, 10]
    elif n == 4:
        w, h, ys, xs = 3, 7, 4, [2, 5, 8, 11]
    elif n == 5:
        w, h, ys, xs = 2, 7, 4, [2, 4, 6, 8, 10]
    else:
        w, h, ys, xs = 2, 7, 4, [2, 4, 6, 8, 10, 12]
    for x, ch in zip(xs, value):
        _seg7(d, x, ys, w, h, ch, (255, 70, 50), (80, 25, 25))
    d.point((13, 14), fill=c1)  # status dot under the display


def _digits_for(value):
    def draw(d, c1, c2, a):
        _led_digits(d, value, c1, c2, a)
    return draw


def m_needle_tapered(d, c1, c2, a):
    _dial(d)
    d.arc([3, 3, 12, 12], 180, 360, fill=c1)
    d.polygon([(7, 8), (12, 2), (8, 9)], fill=RED, outline=INK)
    d.line([7, 8, 4, 11], fill=INK, width=1)  # tail
    d.ellipse([6, 6, 8, 8], fill=INK)


def m_needle_knife(d, c1, c2, a):
    _dial(d)
    d.line([3, 7, 4, 7], fill=MID)
    d.line([11, 7, 12, 7], fill=MID)
    d.line([7, 12, 7, 13], fill=MID)
    d.line([7, 7, 12, 3], fill=INK, width=1)  # knife-edge blade
    d.line([8, 8, 12, 4], fill=c1, width=1)  # edge highlight
    d.ellipse([6, 6, 8, 8], fill=c1)


def m_needle_edgewise(d, c1, c2, a):
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    for x, ht in ((3, 2), (6, 3), (9, 2), (12, 3)):
        d.line([x, 5, x, 5 + ht], fill=MID)
    d.line([2, 5, 13, 5], fill=c1)  # scale line
    d.rectangle([12, 4, 13, 5], fill=RED)  # red zone
    d.line([8, 5, 8, 11], fill=RED, width=2)  # needle
    d.rectangle([7, 10, 9, 11], fill=INK)  # pivot


def m_needle_vu(d, c1, c2, a):
    d.rectangle([1, 2, 14, 13], outline=INK, fill=(255, 250, 220))
    d.arc([2, 3, 13, 12], 180, 360, fill=INK)
    d.arc([2, 3, 13, 12], 315, 360, fill=RED, width=2)  # red zone
    d.line([7, 12, 11, 4], fill=RED, width=2)  # needle
    d.line([4, 9, 5, 8], fill=MID)
    d.line([10, 8, 11, 9], fill=MID)
    d.ellipse([6, 11, 8, 13], fill=INK)


# ---------------------------------------------------------------- dispatch
MOTIFS = {
    "InstVerticalTankLevel": m_vtank,
    "InstHorizontalTankLevel": m_htank,
    "InstSphericalTankLevel": m_sph_tank,
    "InstSightGlassLevel": m_sight,
    "InstLevelColumnGauge": m_colgauge,
    "InstTankLevelAlarms": m_tankalarms,
    "InstOpenChannelLevel": m_openchan,
    "InstLevelTransmitter": m_txtrans,
    "InstRockerSwitch": m_rocker,
    "InstRockerSwitchIlluminated": m_rocker_ill,
    "InstRockerSwitchDouble": m_rocker_dbl,
    "InstToggleSwitch": m_toggle,
    "InstToggleSwitchIlluminated": m_toggle_ill,
    "InstPushButton": m_push,
    "InstPushButtonLatching": m_push_latch,
    "InstEmergencyStop": m_estop,
    "InstRotarySelector": m_rotary,
    "InstKeySwitch": m_keysw,
    "InstSlideSwitch": m_slide,
    "InstCircuitBreaker": m_breaker,
    "InstPanelLampRed": m_lamp_red,
    "InstPanelLampGreen": m_lamp_green,
    "InstPanelLampAmber": m_lamp_amber,
    "InstPanelLampBlue": m_lamp_blue,
    "InstPanelLampWhite": m_lamp_white,
    "InstLedRed": m_led_red,
    "InstLedGreen": m_led_green,
    "InstLedBlue": m_led_blue,
    "InstRgbStatusLed": m_rgb_led,
    "InstStackLight": m_stacklight,
    "InstBeacon": m_beacon,
    "InstLampTestButton": m_lamptest,
    "InstAnalogGauge": m_analog,
    "InstPressureGauge": m_pressure,
    "InstTemperatureGauge": m_temp,
    "InstBarMeter": m_barmeter,
    "InstVuMeter": m_vumeter,
    "InstDigitalReadout": m_digread,
    "InstManometer": m_manometer,
    "InstFlowMeter": m_flowmeter,
    "InstPanelPlate": m_plate,
    "InstNameplate": m_nameplate,
    "InstBezel": m_bezel,
    "InstControlPanel": m_ctrlpanel,
    "InstLedDigits1": _digits_for("5"),
    "InstLedDigits2": _digits_for("42"),
    "InstLedDigits3": _digits_for("318"),
    "InstLedDigits4": _digits_for("2026"),
    "InstLedDigits5": _digits_for("12345"),
    "InstLedDigits6": _digits_for("654321"),
    "InstNeedleTapered": m_needle_tapered,
    "InstNeedleKnifeEdge": m_needle_knife,
    "InstNeedleEdgewise": m_needle_edgewise,
    "InstNeedleVu": m_needle_vu,
}
assert set(MOTIFS) == set(ALL_INST)


def _make(name, idx):
    motif = MOTIFS[name]

    def draw(d, a):
        motif(d, _hue(idx), _hue(idx, off=0.45, sat=0.65, val=0.75), a)
    return draw


DRAW = {}
for _i, _n in enumerate(ALL_INST):
    DRAW[_n] = _make(_n, _i)


def main():
    outdir = os.path.join(ROOT, "icons")
    os.makedirs(outdir, exist_ok=True)
    for name in sorted(DRAW):
        sec = section_of(name)
        accent = SECTION_COLORS.get(sec, ACCENT)
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
