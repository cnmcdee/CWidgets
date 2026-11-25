#!/usr/bin/env python3
"""CWidgets test driver: builds are done separately; this runs all 194 demos
under Xvfb, screenshots each, exercises hover/click/key/exit, writes CSV."""
import subprocess, os, time, sys, re, csv, signal
from collections import Counter
from PIL import Image, ImageChops

ROOT = "/opt/cwidgets"
BUILD = os.path.join(ROOT, "build", "components")
SHOTS = os.path.join(ROOT, "screenshots")
TMP = "/tmp/cw-shots"
RESULTS = os.path.join(ROOT, "test-results", "results.csv")
ENV = dict(os.environ, DISPLAY=":99")
os.makedirs(SHOTS, exist_ok=True)
os.makedirs(TMP, exist_ok=True)
os.makedirs(os.path.dirname(RESULTS), exist_ok=True)

def run(*a, timeout=30, **k):
    return subprocess.run(*a, capture_output=True, text=True, timeout=timeout, env=ENV, **k)

def nonbg_count(path):
    """Pixels whose luminance differs from the modal luminance by >10."""
    im = Image.open(path).convert("L")
    h = im.histogram()
    modal = max(range(256), key=lambda v: h[v])
    total = sum(h)
    diff = sum(c for v, c in enumerate(h) if abs(v - modal) > 10)
    return diff, total, 100.0 * h[modal] / total

def diff_count(a, b):
    ia = Image.open(a).convert("RGB"); ib = Image.open(b).convert("RGB")
    if ia.size != ib.size:
        ib = ib.resize(ia.size)
    d = ImageChops.difference(ia, ib).convert("L")
    h = d.histogram()
    return sum(h[31:])

def geom_from_main(name):
    src = open(os.path.join(ROOT, "components", name, f"main_{name}.cpp"), errors="ignore").read()
    m = re.search(r"widget\(display,\s*screen,\s*window,\s*gc[^;]*?,\s*&styles,\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\)", src)
    if m:
        x, y, w, h = map(int, m.groups())
        return x, y, w, h
    return 40, 40, 340, 200

def win_geom(wid):
    r = run(["xdotool", "getwindowgeometry", "--shell", str(wid)], timeout=10)
    g = {}
    for line in r.stdout.splitlines():
        if "=" in line:
            k, v = line.split("=", 1)
            try: g[k] = int(v)
            except ValueError: pass
    return g.get("X", 100), g.get("Y", 100), g.get("WIDTH", 800), g.get("HEIGHT", 600)

def shot(wid, path):
    r = run(["import", "-window", str(wid), path], timeout=15)
    return os.path.exists(path) and os.path.getsize(path) > 0

