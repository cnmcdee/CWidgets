# Instrumentation widgets — illuminators & status LEDs

Twelve widgets in `widgets/components/`, one per directory. Each directory
contains exactly `<Name>.h`, `main_<Name>.cpp`, `CMakeLists.txt`, and
`core/xframe_core.h` (byte-identical to `widgets/core/xframe_core.h`).
Attribution: XFrame.

Every lamp and LED is an independent illuminator: its own on/off plus
brightness 0–100, with the state observable (`is_on()`,
`get_brightness()`). They share the types in
`widgets/components/InstData.h` (included as `#include "../InstData.h"`):
`xframe::InstLamp {on, brightness}`, `xframe::InstRgb {red, green, blue}`,
and `inst_clamp_level()`. Style is the standard XFrame hand-drawn widget
style — no photorealism, no photo textures. Click (or Space when focused)
toggles a lamp; moving the mouse over a widget draws a hover outline.

## Common API (all twelve)

Constructor (all widgets):

```
Name(Display* display, int screen, Window window, GC gc,
     SStyle_Manager* styles, int x, int y, int width, int height);
```

Key methods, identical on every widget:

- `void draw()` — bezel/lens (or tiers/dome/button) plus label; hover or
  focus draws an outline.
- `bool on_button_press(int x, int y, unsigned int button)`,
  `bool on_button_release(int x, int y, unsigned int button)`,
  `bool on_motion(int x, int y)` — press toggles (or holds, for the
  lamp-test button); motion tracks hover.
- `bool on_key(KeySym key, const char* str)` — only when focused: Space
  toggles (momentary press/release for the lamp-test button); the RGB LED
  also takes `r`/`g`/`b`, the stack light `1`/`2`/`3`, the beacon `f`.
- `void set_bounds(...)`, `width()`, `height()`, `contains(x, y)`,
  `set_visible`/`get_visible`, `set_enabled`/`get_enabled`,
  `set_style_name`, `set_text`/`get_text`, `style()`.
- `std::function<void()> on_click` — called whenever the widget is
  toggled by click or Space.

Build one widget (from the repo root, on the server):

```
cmake -S widgets/components/InstPanelLampRed -B /tmp/b-lampred -DCMAKE_CXX_FLAGS="-Wall -Wextra"
cmake --build /tmp/b-lampred   # 0 warnings
```

Tests: `python3 widgets/tests/test_inst_lamps.py` builds all twelve with
`-Wall -Wextra`, runs each demo under Xvfb `:77` and checks build / draw
(non-blank screenshot) / hover / click / key (process survives) / clean
exit via `WM_DELETE_WINDOW`, runs the C++ state harness
(`widgets/tests/test_inst_lamps_state.cpp`, which hashes drawn pixels to
prove on/off and brightness change the picture, and that the RGB LED's 8
channel combinations are all distinct), and verifies every code example in
this document compiles. Result: see the driver run output (12/12 widgets fully PASS, 0 warnings, driver exit 0).

## InstPanelLampRed

Round bezel panel lamp, red. The lens is a dim red tint when off and bright red when on, brighter with brightness; a highlight and glow ring appear at high brightness.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstPanelLampRed.h"
InstPanelLampRed w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstPanelLampGreen

Round bezel panel lamp, green. Same behaviour as the red lamp, green lens.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstPanelLampGreen.h"
InstPanelLampGreen w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstPanelLampAmber

Round bezel panel lamp, amber. Same behaviour as the red lamp, amber lens.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstPanelLampAmber.h"
InstPanelLampAmber w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstPanelLampBlue

Round bezel panel lamp, blue. Same behaviour as the red lamp, blue lens.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstPanelLampBlue.h"
InstPanelLampBlue w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstPanelLampWhite

Round bezel panel lamp, white. Same behaviour as the red lamp, white lens.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstPanelLampWhite.h"
InstPanelLampWhite w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstLedRed

Single status LED, red: dark bezel block, domed lens, two leads. On/off + brightness like the panel lamps.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstLedRed.h"
InstLedRed w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstLedGreen

Single status LED, green: dark bezel block, domed lens, two leads. On/off + brightness like the panel lamps.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstLedGreen.h"
InstLedGreen w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstLedBlue

