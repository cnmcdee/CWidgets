# Combined IDE -- FDesigner (designer/) + XFrame widgets (widgets/).
#   make            build both halves
#   make designer   build designer/fdesigner (C++/GTK 3)
#   make widgets    build widgets/build (194 demos + palette + layout)
#   make test       run the layout -> FDesigner seam test (no display needed)
#   make clean      remove built artefacts
ROOT := $(CURDIR)
NPROC := $(shell nproc 2>/dev/null || echo 4)

.PHONY: all designer widgets clean test

all: designer widgets

designer:
	$(MAKE) -C $(ROOT)/designer

widgets:
	cmake -S $(ROOT)/widgets -B $(ROOT)/widgets/build
	cmake --build $(ROOT)/widgets/build -j$(NPROC)

test:
	python3 $(ROOT)/widgets/tests/test_layout_to_fdesigner.py

clean:
	$(MAKE) -C $(ROOT)/designer clean
	rm -rf $(ROOT)/widgets/build
