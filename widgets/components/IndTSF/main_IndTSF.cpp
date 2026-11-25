#include "IndTSF.h"
#include <cstdio>
int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) { return 1; }
    int screen = DefaultScreen(display);
    XID Xid = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "IndTSF - XFrame demo");
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindowMessage, 1);
    XMapWindow(display, window);
    GC gc = DefaultGC(display, screen);
    SStyle_Manager styles(display, window, screen, gc);
    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.add_style_ex("dark", Srgb{45, 45, 48}, Srgb{20, 20, 22}, Srgb{200, 200, 200}, Srgb{240, 240, 240}, 6, 1, "-misc-fixed-medium-r-normal--13-*");
    styles.set_active_style_byName("default");
    auto series = xframe::make_seeded_series(120, 42);
    xframe::IndTSF widget(display, screen, window, gc, &styles, 20, 20, 720, 480, series);
    widget.draw();
    widget.on_button_press(400, 300, 1);
    widget.on_button_release(400, 300, 1);
    widget.on_motion(410, 300);
    widget.on_key(XK_Right, "");
    widget.on_click(420, 300);
    widget.draw();
    XFlush(display);
    std::printf("OK IndTSF\n");
    XCloseDisplay(display);
    return 0;
}
