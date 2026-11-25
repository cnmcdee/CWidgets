#!/usr/bin/env python3
"""Unique 16x16 icons for the Windows / Dialogs / Containers groups.

Covers 51 widgets (the ones that used to share the generic window, dialog,
box, tabs, frame, expander, scrollbar, statusbar, splitter, file, search
and image glyphs):

  Windows (4):            GtkApplicationWindow, GtkShortcutsWindow,
                          GtkWindow, TForm
  Dialogs (10):           GtkAboutDialog, GtkAppChooserDialog, GtkAssistant,
                          GtkColorChooserDialog, GtkDialog,
                          GtkFileChooserDialog, GtkFontChooserDialog,
                          GtkMessageDialog, GtkPrintUnixDialog, TTaskDialog
  File/print dialogs (5): TOpenDialog, TPageSetupDialog, TPrintDialog,
                          TPrinterSetupDialog, TSaveDialog
  Find/picture (4):       TFindDialog, TReplaceDialog,
                          TOpenPictureDialog, TSavePictureDialog
  Containers (11):        GtkBox, GtkButtonBox, GtkCenterBox, GtkFixed,
                          GtkFlowBox, GtkLayout, GtkScrolledWindow,
                          GtkViewport, TFlowPanel, TPanel, TScrollBox
  Tabs (6):               GtkNotebook, GtkStackSidebar, GtkStackSwitcher,
                          TNotebook, TPageControl, TTabControl
  Frames (3):             GtkAspectFrame, GtkFrame, TFrame
  Pairs (8):              GtkExpander, GtkRevealer, GtkScrollbar,
                          TScrollBar, GtkStatusbar, TStatusBar,
                          TPairSplitter, TSplitter

Every glyph is drawn with PIL ImageDraw in the 16x16 box, in the same
base language as tools/gen_icons.py (dark ink INK + MID + LIGHT on white,
one accent colour per section from tools/sections.py) plus 1-2 extra
bright colours per glyph where it helps.  Dialogs stay dialog-LIKE
(window frame + title bar) but each carries its own distinct emblem.

Usage: python3 widgets/tools/glyphs_windows.py   (run from the repo root)
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
RED = (220, 38, 38)
ORANGE = (234, 88, 12)
YELLOW = (234, 179, 8)
GREEN = (22, 163, 74)
PURPLE = (147, 51, 234)
PINK = (219, 39, 119)
FOLDER = (255, 214, 120)

# ---------------------------------------------------------------- helpers
# d = ImageDraw, a = section accent colour.  All glyphs draw in 0..15.

def _dialog(d, a):
    """Window frame + filled title bar + close square (dialog base)."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.rectangle([12, 2, 13, 3], fill=WHITE)


def _window(d, a):
    """Window frame + filled title bar + two menu dots (window base)."""
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([1, 1, 14, 4], fill=a)
    d.point((3, 2), fill=WHITE)
    d.point((5, 2), fill=WHITE)


# ---------------------------------------------------------------- windows
def g_application_window(d, a):
    # Window with a sidebar + a 2x2 app grid in the content area.
    _window(d, a)
    d.rectangle([2, 5, 5, 13], outline=INK, fill=LIGHT)
    d.line([3, 7, 4, 7], fill=a, width=2)
    d.line([3, 10, 4, 10], fill=MID)
    d.rectangle([7, 6, 9, 8], fill=GREEN)
    d.rectangle([11, 6, 13, 8], fill=ORANGE)
    d.rectangle([7, 10, 9, 12], fill=PURPLE)
    d.rectangle([11, 10, 13, 12], fill=PINK)


def g_shortcuts_window(d, a):
    # Window with two rows of keyboard keys (wide spacebar, accent).
    _window(d, a)
    for x0 in (2, 5, 8, 11):
        d.rectangle([x0, 6, x0 + 2, 8], outline=INK, fill=LIGHT)
    d.rectangle([2, 10, 6, 12], fill=a)
    d.rectangle([8, 10, 10, 12], outline=INK, fill=WHITE)
    d.rectangle([11, 10, 13, 12], outline=INK, fill=WHITE)


def g_gtk_window(d, a):
    # Plain window: red close square + three text lines.
    _window(d, a)
    d.rectangle([12, 2, 13, 3], fill=RED)
    d.line([3, 7, 12, 7], fill=INK)
    d.line([3, 10, 9, 10], fill=MID)
    d.line([3, 12, 11, 12], fill=MID)


