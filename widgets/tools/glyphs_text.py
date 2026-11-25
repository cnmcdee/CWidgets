#!/usr/bin/env python3
"""Unique 16x16 icons for the Text & Entry / Views & Lists /
Menus & Toolbars / Misc-X widgets (51 widgets).

Each glyph is drawn with PIL ImageDraw in a 16x16 box, in the same
style as tools/gen_icons.py (dark ink INK + MID + LIGHT on white, the
widget's section accent colour from tools/sections.py, plus 1-2 extra
bright colours where it helps).  Every glyph in this module is visibly
different from the others, including the ones that used to share a
glyph in gen_icons.py.

Widgets covered (51):
  Combos (6): GtkComboBox, GtkComboBoxText, GtkDropDown, TComboBox,
    TComboBoxEx, TFilterComboBox
  Entries (6): GtkEntry, GtkEntryCompletion, TEdit, TEditButton,
    TLabeledEdit, TMaskEdit
  Labels (5): GtkAccelLabel, GtkInscription, GtkLabel, TLabel, TStaticText
  Lists (5): GtkListBox, GtkListView, TCheckListBox, TListBox, TListView
  Menus (5): GtkMenu, GtkMenuButton, GtkMenuItem, TActionList, TPopupMenu
  Grids (3): GtkColumnView, GtkGridView, TDrawGrid
  Spins (3): GtkSpinButton, TFloatSpinEdit, TSpinEdit
  Pairs (18): GtkMenuBar, TMainMenu, GtkSearchBar, GtkSearchEntry,
    GtkSeparator, GtkSeparatorMenuItem, GtkToolbar, TToolBar,
    GtkTreeView, TTreeView, TControlBar, TCoolBar, TDateTimePicker,
    TTimeEdit, TShellListView, TShellTreeView, TStringGrid, XSpreadsheet

Usage: python3 widgets/tools/glyphs_text.py   (run from the repo root)
Regenerates ONLY these 51 .ico files into widgets/icons/.
"""
import os, sys
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from sections import SECTION_COLORS, section_of  # noqa: E402

INK = (30, 30, 30)
MID = (140, 140, 140)
LIGHT = (226, 228, 232)
WHITE = (255, 255, 255)
# Extra bright colours, used sparingly to keep the set colourful.
BLUE = (37, 99, 235)
GREEN = (22, 163, 74)
YELLOW = (234, 179, 8)
RED = (220, 38, 38)
FOLDER = (255, 214, 120)

# ---------------------------------------------------------------- combos
# Every glyph draws inside the 16x16 box (0..15).  d = ImageDraw, a = accent.

def g_text_combo(d, a):
    """GtkComboBox: closed combo, accent-filled drop button, grey text."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.rectangle([10, 4, 14, 11], fill=a)
    d.polygon([(11, 6), (13, 6), (12, 9)], fill=WHITE)
    d.line([3, 7, 8, 7], fill=MID)

def g_text_combo_text(d, a):
    """GtkComboBoxText: combo with two lines of text, plain arrow button."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([3, 6, 8, 6], fill=INK)
    d.line([3, 9, 6, 9], fill=MID)
    d.line([10, 4, 10, 11], fill=INK)
    d.polygon([(11, 6), (13, 6), (12, 9)], fill=a)

def g_text_dropdown(d, a):
    """GtkDropDown: button on top, open list hanging below it."""
    d.rounded_rectangle([1, 1, 14, 6], radius=2, outline=INK, fill=LIGHT)
    d.line([3, 3, 8, 3], fill=INK)
    d.polygon([(11, 2), (13, 2), (12, 4)], fill=a)
    d.rectangle([1, 7, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 8, 13, 10], fill=LIGHT)
    d.line([4, 9, 11, 9], fill=INK)
    d.line([4, 12, 9, 12], fill=MID)

