#!/usr/bin/env python3
"""Instrumentation Group B (switches) driver: builds the 12 switch widgets with
0 warnings, runs a unit harness per widget proving the toggle flips state AND
changes drawn pixels (state flip with identical pixels is a FAIL), then runs
each demo headless under Xvfb :92 with screenshot + real X click + key +
clean exit. Re-derives every count from fresh artifacts each round; exits
non-zero on any failure. Attribution: XFrame."""
import os, subprocess, sys, time
from PIL import Image, ImageChops

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMP = os.path.join(ROOT, "components")
BUILD = os.path.join(ROOT, "build", "components")
TMP = "/tmp/inst-switches-shots"
ENV = dict(os.environ, DISPLAY=":92")
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

# --- widget list from the manifest this generator wrote (fresh artifact, not hard-coded) ---
MANIFEST = os.path.join(ROOT, "tools", "instrumentation_switches.txt")
WIDGETS = sorted(l.strip() for l in open(MANIFEST) if l.strip())
print(f"switch widgets found: {len(WIDGETS)}")
# per-widget click point (dx, dy from widget centre): the double rocker needs an
# off-centre click (centre = centre-off), the momentary button is shot while held.
CLICK_OFF = {"InstRockerSwitchDouble": (0, -80)}

# --- send_delete helper ---
sd = "/tmp/send_delete"
if not os.path.exists(sd):
    subprocess.run(["cc", os.path.join(ROOT, "test-driver", "send_delete.c"), "-lX11", "-o", sd],
                   capture_output=True, timeout=60)

# --- Xvfb :92 ---
xvfb = None
if not os.path.exists("/tmp/.X92-lock"):
    xvfb = subprocess.Popen(["Xvfb", ":92", "-screen", "0", "1024x768x24"])
    time.sleep(1.5)

try:
    for name in WIDGETS:
        row = {"name": name, "build": "FAIL", "unit": "FAIL", "draw": "FAIL",
               "click": "FAIL", "key": "FAIL", "exit": "FAIL", "notes": ""}
        notes = []
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
        # --- unit harness: state flip + pixel change ---
        usrc = os.path.join(ROOT, "tools", "inst_switches_unit", "test_" + name + ".cpp")
        ubin = os.path.join(TMP, "test_" + name)
        if os.path.exists(usrc):
            ru = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra", "-I", os.path.join(COMP, name),
                                 usrc, "-o", ubin, "-lX11"], capture_output=True, text=True, timeout=120)
            if ru.returncode != 0 or "warning:" in (ru.stdout + ru.stderr):
                notes.append(f"unit harness build failed/warnings: {(ru.stdout + ru.stderr)[-300:]}")
                failures.append(f"{name}: unit harness build failed")
            else:
                r3 = run([ubin], timeout=30)
                if r3.returncode == 0 and "PASS" in r3.stdout:
                    row["unit"] = "PASS"
                else:
                    notes.append(f"unit harness: {(r3.stdout + r3.stderr)[-300:]}")
                    failures.append(f"{name}: unit harness failed: {(r3.stdout + r3.stderr)[-200:]}")
        else:
            notes.append("unit harness source missing"); failures.append(f"{name}: unit harness missing")
        # --- demo under Xvfb: draw / real click / key / clean exit ---
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
                for c in ("draw", "click", "key", "exit"): row[c] = "FAIL"
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
            cx, cy = wx + 20 + 360, wy + 20 + 240
            time.sleep(0.4)
            dflt = os.path.join(TMP, name + "_default.png")
            if not shot(wid, dflt):
                notes.append("screenshot failed"); failures.append(f"{name}: screenshot failed")
            else:
                nb = nonbg(dflt)
                if nb > 20: row["draw"] = "PASS"
                else: notes.append(f"blank: {nb} non-bg px"); failures.append(f"{name}: blank draw")
            dx, dy = CLICK_OFF.get(name, (0, 0))
            run(["xdotool", "mousemove", str(cx + dx), str(cy + dy)], timeout=10)
            run(["xdotool", "mousedown", "1"], timeout=10)
            time.sleep(0.35)
            clk = os.path.join(TMP, name + "_clicked.png")
            if shot(wid, clk) and os.path.exists(dflt):
                cd = diff(dflt, clk)
                row["click"] = "PASS" if cd > 10 else "FAIL"
                if cd <= 10: notes.append(f"no click pixel change ({cd})"); failures.append(f"{name}: no click pixel change")
            else:
                notes.append("click shot failed"); failures.append(f"{name}: click shot failed")
            run(["xdotool", "mouseup", "1"], timeout=10)
            time.sleep(0.2)
            if p.poll() is None:
                run(["xdotool", "key", "--window", str(wid), "space"], timeout=10)
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
        print(f"{name}: build={row['build']} unit={row['unit']} draw={row['draw']} click={row['click']} key={row['key']} exit={row['exit']}" + (f"  [{row['notes']}]" if row["notes"] else ""), flush=True)
finally:
    if xvfb is not None:
        xvfb.terminate()
        try: xvfb.wait(timeout=5)
        except Exception: xvfb.kill()

npass = sum(1 for r in rows if all(r[c] == "PASS" for c in ("build", "unit", "draw", "click", "key", "exit")))
print(f"Switches group: {npass}/{len(WIDGETS)} widgets fully PASS; failures={len(failures)}")
for f in failures:
    print("FAIL:", f)
sys.exit(0 if not failures and npass == len(WIDGETS) else 1)