def g_form(d, a):
    # Window with two labelled form fields (accent + green tabs).
    _window(d, a)
    d.rectangle([3, 6, 12, 8], outline=INK, fill=WHITE)
    d.rectangle([3, 6, 5, 8], fill=a)
    d.line([6, 7, 11, 7], fill=MID)
    d.rectangle([3, 9, 12, 11], outline=INK, fill=WHITE)
    d.rectangle([3, 9, 5, 11], fill=GREEN)
    d.line([6, 10, 10, 10], fill=MID)


# ---------------------------------------------------------------- dialogs
def g_about_dialog(d, a):
    # Dialog + blue info circle with a white "i".
    _dialog(d, a)
    d.ellipse([5, 6, 10, 11], fill=a)
    d.rectangle([7, 7, 8, 8], fill=WHITE)
    d.rectangle([7, 9, 8, 10], fill=WHITE)
    d.line([4, 13, 11, 13], fill=MID)


def g_app_chooser_dialog(d, a):
    # Dialog + 2x2 grid of colourful app tiles.
    _dialog(d, a)
    d.rectangle([4, 6, 6, 8], fill=GREEN)
    d.rectangle([8, 6, 10, 8], fill=ORANGE)
    d.rectangle([4, 10, 6, 12], fill=PURPLE)
    d.rectangle([8, 10, 10, 12], fill=PINK)
    d.line([4, 13, 10, 13], fill=MID)


def g_assistant(d, a):
    # Wizard dialog: step dots (first done) + green forward arrow.
    _dialog(d, a)
    d.rectangle([4, 6, 5, 7], fill=a)
    d.rectangle([7, 6, 8, 7], fill=LIGHT, outline=MID)
    d.rectangle([10, 6, 11, 7], fill=LIGHT, outline=MID)
    d.line([4, 11, 10, 11], fill=GREEN, width=2)
    d.polygon([(10, 9), (10, 13), (13, 11)], fill=GREEN)


def g_color_chooser_dialog(d, a):
    # Dialog + row of colour swatches + accent slider bar.
    _dialog(d, a)
    d.rectangle([2, 6, 4, 9], fill=RED)
    d.rectangle([5, 6, 7, 9], fill=YELLOW)
    d.rectangle([8, 6, 10, 9], fill=GREEN)
    d.rectangle([11, 6, 13, 9], fill=PURPLE)
    d.line([3, 12, 12, 12], fill=a, width=2)


def g_gtk_dialog(d, a):
    # Generic dialog: text lines + OK (accent) / Cancel buttons.
    _dialog(d, a)
    d.line([3, 7, 12, 7], fill=MID)
    d.line([3, 9, 9, 9], fill=MID)
    d.rectangle([3, 11, 7, 13], fill=a)
    d.rectangle([9, 11, 13, 13], outline=INK, fill=WHITE)


def g_file_chooser_dialog(d, a):
    # Dialog + yellow folder.
    _dialog(d, a)
    d.polygon([(4, 6), (4, 12), (12, 12), (12, 6), (8, 6), (7, 5), (4, 5)],
              outline=INK, fill=FOLDER)
    d.line([4, 8, 12, 8], fill=a)


def g_font_chooser_dialog(d, a):
    # Dialog + big "A" with an accent crossbar.
    _dialog(d, a)
    d.line([7, 6, 4, 13], fill=INK, width=2)
    d.line([7, 6, 10, 13], fill=INK, width=2)
    d.line([5, 10, 9, 10], fill=a, width=2)


def g_message_dialog(d, a):
    # Dialog + red "!" bubble.
    _dialog(d, a)
    d.ellipse([5, 6, 10, 11], fill=RED)
    d.rectangle([7, 7, 8, 9], fill=WHITE)
    d.rectangle([7, 10, 8, 10], fill=WHITE)


def g_print_unix_dialog(d, a):
    # Dialog + small printer (paper up, body, green button).
    _dialog(d, a)
    d.rectangle([5, 5, 10, 8], outline=INK, fill=WHITE)
    d.line([6, 6, 9, 6], fill=MID)
    d.rectangle([3, 8, 12, 13], outline=INK, fill=LIGHT)
    d.line([5, 10, 10, 10], fill=INK)
    d.rectangle([10, 11, 11, 12], fill=GREEN)


