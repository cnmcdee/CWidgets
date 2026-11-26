# Instrumentation — Switches (Group B)

12 switch widgets for the XFrame Instrumentation section. Every switch really
toggles: a click flips the state and the widget redraws in the new position.
State is observable via `is_on()` and `get_state()` (returning
`xframe::InstSwitchState`: `Off`, `On`, `Tripped`), and every widget exposes an
`on_click` callback fired on each state change. Style: standard XFrame
hand-drawn widget style, no photorealism. Attribution: XFrame.

Shared types live in `widgets/components/InstData.h`.

## InstRockerSwitch

SPST rocker switch: up is on, down is off. Click flips it and it redraws tilted the other way.

API: is_on(), get_state() -> InstSwitchState, toggle(), set_on(bool), on_click callback.

```cpp
#include "InstRockerSwitch.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstRockerSwitch sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.on_click = []() { std::printf("rocker toggled\n"); };
    sw.set_on(true);
    std::printf("on=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstRockerSwitchIlluminated

Rocker switch with a lens that lights green when on and dim red when off.

API: is_on(), get_state() -> InstSwitchState, toggle(), set_on(bool), on_click callback. Lens: green lit when on, dim red when off.

```cpp
#include "InstRockerSwitchIlluminated.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstRockerSwitchIlluminated sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.on_click = []() { std::printf("illuminated rocker toggled\n"); };
    sw.set_on(true);
    std::printf("on=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstRockerSwitchDouble

DPDT centre-off rocker: click the top half for up/on, the bottom half for down, the same half again (or the middle) for centre/off.

API: is_on(), get_state() -> InstSwitchState (On = up, Off = centre, Tripped = down), get_position() (-1/0/+1), set_position(int), on_click callback.

```cpp
#include "InstRockerSwitchDouble.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstRockerSwitchDouble sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.set_position(1);
    std::printf("pos=%d on=%d state=%d\n", sw.get_position(), (int)sw.is_on(), (int)sw.get_state());
    sw.set_position(-1);
    std::printf("pos=%d on=%d state=%d\n", sw.get_position(), (int)sw.is_on(), (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstToggleSwitch

Bat-lever toggle switch: lever points up when on, down when off.

API: is_on(), get_state() -> InstSwitchState, toggle(), set_on(bool), on_click callback.

```cpp
#include "InstToggleSwitch.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstToggleSwitch sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.toggle();
    std::printf("on=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstToggleSwitchIlluminated

Bat-lever toggle with an illuminated tip: the tip glows green when on.

API: is_on(), get_state() -> InstSwitchState, toggle(), set_on(bool), on_click callback. Tip glows green when on.

```cpp
#include "InstToggleSwitchIlluminated.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstToggleSwitchIlluminated sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.toggle();
    std::printf("on=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstPushButton

Momentary push button: on only while held down, off on release.

API: is_on(), get_state() -> InstSwitchState. On only while the button is held; release turns it off. on_click fires on press.

```cpp
#include "InstPushButton.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstPushButton sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.on_button_press(380, 260, 1);
    std::printf("held on=%d\n", (int)sw.is_on());
    sw.on_button_release(380, 260, 1);
    std::printf("released on=%d\n", (int)sw.is_on());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstPushButtonLatching

Latching push button: click once and it stays in (on), click again and it pops out (off).

API: is_on(), get_state() -> InstSwitchState, toggle(), set_on(bool), on_click callback. Drawn depressed with a green ring while latched in.

```cpp
#include "InstPushButtonLatching.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstPushButtonLatching sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.toggle();
    std::printf("latched on=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.toggle();
    std::printf("latched on=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstEmergencyStop

Mushroom emergency stop: press once and it latches in (stopped). A second press does nothing; twist (right-click) or the R key resets it.

API: is_on(), get_state() -> InstSwitchState (Tripped = latched/stopped), reset(), on_click callback. Press latches; right-click or R resets.

```cpp
#include "InstEmergencyStop.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstEmergencyStop sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.on_button_press(380, 260, 1);
    std::printf("stopped=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.reset();
    std::printf("after reset stopped=%d\n", (int)sw.is_on());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstRotarySelector

Rotary selector switch with 2, 3 or 4 positions and a pointer. Click (or Space) advances to the next position.

API: get_position(), get_positions(), set_positions(2..4), set_position(int), is_on() (position > 0), get_state(), on_click callback.

```cpp
#include "InstRotarySelector.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstRotarySelector sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.set_positions(4);
    sw.set_position(2);
    std::printf("pos=%d of %d on=%d\n", sw.get_position(), sw.get_positions(), (int)sw.is_on());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstKeySwitch

Key-operated selector: click inserts the key, further clicks move through the positions, K removes the key (position resets to 0).

API: get_key_in(), set_key_in(bool), get_position(), get_positions(), set_positions(2..4), is_on() (key in and position > 0), get_state(), on_click callback.

```cpp
#include "InstKeySwitch.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstKeySwitch sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.set_key_in(true);
    sw.on_button_press(380, 260, 1);
    std::printf("key_in=%d pos=%d on=%d\n", (int)sw.get_key_in(), sw.get_position(), (int)sw.is_on());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstSlideSwitch

Slide switch: knob left is off, right is on. Click toggles; drag the knob across.

API: is_on(), get_state() -> InstSwitchState, toggle(), set_on(bool), on_click callback. Drag the knob or click to flip.

```cpp
#include "InstSlideSwitch.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstSlideSwitch sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.toggle();
    std::printf("on=%d state=%d\n", (int)sw.is_on(), (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```

## InstCircuitBreaker

Circuit breaker: handle up is on, down is off, mid-position with a red TRIPPED flag is the fault state. Click toggles on/off; trip() simulates a fault.

API: is_on(), get_state() -> InstSwitchState (On/Off/Tripped), trip(), reset(), on_click callback. Click toggles On/Off and resets from Tripped.

```cpp
#include "InstCircuitBreaker.h"

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return 1;
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100, 800, 600, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    InstCircuitBreaker sw(display, screen, window, gc, &styles, 20, 20, 720, 480);
    sw.trip();
    std::printf("tripped state=%d on=%d\n", (int)sw.get_state(), (int)sw.is_on());
    sw.reset();
    std::printf("after reset state=%d\n", (int)sw.get_state());
    sw.draw();
    XCloseDisplay(display);
    return 0;
}
```