def g_text_tcombo(d, a):
    """TComboBox: edit cursor at left, up+down arrows in the button."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([3, 7, 3, 9], fill=a, width=2)
    d.line([5, 8, 8, 8], fill=MID)
    d.line([10, 4, 10, 11], fill=INK)
    d.polygon([(11, 5), (13, 5), (12, 7)], fill=a)
    d.polygon([(11, 11), (13, 11), (12, 9)], fill=a)

def g_text_comboex(d, a):
    """TComboBoxEx: combo with a little picture thumbnail inside."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.rectangle([2, 5, 5, 10], outline=INK, fill=WHITE)
    d.rectangle([3, 6, 4, 7], fill=BLUE)
    d.rectangle([3, 8, 4, 9], fill=GREEN)
    d.line([7, 7, 9, 7], fill=MID)
    d.line([11, 4, 11, 11], fill=INK)
    d.polygon([(12, 6), (14, 6), (13, 9)], fill=a)

def g_text_filtercombo(d, a):
    """TFilterComboBox: combo on top, blue filter funnel below it."""
    d.rectangle([1, 2, 14, 8], outline=INK, fill=WHITE)
    d.line([3, 5, 8, 5], fill=MID)
    d.polygon([(11, 4), (13, 4), (12, 6)], fill=a)
    d.polygon([(5, 10), (10, 10), (8, 12), (8, 14), (7, 14), (7, 12)],
              fill=BLUE)

# ---------------------------------------------------------------- entries

def g_text_entry(d, a):
    """GtkEntry: plain entry, accent cursor at left, grey text line."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([4, 6, 4, 9], fill=a, width=2)
    d.line([7, 8, 12, 8], fill=MID)

def g_text_completion(d, a):
    """GtkEntryCompletion: entry on top, completion popup below it."""
    d.rectangle([1, 1, 14, 6], outline=INK, fill=WHITE)
    d.line([3, 3, 3, 5], fill=a, width=2)
    d.line([6, 4, 11, 4], fill=MID)
    d.rectangle([2, 7, 13, 14], outline=INK, fill=WHITE)
    d.rectangle([3, 8, 12, 10], fill=a)
    d.line([4, 9, 11, 9], fill=WHITE)
    d.line([4, 12, 10, 12], fill=MID)

def g_text_tedit(d, a):
    """TEdit: entry with ink text and the cursor at the end of it."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([3, 7, 8, 7], fill=INK)
    d.line([3, 9, 6, 9], fill=MID)
    d.line([10, 6, 10, 9], fill=a, width=2)

def g_text_editbutton(d, a):
    """TEditButton: entry with an accent button (white ...) at right."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.line([3, 7, 8, 7], fill=MID)
    d.rectangle([10, 4, 14, 11], outline=INK, fill=a)
    d.point((11, 7), fill=WHITE)
    d.point((12, 7), fill=WHITE)
    d.point((13, 7), fill=WHITE)

def g_text_labelededit(d, a):
    """TLabeledEdit: accent label tag + text above the entry box."""
    d.rectangle([1, 1, 6, 3], fill=a)
    d.line([8, 2, 13, 2], fill=MID)
    d.rectangle([1, 5, 14, 12], outline=INK, fill=WHITE)
    d.line([4, 8, 4, 10], fill=a, width=2)
    d.line([7, 9, 12, 9], fill=MID)

def g_text_maskedit(d, a):
    """TMaskEdit: entry showing mask dots with an accent dash."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.rectangle([3, 7, 4, 8], fill=INK)
    d.rectangle([5, 7, 6, 8], fill=INK)
    d.line([7, 7, 8, 7], fill=a)
    d.rectangle([9, 7, 10, 8], fill=INK)
    d.rectangle([11, 7, 12, 8], fill=INK)

# ---------------------------------------------------------------- labels

def g_text_accellabel(d, a):
    """GtkAccelLabel: text lines, first letter accent + underlined."""
    d.line([2, 3, 13, 3], fill=INK, width=2)
    d.line([2, 7, 3, 7], fill=a, width=2)
    d.line([5, 7, 10, 7], fill=MID, width=2)
    d.line([2, 9, 3, 9], fill=a, width=2)
    d.line([2, 12, 13, 12], fill=MID, width=2)