def g_task_dialog(d, a):
    # Dialog + green check circle + task lines.
    _dialog(d, a)
    d.ellipse([4, 6, 8, 10], fill=GREEN)
    d.line([5, 8, 6, 9, 8, 7], fill=WHITE, width=1)
    d.line([10, 7, 13, 7], fill=MID)
    d.line([10, 9, 13, 9], fill=MID)


# ------------------------------------------------------- file/print dialogs
def g_open_dialog(d, a):
    # Open folder: white document peeking out of a yellow folder.
    d.rectangle([6, 2, 10, 7], outline=INK, fill=WHITE)
    d.line([7, 4, 9, 4], fill=a)
    d.polygon([(2, 5), (2, 13), (13, 13), (13, 5), (8, 5), (7, 3), (2, 3)],
              outline=INK, fill=FOLDER)
    d.line([2, 7, 13, 7], fill=a)


def g_page_setup_dialog(d, a):
    # Page with fold corner + accent margin frame.
    d.polygon([(4, 1), (11, 1), (13, 3), (13, 14), (4, 14)],
              outline=INK, fill=WHITE)
    d.line([11, 1, 11, 3, 13, 3], fill=INK)
    d.rectangle([6, 5, 11, 12], outline=a)
    d.line([7, 7, 10, 7], fill=MID)
    d.line([7, 9, 10, 9], fill=MID)


def g_print_dialog(d, a):
    # Standalone printer: paper in, body, paper out, green button.
    d.rectangle([4, 1, 11, 6], outline=INK, fill=WHITE)
    d.line([6, 3, 9, 3], fill=MID)
    d.rectangle([2, 6, 13, 11], outline=INK, fill=LIGHT)
    d.line([4, 8, 11, 8], fill=INK)
    d.rectangle([5, 11, 10, 14], outline=INK, fill=WHITE)
    d.line([6, 12, 9, 12], fill=MID)
    d.rectangle([11, 9, 12, 10], fill=GREEN)


def g_printer_setup_dialog(d, a):
    # Small printer + orange gear badge (setup).
    d.rectangle([4, 2, 9, 6], outline=INK, fill=WHITE)
    d.rectangle([2, 6, 11, 11], outline=INK, fill=LIGHT)
    d.line([4, 8, 9, 8], fill=INK)
    d.ellipse([9, 9, 14, 14], fill=ORANGE)
    d.line([9, 11, 14, 11], fill=WHITE)
    d.line([11, 9, 11, 14], fill=WHITE)
    d.ellipse([11, 11, 12, 12], fill=WHITE)


def g_save_dialog(d, a):
    # Blue floppy disk: white shutter + white label.
    d.rectangle([3, 2, 12, 13], outline=INK, fill=a)
    d.rectangle([5, 2, 10, 6], outline=INK, fill=WHITE)
    d.rectangle([4, 8, 11, 12], outline=INK, fill=WHITE)
    d.line([5, 10, 10, 10], fill=MID)


# --------------------------------------------------------- find/picture
def g_find_dialog(d, a):
    # Dialog + magnifier (accent handle).
    _dialog(d, a)
    d.ellipse([4, 6, 10, 12], outline=INK, width=2)
    d.line([9, 11, 12, 13], fill=a, width=2)
    d.line([6, 8, 8, 8], fill=MID)


def g_replace_dialog(d, a):
    # Dialog + swap arrows: green forward, orange back.
    _dialog(d, a)
    d.line([2, 7, 6, 7], fill=MID)
    d.line([7, 7, 11, 7], fill=GREEN, width=2)
    d.polygon([(11, 5), (11, 9), (13, 7)], fill=GREEN)
    d.line([9, 11, 13, 11], fill=MID)
    d.line([8, 11, 4, 11], fill=ORANGE, width=2)
    d.polygon([(4, 9), (4, 13), (2, 11)], fill=ORANGE)


def g_open_picture_dialog(d, a):
    # Dialog + picture: yellow sun over ink mountains.
    _dialog(d, a)
    d.ellipse([4, 6, 6, 8], fill=YELLOW)
    d.line([3, 13, 6, 9, 9, 12, 13, 8], fill=INK, width=1)


