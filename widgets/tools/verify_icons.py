#!/usr/bin/env python3
"""Verify every icon in icons/<Name>.ico: parses the ICO container +
embedded BMP, checks 16x16 / 32bpp, non-blank (real drawn pixels, not a
solid fill), and reports how many distinct glyphs exist.

Usage: python3 tools/verify_icons.py
Exit 0 only if every registry widget's icon exists and passes.
"""
import os, sys, struct, hashlib
from collections import Counter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ICONS = os.path.join(ROOT, "icons")


def parse_ico(path):
    """Return (width, height, bpp, pixel-bytes) for a BMP-format ICO."""
    with open(path, "rb") as f:
        data = f.read()
    if len(data) < 6 or data[0:4] != b"\x00\x00\x01\x00":
        raise ValueError("not an ICO (bad magic)")
    count = struct.unpack_from("<H", data, 4)[0]
    if count != 1:
        raise ValueError(f"expected 1 image, got {count}")
    w, h, _, _, _, bpp, size, off = struct.unpack_from("<BBBBHHII", data, 6)
    w = w or 256
    h = h or 256
    if (w, h) != (16, 16):
        raise ValueError(f"size {w}x{h}, want 16x16")
    if bpp not in (24, 32):
        raise ValueError(f"bpp {bpp}, want 24/32")
    img = data[off:off + size]
    if len(img) < 40:
        raise ValueError("image data too short")
    hdr, bw, bh = struct.unpack_from("<Iii", img, 0)
    if hdr != 40 or bw != 16 or bh != 32:  # height counts XOR+AND masks
        raise ValueError(f"bad BMP header {hdr} {bw}x{bh}")
    return w, h, bpp, img[40:40 + 16 * 16 * (bpp // 8)]


def main():
    # The registry is the integrated set (tools/widget_count.py) — icons
    # are checked for exactly the widgets the palette can show.
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from widget_count import registry_names
    comps = registry_names()
    fails, hashes = [], Counter()
    blank = []
    for name in comps:
        path = os.path.join(ICONS, name + ".ico")
        if not os.path.exists(path):
            fails.append((name, "missing file"))
            continue
        try:
            w, h, bpp, px = parse_ico(path)
        except Exception as e:
            fails.append((name, str(e)))
            continue
        # Non-blank: more than one distinct colour, and not all-white.
        colours = Counter(px[i:i + 3] for i in range(0, len(px), bpp // 8))
        if len(colours) < 3:
            blank.append(name)
            fails.append((name, f"only {len(colours)} distinct colours"))
        hashes[hashlib.md5(px).hexdigest()] += 1
    print(f"icons checked: {len(comps)}")
    print(f"distinct pixel hashes: {len(hashes)} "
          f"(shared glyphs, e.g. GtkButton/TButton, are expected)")
    print(f"most-shared glyph hash: {hashes.most_common(1)[0][1]} icons")
    if fails:
        print(f"FAIL: {len(fails)}")
        for n, why in fails:
            print(f"  {n}: {why}")
        return 1
    print(f"icons OK: {len(comps)}/{len(comps)} parse as 16x16 BMP ICOs, all non-blank")
    return 0


if __name__ == "__main__":
    sys.exit(main())