def g_text_inscription(d, a):
    """GtkInscription: engraved plate, double border with text inside."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([3, 3, 12, 12], outline=a)
    d.line([5, 6, 10, 6], fill=INK)
    d.line([5, 9, 10, 9], fill=MID)

def g_text_label(d, a):
    """GtkLabel: plain three-line label (offset from the old shared one)."""
    d.line([2, 3, 12, 3], fill=INK, width=2)
    d.line([2, 7, 13, 7], fill=a, width=2)
    d.line([2, 11, 9, 11], fill=MID, width=2)

def g_text_tlabel(d, a):
    """TLabel: bold T mark at left, text lines at right."""
    d.rectangle([1, 3, 6, 4], fill=INK)
    d.rectangle([3, 5, 4, 10], fill=INK)
    d.line([8, 4, 13, 4], fill=MID, width=2)
    d.line([8, 8, 11, 8], fill=a, width=2)
    d.line([8, 12, 13, 12], fill=MID, width=2)

def g_text_statictext(d, a):
    """TStaticText: text lines on a light panel."""
    d.rounded_rectangle([1, 2, 14, 13], radius=2, outline=INK, fill=LIGHT)
    d.line([4, 5, 12, 5], fill=INK, width=2)
    d.line([4, 9, 9, 9], fill=a, width=2)

# ---------------------------------------------------------------- lists

def g_text_listbox(d, a):
    """GtkListBox: rows split by dividers, first row tinted."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 5], fill=(233, 240, 213))
    d.line([3, 3, 12, 3], fill=INK)
    d.line([1, 5, 14, 5], fill=MID)
    d.line([3, 8, 10, 8], fill=MID)
    d.line([1, 10, 14, 10], fill=MID)
    d.line([3, 12, 12, 12], fill=MID)

def g_text_listview(d, a):
    """GtkListView: accent header, two-column rows with bullets."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.line([5, 1, 5, 4], fill=WHITE)
    d.line([10, 1, 10, 4], fill=WHITE)
    for y in (7, 10, 13):
        d.rectangle([3, y - 1, 4, y], fill=a)
        d.line([6, y, 9, y], fill=MID)
        d.line([11, y, 13, y], fill=MID)

def g_text_checklistbox(d, a):
    """TCheckListBox: list rows with checkboxes, two ticked green."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    for y in (3, 7, 11):
        d.rectangle([2, y, 5, y + 3], outline=INK, fill=WHITE)
    d.line([2, 5, 3, 6, 5, 3], fill=GREEN)
    d.line([2, 9, 3, 10, 5, 7], fill=GREEN)
    d.line([7, 4, 12, 4], fill=MID)
    d.line([7, 8, 12, 8], fill=MID)
    d.line([7, 12, 10, 12], fill=MID)

def g_text_tlistbox(d, a):
    """TListBox: plain list, selected row filled with the accent."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 13, 5], fill=a)
    d.line([4, 3, 11, 3], fill=WHITE)
    d.line([4, 7, 11, 7], fill=MID)
    d.line([4, 10, 9, 10], fill=MID)
    d.line([4, 13, 12, 13], fill=MID)

def g_text_tlistview(d, a):
    """TListView: light header, coloured row icons, scrollbar at right."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=LIGHT)
    d.line([1, 4, 14, 4], fill=INK)
    d.line([3, 2, 8, 2], fill=INK)
    d.rectangle([2, 6, 4, 8], fill=a)
    d.rectangle([2, 9, 4, 11], fill=BLUE)
    d.rectangle([2, 12, 4, 14], fill=GREEN)
    d.line([6, 7, 10, 7], fill=MID)
    d.line([6, 10, 10, 10], fill=MID)
    d.line([6, 13, 10, 13], fill=MID)
    d.rectangle([12, 5, 14, 14], fill=LIGHT, outline=MID)
    d.rectangle([12, 7, 14, 10], fill=a)

# ---------------------------------------------------------------- menus