def g_save_picture_dialog(d, a):
    # Picture + green down-arrow badge (save).
    d.rectangle([2, 2, 13, 13], outline=INK, fill=WHITE)
    d.ellipse([4, 4, 6, 6], fill=YELLOW)
    d.line([2, 12, 6, 8, 9, 11, 13, 7], fill=INK, width=1)
    d.ellipse([10, 10, 14, 14], fill=GREEN)
    d.line([12, 11, 12, 13], fill=WHITE, width=2)
    d.polygon([(10, 12), (14, 12), (12, 14)], fill=WHITE)


# ---------------------------------------------------------------- containers
def g_box(d, a):
    # Three vertical cells, middle cell light, accent dividers.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([6, 2, 9, 13], fill=LIGHT)
    d.line([5, 1, 5, 14], fill=a)
    d.line([10, 1, 10, 14], fill=a)


def g_button_box(d, a):
    # Three buttons in a row, middle one accent-filled.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rounded_rectangle([2, 5, 5, 10], radius=1, outline=INK, fill=LIGHT)
    d.rounded_rectangle([6, 5, 9, 10], radius=1, outline=INK, fill=a)
    d.rounded_rectangle([10, 5, 13, 10], radius=1, outline=INK, fill=LIGHT)


def g_center_box(d, a):
    # Three cells, centre cell solid accent with a white dot.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([6, 2, 9, 13], fill=a)
    d.rectangle([7, 7, 8, 8], fill=WHITE)
    d.line([5, 1, 5, 14], fill=INK)
    d.line([10, 1, 10, 14], fill=INK)


def g_fixed(d, a):
    # Fixed-position layout: guide lines + red / green placed squares.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([1, 4, 14, 4], fill=LIGHT)
    d.line([1, 11, 14, 11], fill=LIGHT)
    d.rectangle([3, 3, 5, 5], outline=INK, fill=RED)
    d.rectangle([10, 10, 12, 12], outline=INK, fill=GREEN)


def g_flow_box(d, a):
    # Wrapped rows of cells (3 + 2 + 1), accent / light / green.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 5, 5], fill=a)
    d.rectangle([6, 2, 9, 5], outline=INK, fill=LIGHT)
    d.rectangle([10, 2, 13, 5], fill=a)
    d.rectangle([2, 7, 5, 10], outline=INK, fill=LIGHT)
    d.rectangle([6, 7, 9, 10], fill=GREEN)
    d.rectangle([2, 12, 5, 13], fill=a)


def g_layout(d, a):
    # Canvas with two placed widgets + right and bottom scrollbars.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 11, 11], outline=a, fill=WHITE)
    d.rectangle([4, 4, 6, 6], fill=RED)
    d.rectangle([8, 8, 10, 10], fill=GREEN)
    d.rectangle([12, 2, 14, 11], outline=INK, fill=WHITE)
    d.rectangle([12, 4, 14, 7], fill=a)
    d.rectangle([2, 12, 11, 14], outline=INK, fill=WHITE)
    d.rectangle([4, 12, 7, 14], fill=a)


def g_scrolled_window(d, a):
    # Text content + right and bottom scrollbars + corner square.
    d.rectangle([1, 1, 11, 11], outline=INK, fill=WHITE)
    d.line([3, 4, 9, 4], fill=MID)
    d.line([3, 7, 9, 7], fill=MID)
    d.rectangle([12, 1, 14, 11], outline=INK, fill=WHITE)
    d.rectangle([12, 4, 14, 8], fill=a)
    d.rectangle([1, 12, 11, 14], outline=INK, fill=WHITE)
    d.rectangle([3, 12, 7, 14], fill=a)
    d.rectangle([12, 12, 14, 14], outline=INK, fill=LIGHT)


def g_viewport(d, a):
    # View into larger content: red square peeking past the view rect.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 5, 5], fill=RED)
    d.rectangle([4, 4, 13, 13], outline=a, fill=WHITE)
    d.line([6, 7, 11, 7], fill=MID)
    d.line([6, 10, 9, 10], fill=MID)


