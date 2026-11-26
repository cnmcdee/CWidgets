# Instrumentation — Tank Level Widgets

Group A of the XFrame Instrumentation section (11th palette section, amber
accent). Eight tank-level widgets, all in the standard XFrame hand-drawn
widget style — no photorealism, no photo textures.

Every widget shows fill 0–100%, a 0–100 scale, and hi/lo alarm markers.
Level is settable with `set_level()` (clamped to 0–100 via
`xframe::inst_clamp_level`) and readable with `get_level()`. Alarm bands
default to hi-hi 95, hi 80, lo 20, lo-lo 5 (`xframe::InstAlarmBands`) and
can be changed with `set_alarm_bands()`. Clicking a widget sets the level
from the click position; Up/Down (or Left/Right) keys nudge it by 5,
Home sets 0, End sets 100, R resets to the deterministic demo level.
Demos use deterministic values from `widgets/components/InstData.h`
(`xframe::inst_demo_level(seed)` in [5, 95]).

Shared header: `widgets/components/InstData.h` — `inst_clamp_level()`,
`InstAlarmBands`, `inst_demo_level()`, `inst_demo_value()`.

## InstVerticalTankLevel

Upright cylindrical tank, fill rises from the bottom, percent readout,
hi-hi / hi / lo / lo-lo alarm markers on the right of the tank.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstVerticalTankLevel tank(display, screen, window, gc, &styles, 20, 20, 360, 240, 62.5);
tank.set_level(75.0);
tank.set_alarm_bands(xframe::InstAlarmBands{95.0, 80.0, 20.0, 5.0});
tank.draw();
```

## InstHorizontalTankLevel

Horizontal bullet tank, fill by cross-section width, percent readout,
hi-hi / hi / lo / lo-lo alarm markers as vertical lines across the tank.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstHorizontalTankLevel tank(display, screen, window, gc, &styles, 20, 20, 360, 240, 40.0);
tank.set_level(55.0);
tank.draw();
```

## InstSphericalTankLevel

Sphere tank on legs, fill by volume height, percent readout, hi-hi / hi /
lo / lo-lo alarm markers.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstSphericalTankLevel tank(display, screen, window, gc, &styles, 20, 20, 360, 240, 68.0);
tank.set_level(68.0);
tank.draw();
```

## InstSightGlassLevel

Glass tube with liquid column, scale marks every 10% (labelled every 25%),
top/bottom pipe connections, percent readout, hi-hi / hi / lo / lo-lo
alarm markers.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstSightGlassLevel gauge(display, screen, window, gc, &styles, 20, 20, 360, 240, 33.0);
gauge.set_level(33.0);
gauge.draw();
```

## InstLevelColumnGauge

Vertical column gauge with scale and a float marker (red bar) at the
level, percent readout, hi-hi / hi / lo / lo-lo alarm markers.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstLevelColumnGauge gauge(display, screen, window, gc, &styles, 20, 20, 360, 240, 47.0);
gauge.set_level(47.0);
gauge.draw();
```

## InstTankLevelAlarms

Tank with hi-hi / hi / lo / lo-lo alarm bands and lamps: the red HI-HI
lamp lights when the level reaches hi-hi, amber HI at hi, amber LO at or
below lo, red LO-LO at or below lo-lo.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstTankLevelAlarms tank(display, screen, window, gc, &styles, 20, 20, 360, 240, 97.0);
tank.set_level(97.0);
tank.draw();
```

## InstOpenChannelLevel

Open reservoir / channel with waterline and depth scale, percent depth
readout, hi-hi / hi / lo / lo-lo alarm markers.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstOpenChannelLevel channel(display, screen, window, gc, &styles, 20, 20, 360, 240, 58.0);
channel.set_level(58.0);
channel.draw();
```

## InstLevelTransmitter

4–20 mA loop indicator: level bar, mA value (4 mA at 0%, 20 mA at 100%),
percent readout, loop terminals, hi-hi / hi / lo / lo-lo alarm markers.

API: `set_level(double)` / `get_level()`, `set_alarm_bands(InstAlarmBands)` /
`alarm_bands()`, `set_text()`, `set_on_click()`, `set_bounds()`, `contains()`,
`set_visible()` / `get_visible()`, `set_enabled()` / `get_enabled()`,
`draw()`, `on_button_press()` / `on_button_release()` / `on_motion()` /
`on_key()`.

```cpp
InstLevelTransmitter tx(display, screen, window, gc, &styles, 20, 20, 360, 240, 62.5);
tx.set_level(62.5);
double pct = tx.get_level();
double ma = 4.0 + 16.0 * pct / 100.0;
bool loop_ok = (ma >= 4.0 && ma <= 20.0);
tx.set_enabled(loop_ok);
tx.draw();
```
