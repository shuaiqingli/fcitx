#ifndef _TRAY_H_
#define _TRAY_H_

#ifdef _ENABLE_TRAY

#include "TrayWindow.h"

int InitTray(Display* dpy, TrayWindow* win);
void TrayHandleClientMessage(Display *dpy, Window win, XEvent *an_event);
int TrayFindDock(Display *dpy, TrayWindow* tray);
XVisualInfo* TrayGetVisual(Display* dpy, TrayWindow* tray);
Window TrayGetDock(Display* dpy, TrayWindow* tray);
void TraySendOpcode( Display* dpy, Window dock,  TrayWindow* tray, long message,
        long data1, long data2, long data3 );

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define TRAY_ICON_WIDTH 22
#define TRAY_ICON_HEIGHT 22

#define ATOM_SELECTION 0
#define ATOM_MANAGER 1
#define ATOM_SYSTEM_TRAY_OPCODE 2
#define ATOM_ORIENTATION 3
#define ATOM_VISUAL 4

#endif

#endif