def g_text_menu(d, a):
    """GtkMenu: popup menu with shadow, one row highlighted accent."""
    d.rectangle([3, 3, 14, 14], fill=MID)
    d.rectangle([1, 1, 13, 13], outline=INK, fill=WHITE)
    d.line([4, 3, 11, 3], fill=INK)
    d.rectangle([2, 5, 12, 7], fill=a)
    d.line([4, 6, 10, 6], fill=WHITE)
    d.line([4, 10, 11, 10], fill=MID)
    d.line([4, 12, 8, 12], fill=MID)

def g_text_menubutton(d, a):
    """GtkMenuButton: rounded button with menu lines + down arrow."""
    d.rounded_rectangle([1, 3, 14, 12], radius=2, outline=INK, fill=WHITE)
    d.line([3, 6, 8, 6], fill=INK, width=2)
    d.line([3, 9, 8, 9], fill=MID, width=2)
    d.polygon([(11, 6), (13, 6), (12, 9)], fill=a)

def g_text_menuitem(d, a):
    """GtkMenuItem: one highlighted item row inside a faint menu frame."""
    d.rectangle([1, 1, 14, 14], outline=MID, fill=WHITE)
    d.rectangle([1, 5, 14, 10], outline=INK, fill=(254, 243, 199))
    d.line([3, 8, 4, 9, 6, 6], fill=GREEN)
    d.line([8, 7, 12, 7], fill=INK)

def g_text_actionlist(d, a):
    """TActionList: list of actions with a yellow lightning bolt."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 3, 8, 3], fill=MID)
    d.line([3, 6, 8, 6], fill=MID)
    d.line([3, 9, 8, 9], fill=MID)
    d.line([3, 12, 8, 12], fill=MID)
    d.polygon([(12, 1), (9, 6), (11, 6), (10, 10), (14, 4), (12, 4)],
              fill=YELLOW, outline=INK)

def g_text_popupmenu(d, a):
    """TPopupMenu: popup menu with shadow, mouse pointer at its corner."""
    d.rectangle([3, 3, 14, 14], fill=MID)
    d.rectangle([1, 1, 13, 13], outline=INK, fill=WHITE)
    d.line([4, 4, 11, 4], fill=INK)
    d.line([4, 7, 9, 7], fill=MID)
    d.line([4, 10, 11, 10], fill=MID)
    d.polygon([(0, 0), (0, 7), (2, 5), (4, 9), (5, 8), (3, 4), (6, 4)],
              fill=WHITE, outline=INK)

# ---------------------------------------------------------------- grids

def g_text_columnview(d, a):
    """GtkColumnView: accent header with sort arrow, coloured cell marks."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.line([5, 1, 5, 4], fill=WHITE)
    d.line([10, 1, 10, 4], fill=WHITE)
    d.polygon([(2, 2), (4, 2), (3, 4)], fill=WHITE)
    d.line([5, 5, 5, 14], fill=INK)
    d.line([10, 5, 10, 14], fill=INK)
    d.rectangle([2, 6, 3, 7], fill=a)
    d.rectangle([7, 6, 8, 7], fill=BLUE)
    d.rectangle([11, 6, 12, 7], fill=GREEN)
    d.line([1, 8, 14, 8], fill=MID)
    d.line([1, 11, 14, 11], fill=MID)

def g_text_gridview(d, a):
    """GtkGridView: uniform grid, centre cell selected in the accent."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([5, 1, 5, 14], fill=INK)
    d.line([10, 1, 10, 14], fill=INK)
    d.line([1, 5, 14, 5], fill=INK)
    d.line([1, 10, 14, 10], fill=INK)
    d.rectangle([6, 6, 9, 9], fill=a)
    d.line([2, 3, 4, 3], fill=MID)
    d.line([11, 3, 13, 3], fill=MID)
    d.line([2, 12, 4, 12], fill=MID)
    d.line([11, 12, 13, 12], fill=MID)

def g_text_drawgrid(d, a):
    """TDrawGrid: plain grid with a blue pencil drawn across it."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([5, 1, 5, 14], fill=MID)
    d.line([10, 1, 10, 14], fill=MID)
    d.line([1, 5, 14, 5], fill=MID)
    d.line([1, 10, 14, 10], fill=MID)
    d.line([3, 12, 11, 4], fill=BLUE, width=2)
    d.polygon([(11, 4), (13, 2), (13, 5)], fill=INK)

