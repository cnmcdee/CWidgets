#!/usr/bin/env python3
"""Render one example picture per widget (all 481) on the VPS — v2.

Method (why not run_tests.py's import -window): the 250 indicator demos
have no event loop — they draw once, print OK and exit, so their window
lives ~100 ms, and ImageMagick `import -window <id>` hangs on the dying
window. Instead every demo is run with LD_PRELOAD=/tmp/keepalive.so, a
harness-only shim that sleeps 10 s inside XCloseDisplay, keeping the drawn
window mapped. We then capture the whole root with `xwd -root` (always
exists, never a dead-window race) and crop the demo window, which every
main creates at 100,100 800x600. The shot must be non-blank (>20 non-bg
pixels). Existing non-blank shots in widgets/screenshots/ are kept (audit
first, render only the gap); every shot is also copied to
screenshots/widgets/<Name>.png (the per-widget picture location linked
from the docs).

Writes widgets/test-results/widget_pics.csv (name, source, result, note)
and prints a summary. Exit code 0 only if every widget has a non-blank
picture in both locations.

Usage: DISPLAY=:91 python3 widgets/tools/render_widget_pics.py [--only A,B]
       [--force]   # re-render even existing shots
"""
import csv, os, shutil, subprocess, sys, time
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = os.path.dirname(ROOT)
WIDGETS = os.path.join(REPO, "widgets")
BUILD = os.path.join(WIDGETS, "build-pics", "components")
SHOTS = os.path.join(WIDGETS, "screenshots")
DEST = os.path.join(REPO, "screenshots", "widgets")
TMP = "/tmp/cw-pics"
OUT = os.environ.get("WIDGET_PICS_OUT",
                     os.path.join(WIDGETS, "test-results", "widget_pics.csv"))
KEEPALIVE = "/tmp/keepalive.so"
# Every main creates its window at 100,100 800x600 (verified across all 481).
WX, WY, WW, WH = 100, 100, 800, 600
ENV = dict(os.environ, DISPLAY=os.environ.get("DISPLAY", ":91"))
if os.path.exists(KEEPALIVE):
    ENV["LD_PRELOAD"] = KEEPALIVE
os.makedirs(SHOTS, exist_ok=True)
os.makedirs(DEST, exist_ok=True)
os.makedirs(TMP, exist_ok=True)
os.makedirs(os.path.dirname(OUT), exist_ok=True)

sys.path.insert(0, os.path.join(WIDGETS, "tools"))
from widget_count import registry_names

def run(*a, timeout=20):
    return subprocess.run(*a, capture_output=True, text=True, timeout=timeout,
                          env=ENV)

def nonbg_count(im):
    h = im.convert("L").histogram()
    modal = max(range(256), key=lambda v: h[v])
    return sum(c for v, c in enumerate(h) if abs(v - modal) > 10)

def is_good(path):
    try:
        return os.path.getsize(path) > 0 and nonbg_count(Image.open(path)) > 20
    except Exception:
        return False

def render(name):
    """Launch the demo (shim-held), xwd the root, crop the window."""
    binp = os.path.join(BUILD, name, name)
    if not (os.path.exists(binp) and os.access(binp, os.X_OK)):
        return False, "binary missing (not built)"
    logf = open(os.path.join(TMP, name + ".stderr"), "wb")
    try:
        p = subprocess.Popen([binp], env=ENV, stdout=subprocess.DEVNULL,
                             stderr=logf)
    except Exception as e:
        logf.close()
        return False, "launch failed: %s" % e
    try:
        # Draws are synchronous in main; 1.5 s is ample even under load,
        # and the shim holds the window 10 s past XCloseDisplay.
        time.sleep(1.5)
        if p.poll() is not None and name.startswith(("Ind",)):
            # Exit-immediately demo already gone and shim did not hold it?
            # (Should not happen; the shim sleeps in XCloseDisplay.)
            pass
        xwd = os.path.join(TMP, name + ".xwd")
        png = os.path.join(TMP, name + "_root.png")
        try:
            run(["xwd", "-root", "-out", xwd], timeout=20)
            run(["convert", xwd, png], timeout=20)
        except subprocess.TimeoutExpired:
            return False, "root capture timed out"
        if not (os.path.exists(png) and os.path.getsize(png) > 0):
            return False, "root capture failed"
        try:
            im = Image.open(png).convert("RGB")
            crop = im.crop((WX, WY, WX + WW, WY + WH))
        except Exception as e:
            return False, "crop failed: %s" % e
        nb = nonbg_count(crop)
        if nb <= 20:
            return False, "blank draw (%d non-bg px)" % nb
        dest = os.path.join(SHOTS, name + ".png")
        crop.save(dest)
        return True, "rendered (%d non-bg px)" % nb
    finally:
        try:
            p.kill()
        except Exception:
            pass
        try:
            p.wait(timeout=3)
        except Exception:
            pass
        logf.close()

def main():
    only = None
    force = "--force" in sys.argv
    for a in sys.argv[1:]:
        if a.startswith("--only"):
            only = set(a.split("=", 1)[1].split(","))
    names = registry_names()
    print("registry widgets: %d" % len(names), flush=True)
    rows = []
    t0 = time.time()
    for idx, name in enumerate(names, 1):
        if only and name not in only:
            continue
        shot = os.path.join(SHOTS, name + ".png")
        dest = os.path.join(DEST, name + ".png")
        if not force and is_good(shot):
            source, ok, note = "existing", True, "kept existing shot"
        else:
            source = "rendered"
            try:
                ok, note = render(name)
            except Exception as e:
                ok, note = False, "render crashed: %s" % e
        if ok:
            try:
                shutil.copyfile(shot, dest)
            except Exception as e:
                ok, note = False, "copy to screenshots/widgets failed: %s" % e
        rows.append((name, source, "PASS" if ok else "FAIL", note))
        if idx % 50 == 0:
            print("  %d/%d  %.0fs" % (idx, len(names), time.time() - t0),
                  flush=True)
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["name", "source", "result", "note"])
        w.writerows(rows)
    npass = sum(1 for r in rows if r[2] == "PASS")
    nfail = [r for r in rows if r[2] == "FAIL"]
    print("pictures: %d checked  PASS: %d  FAIL: %d  (%.0fs)"
          % (len(rows), npass, len(nfail), time.time() - t0))
    for r in nfail:
        print("FAIL %s [%s] %s" % (r[0], r[1], r[3]))
    print("wrote", OUT)
    return 0 if not nfail else 1

if __name__ == "__main__":
    sys.exit(main())
