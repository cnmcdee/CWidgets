# Doc generators

The scripts that generated `docs/*.md` from the component headers in
`components/<Name>/`, recovered from `/tmp` on the VPS on Nov 26, 2026
(the docs coordinator had left them there; `/tmp` is wiped on reboot).

- `gen_docs.py` — worker 3s generator (Text & Entry, Views & Lists,
  Menus & Toolbars, Misc/X — 65 widgets).
- `dw65_docgen.py` + `dw65_gen_examples.py` + `dw65_extract.py` +
  `dw65_audit.py` / `dw65_audit2.py` + `dw65_verify.py` +
  `dw65_patchdocs.py` + `dw65_missed.py` + `dw65_fixgen.py` /
  `dw65_fixgen2.py` + `dw65_fixnl.py` — worker 2s generator and its
  helpers (Buttons, Display, Choosers — 67 widgets).
- `extract.py` / `extract2.py` / `extract62.py` / `extract62b.py` /
  `extract62c.py` + `docdata.py` — worker 1s extraction and data
  (Windows & Dialogs, Containers & Layout — 62 widgets).

Run order across workers was never recorded, and the scripts still
reference their original `/tmp` working dirs (e.g. `/tmp/dw65-examples`).
The `.md` files in `docs/` are the durable deliverable; these scripts
are kept so the docs can be regenerated rather than re-created.