# ---------------------------------------------------------------- spins

def g_text_spinbutton(d, a):
    """GtkSpinButton: spin box, digit blocks, accent up/down triangles."""
    d.rectangle([1, 3, 14, 12], outline=INK, fill=WHITE)
    d.line([10, 3, 10, 12], fill=INK)
    d.polygon([(11, 6), (13, 6), (12, 4)], fill=a)
    d.polygon([(11, 9), (13, 9), (12, 11)], fill=a)
    d.rectangle([3, 6, 4, 8], fill=INK)
    d.rectangle([6, 6, 7, 8], fill=MID)

def g_text_floatspin(d, a):
    """TFloatSpinEdit: spin box with a decimal point, blue arrows."""
    d.rectangle([1, 3, 14, 12], outline=INK, fill=WHITE)
    d.line([10, 3, 10, 12], fill=INK)
    d.polygon([(11, 6), (13, 6), (12, 4)], fill=BLUE)
    d.polygon([(11, 9), (13, 9), (12, 11)], fill=BLUE)
    d.rectangle([3, 6, 4, 8], fill=INK)
    d.point((5, 8), fill=INK)
    d.rectangle([6, 6, 7, 8], fill=MID)

def g_text_spinedit(d, a):
    """TSpinEdit: spin box with two separate stacked arrow buttons."""
    d.rectangle([1, 3, 14, 12], outline=INK, fill=WHITE)
    d.rectangle([10, 3, 14, 7], outline=INK, fill=LIGHT)
    d.rectangle([10, 8, 14, 12], outline=INK, fill=LIGHT)
    d.polygon([(11, 4), (13, 4), (12, 6)], fill=a)
    d.polygon([(11, 11), (13, 11), (12, 9)], fill=a)
    d.line([3, 7, 8, 7], fill=MID)

# ---------------------------------------------------------------- pairs

