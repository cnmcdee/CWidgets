#include "TArrow.h"

#include <cstdio>

int main()

{

    Display* display = XOpenDisplay(nullptr);

    if (!display) {return 1; }

    int screen = DefaultScreen(display);

    XID Xid = RootWindow(display, screen);

    Window window = XCreateSimpleWindow(display, Xid, 100, 100, 800, 600, 1, BlackPixel(display, screen), WhitePixel(display, screen));

    XStoreName(display, window, "TArrow - XFrame demo");

    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

    Atom deleteWindowMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);

    XSetWMProtocols(display, window, &deleteWindowMessage, 1);

    XMapWindow(display, window);

    GC gc = DefaultGC(display, screen);

    SStyle_Manager styles(display, window, screen, gc);

    styles.add_style_ex("default", Srgb{225, 225, 225}, Srgb{120, 120, 120}, Srgb{60, 60, 60}, Srgb{20, 20, 20}, 6, 1, "-misc-fixed-medium-r-normal--13-*");

    styles.add_style_ex("dark", Srgb{45, 45, 48}, Srgb{20, 20, 22}, Srgb{200, 200, 200}, Srgb{240, 240, 240}, 6, 1, "-misc-fixed-medium-r-normal--13-*");

    styles.set_active_style_byName("default");

    TArrow widget(display, screen, window, gc, &styles, 40, 40, 280, 36);

    widget.draw();

    bool running = true;

    while (running)

    {

        XEvent event;

        XNextEvent(display, &event);

        bool handled = false;

        switch (event.type)

        {

        case Expose:

            handled = true;

            break;

        case ButtonPress:

            handled = widget.on_button_press(event.xbutton.x, event.xbutton.y, event.xbutton.button);

            break;

        case ButtonRelease:

            handled = widget.on_button_release(event.xbutton.x, event.xbutton.y, event.xbutton.button);

            break;

        case MotionNotify:

            handled = widget.on_motion(event.xmotion.x, event.xmotion.y);

            break;

        case KeyPress:

        {

            char buffer[32];

            KeySym key = 0;

            int n = XLookupString(&event.xkey, buffer, (int)sizeof(buffer) - 1, &key, nullptr);

            buffer[n] = 0;

            handled = widget.on_key(key, buffer);

            break;

        }

        case ClientMessage:

            if ((Atom)event.xclient.data.l[0] == deleteWindowMessage)

            {

                running = false;

            }

            break;

        }

        if (handled)

        {

            XClearWindow(display, window);

            widget.draw();

            XFlush(display);

        }

    }

    XCloseDisplay(display);

    return 0;

}
