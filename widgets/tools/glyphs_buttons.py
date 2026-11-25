#!/usr/bin/env python3
"""Unique 16x16 glyphs for the Buttons / Display / Choosers widgets.

Each glyph is drawn with PIL ImageDraw in a 16x16 box, in the same base
language as tools/gen_icons.py (dark ink INK + MID + LIGHT on white, one
accent colour per section, plus 1-2 extra bright colours where it helps),
and is visibly different from every other glyph in this set at 16x16.

Widgets covered (36):
  Buttons (9): GtkButton, GtkModelButton, GtkToggleButton, GtkToolButton,
    GtkVolumeButton, TBitBtn, TButton, TButtonPanel, TSpeedButton
  Colour (7): GtkColorButton, GtkColorChooserWidget, GtkColorDialogButton,
    TColorBox, TColorButton, TColorDialog, TColorListBox
  Image (4): GtkImage, GtkPicture, TImage, TImageList
  Check (3): GtkCheckButton, TCheckBox, TCheckComboBox
  Switch (3): GtkSwitch, GtkToggleToolButton, TToggleBox
  Pairs (10): GtkCalendar, TCalendar, GtkDrawingArea, TPaintBox,
    GtkFileChooserWidget, TFileListBox, GtkProgressBar, TProgressBar,
    GtkRadioButton, TRadioButton

Accent colours come from tools/sections.py via section_of() -- do not
hardcode them.  Note TColorDialog belongs to "Windows & Dialogs", so it
takes the blue accent, not the pink Choosers accent.

Usage: python3 widgets/tools/glyphs_buttons.py   (run from the repo root)
regenerates ONLY these 36 .ico files into widgets/icons/, saved exactly
like gen_icons.py does (16x16 RGB, ICO, sizes=[(16,16)],
bitmap_format="bmp").
"""
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
YELLOW = (234, 179, 8)
BLUE = (37, 99, 235)
ORANGE = (234, 88, 12)
GREEN = (22, 163, 74)
FOLDER_YELLOW = (255, 214, 120)

# ---------------------------------------------------------------- glyphs
# Every glyph draws inside the 16x16 box (0..15).  d = ImageDraw, a = accent.


# --- Buttons (9) ---------------------------------------------------------

def g_GtkButton(d, a):
    # Plain rounded button: accent bar + white bevel highlight.
    d.rounded_rectangle([1, 4, 14, 11], radius=2, outline=INK, fill=LIGHT)
    d.line([3, 5, 12, 5], fill=WHITE)
    d.line([4, 8, 11, 8], fill=a, width=2)


def g_GtkModelButton(d, a):
    # Button that opens a menu: two text lines + accent drop-down arrow.
    d.rounded_rectangle([1, 3, 14, 12], radius=2, outline=INK, fill=WHITE)
    d.line([3, 6, 8, 6], fill=MID)
    d.line([3, 9, 8, 9], fill=MID)
    d.polygon([(10, 5), (14, 5), (12, 9)], fill=a)


def g_GtkToggleButton(d, a):
    # Pressed/active button: filled with the accent, white bar.
    d.rounded_rectangle([1, 3, 14, 12], radius=2, outline=INK, fill=a)
    d.line([4, 7, 11, 7], fill=WHITE, width=2)


def g_GtkToolButton(d, a):
    # Toolbar button with a gear: accent ring, ink hub, accent teeth.
    d.rounded_rectangle([1, 3, 14, 12], radius=2, outline=INK, fill=LIGHT)
    d.ellipse([5, 5, 10, 10], outline=a, width=2)
    d.rectangle([7, 3, 8, 4], fill=a)
    d.rectangle([7, 11, 8, 12], fill=a)
    d.rectangle([3, 7, 4, 8], fill=a)
    d.rectangle([11, 7, 12, 8], fill=a)
    d.point((7, 7), fill=INK)
    d.point((8, 7), fill=INK)
    d.point((7, 8), fill=INK)
    d.point((8, 8), fill=INK)


def g_GtkVolumeButton(d, a):
    # Speaker cone in accent + two sound waves (ink, then accent).
    d.polygon([(1, 6), (5, 6), (8, 3), (8, 12), (5, 9), (1, 9)],
              fill=a, outline=INK)
    d.arc([9, 4, 13, 11], -60, 60, fill=INK, width=1)
    d.arc([10, 2, 15, 13], -60, 60, fill=a, width=1)


