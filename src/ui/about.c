/***************************************************************************
 *   Copyright (C) 2005 by Yunfan                                          *
 *   yunfan_zg@163.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ui/about.h"
#include "ui/ui.h"
#include "core/xim.h"
#include "version.h"

#include <ctype.h>

#include <iconv.h>
#include <X11/Xatom.h>

extern Display *dpy;
extern int      iScreen;

int             ABOUT_WINDOW_WIDTH;

Atom            about_protocol_atom = 0;
Atom            about_kill_atom = 0;

char            AboutCaption[] = "关于 - FCITX";
char            AboutTitle[] = "小企鹅中文输入法";
char            AboutEmail[] = "yuking_net@sohu.com";
char            AboutCopyRight[] = "(c) 2005, Yuking";
char            strTitle[100];

AboutWindow aboutWindow;

Bool CreateAboutWindow (void)
{
    strcpy (strTitle, AboutTitle);
    strcat (strTitle, " ");
    strcat (strTitle, FCITX_VERSION);

    aboutWindow.color.r = aboutWindow.color.g = aboutWindow.color.b = 220.0 / 256;
    aboutWindow.fontColor.r = aboutWindow.fontColor.g = aboutWindow.fontColor.b = 0;
    aboutWindow.fontSize = 11;

    ABOUT_WINDOW_WIDTH = StringWidth (strTitle, skin_config.skin_font.font_zh, aboutWindow.fontSize ) + 50;
    aboutWindow.window =
	XCreateSimpleWindow (dpy, DefaultRootWindow (dpy), (DisplayWidth (dpy, iScreen) - ABOUT_WINDOW_WIDTH) / 2, (DisplayHeight (dpy, iScreen) - ABOUT_WINDOW_HEIGHT) / 2, ABOUT_WINDOW_WIDTH, ABOUT_WINDOW_HEIGHT, 0, WhitePixel (dpy, DefaultScreen (dpy)), WhitePixel (dpy, DefaultScreen (dpy)));

    aboutWindow.surface = cairo_xlib_surface_create(dpy, aboutWindow.window, DefaultVisual(dpy, 0), ABOUT_WINDOW_WIDTH, ABOUT_WINDOW_HEIGHT); 
    if (aboutWindow.window == None)
	return False;

    InitWindowProperty ();
    XSelectInput (dpy, aboutWindow.window, ExposureMask | ButtonPressMask | ButtonReleaseMask  | PointerMotionMask );

    return True;
}

void InitWindowProperty (void)
{
    XTextProperty   tp;

    Atom            about_wm_window_type = XInternAtom (dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom            type_toolbar = XInternAtom (dpy, "_NET_WM_WINDOW_TYPE_TOOLBAR", False);

    XSetTransientForHint (dpy, aboutWindow.window, DefaultRootWindow (dpy));

    XChangeProperty (dpy, aboutWindow.window, about_wm_window_type, XA_ATOM, 32, PropModeReplace, (void *) &type_toolbar, 1);

    about_protocol_atom = XInternAtom (dpy, "WM_PROTOCOLS", False);
    about_kill_atom = XInternAtom (dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols (dpy, aboutWindow.window, &about_kill_atom, 1);

	char           *p;

	p = AboutCaption;

    tp.value = (void *) p;
    tp.encoding = XA_STRING;
    tp.format = 16;
    tp.nitems = strlen (p);
    XSetWMName (dpy, aboutWindow.window, &tp);
}

void DisplayAboutWindow (void)
{
    XMapRaised (dpy, aboutWindow.window);
    XMoveWindow (dpy, aboutWindow.window, (DisplayWidth (dpy, iScreen) - ABOUT_WINDOW_WIDTH) / 2, (DisplayHeight (dpy, iScreen) - ABOUT_WINDOW_HEIGHT) / 2);
}

void DrawAboutWindow (void)
{
    cairo_t *c = cairo_create(aboutWindow.surface);
    cairo_set_source_rgb(c, aboutWindow.color.r, aboutWindow.color.g, aboutWindow.color.b);
    cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
    cairo_paint(c);

    OutputString (c, strTitle, skin_config.skin_font.font_zh, aboutWindow.fontSize, (ABOUT_WINDOW_WIDTH - StringWidth (strTitle, skin_config.skin_font.font_zh, aboutWindow.fontSize)) / 2, aboutWindow.fontSize + 6 + 30, &aboutWindow.fontColor);

    OutputString (c, AboutEmail, skin_config.skin_font.font_zh, aboutWindow.fontSize, (ABOUT_WINDOW_WIDTH - StringWidth (AboutEmail, skin_config.skin_font.font_zh, aboutWindow.fontSize)) / 2, aboutWindow.fontSize + 6 + 60, &aboutWindow.fontColor);

    OutputString (c, AboutCopyRight, skin_config.skin_font.font_zh, aboutWindow.fontSize, (ABOUT_WINDOW_WIDTH - StringWidth (AboutCopyRight, skin_config.skin_font.font_zh, aboutWindow.fontSize)) / 2,aboutWindow.fontSize + 6 + 80, &aboutWindow.fontColor);

    cairo_destroy(c);
}

