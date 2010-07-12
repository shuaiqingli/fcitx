#ifndef _TRAY_WINDOW_H
#define _TRAY_WINDOW_H

#ifdef _ENABLE_TRAY

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <string.h>
#include <cairo.h>
#include <cairo-xlib.h>

#define INACTIVE_ICON 0
#define ACTIVE_ICON   1

typedef struct TrayWindow {
    Window window;

    XImage* icon[2];
    Pixmap picon[2];
    GC gc;
    Bool bTrayMapped;
    XVisualInfo visual;
    Atom atoms[6];

    cairo_surface_t *cs;
    int size;
} TrayWindow;

Bool CreateTrayWindow();
void DrawTrayWindow(int f_state, int x, int y, int w, int h);
void DeInitTrayWindow(TrayWindow *f_tray);
void RedrawTrayWindow(void);
void TrayEventHandler(XEvent* event);

extern TrayWindow tray;

#endif

#endif