def g_TBitBtn(d, a):
    # Button with a bitmap: tiny yellow picture (red dot, green base).
    d.rounded_rectangle([1, 3, 14, 12], radius=2, outline=INK, fill=WHITE)
    d.rectangle([3, 5, 7, 10], outline=INK, fill=YELLOW)
    d.ellipse([5, 5, 6, 6], fill=RED)
    d.line([3, 9, 7, 9], fill=GREEN)
    d.line([9, 6, 12, 6], fill=MID)
    d.line([9, 9, 12, 9], fill=MID)


def g_TButton(d, a):
    # Plain Delphi button: SQUARE corners (vs GtkButton's rounded ones),
    # accent block + ink bar.
    d.rectangle([1, 4, 14, 11], outline=INK, fill=LIGHT)
    d.rectangle([3, 6, 5, 9], fill=a)
    d.line([7, 7, 12, 7], fill=INK, width=2)


def g_TButtonPanel(d, a):
    # Panel holding grouped buttons: two small ones on top, one wide
    # accent one below.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 3, 7, 7], outline=INK, fill=LIGHT)
    d.rectangle([8, 3, 13, 7], outline=INK, fill=LIGHT)
    d.point((4, 5), fill=a)
    d.point((10, 5), fill=a)
    d.rectangle([2, 9, 13, 12], outline=INK, fill=a)
    d.line([4, 10, 11, 10], fill=WHITE, width=2)


def g_TSpeedButton(d, a):
    # Flat speed button with a yellow lightning bolt.
    d.rounded_rectangle([1, 2, 14, 13], radius=2, outline=MID, fill=WHITE)
    d.polygon([(9, 3), (5, 9), (7, 9), (6, 13), (11, 7), (8, 7)],
              fill=YELLOW, outline=INK)


# --- Colour (7) ----------------------------------------------------------

def g_GtkColorButton(d, a):
    # Button with a 2x2 colour swatch + accent drop-down arrow.
    d.rounded_rectangle([1, 3, 14, 12], radius=2, outline=INK, fill=WHITE)
    d.rectangle([2, 4, 5, 7], fill=RED)
    d.rectangle([6, 4, 9, 7], fill=YELLOW)
    d.rectangle([2, 8, 5, 11], fill=BLUE)
    d.rectangle([6, 8, 9, 11], fill=GREEN)
    d.polygon([(11, 6), (14, 6), (12, 9)], fill=a)


