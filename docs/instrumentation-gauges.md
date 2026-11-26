# Instrumentation gauges and panel furniture

Group D (gauges and meters) and Group E (panel furniture) of the XFrame
Instrumentation section. Standard XFrame hand-drawn widget style, no
photorealism. Every widget is header-only, has a settable value where it
applies, and ships with a demo program. Attribution: XFrame.

## InstAnalogGauge

InstAnalogGauge - round dial gauge with a needle over a 0-100 scale.

Range: `0` to `100`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.

```cpp
#include "InstAnalogGauge.h"
InstAnalogGauge gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(0, 100, 7));
gauge.set_value(0);
gauge.draw();
```

## InstPressureGauge

InstPressureGauge - pressure dial in psi with a red zone above 240 psi.

Range: `0` to `300`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.

```cpp
#include "InstPressureGauge.h"
InstPressureGauge gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(0, 300, 7));
gauge.set_value(0);
gauge.draw();
```

## InstTemperatureGauge

InstTemperatureGauge - temperature dial in degrees C with a red zone above 150 C.

Range: `-20` to `200`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.

```cpp
#include "InstTemperatureGauge.h"
InstTemperatureGauge gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(-20, 200, 7));
gauge.set_value(-20);
gauge.draw();
```

## InstVuMeter

InstVuMeter - VU-style meter with a needle, red zone above 0 VU.

Range: `-20` to `3`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.

```cpp
#include "InstVuMeter.h"
InstVuMeter gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(-20, 3, 7));
gauge.set_value(-20);
gauge.draw();
```

## InstBarMeter

InstBarMeter - vertical or horizontal bar meter, green/amber/red by level. Click sets the value.

Range: `0` to `100`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.
`set_horizontal(bool)` switches vertical / horizontal; clicking the bar sets the value.

```cpp
#include "InstBarMeter.h"
InstBarMeter gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(0, 100, 7));
gauge.set_value(0);
gauge.draw();
```

## InstDigitalReadout

InstDigitalReadout - 7-segment style numeric display, 0 to 999.9.

Range: `0` to `999.9`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.

```cpp
#include "InstDigitalReadout.h"
InstDigitalReadout gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(0, 999.9, 7));
gauge.set_value(0);
gauge.draw();
```

## InstManometer

InstManometer - U-tube manometer, liquid levels move apart with differential pressure in mm.

Range: `-50` to `50`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.

```cpp
#include "InstManometer.h"
InstManometer gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(-50, 50, 7));
gauge.set_value(-50);
gauge.draw();
```

## InstFlowMeter

InstFlowMeter - rotameter: a float rides in a tapered tube, 0 to 100 L/min. Click sets the flow.

Range: `0` to `100`. API: `set_value(double)` (clamped to range), `get_value()`, `set_range(lo, hi)`, `min_value()`, `max_value()`, `fraction()`. Arrow keys adjust the value, Home resets to mid-range. `on_click` fires on click.

```cpp
#include "InstFlowMeter.h"
InstFlowMeter gauge(display, screen, window, gc, &styles, 20, 20, 320, 240, xframe::inst_demo_value(0, 100, 7));
gauge.set_value(0);
gauge.draw();
```

## InstPanelPlate

InstPanelPlate - mounting plate with corner screws and an amber title strip.

API: `set_text()` / `get_text()` for the title, `set_bounds()`, `contains()`, `set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`, `on_click` fires on click.

```cpp
#include "InstPanelPlate.h"
InstPanelPlate widget(display, screen, window, gc, &styles, 20, 20, 320, 240);
widget.draw();
```

## InstNameplate

InstNameplate - engraved label plate with two screws.

API: `set_text()` / `get_text()` for the title, `set_bounds()`, `contains()`, `set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`, `on_click` fires on click.

```cpp
#include "InstNameplate.h"
InstNameplate widget(display, screen, window, gc, &styles, 20, 20, 320, 240);
widget.draw();
```

## InstBezel

InstBezel - mounting bezel frame with an open window and corner screws.

API: `set_text()` / `get_text()` for the title, `set_bounds()`, `contains()`, `set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`, `on_click` fires on click.

```cpp
#include "InstBezel.h"
InstBezel widget(display, screen, window, gc, &styles, 20, 20, 320, 240);
widget.draw();
```

## InstControlPanel

InstControlPanel - grid container: title strip, screws, and labelled cells other widgets sit on. Click a cell to select it.

API: `set_text()` / `get_text()` for the title, `set_bounds()`, `contains()`, `set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`, `on_click` fires on click.
`set_grid(rows, cols)`, `place_cell(row, col, label)`, `cell_label(row, col)`, `selected_row()` / `selected_col()` / `selected_label()` — click a cell to select it.

```cpp
#include "InstControlPanel.h"
InstControlPanel panel(display, screen, window, gc, &styles, 20, 20, 320, 240);
panel.set_grid(2, 2);
panel.place_cell(0, 0, "PUMP");
panel.place_cell(0, 1, "VALVE");
panel.draw();
```