def g_text_menubar(d, a):
    """GtkMenuBar: accent bar with white menu words, content below."""
    d.rectangle([1, 1, 14, 4], fill=a)
    d.rectangle([2, 2, 5, 3], fill=WHITE)
    d.rectangle([7, 2, 10, 3], fill=WHITE)
    d.rectangle([1, 5, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID)
    d.line([3, 11, 8, 11], fill=MID)

def g_text_mainmenu(d, a):
    """TMainMenu: white bar with accent menu words + arrow, content below."""
    d.rectangle([1, 1, 14, 4], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 5, 3], fill=a)
    d.rectangle([7, 2, 10, 3], fill=a)
    d.polygon([(12, 1), (14, 1), (13, 3)], fill=INK)
    d.rectangle([1, 5, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID)
    d.line([3, 11, 8, 11], fill=MID)

def g_text_searchbar(d, a):
    """GtkSearchBar: wide search bar, magnifier at left, content below."""
    d.rectangle([1, 2, 14, 9], outline=INK, fill=WHITE)
    d.ellipse([3, 3, 7, 7], outline=INK, width=1)
    d.line([6, 6, 9, 9], fill=INK, width=2)
    d.line([10, 4, 13, 4], fill=MID)
    d.line([10, 7, 12, 7], fill=MID)
    d.line([3, 12, 12, 12], fill=MID)

def g_text_searchentry(d, a):
    """GtkSearchEntry: entry box, magnifier inside, clear X at right."""
    d.rectangle([1, 4, 14, 11], outline=INK, fill=WHITE)
    d.ellipse([3, 5, 6, 8], outline=INK, width=1)
    d.line([6, 8, 8, 10], fill=a, width=1)
    d.line([9, 7, 11, 7], fill=MID)
    d.line([12, 6, 14, 8], fill=MID)
    d.line([14, 6, 12, 8], fill=MID)

def g_text_separator(d, a):
    """GtkSeparator: full-width double rule."""
    d.line([1, 7, 14, 7], fill=INK, width=2)
    d.line([1, 11, 14, 11], fill=MID)

def g_text_separatormenuitem(d, a):
    """GtkSeparatorMenuItem: menu frame with a divider row inside it."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 3, 8, 3], fill=MID)
    d.line([3, 7, 12, 7], fill=INK, width=2)
    d.line([3, 11, 10, 11], fill=MID)

def g_text_toolbar(d, a):
    """GtkToolbar: tool strip with buttons, content below."""
    d.rectangle([1, 1, 14, 5], fill=LIGHT, outline=INK)
    d.rectangle([2, 2, 5, 4], fill=a)
    d.rectangle([7, 2, 9, 4], outline=INK)
    d.rectangle([11, 2, 13, 4], outline=INK)
    d.rectangle([1, 6, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 9, 11, 9], fill=MID)
    d.line([3, 12, 8, 12], fill=MID)

def g_text_ttoolbar(d, a):
    """TToolBar: tool strip with grip dots and a drop-down button."""
    d.rectangle([1, 1, 14, 5], fill=LIGHT, outline=INK)
    d.point((2, 2), fill=INK)
    d.point((2, 4), fill=INK)
    d.rectangle([4, 2, 7, 4], fill=a)
    d.rectangle([9, 2, 11, 4], outline=INK)
    d.polygon([(12, 2), (14, 2), (13, 4)], fill=INK)
    d.rectangle([1, 6, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 9, 11, 9], fill=MID)
    d.line([3, 12, 8, 12], fill=MID)

def g_text_gtreeview(d, a):
    """GtkTreeView: tree with +/- expander boxes and node squares."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([4, 3, 4, 12], fill=MID)
    d.rectangle([2, 2, 4, 4], outline=INK, fill=WHITE)
    d.line([2, 3, 4, 3], fill=INK)
    d.rectangle([2, 6, 4, 8], outline=INK, fill=WHITE)
    d.line([3, 6, 3, 8], fill=INK)
    d.line([2, 7, 4, 7], fill=INK)
    d.rectangle([6, 2, 8, 4], fill=a)
    d.line([10, 3, 12, 3], fill=MID)
    d.line([4, 7, 5, 7], fill=MID)
    d.rectangle([6, 6, 8, 8], outline=INK, fill=LIGHT)
    d.line([10, 7, 12, 7], fill=MID)
    d.line([4, 11, 5, 11], fill=MID)
    d.rectangle([6, 10, 8, 12], outline=INK, fill=LIGHT)
    d.line([10, 11, 12, 11], fill=MID)

def g_text_ttreeview(d, a):
    """TTreeView: tree of yellow folders, root folder + nested child."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(2, 2), (2, 6), (8, 6), (8, 2), (5, 2), (4, 1), (2, 1)],
              outline=INK, fill=FOLDER)
    d.line([4, 6, 4, 8], fill=MID)
    d.line([4, 7, 6, 7], fill=MID)
    d.rectangle([6, 8, 9, 11], outline=INK, fill=BLUE)
    d.line([10, 4, 13, 4], fill=MID)
    d.line([11, 9, 13, 9], fill=MID)
    d.line([4, 13, 6, 13], fill=MID)

def g_text_controlbar(d, a):
    """TControlBar: control strip with play / stop / pause buttons."""
    d.rectangle([1, 1, 14, 6], fill=LIGHT, outline=INK)
    d.polygon([(3, 2), (3, 5), (6, 3)], fill=GREEN)
    d.rectangle([8, 2, 10, 5], fill=RED)
    d.line([12, 2, 12, 5], fill=INK)
    d.line([14, 2, 14, 5], fill=INK)
    d.rectangle([1, 7, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 10, 11, 10], fill=MID)
    d.line([3, 13, 8, 13], fill=MID)

def g_text_coolbar(d, a):
    """TCoolBar: two grip bands with buttons and a chevron."""
    d.rectangle([1, 1, 14, 6], fill=LIGHT, outline=INK)
    d.point((2, 2), fill=INK)
    d.point((2, 4), fill=INK)
    d.rectangle([4, 2, 7, 4], fill=a)
    d.rectangle([9, 2, 12, 4], outline=INK)
    d.line([13, 2, 14, 3], fill=INK)
    d.line([14, 3, 13, 4], fill=INK)
    d.rectangle([1, 8, 14, 13], fill=WHITE, outline=INK)
    d.point((2, 9), fill=INK)
    d.point((2, 11), fill=INK)
    d.rectangle([4, 9, 7, 11], fill=BLUE)
    d.rectangle([9, 9, 12, 11], outline=INK)

def g_text_datetimepicker(d, a):
    """TDateTimePicker: calendar page with a clock overlapping it."""
    d.rectangle([1, 1, 12, 11], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 12, 4], fill=a)
    d.point((3, 7), fill=INK)
    d.point((6, 7), fill=INK)
    d.point((9, 7), fill=INK)
    d.point((3, 9), fill=INK)
    d.point((6, 9), fill=INK)
    d.ellipse([8, 8, 15, 15], outline=INK, fill=WHITE)
    d.line([11, 11, 11, 9], fill=a)
    d.line([11, 11, 13, 12], fill=INK)

def g_text_timeedit(d, a):
    """TTimeEdit: clock face with a digital time row underneath."""
    d.ellipse([3, 1, 12, 10], outline=INK, width=2)
    d.line([7, 5, 7, 3], fill=a, width=2)
    d.line([7, 5, 9, 7], fill=INK, width=2)
    d.line([2, 13, 4, 13], fill=INK)
    d.point((6, 12), fill=a)
    d.point((6, 14), fill=a)
    d.line([8, 13, 10, 13], fill=INK)
    d.line([12, 13, 13, 13], fill=MID)

def g_text_shelllistview(d, a):
    """TShellListView: yellow folder on top, file rows below it."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(2, 2), (2, 6), (8, 6), (8, 2), (5, 2), (4, 1), (2, 1)],
              outline=INK, fill=FOLDER)
    d.rectangle([2, 8, 4, 10], outline=INK, fill=WHITE)
    d.line([6, 9, 12, 9], fill=MID)
    d.rectangle([2, 12, 4, 14], outline=INK, fill=WHITE)
    d.line([6, 13, 12, 13], fill=MID)

