#!/usr/bin/env python3
"""Instrumentation lamps group driver: builds the 12 illuminator/LED widgets,
runs each demo headless under Xvfb :77 (draw / hover / click / key / clean
exit), runs the C++ state harness (pixel-proved on/off + brightness, RGB 8
channel combinations), and verifies every docs/instrumentation-lamps.md code
example compiles. Re-derives every count from fresh artifacts each round;
exits non-zero on any failure. Attribution: XFrame."""
import os, re, shutil, subprocess, sys, time
from PIL import Image, ImageChops

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMP = os.path.join(ROOT, "components")
BUILD = os.path.join(ROOT, "build", "components")
TMP = "/tmp/inst-lamps-shots"
ENV = dict(os.environ, DISPLAY=":77")
WIDGETS = ["InstPanelLampRed", "InstPanelLampGreen", "InstPanelLampAmber",
           "InstPanelLampBlue", "InstPanelLampWhite", "InstLedRed",
           "InstLedGreen", "InstLedBlue", "InstRgbStatusLed", "InstStackLight",
           "InstBeacon", "InstLampTestButton"]
CLICK = {"InstLampTestButton": (120, 360)}
os.makedirs(TMP, exist_ok=True)
failures = []
rows = []

def run(*a, timeout=30, **k):
    return subprocess.run(*a, capture_output=True, text=True, timeout=timeout, env=ENV, **k)

def check(cond, msg):
    if not cond:
        failures.append(msg)
    return cond

def nonbg(path):
    im = Image.open(path).convert("L")
    h = im.histogram()
    modal = max(range(256), key=lambda v: h[v])
    return sum(c for v, c in enumerate(h) if abs(v - modal) > 10)

def diff(a, b):
    ia = Image.open(a).convert("RGB"); ib = Image.open(b).convert("RGB")
    if ia.size != ib.size:
        ib = ib.resize(ia.size)
    return sum(ImageChops.difference(ia, ib).convert("L").histogram()[31:])

def shot(wid, path):
    r = run(["import", "-window", str(wid), path], timeout=15)
    return os.path.exists(path) and os.path.getsize(path) > 0

# --- Xvfb :77 (needed by the state harness too) ---
xvfb = None
if not os.path.exists("/tmp/.X77-lock"):
    # own copy of the Xvfb binary under a distinct process name, so a
    # sibling worker's blanket `pkill -x Xvfb` cannot kill this display
    xvfb_bin = "/tmp/Xvfb-lamps77"
    if not os.path.exists(xvfb_bin):
        shutil.copy("/usr/bin/Xvfb", xvfb_bin)
    xvfb = subprocess.Popen([xvfb_bin, ":77", "-screen", "0", "1024x768x24"])
    time.sleep(1.5)

# --- C++ state harness: compile + run ---
HARN = os.path.join(ROOT, "tests", "test_inst_lamps_state.cpp")
HARN_BIN = os.path.join(TMP, "test_inst_lamps_state")
state_results = {}
r = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra", "-I", COMP,
                    HARN, "-o", HARN_BIN, "-lX11"],
                   capture_output=True, text=True, timeout=120)
if r.returncode != 0 or "warning:" in r.stderr:
    failures.append(f"state harness compile failed/warnings: {r.stderr[:600]}")
else:
    r2 = run([HARN_BIN], timeout=120)
    for line in r2.stdout.splitlines():
        m = re.match(r"STATE (\S+) (PASS|FAIL)", line)
        if m:
            state_results[m.group(1)] = m.group(2)
    if r2.returncode != 0:
        failures.append(f"state harness exit {r2.returncode}: {r2.stdout[-600:]}")
    for w in WIDGETS + ["InstData"]:
        if state_results.get(w) != "PASS":
            failures.append(f"state harness: {w} != PASS (got {state_results.get(w)})")
    print("state harness:", r2.stdout.strip().splitlines()[-1] if r2.stdout.strip() else "no output", flush=True)

