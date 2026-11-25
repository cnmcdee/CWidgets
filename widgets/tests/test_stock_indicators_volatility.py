#!/usr/bin/env python3
# test_stock_indicators_volatility.py -- driver for the 80 volatility/volume/
# other Stock Market indicators. Re-derives every count from fresh artifacts.
# Attribution: XFrame. Exit non-zero on any failure.
import json, math, os, subprocess, sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
COMP = os.path.join(ROOT, "widgets", "components")
TOOLS = os.path.join(ROOT, "widgets", "tools")
WORK = "/tmp/ind_vol_test"
os.makedirs(WORK, exist_ok=True)

HAND_CLOSES = [100.0, 101.0, 102.0, 101.0, 103.0, 102.0]

def hand_expected():
    # Python reference for the two hand-checks, same 6-bar series as the C++ test.
    o, h, l, c = [], [], [], HAND_CLOSES
    prev = 100.0
    for i, cl in enumerate(c):
        op = prev
        o.append(op); h.append(max(op, cl) + 0.5); l.append(min(op, cl) - 0.5)
        prev = cl
    tr = [h[0] - l[0]] + [max(h[i] - l[i], abs(h[i] - c[i-1]), abs(l[i] - c[i-1])) for i in range(1, 6)]
    atr = [None]*6
    atr[2] = sum(tr[:3]) / 3.0
    for i in range(3, 6):
        atr[i] = (atr[i-1] * 2 + tr[i]) / 3.0
    bb = {}
    for idx in (4, 5):
        w = c[idx-4:idx+1]
        m = sum(w) / 5.0
        sd = math.sqrt(sum((x - m) ** 2 for x in w) / 5.0)
        bb[idx] = (m + 2*sd, m, m - 2*sd)
    return tr, atr, bb

TEST_CPP = r'''
#include "%(CLS)s.h"
#include <cstdio>
#include <cmath>
int main() {
    using namespace xframe;
    OHLCVSeries s250 = make_seeded_series(250);
    auto r = %(FUNC)s(s250);
    std::vector<double> p = %(PRIMARY)s;
    int fails = 0;
    if (p.size() != 250) { printf("SMOKE size %%zu\n", p.size()); fails++; }
    int warm = %(WARM)d;
    int nfinite = 0;
    for (size_t i = 0; i < p.size(); ++i) {
        if (std::isfinite(p[i])) nfinite++;
        else if ((int)i >= warm) { printf("SMOKE nan at %%zu\n", i); fails++; }
    }
    if (nfinite == 0) { printf("SMOKE no finite values\n"); fails++; }
    // Hand series: closes 100,101,102,101,103,102; open=prev close; high/low +-0.5.
    OHLCVSeries hand;
    double prev = 100.0;
    double closes[6] = {100.0, 101.0, 102.0, 101.0, 103.0, 102.0};
    for (int i = 0; i < 6; ++i) {
        double op = prev, cl = closes[i];
        hand.push_back(OHLCV{op, std::max(op, cl) + 0.5, std::min(op, cl) - 0.5, cl, 1000000.0, i});
        prev = cl;
    }
    %(HAND)s
    printf("LAST %%s %%.17g\n", "%(CLS)s", nfinite > 0 ? p.back() : 0.0);
    printf("SMOKE %%s\n", fails == 0 ? "PASS" : "FAIL");
    return fails == 0 ? 0 : 1;
}
'''

def hand_code(sp):
    if sp["name"] == "BollingerBands":
        return ('auto hb = ind_bollinger_bands(hand, 5, 2.0);\n'
                '    printf("HAND BB4 %.17g %.17g %.17g\\n", hb.upper[4], hb.middle[4], hb.lower[4]);\n'
                '    printf("HAND BB5 %.17g %.17g %.17g\\n", hb.upper[5], hb.middle[5], hb.lower[5]);')
    if sp["name"] == "ATR":
        return ('auto ha = ind_atr(hand, 3);\n'
                '    printf("HAND ATR2 %.17g\\n", ha[2]);\n'
                '    printf("HAND ATR3 %.17g\\n", ha[3]);\n'
                '    printf("HAND ATR4 %.17g\\n", ha[4]);\n'
                '    printf("HAND ATR5 %.17g\\n", ha[5]);')
    return ''

def run(cmd, env=None, timeout=120):
    e = dict(os.environ); e.update(env or {})
    return subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=timeout, env=e)

