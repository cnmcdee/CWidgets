#include "IndTypicalPrice.h"

#include <cstdio>
#include <unistd.h>

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1,
        BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "IndTypicalPrice - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60},
        Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    xframe::OHLCVSeries series = xframe::make_seeded_series(250);
    xframe::IndTypicalPrice widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
    widget.draw();
    // Exercise the interaction handlers headlessly, then redraw.
    widget.on_button_press(400, 300, 1);
    widget.on_motion(420, 310);
    widget.on_button_release(420, 310, 1);
    widget.on_key(XK_Right, "");
    widget.on_button_press(400, 300, 4);
    widget.draw();
    XFlush(display);
    usleep(150000);
    XCloseDisplay(display);
    return 0;
}
