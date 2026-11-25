import os, sys
from PIL import Image
from collections import Counter

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(_HERE, "tools"))
from widget_count import (composite_cols, composite_rows, composite_tiles,
                          widget_count)

SHOT_DIR = os.path.join(_HERE, "screenshots")
SHEET = os.path.join(SHOT_DIR, "all_widgets.png")
TILE_W, TILE_H, LABEL_H, COLS_UNUSED, GAP, MARGIN = 240, 180, 26, 14, 4, 8
CELL_W, CELL_H = TILE_W, TILE_H + LABEL_H

# Same tile set and grid derivation as build_composite.py (never
# hard-coded): cols = ceil(sqrt(n)) over the widget screenshots on disk.
files = composite_tiles(SHOT_DIR)
n = len(files)
COLS = composite_cols(n)
ROWS = composite_rows(n, COLS)
reg_n = widget_count()
sheet = Image.open(SHEET).convert("RGB")
print("sheet size:", sheet.size, "tiles:", n, "registry:", reg_n)
exp_w = MARGIN*2 + COLS*CELL_W + (COLS-1)*GAP
exp_h = MARGIN*2 + ROWS*CELL_H + (ROWS-1)*GAP
if sheet.size != (exp_w, exp_h):
    raise SystemExit(
        f"FAIL: sheet size {sheet.size}, expected {(exp_w, exp_h)} "
        f"for a {COLS}x{ROWS} grid over {n} tiles")
if n < reg_n:
    raise SystemExit(
        f"FAIL: only {n} tiles, registry has {reg_n} widgets — "
        "render the missing screenshots first")
if n > reg_n:
    print(f"note: {n} tiles but registry has {reg_n} widgets — "
          "screenshots are ahead of the registry (mid-landing)")

blank, mismatched, bad_label = [], [], []
for i, path in enumerate(files):
    name = os.path.splitext(os.path.basename(path))[0]
    r, c = divmod(i, COLS)
    x = MARGIN + c*(CELL_W+GAP); y = MARGIN + r*(CELL_H+GAP)
    tile = sheet.crop((x, y, x+TILE_W, y+TILE_H))
    # fidelity: tile must equal downscaled source
    src = Image.open(path).convert("RGB").resize((TILE_W, TILE_H), Image.LANCZOS)
    diff = sum(1 for a, b in zip(tile.getdata(), src.getdata()) if a != b)
    if diff > 0:
        mismatched.append((name, diff))
    # blank check: pixels differing from tile's dominant color
    counts = Counter(tile.getdata())
    dom, dom_n = counts.most_common(1)[0]
    differing = TILE_W*TILE_H - dom_n
    if differing <= 20:
        blank.append((name, differing, dom))
    # label bar: strip below tile must be dark fill (30,30,30). The
    # white label text sits on the bar, so it is only ~75-90% dark, not
    # 100%: majority-dark is the correct bar-present signal.
    bar = sheet.crop((x, y+TILE_H, x+TILE_W, y+TILE_H+LABEL_H))
    bar_counts = Counter(bar.getdata())
    bar_dom, bar_n = bar_counts.most_common(1)[0]
    if bar_dom != (30, 30, 30) or bar_n < TILE_W*LABEL_H*0.7:
        bad_label.append((name, bar_dom, bar_n))

print("blank tiles (<=20 differing px):", blank)
print("fidelity mismatches:", mismatched[:10], "count:", len(mismatched))
print("bad label bars:", bad_label)
# GtkInvisible draws nothing by design, so its tile is blank on
# purpose; any other blank tile is a real failure.
BLANK_OK = {"GtkInvisible"}
real_blank = [b for b in blank if b[0] not in BLANK_OK]
print(f"RESULT: {n-len(blank)} non-blank, {len(blank)} blank, {len(bad_label)} bad labels, {len(mismatched)} mismatched")
if real_blank or mismatched or bad_label:
    raise SystemExit(1)
