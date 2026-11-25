#!/usr/bin/env python3
"""Stock Market drawing widgets test driver (14 widgets).
Builds each demo with -Wall -Wextra (0 warnings), runs it under Xvfb :93,
screenshots, exercises hover/click/key/clean-exit, re-derives every count
from fresh artifacts. Exits non-zero on any FAIL. Usage: python3 test_stock_drawing.py"""
import subprocess, os, sys, time, csv
from PIL import Image, ImageChops

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "build", "components")
SHOTS = os.path.join(ROOT, "screenshots")
TMP = "/tmp/stock-draw-shots"
RESULTS = os.path.join(ROOT, "test-results", "stock_drawing.csv")
ENV = dict(os.environ, DISPLAY=":93")
NAMES = ["StockTrendline","StockHorizontalLine","StockVerticalLine","StockRectangle",
"StockEllipse","StockFibRetracement","StockFibExtension","StockTextAnnotation",
"StockArrow","StockParallelChannel","StockPitchfork","StockBrush","StockPriceLabel",
"StockMeasureTool"]
os.makedirs(SHOTS, exist_ok=True); os.makedirs(TMP, exist_ok=True)
os.makedirs(os.path.dirname(RESULTS), exist_ok=True)

def run(*a, timeout=60, **k):
    return subprocess.run(*a, capture_output=True, text=True, timeout=timeout, env=ENV, **k)

def nonbg(path):
    im = Image.open(path).convert("L"); h = im.histogram()
    modal = max(range(256), key=lambda v: h[v])
    return sum(c for v, c in enumerate(h) if abs(v - modal) > 10)

def diff(a, b):
    ia = Image.open(a).convert("RGB"); ib = Image.open(b).convert("RGB")
    if ia.size != ib.size: ib = ib.resize(ia.size)
    return sum(ImageChops.difference(ia, ib).convert("L").histogram()[31:])

def shot(wid, path):
    r = run(["import", "-window", str(wid), path], timeout=15)
    return os.path.exists(path) and os.path.getsize(path) > 0

def geom(wid):
    r = run(["xdotool", "getwindowgeometry", "--shell", str(wid)], timeout=10)
    g = {}
    for line in r.stdout.splitlines():
        if "=" in line:
            k, v = line.split("=", 1)
            try: g[k] = int(v)
            except ValueError: pass
    return g.get("X", 100), g.get("Y", 100), g.get("WIDTH", 800), g.get("HEIGHT", 600)