def g_flow_panel(d, a):
    # Columns of cells flowing downward (3 + 2 + 2).
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 5, 5], fill=a)
    d.rectangle([2, 6, 5, 9], outline=INK, fill=LIGHT)
    d.rectangle([2, 10, 5, 13], fill=GREEN)
    d.rectangle([6, 2, 9, 5], outline=INK, fill=LIGHT)
    d.rectangle([6, 6, 9, 9], fill=a)
    d.rectangle([10, 2, 13, 5], fill=GREEN)
    d.rectangle([10, 6, 13, 9], outline=INK, fill=LIGHT)


def g_panel(d, a):
    # Raised bevel panel: white top/left highlight, grey shadow, caption.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=LIGHT)
    d.line([2, 2, 13, 2], fill=WHITE)
    d.line([2, 2, 2, 13], fill=WHITE)
    d.line([13, 2, 13, 13], fill=MID)
    d.line([2, 13, 13, 13], fill=MID)
    d.line([4, 5, 11, 5], fill=INK, width=2)
    d.line([4, 8, 9, 8], fill=MID)
    d.line([4, 11, 11, 11], fill=MID)


def g_scroll_box(d, a):
    # Content lines + right scrollbar with arrowheads and accent thumb.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 4, 8, 4], fill=MID)
    d.line([3, 7, 10, 7], fill=MID)
    d.line([3, 10, 8, 10], fill=MID)
    d.rectangle([11, 1, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(12, 2), (14, 4), (11, 4)], fill=a)
    d.rectangle([11, 5, 14, 9], fill=a)
    d.polygon([(12, 13), (14, 11), (11, 11)], fill=a)