def g_text_shelltreeview(d, a):
    """TShellTreeView: folder tree, root folder + indented child folder."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(2, 2), (2, 6), (8, 6), (8, 2), (5, 2), (4, 1), (2, 1)],
              outline=INK, fill=FOLDER)
    d.line([10, 4, 13, 4], fill=MID)
    d.line([4, 6, 4, 8], fill=MID)
    d.line([4, 7, 6, 7], fill=MID)
    d.polygon([(6, 8), (6, 12), (12, 12), (12, 8), (9, 8), (8, 7), (6, 7)],
              outline=INK, fill=FOLDER)
    d.line([4, 13, 6, 13], fill=MID)

def g_text_stringgrid(d, a):
    """TStringGrid: grid with an accent header, text in every cell."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.line([5, 1, 5, 14], fill=INK)
    d.line([10, 1, 10, 14], fill=INK)
    d.line([1, 8, 14, 8], fill=INK)
    d.line([1, 11, 14, 11], fill=INK)
    d.line([2, 6, 4, 6], fill=INK)
    d.line([7, 6, 8, 6], fill=MID)
    d.line([11, 6, 13, 6], fill=MID)
    d.line([2, 9, 4, 9], fill=MID)
    d.line([7, 9, 9, 9], fill=MID)
    d.line([2, 12, 3, 12], fill=MID)
    d.line([7, 12, 9, 12], fill=MID)
    d.line([11, 12, 13, 12], fill=MID)

