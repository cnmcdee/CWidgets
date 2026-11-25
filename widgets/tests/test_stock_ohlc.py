#!/usr/bin/env python3
"""Stock Market OHLC group driver: builds the 8 OHLC/price-chart widgets,
checks StockData.h determinism, runs each demo headless under Xvfb :91,
screenshots + hover/click/key/clean-exit. Re-derives every count from fresh
artifacts each round; exits non-zero on any failure. Attribution: XFrame."""
import os, subprocess, sys, time, shutil
from PIL import Image, ImageChops

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMP = os.path.join(ROOT, "components")
BUILD = os.path.join(ROOT, "build", "components")
TMP = "/tmp/stock-ohlc-shots"
ENV = dict(os.environ, DISPLAY=":91")
WIDGETS = ["StockCandlestickChart", "StockOHLCBarChart", "StockLineChart",
           "StockAreaChart", "StockHeikinAshi", "StockRenko",
           "StockHollowCandles", "StockBaselineChart"]
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

# --- StockData.h determinism check (standalone compile) ---
chk = os.path.join(TMP, "stockdata_check.cpp")
open(chk, "w").write(r'''
#include "StockData.h"
#include <cstdio>
int main() {
    auto a = xframe::make_seeded_series(250);
    auto b = xframe::make_seeded_series(250);
    auto c = xframe::make_seeded_series(250, 7);
    if (a.size() != 250 || b.size() != 250 || c.size() != 250) return 2;
    for (int i = 0; i < 250; ++i) {
        if (a[i].open != b[i].open || a[i].close != b[i].close) return 3;
        if (a[i].high < std::max(a[i].open, a[i].close) || a[i].low > std::min(a[i].open, a[i].close)) return 4;
        if (!(a[i].volume > 0) || a[i].timestamp != 1767225600LL + (long long)i * 86400) return 5;
    }
    if (a[10].close == c[10].close && a[10].open == c[10].open) return 6;
    auto t = xframe::series_typical(a);
    if (t.size() != 250 || t[0] != (a[0].high + a[0].low + a[0].close) / 3.0) return 7;
    std::printf("stockdata ok\n");
    return 0;
}
''')
r = subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra", "-I", COMP, chk, "-o", os.path.join(TMP, "stockdata_check")],
                   capture_output=True, text=True, timeout=60)
check(r.returncode == 0 and "warning:" not in r.stderr, f"StockData compile failed/warnings: {r.stderr[:500]}")
if r.returncode == 0:
    r2 = subprocess.run([os.path.join(TMP, "stockdata_check")], capture_output=True, text=True, timeout=30)
    check(r2.returncode == 0, f"StockData determinism check rc={r2.returncode}")
    print("StockData.h: deterministic, seed differs, sane bars, typical OK" if r2.returncode == 0 else "StockData check FAILED")

# --- send_delete helper ---
sd = "/tmp/send_delete"
if not os.path.exists(sd):
    subprocess.run(["cc", os.path.join(ROOT, "test-driver", "send_delete.c"), "-lX11", "-o", sd],
                   capture_output=True, timeout=60)

# --- Xvfb :91 ---
xvfb = None
if not os.path.exists("/tmp/.X91-lock"):
    xvfb = subprocess.Popen(["Xvfb", ":91", "-screen", "0", "1024x768x24"])
    time.sleep(1.5)

try:
    for name in WIDGETS:
        row = {"name": name, "build": "FAIL", "draw": "FAIL", "hover": "FAIL",
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
                for c in ("draw", "hover", "click", "key", "exit"): row[c] = "FAIL"
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
            # hover sweep -> crosshair
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
            # click (press) -> crosshair + focus
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
            # key: Tab + a, process must survive
            if p.poll() is None:
                run(["xdotool", "key", "--window", str(wid), "Tab"], timeout=10)
                time.sleep(0.2)
                run(["xdotool", "key", "--window", str(wid), "a"], timeout=10)
                time.sleep(0.3)
                row["key"] = "PASS" if p.poll() is None else "FAIL"
                if p.poll() is not None: notes.append(f"crashed on key rc={p.poll()}"); failures.append(f"{name}: crashed on key")
            else:
                row["key"] = "FAIL"; notes.append(f"died before key rc={p.poll()}"); failures.append(f"{name}: died before key")
            # clean exit
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
        print(f"{name}: build={row['build']} draw={row['draw']} hover={row['hover']} click={row['click']} key={row['key']} exit={row['exit']}" + (f"  [{row['notes']}]" if row["notes"] else ""), flush=True)
finally:
    if xvfb is not None:
        xvfb.terminate()
        try: xvfb.wait(timeout=5)
        except Exception: xvfb.kill()

npass = sum(1 for r in rows if all(r[c] == "PASS" for c in ("build", "draw", "hover", "click", "key", "exit")))
print(f"OHLC group: {npass}/{len(WIDGETS)} widgets fully PASS; failures={len(failures)}")
for f in failures:
    print("FAIL:", f)
sys.exit(0 if not failures and npass == len(WIDGETS) else 1)