Single status LED, blue: dark bezel block, domed lens, two leads. On/off + brightness like the panel lamps.

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()` — the lamp state.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; the lens brightens with brightness (off is a dim tint).
- `xframe::InstLamp lamp_state() const` / `void set_lamp(const xframe::InstLamp& s)` — the whole illuminator state at once.
- Click or Space toggles; `on_click` fires on every toggle.

```cpp
#include "InstLedBlue.h"
InstLedBlue w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_brightness(75);
w.draw();
// Click or Space toggles the lamp; w.is_on() / w.get_brightness() read it back.
```
## InstRgbStatusLed

ONE status LED with INDEPENDENT red, green and blue channels — each channel is its own illuminator, so the LED shows any of the 8 colours plus off (red, green, blue, yellow, cyan, magenta, white, off).

- `void set_red(bool v)` / `set_green(bool v)` / `set_blue(bool v)` — each channel independently.
- `bool get_red() const` / `get_green() const` / `get_blue() const`, `xframe::InstRgb rgb_state() const` / `void set_rgb(const xframe::InstRgb& s)`.
- `void colour(int& r, int& g, int& b) const` — the combined 0–255 colour (any of the 8 combinations + off).
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped; scales all channels together.
- `void set_all(bool v)` / `void toggle_all()` / `bool is_on() const` — all channels at once.
- Click or Space toggles all channels; keys `r`/`g`/`b` toggle one channel; the three squares under the dome show the channel states.

```cpp
#include "InstRgbStatusLed.h"
InstRgbStatusLed w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_red(true);
w.set_green(true);   // red + green = yellow; each channel independent
w.draw();
int r = 0, g = 0, b = 0;
w.colour(r, g, b);
// Keys r/g/b toggle one channel; w.get_red() / get_green() / get_blue().
```
## InstStackLight

3-tier tower light on a pole and base: red (top), amber, green tiers, each an independent illuminator with its own on/off + brightness.

- `void set_red(bool v)` / `set_amber(bool v)` / `set_green(bool v)` — each tier independently (red top, amber middle, green bottom).
- `bool is_red_on() const` / `is_amber_on() const` / `is_green_on() const`.
- `void set_red_brightness(int b)` / `set_amber_brightness(int b)` / `set_green_brightness(int b)` (0–100, clamped) and matching getters.
- `xframe::InstLamp red_lamp() const` / `amber_lamp() const` / `green_lamp() const`, `void set_all(bool v)`, `bool is_on() const`.
- Click a tier to toggle just that tier; keys `1`/`2`/`3` toggle red/amber/green; Space toggles all.

```cpp
#include "InstStackLight.h"
InstStackLight w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_red(true);
w.set_green(true);
w.draw();
// Click a tier to toggle it; keys 1/2/3 toggle red/amber/green.
```
## InstBeacon

Rotating/flashing beacon on a base: amber dome with a beam when lit, on/off plus a flash rate of 0.5–10 Hz stepped with tick().

- `void set_on(bool on)` / `bool is_on() const` / `void toggle()`.
- `void set_brightness(int b)` / `int get_brightness() const` — 0–100, clamped.
- `void set_flash_rate(double hz)` / `double get_flash_rate() const` — 0.5–10 Hz, clamped.
- `void tick()` / `void set_phase(bool p)` / `bool get_phase() const` — advance one flash step; `bool is_lit() const` is on && in the bright phase.
- Click or Space toggles on/off; key `f` steps the flash.

```cpp
#include "InstBeacon.h"
InstBeacon w(display, screen, window, gc, &styles, 20, 20, 720, 480);
w.set_on(true);
w.set_flash_rate(2.0);
w.draw();
// w.tick() advances one flash step; key f steps it, Space toggles on/off.
```
## InstLampTestButton

Lamp-test push button: while held it lights every lamp wired up with add_lamp(), then restores each lamp's previous state on release. A panel wires its lamps in and gets an on_test callback.

- `void add_lamp(std::function<void(bool)> setter, std::function<bool()> getter)` — wire a lamp in; the button drives it while held.
- `int lamp_count() const` — how many lamps are wired.
- `void set_on_test(std::function<void(bool)> cb)` — callback fired with true on press and false on release, for the panel to wire up.
- `void press()` / `void release()` / `bool is_testing() const` — while held every wired lamp is on; on release each lamp's previous state is restored.
- Click-and-hold (or Space) is the momentary action; `on_click` fires on press.

```cpp
#include "InstLampTestButton.h"
#include "InstPanelLampRed/InstPanelLampRed.h"
#include "InstPanelLampGreen/InstPanelLampGreen.h"
InstPanelLampRed lamp_r(display, screen, window, gc, &styles, 20, 20, 200, 200);
InstPanelLampGreen lamp_g(display, screen, window, gc, &styles, 240, 20, 200, 200);
InstLampTestButton w(display, screen, window, gc, &styles, 20, 260, 200, 200);
w.add_lamp([&](bool v) { lamp_r.set_on(v); }, [&]() { return lamp_r.is_on(); });
w.add_lamp([&](bool v) { lamp_g.set_on(v); }, [&]() { return lamp_g.is_on(); });
w.set_on_test([](bool testing) { (void)testing; });
w.draw();
```