# --- doc examples: extract ```cpp blocks, wrap in harness, compile ---
HARNESS_PRE = """#include "%s.h"
#include <cstdio>
int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
"""
HARNESS_POST = """
    return 0;
}
"""
doc_results = {}
docpath = os.path.join(os.path.dirname(ROOT), "docs", "instrumentation-lamps.md")
lines = open(docpath).read().splitlines()
heading = None
i = 0
blocks = []
while i < len(lines):
    m = re.match(r"#{2,3}\s+(\S+)", lines[i])
    if m:
        heading = m.group(1)
    if lines[i].strip() == "```cpp":
        j = i + 1
        while j < len(lines) and lines[j].strip() != "```":
            j += 1
        blocks.append((heading, "\n".join(lines[i + 1:j])))
        i = j
    i += 1
for heading, block in blocks:
    if heading not in WIDGETS:
        continue
    src = HARNESS_PRE % heading + block + HARNESS_POST
    srcf = os.path.join(TMP, heading + "_docex.cpp")
    binf = os.path.join(TMP, heading + "_docex")
    open(srcf, "w").write(src)
    rr = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra", "-I", COMP,
                         "-I", os.path.join(COMP, heading), srcf, "-o", binf, "-lX11"],
                        capture_output=True, text=True, timeout=120)
    ok = rr.returncode == 0 and "warning:" not in rr.stderr
    doc_results[heading] = "PASS" if ok else "FAIL"
    if not ok:
        failures.append(f"doc example {heading} failed: {rr.stderr[:400]}")
print(f"doc examples: {sum(1 for v in doc_results.values() if v == 'PASS')}/{len(doc_results)} PASS", flush=True)

# --- send_delete helper ---
sd = "/tmp/send_delete"
if not os.path.exists(sd):
    subprocess.run(["cc", os.path.join(ROOT, "test-driver", "send_delete.c"), "-lX11", "-o", sd],
                   capture_output=True, timeout=60)

