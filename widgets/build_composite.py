import os, sys
from PIL import Image, ImageDraw, ImageFont

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(_HERE, "tools"))
from widget_count import (composite_cols, composite_rows, composite_tiles,
                          widget_count)

SHOT_DIR = os.path.join(_HERE, "screenshots")
OUT = os.path.join(SHOT_DIR, "all_widgets.png")
TILE_W, TILE_H = 240, 180
LABEL_H = 26
CELL_W, CELL_H = TILE_W, TILE_H + LABEL_H
GAP = 4
MARGIN = 8

# Tiles are the widget screenshots on disk (tools/widget_count.py); the
# grid sizes itself from that count -- near-square, spare slots in the
# last row stay blank. The count is never hard-coded: it follows the
# screenshots, which follow the registry as widgets land.
files = composite_tiles(SHOT_DIR)
n = len(files)
COLS = composite_cols(n)
ROWS = composite_rows(n, COLS)
reg_n = widget_count()
if n < reg_n:
    raise SystemExit(
        f"only {n} widget screenshots, registry has {reg_n} widgets — "
        "render the missing screenshots first (test-driver/run_tests.py)")
if n > reg_n:
    print(f"note: {n} tiles but registry has {reg_n} widgets — "
          "screenshots are ahead of the registry (mid-landing)")
W = MARGIN*2 + COLS*CELL_W + (COLS-1)*GAP
H = MARGIN*2 + ROWS*CELL_H + (ROWS-1)*GAP
print(f"grid {COLS}x{ROWS} = {COLS*ROWS} slots, {n} tiles, sheet {W}x{H}")

font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 15)
sheet = Image.new("RGB", (W, H), (245, 245, 245))
draw = ImageDraw.Draw(sheet)
for i, path in enumerate(files):
    name = os.path.splitext(os.path.basename(path))[0]
    r, c = divmod(i, COLS)
    x = MARGIN + c*(CELL_W+GAP)
    y = MARGIN + r*(CELL_H+GAP)
    img = Image.open(path).convert("RGB").resize((TILE_W, TILE_H), Image.LANCZOS)
    sheet.paste(img, (x, y))
    draw.rectangle([x, y+TILE_H, x+TILE_W-1, y+TILE_H+LABEL_H-1], fill=(30, 30, 30))
    tw = draw.textlength(name, font=font)
    draw.text((x + (TILE_W-tw)/2, y+TILE_H+4), name, font=font, fill=(255, 255, 255))
sheet.save(OUT, optimize=True)
print("saved", OUT, os.path.getsize(OUT), "bytes")