def g_GtkColorChooserWidget(d, a):
    # Full 3x3 colour grid, centre cell picked out in white.
    cols = [RED, YELLOW, GREEN, BLUE, ORANGE, (8, 145, 178),
            (147, 51, 234), (219, 39, 119), a]
    for i, c in enumerate(cols):
        x = 1 + (i % 3) * 5
        y = 1 + (i // 3) * 5
        d.rectangle([x, y, x + 3, y + 3], fill=c)
    d.rectangle([1, 1, 14, 14], outline=INK)
    d.rectangle([6, 6, 9, 9], outline=WHITE)


def g_GtkColorDialogButton(d, a):
    # Dialog frame (accent title bar) + horizontal rainbow strip.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.point((3, 2), fill=WHITE)
    d.point((5, 2), fill=WHITE)
    d.rectangle([3, 7, 5, 10], fill=RED)
    d.rectangle([6, 7, 8, 10], fill=YELLOW)
    d.rectangle([9, 7, 12, 10], fill=BLUE)
    d.line([3, 12, 12, 12], fill=MID)


def g_TColorBox(d, a):
    # Combo field: three vertical colour stripes + accent arrow.
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.rectangle([2, 5, 4, 10], fill=RED)
    d.rectangle([5, 5, 7, 10], fill=YELLOW)
    d.rectangle([8, 5, 10, 10], fill=BLUE)
    d.line([11, 4, 11, 11], fill=INK)
    d.polygon([(12, 6), (14, 6), (13, 9)], fill=a)


def g_TColorButton(d, a):
    # Button with a rainbow bar underneath it.
    d.rounded_rectangle([1, 2, 14, 10], radius=2, outline=INK, fill=LIGHT)
    d.line([4, 6, 11, 6], fill=a, width=2)
    d.rectangle([2, 12, 4, 14], fill=RED)
    d.rectangle([5, 12, 7, 14], fill=YELLOW)
    d.rectangle([8, 12, 10, 14], fill=BLUE)
    d.rectangle([11, 12, 13, 14], fill=GREEN)


def g_TColorDialog(d, a):
    # Dialog frame (BLUE title bar: Windows & Dialogs accent) + 2x2
    # colour grid with an ink outline.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.point((3, 2), fill=WHITE)
    d.point((5, 2), fill=WHITE)
    d.rectangle([4, 6, 7, 9], fill=RED)
    d.rectangle([8, 6, 11, 9], fill=YELLOW)
    d.rectangle([4, 10, 7, 13], fill=BLUE)
    d.rectangle([8, 10, 11, 13], fill=GREEN)
    d.rectangle([4, 6, 11, 13], outline=INK)


def g_TColorListBox(d, a):
    # List rows, each with a colour square; middle row selected.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 6, 13, 9], fill=LIGHT)
    d.rectangle([3, 2, 5, 4], fill=RED)
    d.line([7, 3, 12, 3], fill=MID)
    d.rectangle([3, 6, 5, 8], fill=BLUE)
    d.line([7, 7, 12, 7], fill=INK)
    d.rectangle([3, 10, 5, 12], fill=GREEN)
    d.line([7, 11, 12, 11], fill=MID)
    d.line([2, 6, 2, 9], fill=a, width=2)


# --- Image (4) -----------------------------------------------------------

def g_GtkImage(d, a):
    # Full-bleed photo: yellow sun, ink mountain polyline.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.ellipse([4, 3, 6, 5], fill=YELLOW)
    d.line([1, 12, 6, 7, 9, 10, 14, 5], fill=INK, width=1)


def g_GtkPicture(d, a):
    # Framed picture: double frame, orange sun, triangle mountain.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([3, 3, 12, 12], outline=a, fill=WHITE)
    d.ellipse([5, 4, 7, 6], fill=ORANGE)
    d.polygon([(4, 11), (8, 6), (11, 11)], outline=INK)
    d.line([4, 11, 11, 11], fill=MID)


def g_TImage(d, a):
    # Photo with a folded corner: red sun top-right, accent hills.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.ellipse([10, 3, 12, 5], fill=RED)
    d.line([1, 12, 6, 7, 9, 10, 14, 6], fill=a, width=1)
    d.polygon([(11, 1), (14, 1), (14, 4)], fill=LIGHT, outline=INK)


def g_TImageList(d, a):
    # Filmstrip of three mini photos, each with its own coloured dot.
    d.rectangle([1, 2, 14, 13], outline=INK, fill=WHITE)
    d.line([5, 2, 5, 13], fill=INK)
    d.line([10, 2, 10, 13], fill=INK)
    d.ellipse([2, 4, 3, 5], fill=RED)
    d.ellipse([7, 4, 8, 5], fill=YELLOW)
    d.ellipse([12, 4, 13, 5], fill=BLUE)
    d.line([2, 10, 4, 10], fill=MID)
    d.line([7, 10, 9, 10], fill=MID)
    d.line([11, 10, 13, 10], fill=MID)


# --- Check (3) -----------------------------------------------------------

def g_GtkCheckButton(d, a):
    # Classic checkbox (light-grey box) + accent tick + two text lines.
    d.rectangle([1, 3, 8, 10], outline=INK, fill=LIGHT)
    d.line([2, 6, 4, 8, 7, 4], fill=a, width=2)
    d.line([10, 5, 14, 5], fill=MID)
    d.line([10, 8, 14, 8], fill=MID)


def g_TCheckBox(d, a):
    # Bigger square box, BLUE tick (not the accent green), two lines.
    d.rectangle([2, 2, 9, 9], outline=INK, fill=WHITE)
    d.line([3, 5, 5, 7, 8, 3], fill=BLUE, width=2)
    d.line([11, 4, 14, 4], fill=MID)
    d.line([11, 7, 13, 7], fill=MID)


def g_TCheckComboBox(d, a):
    # Combo field: checkbox with accent tick + accent drop-down arrow.
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.rectangle([2, 5, 6, 10], outline=INK, fill=WHITE)
    d.line([3, 7, 4, 8, 5, 6], fill=a, width=1)
    d.line([8, 4, 8, 11], fill=INK)
    d.polygon([(10, 6), (13, 6), (11, 9)], fill=a)


# --- Switch (3) ----------------------------------------------------------

def g_GtkSwitch(d, a):
    # Pill switch ON: accent fill, white knob on the right.
    d.rounded_rectangle([1, 4, 14, 11], radius=5, outline=INK, fill=a)
    d.ellipse([8, 5, 13, 10], fill=WHITE, outline=INK)


def g_GtkToggleToolButton(d, a):
    # Square toolbar button holding a mini toggle pill (knob left).
    d.rectangle([1, 3, 14, 12], outline=INK, fill=LIGHT)
    d.rounded_rectangle([3, 6, 12, 9], radius=2, outline=INK, fill=WHITE)
    d.ellipse([3, 6, 6, 9], fill=a)


def g_TToggleBox(d, a):
    # Pill switch OFF: white fill, accent knob on the left.
    d.rounded_rectangle([1, 4, 14, 11], radius=5, outline=INK, fill=WHITE)
    d.ellipse([2, 5, 7, 10], fill=a, outline=INK)
    d.line([9, 7, 12, 7], fill=MID)


# --- Pairs (10) ----------------------------------------------------------

def g_GtkCalendar(d, a):
    # Calendar with accent header and a RED highlighted day.
    d.rectangle([1, 2, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 2, 14, 5], fill=a)
    d.line([4, 1, 4, 3], fill=INK, width=2)
    d.line([11, 1, 11, 3], fill=INK, width=2)
    for y in (8, 11):
        for x in (4, 7, 10):
            d.point((x, y), fill=INK)
    d.rectangle([9, 10, 11, 12], fill=RED)


def g_TCalendar(d, a):
    # Calendar with white nav arrows in the header and a RED circled day.
    d.rectangle([1, 2, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 2, 14, 5], fill=a)
    d.polygon([(5, 2), (5, 5), (2, 3)], fill=WHITE)
    d.polygon([(10, 2), (10, 5), (13, 3)], fill=WHITE)
    for y in (8, 11):
        for x in (4, 7, 10):
            d.point((x, y), fill=INK)
    d.ellipse([9, 10, 12, 13], outline=RED)


def g_GtkDrawingArea(d, a):
    # Blank canvas, accent pencil (ink tip) and a red line it drew.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([2, 13, 6, 11, 10, 12, 13, 10], fill=RED, width=1)
    d.line([3, 9, 10, 3], fill=a, width=2)
    d.polygon([(10, 3), (13, 1), (13, 4)], fill=INK)


def g_TPaintBox(d, a):
    # Paint palette (four colour dots) + accent brush stroke below.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.ellipse([3, 2, 12, 8], outline=INK, fill=WHITE)
    d.ellipse([4, 3, 6, 5], fill=RED)
    d.ellipse([7, 3, 9, 5], fill=YELLOW)
    d.ellipse([9, 5, 11, 7], fill=BLUE)
    d.ellipse([5, 5, 7, 7], fill=GREEN)
    d.line([2, 12, 13, 10], fill=a, width=2)


def g_GtkFileChooserWidget(d, a):
    # Chooser: yellow folder strip on top, two file rows below.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(2, 3), (2, 5), (13, 5), (13, 3), (7, 3), (6, 2), (2, 2)],
              outline=INK, fill=FOLDER_YELLOW)
    d.rectangle([1, 6, 14, 9], fill=LIGHT)
    d.rectangle([3, 7, 4, 8], fill=a)
    d.line([6, 7, 12, 7], fill=INK)
    d.rectangle([3, 11, 4, 12], fill=a)
    d.line([6, 11, 12, 11], fill=MID)


