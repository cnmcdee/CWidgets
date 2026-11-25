#!/usr/bin/env python3
"""Test driver for the 15 Stock Market compare/volume widgets.

Builds each demo with -Wall -Wextra (0 warnings required), runs it under
Xvfb :92, screenshots the default draw, exercises hover/click/key, then
asks for a clean exit via WM_DELETE_WINDOW. Re-derives every count from
fresh artifacts on each run; exits non-zero if any cell fails.
"""
import csv
import os
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "build", "components")
SHOTS = os.path.join(ROOT, "screenshots")
TMP = "/tmp/cw-scv"
RESULTS = os.path.join(ROOT, "test-results", "stock_compare_volume.csv")
ENV = dict(os.environ, DISPLAY=":92")
NAMES = [
    "StockOverlayChart", "StockMultiSymbolChart", "StockRelativePerformance",
    "StockSpreadChart", "StockRatioChart", "StockCorrelationView",
    "StockIndexedComparison", "StockSectorComparison", "StockVolumeBars",
    "StockVWAP", "StockVolumeProfile", "StockOBVChart", "StockVolumeCandles",
    "StockAccumulationDistribution", "StockMoneyFlowChart",
]

os.makedirs(SHOTS, exist_ok=True)
os.makedirs(TMP, exist_ok=True)
os.makedirs(os.path.dirname(RESULTS), exist_ok=True)


def run(*a, timeout=30, **k):
    return subprocess.run(*a, capture_output=True, text=True, timeout=timeout, env=ENV, **k)


def nonbg_count(path):
    from PIL import Image
    im = Image.open(path).convert("L")
    h = im.histogram()
    modal = max(range(256), key=lambda v: h[v])
    return sum(c for v, c in enumerate(h) if abs(v - modal) > 10)


def main():
    # compile the WM_DELETE sender once
    send_delete = os.path.join(TMP, "send_delete")
    if not os.path.exists(send_delete):
        r = subprocess.run(["cc", "-O2", "-o", send_delete,
                            os.path.join(ROOT, "test-driver", "send_delete.c"),
                            "-lX11"], capture_output=True, text=True)
        if r.returncode != 0:
            print("send_delete build failed:", r.stderr)
            return 1
    rows = []
    nfail = 0
    for name in NAMES:
        row = {"name": name, "build": "FAIL", "draw": "FAIL", "click": "FAIL",
               "key": "FAIL", "exit": "FAIL", "notes": ""}
        notes = []
        # build
        bdir = os.path.join(BUILD, name)
        subprocess.run(["rm", "-rf", bdir])
        r1 = subprocess.run(["cmake", "-S", os.path.join(ROOT, "components", name),
                             "-B", bdir], capture_output=True, text=True)
        r2 = subprocess.run(["cmake", "--build", bdir], capture_output=True, text=True) \
            if r1.returncode == 0 else None
        log = (r1.stdout + r1.stderr) + ((r2.stdout + r2.stderr) if r2 else "")
        warns = log.lower().count("warning:")
        binp = os.path.join(bdir, name)
        if r1.returncode != 0 or (r2 and r2.returncode != 0):
            notes.append("build rc!=0")
        elif warns:
            notes.append(f"{warns} warnings")
        elif not (os.path.exists(binp) and os.access(binp, os.X_OK)):
            notes.append("binary missing")
        else:
            row["build"] = "PASS"
        if row["build"] == "FAIL":
            row["notes"] = "; ".join(notes)
            rows.append(row)
            nfail += 1
            continue
        # run
        logf = open(os.path.join(TMP, name + ".stderr"), "wb")
        p = subprocess.Popen([binp], env=ENV, stdout=subprocess.DEVNULL, stderr=logf)
        wid = None
        try:
            for _ in range(50):
                if p.poll() is not None:
                    break
                r = run(["xdotool", "search", "--name", f"{name} - XFrame demo"], timeout=10)
                if r.stdout.strip():
                    wid = r.stdout.strip().split()[0]
                    break
                time.sleep(0.2)
            if wid is None:
                notes.append(f"no window; rc={p.poll()}")
                row["notes"] = "; ".join(notes)
                rows.append(row)
                nfail += 1
                try:
                    p.wait(timeout=3)
                except Exception:
                    p.kill()
                logf.close()
                continue
            r = run(["xdotool", "getwindowgeometry", "--shell", str(wid)], timeout=10)
            g = {}
            for line in r.stdout.splitlines():
                if "=" in line:
                    k, v = line.split("=", 1)
                    try:
                        g[k] = int(v)
                    except ValueError:
                        pass
            cx, cy = g.get("X", 100) + 380, g.get("Y", 100) + 260
            time.sleep(0.4)
            shot = os.path.join(TMP, name + "_default.png")
            if run(["import", "-window", str(wid), shot], timeout=15).returncode == 0 \
                    and os.path.exists(shot) and os.path.getsize(shot) > 0:
                nb = nonbg_count(shot)
                if nb > 20:
                    row["draw"] = "PASS"
                    import shutil
                    shutil.copyfile(shot, os.path.join(SHOTS, name + ".png"))
                else:
                    notes.append(f"blank window: {nb} non-bg px")
            else:
                notes.append("screenshot failed")
            # hover + click (centre), then a keypress; alive afterwards = pass
            run(["xdotool", "mousemove", str(cx), str(cy)], timeout=10)
            time.sleep(0.2)
            run(["xdotool", "mousedown", "1", "mousemove", str(cx + 30), str(cy),
                 "mouseup", "1"], timeout=10)
            time.sleep(0.2)
            if p.poll() is None:
                row["click"] = "PASS"
            else:
                notes.append(f"died after click rc={p.poll()}")
            run(["xdotool", "key", "Right"], timeout=10)
            time.sleep(0.2)
            run(["xdotool", "key", "r"], timeout=10)
            time.sleep(0.2)
            if p.poll() is None:
                row["key"] = "PASS"
            else:
                notes.append(f"died after key rc={p.poll()}")
            # clean exit via WM_DELETE_WINDOW
            run([send_delete, str(wid)], timeout=10)
            try:
                rc = p.wait(timeout=5)
                row["exit"] = "PASS" if rc == 0 else "FAIL"
                if rc != 0:
                    notes.append(f"exit rc={rc}")
            except subprocess.TimeoutExpired:
                notes.append("exit timeout; killed")
                p.kill()
                p.wait(timeout=3)
        finally:
            if p.poll() is None:
                p.kill()
            logf.close()
        row["notes"] = "; ".join(notes)
        rows.append(row)
        if any(row[c] == "FAIL" for c in ("build", "draw", "click", "key", "exit")):
            nfail += 1
    with open(RESULTS, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=["name", "build", "draw", "click", "key", "exit", "notes"])
        w.writeheader()
        w.writerows(rows)
    print(f"{'widget':32} {'build':6} {'draw':6} {'click':6} {'key':6} {'exit':6} notes")
    for r in rows:
        print(f"{r['name']:32} {r['build']:6} {r['draw']:6} {r['click']:6} {r['key']:6} {r['exit']:6} {r['notes']}")
    npass = sum(1 for r in rows if all(r[c] == "PASS" for c in ("build", "draw", "click", "key", "exit")))
    print(f"PASS {npass}/{len(rows)} widgets, {npass * 5}/{len(rows) * 5} cells; results: {RESULTS}")
    return 0 if nfail == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
