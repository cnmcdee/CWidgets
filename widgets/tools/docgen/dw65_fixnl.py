import os
OUT = "/tmp/dw65-examples"
Q = chr(34)
BS = chr(92)
NL = chr(10)
fixed = 0
for fn in sorted(os.listdir(OUT)):
    if not fn.endswith(".cpp"):
        continue
    path = os.path.join(OUT, fn)
    text = open(path).read()
    res = []
    in_str = False
    esc = False
    changed = False
    for ch in text:
        if in_str:
            if esc:
                esc = False
                res.append(ch)
            elif ch == BS:
                esc = True
                res.append(ch)
            elif ch == Q:
                in_str = False
                res.append(ch)
            elif ch == NL:
                res.append(BS)
                res.append("n")
                changed = True
            else:
                res.append(ch)
        else:
            if ch == Q:
                in_str = True
            res.append(ch)
    if changed:
        open(path, "w").write("".join(res))
        fixed += 1
print("fixed files:", fixed)