rows = []
fails = 0
for name in NAMES:
    row = {"name": name, "build": "FAIL", "warnings": "", "draw": "FAIL", "hover": "FAIL",
           "click": "FAIL", "key": "FAIL", "exit": "FAIL", "notes": ""}
    notes = []
    src = os.path.join(ROOT, "components", name)
    bdir = os.path.join(BUILD, name)
    try:
        c = run(["cmake", "-S", src, "-B", bdir, "-DCMAKE_CXX_FLAGS=-Wall -Wextra"], timeout=60)
        b = run(["cmake", "--build", bdir, "-j4"], timeout=120)
        out = c.stdout + c.stderr + b.stdout + b.stderr
        warns = sum(1 for ln in out.splitlines() if "warning:" in ln.lower())
        row["warnings"] = str(warns)
        binp = os.path.join(bdir, name)
        if b.returncode != 0 or not os.path.exists(binp):
            notes.append("build failed rc=%d" % b.returncode); row["notes"] = "; ".join(notes); rows.append(row); fails += 1; continue
        if warns: notes.append("%d warnings" % warns)
        row["build"] = "PASS"
    except Exception as e:
        notes.append("build exception %s" % e); row["notes"] = "; ".join(notes); rows.append(row); fails += 1; continue
    logf = open(os.path.join(TMP, name + ".stderr"), "wb")
    p = None
    try:
        p = subprocess.Popen([binp], env=ENV, stdout=subprocess.DEVNULL, stderr=logf)
        wid = None
        for _ in range(50):
            if p.poll() is not None: break
            r = run(["xdotool", "search", "--name", "%s - XFrame demo" % name], timeout=10)
            if r.stdout.strip(): wid = r.stdout.strip().split()[0]; break
            time.sleep(0.2)
        if wid is None:
            notes.append("no window; rc=%s" % p.poll()); raise RuntimeError("no window")
        wx, wy, ww, wh = geom(wid)
        gx, gy, gw, gh = 20, 20, 720, 480
        cx, cy = wx + gx + gw // 2, wy + gy + gh // 2
        time.sleep(0.4)
        dflt = os.path.join(TMP, name + "_default.png")
        if not shot(wid, dflt): notes.append("default screenshot failed"); raise RuntimeError("shot")
        import shutil; shutil.copyfile(dflt, os.path.join(SHOTS, name + ".png"))
        nb = nonbg(dflt)
        row["draw"] = "PASS" if nb > 20 else "FAIL"
        if nb <= 20: notes.append("blank: %d non-bg px" % nb)
        # hover
        for i in range(1, 9):
            run(["xdotool", "mousemove", str(wx + gx + gw * i // 9), str(wy + gy + gh * i // 9)], timeout=10); time.sleep(0.04)
        run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10); time.sleep(0.35)
        hov = os.path.join(TMP, name + "_hover.png")
        if p.poll() is None and shot(wid, hov):
            row["hover"] = "PASS"; row["hover_diff"] = str(diff(dflt, hov))
        else:
            row["hover"] = "FAIL"; notes.append("hover failed / died")
        # click (moves nearest anchor / adds brush point)
        run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
        run(["xdotool", "mousedown", "1"], timeout=10); time.sleep(0.35)
        run(["xdotool", "mouseup", "1"], timeout=10); time.sleep(0.35)
        prs = os.path.join(TMP, name + "_clicked.png")
        if p.poll() is None and shot(wid, prs):
            d = diff(dflt, prs); row["click_diff"] = str(d)
            row["click"] = "PASS" if d > 10 else "FAIL"
            if d <= 10: notes.append("click diff only %d px" % d)
        else:
            row["click"] = "FAIL"; notes.append("click shot failed / died")
        # key: Left arrow shifts anchors
        if p.poll() is None:
            run(["xdotool", "key", "--window", str(wid), "Left"], timeout=10); time.sleep(0.3)
            keyp = os.path.join(TMP, name + "_key.png")
            if shot(wid, keyp):
                kd = diff(prs if os.path.exists(prs) else dflt, keyp); row["key_diff"] = str(kd)
            row["key"] = "PASS" if p.poll() is None else "FAIL"
            if p.poll() is not None: notes.append("crashed on key rc=%s" % p.poll())
        else:
            row["key"] = "FAIL"; notes.append("dead before key rc=%s" % p.poll())
        # clean exit
        if p.poll() is None:
            run(["/tmp/send_delete", str(wid)], timeout=10)
            try:
                rc = p.wait(timeout=5); row["exit"] = "PASS" if rc == 0 else "FAIL"
                if rc != 0: notes.append("exit rc=%d" % rc)
            except subprocess.TimeoutExpired:
                row["exit"] = "FAIL"; notes.append("hang on WM_DELETE; killed"); p.kill(); p.wait(timeout=5)
        else:
            row["exit"] = "FAIL"; notes.append("already dead rc=%s" % p.poll())
    except Exception as e:
        notes.append("harness: %s" % e)
        try:
            if p is not None and p.poll() is None: p.kill(); p.wait(timeout=5)
        except Exception: pass
    finally:
        logf.close()
    row["notes"] = "; ".join(notes)
    if any(row[c] == "FAIL" for c in ("build", "draw", "hover", "click", "key", "exit")) or row["warnings"] not in ("", "0"):
        fails += 1
    rows.append(row)
    print("%-24s build=%s warn=%s draw=%s hover=%s click=%s key=%s exit=%s %s" % (
        name, row["build"], row["warnings"], row["draw"], row["hover"], row["click"], row["key"], row["exit"], row["notes"]), flush=True)

fields = ["name","build","warnings","draw","hover","click","key","exit","hover_diff","click_diff","key_diff","notes"]
with open(RESULTS, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=fields, extrasaction="ignore"); w.writeheader()
    for r in rows: w.writerow(r)
npass = sum(1 for r in rows if all(r[c] == "PASS" for c in ("build","draw","hover","click","key","exit")) and r["warnings"] in ("", "0"))
print("TOTAL %d/%d widgets fully PASS; failures=%d; csv=%s" % (npass, len(NAMES), fails, RESULTS))
sys.exit(1 if fails else 0)