def g_text_xspreadsheet(d, a):
    """XSpreadsheet: formula bar on top, green header, red selected cell."""
    d.rectangle([1, 1, 14, 3], outline=INK, fill=LIGHT)
    d.line([2, 1, 4, 3], fill=RED)
    d.line([4, 1, 2, 3], fill=RED)
    d.line([6, 2, 12, 2], fill=MID)
    d.rectangle([1, 4, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 4, 14, 6], fill=GREEN)
    d.line([5, 4, 5, 14], fill=INK)
    d.line([10, 4, 10, 14], fill=INK)
    d.line([1, 10, 14, 10], fill=INK)
    d.rectangle([6, 7, 9, 9], outline=RED)

# ---------------------------------------------------------------- registry

DRAW = {
    "GtkComboBox": g_text_combo,
    "GtkComboBoxText": g_text_combo_text,
    "GtkDropDown": g_text_dropdown,
    "TComboBox": g_text_tcombo,
    "TComboBoxEx": g_text_comboex,
    "TFilterComboBox": g_text_filtercombo,
    "GtkEntry": g_text_entry,
    "GtkEntryCompletion": g_text_completion,
    "TEdit": g_text_tedit,
    "TEditButton": g_text_editbutton,
    "TLabeledEdit": g_text_labelededit,
    "TMaskEdit": g_text_maskedit,
    "GtkAccelLabel": g_text_accellabel,
    "GtkInscription": g_text_inscription,
    "GtkLabel": g_text_label,
    "TLabel": g_text_tlabel,
    "TStaticText": g_text_statictext,
    "GtkListBox": g_text_listbox,
    "GtkListView": g_text_listview,
    "TCheckListBox": g_text_checklistbox,
    "TListBox": g_text_tlistbox,
    "TListView": g_text_tlistview,
    "GtkMenu": g_text_menu,
    "GtkMenuButton": g_text_menubutton,
    "GtkMenuItem": g_text_menuitem,
    "TActionList": g_text_actionlist,
    "TPopupMenu": g_text_popupmenu,
    "GtkColumnView": g_text_columnview,
    "GtkGridView": g_text_gridview,
    "TDrawGrid": g_text_drawgrid,
    "GtkSpinButton": g_text_spinbutton,
    "TFloatSpinEdit": g_text_floatspin,
    "TSpinEdit": g_text_spinedit,
    "GtkMenuBar": g_text_menubar,
    "TMainMenu": g_text_mainmenu,
    "GtkSearchBar": g_text_searchbar,
    "GtkSearchEntry": g_text_searchentry,
    "GtkSeparator": g_text_separator,
    "GtkSeparatorMenuItem": g_text_separatormenuitem,
    "GtkToolbar": g_text_toolbar,
    "TToolBar": g_text_ttoolbar,
    "GtkTreeView": g_text_gtreeview,
    "TTreeView": g_text_ttreeview,
    "TControlBar": g_text_controlbar,
    "TCoolBar": g_text_coolbar,
    "TDateTimePicker": g_text_datetimepicker,
    "TTimeEdit": g_text_timeedit,
    "TShellListView": g_text_shelllistview,
    "TShellTreeView": g_text_shelltreeview,
    "TStringGrid": g_text_stringgrid,
    "XSpreadsheet": g_text_xspreadsheet,
}


def main():
    outdir = os.path.join(ROOT, "icons")
    os.makedirs(outdir, exist_ok=True)
    for name in sorted(DRAW):
        accent = SECTION_COLORS[section_of(name)]
        img = Image.new("RGB", (16, 16), WHITE)
        d = ImageDraw.Draw(img)
        DRAW[name](d, accent)
        path = os.path.join(outdir, name + ".ico")
        # BMP-format ICO (32bpp, uncompressed): readable by the palette's
        # decoder with no image library (same as gen_icons.py).
        img.save(path, format="ICO", sizes=[(16, 16)], bitmap_format="bmp")
    print(f"icons written: {len(DRAW)} in {outdir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
