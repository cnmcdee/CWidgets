import os, glob
from PIL import Image
from collections import Counter

SHOT_DIR = "/opt/cwidgets/screenshots"
SHEET = os.path.join(SHOT_DIR, "all_widgets.png")
TILE_W, TILE_H, LABEL_H, COLS, GAP, MARGIN = 240, 180, 26, 14, 4, 8
CELL_W, CELL_H = TILE_W, TILE_H + LABEL_H

files = sorted(f for f in glob.glob(os.path.join(SHOT_DIR, "*.png"))
               if os.path.basename(f) != "all_widgets.png")
sheet = Image.open(SHEET).convert("RGB")
print("sheet size:", sheet.size, "tiles expected:", len(files))
assert len(files) == 194

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
    # label bar: strip below tile must be dark fill (30,30,30)
    bar = sheet.crop((x, y+TILE_H, x+TILE_W, y+TILE_H+LABEL_H))
    bar_counts = Counter(bar.getdata())
    bar_dom, bar_n = bar_counts.most_common(1)[0]
    if bar_dom != (30, 30, 30) or bar_n < TILE_W*LABEL_H*0.9:
        bad_label.append((name, bar_dom, bar_n))

print("blank tiles (<=20 differing px):", blank)
print("fidelity mismatches:", mismatched[:10], "count:", len(mismatched))
print("bad label bars:", bad_label)
print(f"RESULT: {194-len(blank)} non-blank, {len(blank)} blank, {len(bad_label)} bad labels, {len(mismatched)} mismatched")