names = sorted(os.listdir(os.path.join(ROOT, "components")))
print(f"components: {len(names)}", flush=True)
rows = []
t0 = time.time()
for idx, name in enumerate(names, 1):
    row = {"name": name, "build": "", "draw": "", "hover": "", "click": "", "key": "", "exit": "",
           "notes": "", "modal_pct": "", "distinct": "", "hover_diff": "", "pressed_diff": "",
           "released_diff": "", "key_diff": ""}
    notes = []
    binp = os.path.join(BUILD, name, name)
    if not (os.path.exists(binp) and os.access(binp, os.X_OK)):
        row["build"] = "FAIL"; notes.append("binary missing")
        for c in ("draw", "hover", "click", "key", "exit"): row[c] = "FAIL"
        row["notes"] = "; ".join(notes); rows.append(row); continue
    row["build"] = "PASS"
    logf = open(os.path.join(TMP, name + ".stderr"), "wb")
    try:
        p = subprocess.Popen([binp], env=ENV, stdout=subprocess.DEVNULL, stderr=logf)
    except Exception as e:
        row["notes"] = f"launch failed: {e}"
        for c in ("draw", "hover", "click", "key", "exit"): row[c] = "FAIL"
        rows.append(row); continue
    wid = None
    try:
        for _ in range(50):
            if p.poll() is not None:
                break
            r = run(["xdotool", "search", "--name", f"{name} - XFrame demo"], timeout=10)
            if r.stdout.strip():
                wid = r.stdout.strip().split()[0]; break
            time.sleep(0.2)
        if wid is None:
            rc = p.poll()
            if rc is not None:
                notes.append(f"no window; exited rc={rc} at startup")
            else:
                notes.append("no window within 10s; process alive, killed")
                p.kill()
            for c in ("draw", "hover", "click", "key", "exit"): row[c] = "FAIL"
            row["notes"] = "; ".join(notes); rows.append(row)
            try: p.wait(timeout=3)
            except Exception: p.kill()
            logf.close(); continue
        wx, wy, ww, wh = win_geom(wid)
        gx, gy, gw, gh = geom_from_main(name)
        cx, cy = wx + gx + gw // 2, wy + gy + gh // 2
        time.sleep(0.4)
        dflt = os.path.join(SHOTS, name + ".png")
        tmp_dflt = os.path.join(TMP, name + "_default.png")
        if not shot(wid, tmp_dflt):
            notes.append("default screenshot failed"); row["draw"] = "FAIL"
        else:
            import shutil; shutil.copyfile(tmp_dflt, dflt)
            nb, total, modal_pct = nonbg_count(tmp_dflt)
            row["modal_pct"] = f"{modal_pct:.1f}"
            im = Image.open(tmp_dflt).convert("RGB")
            row["distinct"] = str(len(im.getcolors(maxcolors=1000000) or []))
            if nb > 20:
                row["draw"] = "PASS"
            else:
                row["draw"] = "FAIL"; notes.append(f"blank window: only {nb} non-bg px, modal {modal_pct:.1f}%")
        # HOVER: sweep mouse in steps across widget, end at center
        if row["draw"] in ("PASS", "FAIL"):
            steps = 8
            for i in range(1, steps + 1):
                run(["xdotool", "mousemove", str(wx + gx + gw * i // (steps + 1)),
                     str(wy + gy + gh * i // (steps + 1))], timeout=10)
                time.sleep(0.04)
            run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
            time.sleep(0.35)
            hov = os.path.join(TMP, name + "_hover.png")
            if shot(wid, hov) and os.path.exists(tmp_dflt):
                hd = diff_count(tmp_dflt, hov); row["hover_diff"] = str(hd)
                row["hover"] = "PASS" if hd > 50 else "NORESP"
                if hd <= 50: notes.append("no visible hover response (on_motion no-op)")
            else:
                row["hover"] = "FAIL"; notes.append("hover screenshot failed")
        # CLICK: press, shot, release, shot
        run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
        run(["xdotool", "mousedown", "1"], timeout=10)
        time.sleep(0.35)
        prs = os.path.join(TMP, name + "_pressed.png")
        pd = -1
        if shot(wid, prs) and os.path.exists(tmp_dflt):
            pd = diff_count(tmp_dflt, prs); row["pressed_diff"] = str(pd)
        run(["xdotool", "mouseup", "1"], timeout=10)
        time.sleep(0.35)
        rel = os.path.join(TMP, name + "_released.png")
        rd = -1
        if shot(wid, rel) and os.path.exists(tmp_dflt):
            rd = diff_count(tmp_dflt, rel); row["released_diff"] = str(rd)
        if pd > 10:
            row["click"] = "PASS"
            if pd <= 50:
                notes.append("minimal but real click response (text cursor / click mark, %d px)" % pd)
        elif rd > 50:
            row["click"] = "PASS"; notes.append("click response visible only after release (focus ring/state change)")
        elif pd >= 0:
            row["click"] = "NORESP"; notes.append("no observable click response (no pressed/focus visual, no on_click in demo)")
        else:
            row["click"] = "FAIL"; notes.append("click screenshots failed")
        # KEY: Tab then a letter; process must survive
        if p.poll() is None:
            run(["xdotool", "key", "--window", str(wid), "Tab"], timeout=10)
            time.sleep(0.2)
            run(["xdotool", "key", "--window", str(wid), "a"], timeout=10)
            time.sleep(0.3)
            keyp = os.path.join(TMP, name + "_key.png")
            if shot(wid, keyp) and os.path.exists(tmp_dflt):
                kd = diff_count(tmp_dflt, keyp); row["key_diff"] = str(kd)
            if p.poll() is None:
                row["key"] = "PASS"
            else:
                row["key"] = "FAIL"; notes.append(f"crashed on keypress rc={p.poll()}")
        else:
            row["key"] = "FAIL"; notes.append(f"process died before key test rc={p.poll()}")
        # CLEAN EXIT via WM_DELETE_WINDOW
        if p.poll() is None:
            run(["/tmp/send_delete", str(wid)], timeout=10)
            try:
                rc = p.wait(timeout=5)
                row["exit"] = "PASS" if rc == 0 else "FAIL"
                if rc != 0: notes.append(f"exit rc={rc}")
            except subprocess.TimeoutExpired:
                row["exit"] = "FAIL"; notes.append("hang on WM_DELETE_WINDOW; killed")
                p.kill(); p.wait(timeout=5)
        else:
            row["exit"] = "FAIL"; notes.append(f"already dead before close rc={p.poll()}")
    except Exception as e:
        notes.append(f"harness exception: {e}")
        for c in ("draw", "hover", "click", "key", "exit"):
            if not row[c]: row[c] = "FAIL"
        try:
            if p.poll() is None: p.kill(); p.wait(timeout=5)
        except Exception: pass
    finally:
        logf.close()
    row["notes"] = "; ".join(notes)
    rows.append(row)
    if idx % 25 == 0 or idx == len(names):
        el = time.time() - t0
        npass = sum(1 for r in rows if r["build"] == "PASS" and r["draw"] == "PASS" and r["exit"] == "PASS")
        print(f"[{idx}/{len(names)}] {el:.0f}s elapsed; build+draw+exit PASS so far: {npass}", flush=True)
    # write incremental CSV
    with open(RESULTS, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0].keys())); w.writeheader(); w.writerows(rows)
print(f"DONE {len(rows)} in {time.time()-t0:.0f}s", flush=True)