def check_one(sp):
    cls, func = sp["cls"], sp["func"]
    d = os.path.join(COMP, cls)
    res = dict(name=cls, build="FAIL", draw="FAIL", click="FAIL", key="FAIL", exit="FAIL", smoke="FAIL", cmake="FAIL")
    details = []
    primary = "r" if sp["ret"] == "vec" else "r." + sp["primary"]
    cpp = TEST_CPP % dict(CLS=cls, FUNC=func, PRIMARY=primary, WARM=sp["warmup"], HAND=hand_code(sp))
    tcpp = os.path.join(WORK, "test_" + cls + ".cpp")
    open(tcpp, "w").write(cpp)
    binp = os.path.join(WORK, "test_" + cls)
    c = run("g++ -std=c++17 -Wall -Wextra -I %s -o %s %s 2>&1" % (d, binp, tcpp))
    if c.returncode != 0:
        details.append("compile: " + c.stdout[-1500:])
        return res, details
    res["build"] = "PASS"
    r = run(binp, timeout=60)
    out = r.stdout
    if "SMOKE PASS" in out and r.returncode == 0:
        res["smoke"] = "PASS"
    else:
        details.append("smoke out: " + out[-1500:] + r.stderr[-500:])
    # hand-checks
    if sp["name"] in ("BollingerBands", "ATR"):
        tr, atr, bb = hand_expected()
        ok = True
        for line in out.splitlines():
            if line.startswith("HAND BB4"):
                vals = [float(x) for x in line.split()[2:]]
                exp = bb[4]
                if any(abs(a - b) > 1e-9 for a, b in zip(vals, exp)): ok = False; details.append("BB4 %r vs %r" % (vals, exp))
            if line.startswith("HAND BB5"):
                vals = [float(x) for x in line.split()[2:]]
                exp = bb[5]
                if any(abs(a - b) > 1e-9 for a, b in zip(vals, exp)): ok = False; details.append("BB5 %r vs %r" % (vals, exp))
            if line.startswith("HAND ATR"):
                idx = int(line.split()[1][3:])
                val = float(line.split()[2])
                if abs(val - atr[idx]) > 1e-9: ok = False; details.append("ATR%d %r vs %r" % (idx, val, atr[idx]))
        if ok:
            res["hand"] = "PASS"
        else:
            res["hand"] = "FAIL"
    # demo build + run under Xvfb (click/key exercised inside the demo main)
    demobin = os.path.join(WORK, "demo_" + cls)
    c = run("g++ -std=c++17 -Wall -Wextra -I %s -o %s %s 2>&1" % (d, demobin, os.path.join(d, "main_" + cls + ".cpp -lX11")))
    if c.returncode != 0:
        details.append("demo compile: " + c.stdout[-1500:])
        return res, details
    r = run("DISPLAY=:96 timeout 20 %s" % demobin, timeout=60)
    if r.returncode == 0:
        res["draw"] = "PASS"; res["click"] = "PASS"; res["key"] = "PASS"; res["exit"] = "PASS"
    else:
        details.append("demo run rc=%d out=%s err=%s" % (r.returncode, r.stdout[-500:], r.stderr[-500:]))
    # cmake build
    bdir = os.path.join(WORK, "cmake_" + cls)
    c = run("cmake -S %s -B %s > /dev/null 2>&1 && cmake --build %s > /dev/null 2>&1" % (d, bdir, bdir), timeout=180)
    if c.returncode == 0:
        res["cmake"] = "PASS"
    else:
        details.append("cmake build failed")
    return res, details

def main():
    spec = json.load(open(os.path.join(TOOLS, "stock_indicators_volatility_spec.json")))
    lst = [l for l in open(os.path.join(TOOLS, "stock_indicators_volatility.txt")).read().splitlines() if l.strip()]
    dirs = sorted(s["cls"] for s in spec if os.path.isdir(os.path.join(COMP, s["cls"])))
    all_ind = sorted(x for x in os.listdir(COMP) if x.startswith("Ind"))
    print("list lines: %d  spec entries: %d  my Ind* dirs: %d  all Ind* dirs: %d" % (len(lst), len(spec), len(dirs), len(all_ind)))
    assert len(lst) == 80 and len(spec) == 80 and len(dirs) == 80, "count mismatch"
    assert len(set(s["name"] for s in spec)) == 80, "duplicate names"
    cats = {}
    for s in spec: cats[s["cat"]] = cats.get(s["cat"], 0) + 1
    print("categories: %s" % cats)
    for s in spec:
        d = os.path.join(COMP, s["cls"])
        for f in (s["cls"] + ".h", "main_" + s["cls"] + ".cpp", "CMakeLists.txt", os.path.join("core", "xframe_core.h")):
            assert os.path.exists(os.path.join(d, f)), "missing %s in %s" % (f, s["cls"])
    with ThreadPoolExecutor(max_workers=4) as ex:
        results = list(ex.map(check_one, spec))
    fails = 0
    print("%-32s %-6s %-6s %-6s %-6s %-6s %-6s %-6s %s" % ("indicator", "build", "draw", "click", "key", "exit", "smoke", "cmake", "hand"))
    for res, details in results:
        hand = res.get("hand", "-")
        print("%-32s %-6s %-6s %-6s %-6s %-6s %-6s %-6s %s" % (res["name"], res["build"], res["draw"], res["click"], res["key"], res["exit"], res["smoke"], res["cmake"], hand))
        if any(v == "FAIL" for v in res.values()):
            fails += 1
            for dt in details: print("  FAIL %s: %s" % (res["name"], dt))
    print("TOTAL %d/%d pass" % (len(results) - fails, len(results)))
    sys.exit(1 if fails else 0)

if __name__ == "__main__":
    main()