# ---------------------------------------------------------------- tabs
def g_notebook(d, a):
    # Two top tabs, left one active (accent), red dot on the other.
    d.rectangle([2, 1, 7, 4], outline=INK, fill=a)
    d.rectangle([8, 1, 13, 4], outline=INK, fill=LIGHT)
    d.rectangle([11, 2, 12, 3], fill=RED)
    d.rectangle([1, 4, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID)
    d.line([3, 11, 8, 11], fill=MID)


def g_stack_sidebar(d, a):
    # Left sidebar (accent item + grey items) + content area.
    d.rectangle([1, 1, 5, 14], outline=INK, fill=LIGHT)
    d.line([2, 3, 4, 3], fill=a, width=2)
    d.line([2, 6, 4, 6], fill=MID)
    d.line([2, 9, 4, 9], fill=MID)
    d.line([2, 12, 4, 12], fill=MID)
    d.rectangle([6, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([8, 5, 12, 5], fill=MID)
    d.line([8, 8, 12, 8], fill=MID)
    d.line([8, 11, 10, 11], fill=MID)


def g_stack_switcher(d, a):
    # Segmented control (middle segment active) over content.
    d.rectangle([1, 2, 5, 5], outline=INK, fill=LIGHT)
    d.rectangle([6, 2, 10, 5], outline=INK, fill=a)
    d.rectangle([11, 2, 14, 5], outline=INK, fill=LIGHT)
    d.rectangle([1, 6, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 9, 12, 9], fill=MID)
    d.line([3, 12, 9, 12], fill=MID)


def g_tnotebook(d, a):
    # Three top tabs, middle one active (accent, white dot).
    d.rectangle([1, 1, 5, 4], outline=INK, fill=LIGHT)
    d.rectangle([6, 1, 10, 4], outline=INK, fill=a)
    d.rectangle([11, 1, 14, 4], outline=INK, fill=LIGHT)
    d.rectangle([1, 4, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([7, 2, 8, 3], fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID)
    d.line([3, 11, 8, 11], fill=MID)


def g_page_control(d, a):
    # Tabs with a red close X on the active tab.
    d.rectangle([2, 1, 8, 4], outline=INK, fill=a)
    d.line([6, 2, 7, 3], fill=RED)
    d.line([7, 2, 6, 3], fill=RED)
    d.rectangle([9, 1, 13, 4], outline=INK, fill=LIGHT)
    d.rectangle([1, 4, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 8, 11, 8], fill=MID)
    d.line([3, 11, 8, 11], fill=MID)


def g_tab_control(d, a):
    # Tabs along the BOTTOM of the content area.
    d.rectangle([1, 1, 14, 11], outline=INK, fill=WHITE)
    d.line([3, 4, 11, 4], fill=MID)
    d.line([3, 7, 8, 7], fill=MID)
    d.rectangle([2, 11, 7, 14], outline=INK, fill=a)
    d.rectangle([8, 11, 13, 14], outline=INK, fill=LIGHT)


# ---------------------------------------------------------------- frames
def g_aspect_frame(d, a):
    # Frame with accent label chip + fixed-ratio inner rect + width arrows.
    d.rectangle([1, 2, 14, 14], outline=INK)
    d.line([5, 2, 9, 2], fill=WHITE)
    d.rectangle([5, 1, 8, 2], fill=a)
    d.rectangle([3, 5, 12, 11], outline=a, fill=WHITE)
    d.line([4, 13, 11, 13], fill=GREEN)
    d.polygon([(4, 13), (6, 12), (6, 14)], fill=GREEN)
    d.polygon([(11, 13), (9, 12), (9, 14)], fill=GREEN)


def g_frame(d, a):
    # Classic frame: top edge gap + accent label chip + text lines.
    d.rectangle([1, 3, 14, 14], outline=INK)
    d.line([5, 3, 9, 3], fill=WHITE)
    d.rectangle([5, 2, 8, 2], fill=a)
    d.line([4, 7, 11, 7], fill=MID)
    d.line([4, 10, 8, 10], fill=MID)


def g_tframe(d, a):
    # Thick double border + accent inner panel.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.rectangle([2, 2, 13, 13], outline=INK)
    d.rectangle([4, 4, 11, 11], outline=a, fill=LIGHT)
    d.line([6, 7, 9, 7], fill=MID)


# ---------------------------------------------------------------- pairs
def g_expander(d, a):
    # Collapsed expander: right-pointing accent triangle + label lines.
    d.polygon([(3, 4), (3, 11), (9, 7)], outline=INK, fill=a)
    d.line([11, 6, 13, 6], fill=INK)
    d.line([11, 9, 13, 9], fill=MID)


def g_revealer(d, a):
    # Expanded revealer: down-pointing triangle + revealed box.
    d.polygon([(3, 2), (11, 2), (7, 7)], outline=INK, fill=a)
    d.rectangle([2, 8, 13, 14], outline=INK, fill=LIGHT)
    d.line([4, 11, 11, 11], fill=MID)


def g_gtk_scrollbar(d, a):
    # Vertical scrollbar: arrowheads + accent thumb, content ticks left.
    d.rectangle([1, 1, 7, 14], outline=INK, fill=WHITE)
    d.line([3, 4, 5, 4], fill=MID)
    d.line([3, 7, 5, 7], fill=MID)
    d.line([3, 10, 5, 10], fill=MID)
    d.rectangle([9, 1, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(10, 4), (14, 4), (12, 2)], fill=INK)
    d.rectangle([10, 6, 13, 10], fill=a)
    d.polygon([(10, 11), (14, 11), (12, 13)], fill=INK)


def g_tscrollbar(d, a):
    # Horizontal scrollbar: arrowheads + accent thumb, content ticks above.
    d.rectangle([1, 1, 14, 7], outline=INK, fill=WHITE)
    d.line([4, 3, 4, 5], fill=MID)
    d.line([7, 3, 7, 5], fill=MID)
    d.line([10, 3, 10, 5], fill=MID)
    d.rectangle([1, 9, 14, 14], outline=INK, fill=WHITE)
    d.polygon([(4, 10), (4, 13), (2, 11)], fill=INK)
    d.rectangle([6, 10, 10, 13], fill=a)
    d.polygon([(11, 10), (11, 13), (13, 11)], fill=INK)


def g_gtk_statusbar(d, a):
    # Statusbar: text lines + bottom strip, green dot, grip diagonal.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 4, 11, 4], fill=MID)
    d.line([3, 7, 8, 7], fill=MID)
    d.rectangle([1, 11, 14, 14], fill=LIGHT, outline=INK)
    d.ellipse([3, 12, 5, 13], fill=GREEN)
    d.line([7, 12, 11, 12], fill=a)
    d.line([12, 13, 13, 12], fill=a)


def g_tstatusbar(d, a):
    # Statusbar with TWO panels: red square left, accent line right.
    d.rectangle([1, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 4, 11, 4], fill=MID)
    d.line([3, 7, 8, 7], fill=MID)
    d.rectangle([1, 11, 7, 14], fill=LIGHT, outline=INK)
    d.rectangle([8, 11, 14, 14], fill=LIGHT, outline=INK)
    d.rectangle([3, 12, 4, 13], fill=RED)
    d.line([10, 12, 12, 12], fill=a)


def g_pair_splitter(d, a):
    # Vertical split: two panes + accent splitter bar with white grip dots.
    d.rectangle([1, 1, 6, 14], outline=INK, fill=WHITE)
    d.rectangle([9, 1, 14, 14], outline=INK, fill=WHITE)
    d.line([2, 5, 5, 5], fill=MID)
    d.line([10, 5, 13, 5], fill=MID)
    d.rectangle([7, 1, 8, 14], fill=a)
    d.point((7, 6), fill=WHITE)
    d.point((7, 8), fill=WHITE)
    d.point((7, 10), fill=WHITE)


def g_splitter(d, a):
    # Horizontal split: two panes + accent splitter bar with white grip dots.
    d.rectangle([1, 1, 14, 6], outline=INK, fill=WHITE)
    d.rectangle([1, 9, 14, 14], outline=INK, fill=WHITE)
    d.line([3, 3, 12, 3], fill=MID)
    d.line([3, 11, 9, 11], fill=MID)
    d.rectangle([1, 7, 14, 8], fill=a)
    d.point((6, 7), fill=WHITE)
    d.point((8, 7), fill=WHITE)
    d.point((10, 7), fill=WHITE)


# ---------------------------------------------------------------- registry
DRAW = {
    "GtkApplicationWindow": g_application_window,
    "GtkShortcutsWindow": g_shortcuts_window,
    "GtkWindow": g_gtk_window,
    "TForm": g_form,
    "GtkAboutDialog": g_about_dialog,
    "GtkAppChooserDialog": g_app_chooser_dialog,
    "GtkAssistant": g_assistant,
    "GtkColorChooserDialog": g_color_chooser_dialog,
    "GtkDialog": g_gtk_dialog,
    "GtkFileChooserDialog": g_file_chooser_dialog,
    "GtkFontChooserDialog": g_font_chooser_dialog,
    "GtkMessageDialog": g_message_dialog,
    "GtkPrintUnixDialog": g_print_unix_dialog,
    "TTaskDialog": g_task_dialog,
    "TOpenDialog": g_open_dialog,
    "TPageSetupDialog": g_page_setup_dialog,
    "TPrintDialog": g_print_dialog,
    "TPrinterSetupDialog": g_printer_setup_dialog,
    "TSaveDialog": g_save_dialog,
    "TFindDialog": g_find_dialog,
    "TReplaceDialog": g_replace_dialog,
    "TOpenPictureDialog": g_open_picture_dialog,
    "TSavePictureDialog": g_save_picture_dialog,
    "GtkBox": g_box,
    "GtkButtonBox": g_button_box,
    "GtkCenterBox": g_center_box,
    "GtkFixed": g_fixed,
    "GtkFlowBox": g_flow_box,
    "GtkLayout": g_layout,
    "GtkScrolledWindow": g_scrolled_window,
    "GtkViewport": g_viewport,
    "TFlowPanel": g_flow_panel,
    "TPanel": g_panel,
    "TScrollBox": g_scroll_box,
    "GtkNotebook": g_notebook,
    "GtkStackSidebar": g_stack_sidebar,
    "GtkStackSwitcher": g_stack_switcher,
    "TNotebook": g_tnotebook,
    "TPageControl": g_page_control,
    "TTabControl": g_tab_control,
    "GtkAspectFrame": g_aspect_frame,
    "GtkFrame": g_frame,
    "TFrame": g_tframe,
    "GtkExpander": g_expander,
    "GtkRevealer": g_revealer,
    "GtkScrollbar": g_gtk_scrollbar,
    "TScrollBar": g_tscrollbar,
    "GtkStatusbar": g_gtk_statusbar,
    "TStatusBar": g_tstatusbar,
    "TPairSplitter": g_pair_splitter,
    "TSplitter": g_splitter,
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
        # BMP-format ICO (32bpp, uncompressed), exactly like gen_icons.py.
        img.save(path, format="ICO", sizes=[(16, 16)], bitmap_format="bmp")
    print(f"icons written: {len(DRAW)} in {outdir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