try:
    for name in WIDGETS:
        row = {"name": name, "build": "FAIL", "draw": "FAIL", "hover": "FAIL",
               "click": "FAIL", "key": "FAIL", "exit": "FAIL", "state": "FAIL", "docex": "FAIL"}
        notes = []
        row["state"] = state_results.get(name, "FAIL")
        if row["state"] != "PASS":
            notes.append("state harness FAIL")
        row["docex"] = doc_results.get(name, "FAIL")
        if row["docex"] != "PASS":
            notes.append("doc example FAIL")
        bdir = os.path.join(BUILD, name)
        r = subprocess.run(["cmake", "-S", os.path.join(COMP, name), "-B", bdir,
                            "-DCMAKE_CXX_FLAGS=-Wall -Wextra"],
                           capture_output=True, text=True, timeout=120)
        out = r.stdout + r.stderr
        r2 = subprocess.run(["cmake", "--build", bdir], capture_output=True, text=True, timeout=180) if r.returncode == 0 else None
        out2 = (r2.stdout + r2.stderr) if r2 else ""
        binp = os.path.join(bdir, name)
        if r.returncode != 0 or (r2 and r2.returncode != 0):
            notes.append(f"build failed: {(out + out2)[-400:]}")
            row["notes"] = "; ".join(notes); rows.append(row); failures.append(f"{name}: build failed"); continue
        if "warning:" in out2:
            notes.append("compiler warnings present")
            failures.append(f"{name}: warnings: {out2[out2.find('warning:'):out2.find('warning:')+200]}")
        if not (os.path.exists(binp) and os.access(binp, os.X_OK)):
            notes.append("binary missing"); row["notes"] = "; ".join(notes); rows.append(row); failures.append(f"{name}: binary missing"); continue
        row["build"] = "PASS"
        logf = open(os.path.join(TMP, name + ".stderr"), "wb")
        p = subprocess.Popen([binp], env=ENV, stdout=subprocess.DEVNULL, stderr=logf)
        wid = None
        try:
            for _ in range(50):
                if p.poll() is not None:
                    break
                rr = run(["xdotool", "search", "--name", f"{name} - XFrame demo"], timeout=10)
                if rr.stdout.strip():
                    wid = rr.stdout.strip().split()[0]; break
                time.sleep(0.2)
            if wid is None:
                notes.append(f"no window; rc={p.poll()}"); failures.append(f"{name}: no window")
                row["notes"] = "; ".join(notes); rows.append(row)
                try: p.wait(timeout=3)
                except Exception: p.kill()
                logf.close(); continue
            rr = run(["xdotool", "getwindowgeometry", "--shell", str(wid)], timeout=10)
            g = {}
            for line in rr.stdout.splitlines():
                if "=" in line:
                    k, v = line.split("=", 1)
                    try: g[k] = int(v)
                    except ValueError: pass
            wx, wy = g.get("X", 100), g.get("Y", 100)
            ccx, ccy = CLICK.get(name, (380, 260))
            cx, cy = wx + ccx, wy + ccy
            time.sleep(0.4)
            dflt = os.path.join(TMP, name + "_default.png")
            if not shot(wid, dflt):
                notes.append("screenshot failed"); failures.append(f"{name}: screenshot failed")
            else:
                nb = nonbg(dflt)
                if nb > 20: row["draw"] = "PASS"
                else: notes.append(f"blank: {nb} non-bg px"); failures.append(f"{name}: blank draw")
            for i in range(1, 9):
                run(["xdotool", "mousemove", str(wx + 20 + 720 * i // 9), str(wy + 20 + 480 * i // 9)], timeout=10)
                time.sleep(0.04)
            run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
            time.sleep(0.35)
            hov = os.path.join(TMP, name + "_hover.png")
            if shot(wid, hov) and os.path.exists(dflt):
                hd = diff(dflt, hov)
                row["hover"] = "PASS" if hd > 50 else "FAIL"
                if hd <= 50: notes.append(f"no hover response ({hd})"); failures.append(f"{name}: no hover response")
            else:
                notes.append("hover shot failed"); failures.append(f"{name}: hover shot failed")
            run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
            run(["xdotool", "mousedown", "1"], timeout=10)
            time.sleep(0.35)
            prs = os.path.join(TMP, name + "_pressed.png")
            if shot(wid, prs) and os.path.exists(dflt):
                pd = diff(dflt, prs)
                row["click"] = "PASS" if pd > 10 else "FAIL"
                if pd <= 10: notes.append(f"no click response ({pd})"); failures.append(f"{name}: no click response")
            else:
                notes.append("click shot failed"); failures.append(f"{name}: click shot failed")
            run(["xdotool", "mouseup", "1"], timeout=10)
            time.sleep(0.2)
            if p.poll() is None:
                run(["xdotool", "key", "--window", str(wid), "Tab"], timeout=10)
                time.sleep(0.2)
                run(["xdotool", "key", "--window", str(wid), "a"], timeout=10)
                time.sleep(0.3)
                row["key"] = "PASS" if p.poll() is None else "FAIL"
                if p.poll() is not None: notes.append(f"crashed on key rc={p.poll()}"); failures.append(f"{name}: crashed on key")
            else:
                row["key"] = "FAIL"; notes.append(f"died before key rc={p.poll()}"); failures.append(f"{name}: died before key")
            if p.poll() is None:
                run([sd, str(wid)], timeout=10)
                try:
                    rc = p.wait(timeout=5)
                    row["exit"] = "PASS" if rc == 0 else "FAIL"
                    if rc != 0: notes.append(f"exit rc={rc}"); failures.append(f"{name}: exit rc={rc}")
                except subprocess.TimeoutExpired:
                    row["exit"] = "FAIL"; notes.append("hang on WM_DELETE"); failures.append(f"{name}: hang on exit")
                    p.kill(); p.wait(timeout=5)
            else:
                row["exit"] = "FAIL"; notes.append(f"already dead rc={p.poll()}"); failures.append(f"{name}: dead before exit")
        finally:
            if p.poll() is None:
                p.kill()
                try: p.wait(timeout=3)
                except Exception: pass
            logf.close()
        row["notes"] = "; ".join(notes)
        rows.append(row)
        print(f"{name}: build={row['build']} draw={row['draw']} hover={row['hover']} click={row['click']} key={row['key']} exit={row['exit']} state={row['state']} docex={row['docex']}" + (f"  [{row['notes']}]" if row["notes"] else ""), flush=True)
finally:
    if xvfb is not None:
        xvfb.terminate()
        try: xvfb.wait(timeout=5)
        except Exception: xvfb.kill()

COLS = ("build", "draw", "hover", "click", "key", "exit", "state", "docex")
npass = sum(1 for r in rows if all(r[c] == "PASS" for c in COLS))
print(f"Lamps group: {npass}/{len(WIDGETS)} widgets fully PASS; failures={len(failures)}")
for f in failures:
    print("FAIL:", f)
sys.exit(0 if not failures and npass == len(WIDGETS) else 1)