def g_TFileListBox(d, a):
    # Plain file list: three file rows, middle one selected (light bg,
    # accent left bar, ink text line).
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 5, 14, 8], fill=LIGHT)
    for y in (3, 7, 11):
        d.rectangle([2, y - 1, 4, y + 1], outline=INK, fill=WHITE)
        d.line([6, y, 12, y], fill=INK if y == 7 else MID)
    d.line([1, 5, 1, 8], fill=a, width=2)


def g_GtkProgressBar(d, a):
    # Horizontal bar, solid accent fill to ~70% + white shine line.
    d.rectangle([1, 5, 14, 10], outline=INK, fill=WHITE)
    d.rectangle([2, 6, 10, 9], fill=a)
    d.line([2, 6, 10, 6], fill=WHITE)


def g_TProgressBar(d, a):
    # Horizontal bar, SEGMENTED accent blocks + one empty block.
    d.rectangle([1, 5, 14, 10], outline=INK, fill=WHITE)
    for x in (2, 5, 8):
        d.rectangle([x, 6, x + 2, 9], fill=a)
    d.rectangle([11, 6, 13, 9], fill=LIGHT)


def g_GtkRadioButton(d, a):
    # Classic radio: light-grey circle + accent dot + two text lines.
    d.ellipse([1, 2, 8, 9], outline=INK, fill=LIGHT)
    d.ellipse([3, 4, 6, 7], fill=a)
    d.line([10, 4, 14, 4], fill=MID)
    d.line([10, 7, 14, 7], fill=MID)


