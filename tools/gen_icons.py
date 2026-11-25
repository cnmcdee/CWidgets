#!/usr/bin/env python3
"""Generate one real 16x16 .ico icon per CWidgets widget (icons/<Name>.ico).

Icons are purpose-drawn glyphs (PIL ImageDraw), NOT downscaled screenshots:
at 16x16 a screenshot is mush, a 2-3 element glyph stays readable.
Each glyph is drawn with the widget's section accent colour + dark ink on
white, saved as a BMP-format ICO (32bpp, no compression) so the palette's
tiny ICO decoder can read it without libpng.

Usage: python3 tools/gen_icons.py   (run from the repo root)
"""
import os, sys
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from sections import ASSIGNMENT, SECTION_COLORS, section_of, validate  # noqa: E402

INK = (30, 30, 30)
MID = (140, 140, 140)
LIGHT = (226, 228, 232)
WHITE = (255, 255, 255)

# ---------------------------------------------------------------- glyphs
# Every glyph draws inside the 16x16 box (0..15).  d = ImageDraw, a = accent.

def g_window(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.point((3, 2), fill=WHITE); d.point((5, 2), fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID); d.line([3, 11, 8, 11], fill=MID)

def g_dialog(d, a):
    g_window(d, a)
    d.rectangle([4, 10, 11, 12], outline=INK, fill=LIGHT)

def g_button(d, a):
    d.rounded_rectangle([1, 4, 14, 11], radius=2, outline=INK, fill=LIGHT)
    d.line([4, 7, 11, 7], fill=a, width=2)

def g_check(d, a):
    d.rectangle([1, 3, 8, 10], outline=INK, fill=WHITE)
    d.line([2, 6, 4, 8, 7, 4], fill=a, width=2)
    d.line([10, 5, 14, 5], fill=MID); d.line([10, 8, 14, 8], fill=MID)

def g_radio(d, a):
    d.ellipse([1, 2, 8, 9], outline=INK, fill=WHITE)
    d.ellipse([3, 4, 6, 7], fill=a)
    d.line([10, 4, 14, 4], fill=MID); d.line([10, 7, 14, 7], fill=MID)

def g_toggle(d, a):
    d.rounded_rectangle([1, 4, 14, 11], radius=5, outline=INK, fill=WHITE)
    d.ellipse([3, 6, 7, 10], fill=a)
    d.line([9, 7, 12, 7], fill=MID)

def g_link(d, a):
    d.rounded_rectangle([1, 3, 8, 9], radius=2, outline=INK)
    d.rounded_rectangle([7, 6, 14, 12], radius=2, outline=a)
    d.line([3, 13, 12, 13], fill=a)

def g_label(d, a):
    d.line([2, 4, 13, 4], fill=INK, width=2)
    d.line([2, 8, 10, 8], fill=a, width=2)
    d.line([2, 12, 13, 12], fill=MID, width=2)

def g_image(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.ellipse([4, 3, 6, 5], fill=a)
    d.line([1, 12, 6, 7, 9, 10, 14, 5], fill=INK, width=1)

def g_progress(d, a):
    d.rectangle([1, 5, 14, 10], outline=INK, fill=WHITE)
    d.rectangle([2, 6, 9, 9], fill=a)

def g_level(d, a):
    for i, h in enumerate((4, 6, 9, 12)):
        d.rectangle([1 + i * 4, 13 - h, 3 + i * 4, 13], fill=a if i < 3 else LIGHT, outline=INK)

def g_spinner(d, a):
    d.ellipse([2, 2, 13, 13], outline=MID, width=2)
    d.arc([2, 2, 13, 13], 90, 270, fill=a, width=2)

def g_slider(d, a):
    d.line([1, 8, 14, 8], fill=INK, width=2)
    d.rectangle([6, 4, 9, 11], outline=INK, fill=a)
    d.line([3, 12, 3, 14], fill=MID); d.line([12, 12, 12, 14], fill=MID)

def g_spin(d, a):
    d.rectangle([1, 3, 14, 12], outline=INK, fill=WHITE)
    d.line([10, 3, 10, 12], fill=INK)
    d.polygon([(11, 6), (13, 6), (12, 4)], fill=a)
    d.polygon([(11, 9), (13, 9), (12, 11)], fill=a)
    d.line([3, 7, 8, 7], fill=MID)

def g_entry(d, a):
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([4, 7, 4, 9], fill=a, width=2)
    d.line([6, 8, 12, 8], fill=MID)

def g_textview(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 4, 12, 4], fill=INK); d.line([3, 7, 12, 7], fill=MID)
    d.line([3, 10, 9, 10], fill=MID); d.line([3, 12, 12, 12], fill=MID)

def g_combo(d, a):
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([10, 4, 10, 11], fill=INK)
    d.polygon([(11, 6), (14, 6), (12, 9)], fill=a)

def g_list(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    for y in (3, 7, 11):
        d.rectangle([3, y, 5, y + 1], fill=a)
        d.line([7, y, 12, y], fill=MID)

def g_tree(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 3, 3, 12], fill=MID)
    for y in (3, 7, 11):
        d.line([3, y, 5, y], fill=MID)
        d.rectangle([6, y - 1, 8, y + 1], fill=a)
        d.line([10, y, 12, y], fill=MID)

def g_grid(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    for x in (5, 10):
        d.line([x, 1, x, 14], fill=INK)
    for y in (8, 11):
        d.line([1, y, 14, y], fill=INK)

def g_tabs(d, a):
    d.rectangle([2, 1, 7, 4], outline=INK, fill=a)
    d.rectangle([8, 1, 13, 4], outline=INK, fill=LIGHT)
    d.rectangle([1, 4, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID); d.line([3, 11, 8, 11], fill=MID)

def g_paned(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([7, 1, 7, 14], fill=INK, width=2)
    d.point((8, 6), fill=a); d.point((8, 8), fill=a); d.point((8, 10), fill=a)

def g_box(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([5, 1, 5, 14], fill=a); d.line([10, 1, 10, 14], fill=a)

def g_frame(d, a):
    d.rectangle([1, 3, 14, 14], outline=INK)
    d.line([1, 3, 4, 3], fill=INK); d.line([9, 3, 14, 3], fill=INK)
    d.line([5, 1, 8, 1], fill=a, width=2)

def g_expander(d, a):
    d.polygon([(5, 3), (5, 12), (11, 7)], fill=a)
    d.line([12, 7, 14, 7], fill=INK)

def g_menu(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 4, 12, 4], fill=INK); d.line([3, 7, 12, 7], fill=MID)
    d.line([3, 10, 12, 10], fill=MID); d.line([3, 13, 8, 13], fill=MID)

def g_menubar(d, a):
    d.rectangle([1, 1, 14, 4], fill=a)
    d.rectangle([2, 2, 5, 3], fill=WHITE); d.rectangle([7, 2, 10, 3], fill=WHITE)
    d.rectangle([1, 5, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID); d.line([3, 11, 8, 11], fill=MID)

def g_toolbar(d, a):
    d.rectangle([1, 1, 14, 5], fill=LIGHT, outline=INK)
    d.rectangle([2, 2, 5, 4], fill=a)
    d.rectangle([7, 2, 9, 4], outline=INK); d.rectangle([11, 2, 13, 4], outline=INK)
    d.rectangle([1, 6, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 9, 11, 9], fill=MID); d.line([3, 12, 8, 12], fill=MID)

def g_scrollbar(d, a):
    d.rectangle([9, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([10, 4, 13, 10], fill=a)
    d.rectangle([1, 1, 7, 14], outline=INK, fill=WHITE)
    d.line([3, 4, 5, 4], fill=MID); d.line([3, 7, 5, 7], fill=MID); d.line([3, 10, 5, 10], fill=MID)

def g_separator(d, a):
    d.line([1, 7, 14, 7], fill=INK, width=2)
    d.line([1, 11, 14, 11], fill=MID)

def g_color(d, a):
    d.rectangle([1, 1, 7, 7], fill=(220, 38, 38))
    d.rectangle([8, 1, 14, 7], fill=(234, 179, 8))
    d.rectangle([1, 8, 7, 14], fill=(37, 99, 235))
    d.rectangle([8, 8, 14, 14], fill=(22, 163, 74))
    d.rectangle([1, 1, 14, 14], outline=INK)

def g_font(d, a):
    d.line([7, 2, 4, 13], fill=INK, width=2)
    d.line([7, 2, 10, 13], fill=INK, width=2)
    d.line([5, 9, 9, 9], fill=a, width=2)
    d.line([2, 13, 13, 13], fill=INK)

def g_file(d, a):
    d.polygon([(3, 1), (10, 1), (13, 4), (13, 14), (3, 14)], outline=INK, fill=WHITE)
    d.line([10, 1, 10, 4, 13, 4], fill=INK)
    d.line([5, 7, 11, 7], fill=a); d.line([5, 10, 11, 10], fill=MID)

def g_search(d, a):
    d.ellipse([2, 2, 10, 10], outline=INK, width=2)
    d.line([9, 9, 13, 13], fill=a, width=2)

def g_statusbar(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 11, 14, 14], fill=LIGHT, outline=INK)
    d.line([11, 12, 13, 14], fill=a); d.line([12, 12, 14, 14], fill=a)

def g_headerbar(d, a):
    d.rectangle([1, 1, 14, 5], fill=a)
    d.ellipse([3, 2, 5, 4], fill=WHITE); d.ellipse([11, 2, 13, 4], fill=WHITE)
    d.rectangle([1, 6, 14, 14], outline=INK, fill=WHITE)

def g_video(d, a):
    d.rectangle([1, 2, 14, 13], outline=INK, fill=WHITE)
    d.polygon([(6, 5), (6, 10), (11, 7)], fill=a)

def g_pencil(d, a):
    d.line([3, 12, 11, 4], fill=a, width=2)
    d.polygon([(11, 4), (13, 2), (13, 5)], fill=INK)
    d.line([2, 13, 5, 13], fill=INK)

def g_eventbox(d, a):
    for x in range(1, 15, 2):
        d.point((x, 1), fill=INK); d.point((x, 14), fill=INK)
    for y in range(1, 15, 2):
        d.point((1, y), fill=INK); d.point((14, y), fill=INK)
    d.line([5, 7, 10, 7], fill=a, width=2)

def g_overlay(d, a):
    d.rectangle([1, 3, 11, 13], outline=INK, fill=WHITE)
    d.rectangle([4, 1, 14, 10], outline=a, fill=(255, 255, 255))

def g_stack(d, a):
    d.rectangle([3, 4, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 13, 12], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 12, 11], outline=a, fill=WHITE)

def g_calendar(d, a):
    d.rectangle([1, 2, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 2, 14, 5], fill=a)
    d.line([4, 1, 4, 3], fill=INK, width=2); d.line([11, 1, 11, 3], fill=INK, width=2)
    for y in (8, 11):
        for x in (4, 7, 10):
            d.point((x, y), fill=INK)

def g_clock(d, a):
    d.ellipse([2, 2, 13, 13], outline=INK, width=2)
    d.line([7, 7, 7, 4], fill=a, width=2)
    d.line([7, 7, 10, 9], fill=INK, width=2)
    d.line([7, 1, 7, 2], fill=INK)

def g_calculator(d, a):
    d.rectangle([2, 1, 13, 14], outline=INK, fill=WHITE)
    d.rectangle([4, 3, 11, 5], fill=a)
    for y in (8, 11):
        for x in (4, 7, 10):
            d.point((x, y), fill=INK)

def g_memo(d, a):
    g_textview(d, a)
    d.line([11, 1, 11, 4, 14, 4], fill=INK)

def g_shape(d, a):
    d.ellipse([2, 2, 9, 9], outline=INK)
    d.rectangle([7, 7, 14, 14], outline=a, fill=WHITE)

def g_splitter(d, a):
    d.rectangle([1, 1, 6, 14], outline=INK, fill=WHITE)
    d.rectangle([9, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([7, 4, 8, 4], fill=a, width=2); d.line([7, 7, 8, 7], fill=a, width=2)
    d.line([7, 10, 8, 10], fill=a, width=2)

def g_tray(d, a):
    d.rectangle([1, 1, 14, 5], fill=LIGHT, outline=INK)
    d.rectangle([2, 2, 5, 4], fill=a)
    d.ellipse([10, 2, 13, 5], fill=INK)
    d.rectangle([1, 7, 14, 14], outline=INK, fill=WHITE)

def g_timer(d, a):
    d.ellipse([2, 4, 13, 14], outline=INK, width=2)
    d.line([7, 1, 7, 3], fill=INK, width=2)
    d.line([7, 9, 7, 6], fill=a, width=2)
    d.line([7, 9, 10, 10], fill=INK, width=2)

def g_graph(d, a):
    d.line([2, 2, 2, 13], fill=INK, width=2)
    d.line([2, 13, 14, 13], fill=INK, width=2)
    d.line([3, 10, 6, 7, 9, 9, 13, 3], fill=a, width=2)

def g_spreadsheet(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=(22, 163, 74))
    for x in (5, 10):
        d.line([x, 1, x, 14], fill=INK)
    for y in (8, 11):
        d.line([1, y, 14, y], fill=INK)

def g_checkgroup(d, a):
    d.rectangle([1, 3, 14, 14], outline=INK)
    d.line([1, 3, 4, 3], fill=INK); d.line([9, 3, 14, 3], fill=INK)
    d.rectangle([3, 6, 6, 9], outline=INK)
    d.line([3, 7, 5, 9, 6, 6], fill=a)
    d.rectangle([3, 11, 6, 14], outline=INK)
    d.line([8, 7, 12, 7], fill=MID); d.line([8, 12, 12, 12], fill=MID)

def g_radiogroup(d, a):
    d.rectangle([1, 3, 14, 14], outline=INK)
    d.line([1, 3, 4, 3], fill=INK); d.line([9, 3, 14, 3], fill=INK)
    d.ellipse([3, 6, 6, 9], outline=INK); d.ellipse([4, 7, 5, 8], fill=a)
    d.ellipse([3, 11, 6, 14], outline=INK)
    d.line([8, 7, 12, 7], fill=MID); d.line([8, 12, 12, 12], fill=MID)

def g_updown(d, a):
    d.rectangle([1, 3, 14, 12], outline=INK, fill=WHITE)
    d.line([10, 3, 10, 12], fill=INK)
    d.polygon([(11, 6), (13, 6), (12, 4)], fill=a)
    d.polygon([(11, 9), (13, 9), (12, 11)], fill=a)

def g_trackbar(d, a):
    d.line([1, 8, 14, 8], fill=INK, width=2)
    for x in (3, 7, 11, 13):
        d.line([x, 5, x, 11], fill=MID)
    d.rectangle([6, 4, 9, 11], outline=INK, fill=a)

def g_invisible(d, a):
    d.line([3, 3, 12, 12], fill=a, width=2)
    d.line([12, 3, 3, 12], fill=a, width=2)
    d.rectangle([1, 1, 14, 14], outline=MID)

def g_lock(d, a):
    d.rectangle([3, 7, 12, 14], outline=INK, fill=LIGHT)
    d.arc([5, 2, 10, 9], 180, 360, fill=INK, width=2)
    d.point((7, 10), fill=a)

def g_password(d, a):
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    for x in (4, 7, 10):
        d.point((x, 7), fill=a); d.point((x, 8), fill=a)

def g_scalebutton(d, a):
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([3, 7, 9, 7], fill=a, width=2)
    d.polygon([(11, 5), (11, 9), (13, 7)], fill=INK)

def g_popover(d, a):
    d.polygon([(7, 1), (9, 4), (5, 4)], fill=a)
    d.rectangle([2, 4, 13, 13], outline=INK, fill=WHITE)
    d.line([4, 7, 11, 7], fill=MID); d.line([4, 10, 8, 10], fill=MID)

def g_actionbar(d, a):
    d.rectangle([1, 10, 14, 14], fill=a)
    d.rectangle([1, 1, 14, 9], outline=INK, fill=WHITE)
    d.line([3, 4, 11, 4], fill=MID); d.line([3, 7, 8, 7], fill=MID)

def g_infobar(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 5], fill=a)
    d.ellipse([6, 2, 9, 5], fill=WHITE)
    d.line([3, 8, 12, 8], fill=MID); d.line([3, 11, 9, 11], fill=MID)

def g_cellview(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([3, 3, 6, 6], fill=a)
    d.line([8, 4, 12, 4], fill=MID); d.line([3, 9, 12, 9], fill=MID)
    d.line([3, 12, 9, 12], fill=MID)

def g_glarea(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(3, 12), (8, 4), (13, 12)], outline=a)
    d.line([8, 4, 8, 12], fill=a)

def g_media(d, a):
    d.rectangle([1, 2, 14, 13], outline=INK, fill=WHITE)
    d.polygon([(6, 5), (6, 10), (11, 7)], fill=a)
    d.line([2, 13, 13, 13], fill=INK, width=2)

def g_arrow(d, a):
    d.polygon([(2, 7), (10, 7), (10, 3), (14, 8), (10, 13), (10, 9), (2, 9)],
              fill=a, outline=INK)
    d.line([3, 8, 9, 8], fill=WHITE)

def g_bevel(d, a):
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([4, 4, 11, 11], outline=a, fill=LIGHT)

def g_headerctl(d, a):
    d.rectangle([1, 1, 14, 5], fill=a)
    d.line([5, 1, 5, 5], fill=WHITE); d.line([10, 1, 10, 5], fill=WHITE)
    d.rectangle([1, 6, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 9, 12, 9], fill=MID); d.line([3, 12, 8, 12], fill=MID)

def g_folder(d, a):
    d.polygon([(2, 4), (2, 13), (13, 13), (13, 4), (7, 4), (6, 2), (2, 2)], outline=INK, fill=(255, 214, 120))
    d.line([2, 6, 13, 6], fill=a)

def g_misc(d, a):
    d.rectangle([3, 3, 12, 12], outline=INK, fill=LIGHT)
    d.point((7, 7), fill=a); d.point((8, 7), fill=a)
    d.point((7, 8), fill=a); d.point((8, 8), fill=a)

GLYPHS = {
    "window": g_window, "dialog": g_dialog, "button": g_button,
    "check": g_check, "radio": g_radio, "toggle": g_toggle,
    "link": g_link, "label": g_label, "image": g_image,
    "progress": g_progress, "level": g_level, "spinner": g_spinner,
    "slider": g_slider, "spin": g_spin, "entry": g_entry,
    "textview": g_textview, "combo": g_combo, "list": g_list,
    "tree": g_tree, "grid": g_grid, "tabs": g_tabs, "paned": g_paned,
    "box": g_box, "frame": g_frame, "expander": g_expander,
    "menu": g_menu, "menubar": g_menubar, "toolbar": g_toolbar,
    "scrollbar": g_scrollbar, "separator": g_separator, "color": g_color,
    "font": g_font, "file": g_file, "search": g_search,
    "statusbar": g_statusbar, "headerbar": g_headerbar, "video": g_video,
    "pencil": g_pencil, "eventbox": g_eventbox, "overlay": g_overlay,
    "stack": g_stack, "calendar": g_calendar, "clock": g_clock,
    "calculator": g_calculator, "memo": g_memo, "shape": g_shape,
    "splitter": g_splitter, "tray": g_tray, "timer": g_timer,
    "graph": g_graph, "spreadsheet": g_spreadsheet,
    "checkgroup": g_checkgroup, "radiogroup": g_radiogroup,
    "updown": g_updown, "trackbar": g_trackbar, "invisible": g_invisible,
    "lock": g_lock, "password": g_password, "scalebutton": g_scalebutton,
    "popover": g_popover, "actionbar": g_actionbar, "infobar": g_infobar,
    "cellview": g_cellview, "glarea": g_glarea, "media": g_media,
    "arrow": g_arrow, "bevel": g_bevel, "headerctl": g_headerctl,
    "folder": g_folder, "misc": g_misc,
}

# Explicit glyph per widget where the keyword rules would misfire.
OVERRIDES = {
    "GtkAboutDialog": "dialog", "GtkAppChooserDialog": "dialog",
    "GtkApplicationWindow": "window", "GtkAssistant": "dialog",
    "GtkColorChooserDialog": "dialog", "GtkDialog": "dialog",
    "GtkFileChooserDialog": "dialog", "GtkFontChooserDialog": "dialog",
    "GtkMessageDialog": "dialog", "GtkPrintUnixDialog": "dialog",
    "GtkShortcutsWindow": "window", "GtkWindow": "window",
    "TForm": "window", "TTaskDialog": "dialog",
    "TCalculatorDialog": "calculator", "TCalendarDialog": "calendar",
    "TColorDialog": "color", "TFindDialog": "search",
    "TFontDialog": "font", "TOpenDialog": "file", "TOpenPictureDialog": "image",
    "TPageSetupDialog": "file", "TPrintDialog": "file",
    "TPrinterSetupDialog": "file", "TReplaceDialog": "search",
    "TSaveDialog": "file", "TSavePictureDialog": "image",
    "TSelectDirectoryDialog": "folder", "GtkCheckMenuItem": "check",
    "GtkRadioMenuItem": "radio", "GtkSeparatorMenuItem": "separator",
    "GtkSeparator": "separator", "GtkInvisible": "invisible",
    "GtkAccelLabel": "label", "GtkInscription": "label",
    "GtkCellView": "cellview", "GtkGLArea": "glarea",
    "GtkMediaControls": "media", "GtkVideo": "video",
    "GtkInfoBar": "infobar", "GtkActionBar": "actionbar",
    "GtkPopover": "popover", "GtkHeaderBar": "headerbar",
    "GtkStatusbar": "statusbar", "TStatusBar": "statusbar",
    "GtkPasswordEntry": "password", "GtkScaleButton": "scalebutton",
    "GtkVolumeButton": "button", "GtkLinkButton": "link",
    "GtkLockButton": "lock", "GtkColorButton": "color",
    "GtkColorDialogButton": "color", "GtkFileChooserButton": "file",
    "GtkFontButton": "font", "GtkModelButton": "button",
    "GtkToggleToolButton": "toggle", "GtkToolButton": "button",
    "TBitBtn": "button", "TButtonPanel": "button", "TSpeedButton": "button",
    "TCheckComboBox": "check", "TColorButton": "color",
    "TColorBox": "color", "TColorListBox": "color",
    "TUpDown": "updown", "TTrackBar": "trackbar",
    "TDateTimePicker": "clock", "TTimeEdit": "clock", "TDateEdit": "calendar",
    "TCalendar": "calendar", "GtkCalendar": "calendar",
    "TCalculatorDialog": "calculator", "TCalcEdit": "calculator",
    "TDrawGrid": "grid", "TStringGrid": "spreadsheet",
    "XSpreadsheet": "spreadsheet", "XGraph": "graph",
    "GtkDrawingArea": "pencil", "TPaintBox": "pencil",
    "GtkEventBox": "eventbox", "GtkOverlay": "overlay",
    "GtkStack": "stack", "GtkRevealer": "expander",
    "GtkExpander": "expander", "GtkScrollbar": "scrollbar",
    "TScrollBar": "scrollbar", "GtkNotebook": "tabs",
    "TNotebook": "tabs", "TPageControl": "tabs", "TTabControl": "tabs",
    "GtkPaned": "paned", "TPairSplitter": "splitter",
    "TSplitter": "splitter", "GtkAspectFrame": "frame",
    "GtkFrame": "frame", "TFrame": "frame", "TGroupBox": "checkgroup",
    "TCheckGroup": "checkgroup", "TRadioGroup": "radiogroup",
    "TCheckListBox": "list", "TShellListView": "folder",
    "TShellTreeView": "folder", "TFileListBox": "file",
    "THeaderControl": "headerctl", "TImageList": "image",
    "GtkAppChooserWidget": "grid", "GtkPlacesSidebar": "folder",
    "GtkSwitch": "toggle", "TToggleBox": "toggle",
    "TApplicationProperties": "misc", "TTimer": "timer",
    "TTrayIcon": "tray", "TImage": "image", "GtkImage": "image",
    "GtkPicture": "image", "TShape": "shape", "TArrow": "arrow",
    "TBevel": "bevel", "GtkSpinner": "spinner",
    "GtkProgressBar": "progress", "TProgressBar": "progress",
    "GtkLevelBar": "level", "GtkScale": "slider",
    "GtkSpinButton": "spin", "TSpinEdit": "spin",
    "TFloatSpinEdit": "spin", "GtkComboBox": "combo",
    "GtkComboBoxText": "combo", "GtkDropDown": "combo",
    "TComboBox": "combo", "TComboBoxEx": "combo",
    "TFilterComboBox": "combo", "GtkEntry": "entry",
    "GtkEntryCompletion": "entry", "GtkSearchEntry": "search",
    "GtkSearchBar": "search", "TEdit": "entry", "TEditButton": "entry",
    "TFileNameEdit": "file", "TDirectoryEdit": "folder",
    "TMaskEdit": "entry", "TLabeledEdit": "entry",
    "TValueListEditor": "list", "TMemo": "memo", "GtkTextView": "textview",
    "GtkEditableLabel": "label", "TLabel": "label", "GtkLabel": "label",
    "TStaticText": "label", "GtkListBox": "list", "TListBox": "list",
    "GtkListView": "list", "TListView": "list",
    "GtkColumnView": "grid", "GtkGridView": "grid",
    "GtkIconView": "image", "GtkTreeView": "tree", "TTreeView": "tree",
    "GtkMenu": "menu", "GtkMenuBar": "menubar", "GtkMenuButton": "menu",
    "GtkMenuItem": "menu", "GtkToolbar": "toolbar", "TToolBar": "toolbar",
    "TMainMenu": "menubar", "TPopupMenu": "menu", "TPopupNotifier": "infobar",
    "TActionList": "menu", "TControlBar": "toolbar", "TCoolBar": "toolbar",
    "GtkButtonBox": "box", "GtkBox": "box", "GtkCenterBox": "box",
    "GtkFixed": "box", "GtkFlowBox": "box", "GtkGrid": "grid",
    "GtkLayout": "box", "GtkScrolledWindow": "box",
    "GtkStackSidebar": "tabs", "GtkStackSwitcher": "tabs",
    "GtkViewport": "box", "TFlowPanel": "box", "TPanel": "box",
    "TScrollBox": "box",
}


def glyph_for(name):
    if name in OVERRIDES:
        return OVERRIDES[name]
    n = name.lower()
    for key in ("calendar", "calculator", "check", "radio", "color",
                "combo", "button", "dialog", "window", "menu", "tree",
                "grid", "list", "entry", "label", "image", "graph",
                "progress", "spin", "slider", "search", "file", "font"):
        if key in n:
            return key
    return "misc"


def main():
    comps = sorted(os.listdir(os.path.join(ROOT, "components")))
    errs = validate(comps)
    if errs:
        for e in errs:
            print("SECTION ERROR: " + e, file=sys.stderr)
        return 1
    outdir = os.path.join(ROOT, "icons")
    os.makedirs(outdir, exist_ok=True)
    used = {}
    for name in comps:
        glyph = glyph_for(name)
        used.setdefault(glyph, []).append(name)
        accent = SECTION_COLORS[section_of(name)]
        img = Image.new("RGB", (16, 16), WHITE)
        d = ImageDraw.Draw(img)
        GLYPHS[glyph](d, accent)
        path = os.path.join(outdir, name + ".ico")
        # BMP-format ICO (32bpp, uncompressed): readable by the palette's
        # decoder with no image library.
        img.save(path, format="ICO", sizes=[(16, 16)], bitmap_format="bmp")
    print(f"icons written: {len(comps)} in {outdir}")
    print(f"glyph kinds used: {len(used)}")
    for g in sorted(used):
        print(f"  {g}: {len(used[g])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
