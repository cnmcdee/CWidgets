#!/usr/bin/env python3
"""Fix the doc code examples the verifier found broken (111 widgets).

Fix classes (inside ```cpp blocks only):
  1. Volatility indicators: the widget class lives in namespace xframe, but
     the Example block used the bare name -> qualify `IndX widget(` as
     `xframe::IndX widget(`.
  2. Drawing tools: snippets called `w.set_series(make_seeded_series(...))`
     with a temporary, then used an undeclared `series` variable -> bind
     the series first: `auto series = ...; w.set_series(series);`
  3. Fib tools: `w.set_high_low(hi, lo)` with hi/lo undeclared -> compute
     them from the series range first (same lines as the demo mains).

Applies to the fragment docs and docs/stock-market.md (which mirrors them).
Idempotent. Run verify_doc_examples.py afterwards for the affected names.
"""
import os, re, sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
FENCE = chr(96) * 3

FILES = ["docs/stock-market-drawing.md",
         "docs/stock-market-indicators-volatility.md",
         "docs/stock-market.md"]

RE_SERIES = re.compile(
    r"^(\s*)w\.set_series\(xframe::make_seeded_series\(60, 42\)\);$")

def fix_lines(lines):
    out = []
    in_block = False
    n_qual = n_series = n_hilo = 0
    for ln in lines:
        s = ln.strip()
        if s == FENCE + "cpp":
            in_block = True
            out.append(ln)
            continue
        if s == FENCE and in_block:
            in_block = False
            out.append(ln)
            continue
        if in_block:
            m = re.match(r"^(\s*)(Ind\w+) widget\(display", ln)
            if m and not m.group(2).startswith("xframe"):
                ln = "%sxframe::%s widget(display%s" % (
                    m.group(1), m.group(2), ln[m.end():])
                n_qual += 1
            m = RE_SERIES.match(ln)
            if m:
                pad = m.group(1)
                out.append("%sauto series = xframe::make_seeded_series(60, 42);" % pad)
                out.append("%sw.set_series(series);" % pad)
                n_series += 1
                continue
            m = re.match(r"^(\s*)w\.set_high_low\(hi, lo\);", ln)
            if m:
                pad = m.group(1)
                out.append("%sdouble hi = series[0].high, lo = series[0].low; for (auto& b : series) { hi = std::max(hi, b.high); lo = std::min(lo, b.low); }" % pad)
                n_hilo += 1
        out.append(ln)
    return out, n_qual, n_series, n_hilo

def main():
    for path in FILES:
        p = os.path.join(REPO, path)
        lines = open(p).read().splitlines()
        new, nq, ns, nh = fix_lines(lines)
        open(p, "w").write("\n".join(new) + "\n")
        print("%s: qualified=%d series-bound=%d hi/lo-added=%d"
              % (path, nq, ns, nh))

if __name__ == "__main__":
    main()