def g_TRadioButton(d, a):
    # Two stacked radios: selected (accent dot) above, empty below.
    d.ellipse([2, 2, 9, 9], outline=INK, fill=WHITE)
    d.ellipse([4, 4, 7, 7], fill=a)
    d.line([11, 5, 14, 5], fill=MID)
    d.ellipse([2, 11, 6, 14], outline=INK, fill=WHITE)
    d.line([8, 12, 14, 12], fill=MID)


DRAW = {
    "GtkButton": g_GtkButton,
    "GtkModelButton": g_GtkModelButton,
    "GtkToggleButton": g_GtkToggleButton,
    "GtkToolButton": g_GtkToolButton,
    "GtkVolumeButton": g_GtkVolumeButton,
    "TBitBtn": g_TBitBtn,
    "TButton": g_TButton,
    "TButtonPanel": g_TButtonPanel,
    "TSpeedButton": g_TSpeedButton,
    "GtkColorButton": g_GtkColorButton,
    "GtkColorChooserWidget": g_GtkColorChooserWidget,
    "GtkColorDialogButton": g_GtkColorDialogButton,
    "TColorBox": g_TColorBox,
    "TColorButton": g_TColorButton,
    "TColorDialog": g_TColorDialog,
    "TColorListBox": g_TColorListBox,
    "GtkImage": g_GtkImage,
    "GtkPicture": g_GtkPicture,
    "TImage": g_TImage,
    "TImageList": g_TImageList,
    "GtkCheckButton": g_GtkCheckButton,
    "TCheckBox": g_TCheckBox,
    "TCheckComboBox": g_TCheckComboBox,
    "GtkSwitch": g_GtkSwitch,
    "GtkToggleToolButton": g_GtkToggleToolButton,
    "TToggleBox": g_TToggleBox,
    "GtkCalendar": g_GtkCalendar,
    "TCalendar": g_TCalendar,
    "GtkDrawingArea": g_GtkDrawingArea,
    "TPaintBox": g_TPaintBox,
    "GtkFileChooserWidget": g_GtkFileChooserWidget,
    "TFileListBox": g_TFileListBox,
    "GtkProgressBar": g_GtkProgressBar,
    "TProgressBar": g_TProgressBar,
    "GtkRadioButton": g_GtkRadioButton,
    "TRadioButton": g_TRadioButton,
}


def main():
    outdir = os.path.join(ROOT, "icons")
    os.makedirs(outdir, exist_ok=True)
    for name in sorted(DRAW):
        sec = section_of(name)
        if sec is None:
            print(f"no section for {name}", file=sys.stderr)
            return 1
        accent = SECTION_COLORS[sec]
        img = Image.new("RGB", (16, 16), WHITE)
        d = ImageDraw.Draw(img)
        DRAW[name](d, accent)
        path = os.path.join(outdir, name + ".ico")
        img.save(path, format="ICO", sizes=[(16, 16)], bitmap_format="bmp")
    print(f"icons written: {len(DRAW)} in {outdir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
