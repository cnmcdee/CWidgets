import os, glob
from PIL import Image, ImageDraw, ImageFont

_HERE = os.path.dirname(os.path.abspath(__file__))
SHOT_DIR = os.path.join(_HERE, "screenshots")
OUT = os.path.join(SHOT_DIR, "all_widgets.png")
TILE_W, TILE_H = 240, 180
LABEL_H = 26
CELL_W, CELL_H = TILE_W, TILE_W*0 + TILE_H + LABEL_H  # 240x206
COLS = 14
GAP = 4
MARGIN = 8

files = sorted(glob.glob(os.path.join(SHOT_DIR, "*.png")))
files = [f for f in files if os.path.basename(f) != "all_widgets.png"]
assert len(files) == 194, f"expected 194 shots, got {len(files)}"
ROWS = (len(files) + COLS - 1) // COLS
W = MARGIN*2 + COLS*CELL_W + (COLS-1)*GAP
H = MARGIN*2 + ROWS*CELL_H + (ROWS-1)*GAP
print(f"grid {COLS}x{ROWS} = {COLS*ROWS} slots, {len(files)} tiles, sheet {W}x{H}")

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
