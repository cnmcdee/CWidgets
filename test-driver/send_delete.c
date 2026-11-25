#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdlib.h>
int main(int argc, char**argv){
  Display*d=XOpenDisplay(NULL); if(!d) return 1;
  Window w=(Window)strtoul(argv[1],NULL,0);
  Atom wm=XInternAtom(d,"WM_DELETE_WINDOW",False);
  XEvent e; e.xclient.type=ClientMessage; e.xclient.window=w;
  e.xclient.message_type=XInternAtom(d,"WM_PROTOCOLS",False);
  e.xclient.format=32; e.xclient.data.l[0]=wm; e.xclient.data.l[1]=CurrentTime;
  XSendEvent(d,w,False,NoEventMask,&e); XFlush(d); XCloseDisplay(d); return 0;
}
