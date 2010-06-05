/***************************************************************************
 *   Copyright (C) 2002~2005 by Yuking                                     *
 *   yuking_net@sohu.com                                                   *
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
/**
 * @file   MainWindow.c
 * @author Yuking yuking_net@sohu.com
 * @date   2008-1-16
 * 
 * @brief  主窗口
 * 
 * 
 */

#include "MainWindow.h"

#include <stdio.h>
#include <ctype.h>
#include <X11/xpm.h>
#include <X11/Xatom.h>

#include "core/IC.h"
#include "ui/ui.h"
#include "core/ime.h"
#include "tools/tools.h"

#include "ui/skin.h"
#include "ui/ui.h"
#include "im/special/vk.h"

Window          mainWindow = (Window) NULL;
int             MAINWND_WIDTH = _MAINWND_WIDTH;

int             iMainWindowX = MAINWND_STARTX;
int             iMainWindowY = MAINWND_STARTY;

HIDE_MAINWINDOW hideMainWindow = HM_SHOW;

Bool            bCompactMainWindow = False;
Bool            bShowVK = False;
Bool		bMainWindow_Hiden = False;

char           *strFullCorner = "全角模式";

extern Display *dpy;
extern Bool     bCorner;
extern Bool     bLocked;
extern Bool     bChnPunc;
extern INT8     iIMIndex;
extern Bool     bSP;
extern Bool     bUseLegend;
extern Bool     bVK;
extern IM      *im;
extern CARD16   connect_id;

extern Bool     bUseGBKT;

extern VKS      vks[];
extern unsigned char iCurrentVK;


Pixmap pm_main_bar;

Bool CreateMainWindow (void)
{
	int depth;
	Colormap cmap;
	Visual * vs;
	int scr;
	XSetWindowAttributes attrib;
    unsigned long	attribmask;
	GC gc;

	XGCValues xgv;
    
	scr=DefaultScreen(dpy);
   	vs=find_argb_visual (dpy, scr);
   	cmap = XCreateColormap (dpy, RootWindow(dpy, scr),vs, AllocNone);
   	
    attrib.override_redirect = True;//False;
	attrib.background_pixel = 0;
	attrib.border_pixel = 0;
	attrib.colormap =cmap;
	attribmask = (CWBackPixel|CWBorderPixel|CWOverrideRedirect |CWColormap);
	depth = 32;
	
	mainWindow=XCreateWindow (dpy, 
							RootWindow(dpy, scr),
							iMainWindowX,
							iMainWindowY,
							 MAINWND_WIDTH,
							 MAINWND_HEIGHT,
							 0, depth,InputOutput, vs,attribmask, &attrib);

	if(mainWindow == (Window)NULL)
		return False;
		
	xgv.foreground = WhitePixel(dpy, scr);
	//创建pixmap缓冲区,创建主cs
	pm_main_bar = XCreatePixmap(dpy,mainWindow, SIZEX, SIZEY, depth);
	gc = XCreateGC(dpy,pm_main_bar, GCForeground, &xgv);
	XFillRectangle(dpy, pm_main_bar, gc, 0, 0,SIZEX , SIZEY);	
	cs_main_bar=cairo_xlib_surface_create(dpy, pm_main_bar, vs, SIZEX, SIZEY);	
	//cs_main_bar=cairo_xlib_surface_create(dpy, mainWindow, vs, MAINWND_WIDTH, MAINWND_HEIGHT);
	load_main_img();
	
	XChangeWindowAttributes (dpy, mainWindow, attribmask, &attrib);	
	XSelectInput (dpy, mainWindow, ExposureMask | ButtonPressMask | ButtonReleaseMask  | PointerMotionMask);

	
    return True;
}

void DisplayMainWindow (void)
{
#ifdef _DEBUG
    fprintf (stderr, "DISPLAY MainWindow\n");
#endif
	
    if (!bMainWindow_Hiden)
	XMapRaised (dpy, mainWindow);
}

void DrawMainWindow (void)
{
    INT8            iIndex = 0;
	cairo_t *c;
	GC gc = XCreateGC( dpy, mainWindow, 0, NULL );
    if ( bMainWindow_Hiden )
    	return;
	
    iIndex = IS_CLOSED;

#ifdef _DEBUG
    fprintf (stderr, "DRAW MainWindow\n");
#endif
	XResizeWindow(dpy, mainWindow, skin_config.skin_main_bar.mbbg_img.width, skin_config.skin_main_bar.mbbg_img.height);

	c=cairo_create(cs_main_bar);
	//把背景清空
	cairo_set_source_rgba(c, 0, 0, 0,0);
	cairo_rectangle (c, 0, 0, SIZEX, SIZEY);
	cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
	cairo_fill(c);
	
	cairo_set_operator(c, CAIRO_OPERATOR_OVER);

	if (hideMainWindow == HM_SHOW || (hideMainWindow == HM_AUTO && (ConnectIDGetState (connect_id) != IS_CLOSED)))
    {	
		draw_a_img(&c, skin_config.skin_main_bar.mbbg_img,bar );
		draw_a_img(&c, skin_config.skin_main_bar.logo_img,logo);
		draw_a_img(&c, skin_config.skin_main_bar.zhpunc_img,punc[bChnPunc]);
		draw_a_img(&c, skin_config.skin_main_bar.chs_img,chs_t[bUseGBKT]);
		draw_a_img(&c, skin_config.skin_main_bar.half_corner_img,corner[bCorner]);
		draw_a_img(&c, skin_config.skin_main_bar.unlock_img,lock[bLocked]);
		draw_a_img(&c, skin_config.skin_main_bar.lxoff_img,lx[bUseLegend]);
		draw_a_img(&c, skin_config.skin_main_bar.vkhide_img,vk[bVK]);
		
		iIndex = ConnectIDGetState (connect_id);
		//printf("iIndex %d %d \n",iIndex,iIMIndex);
		if( iIndex == 1 || iIndex ==0 )
		{
			//英文
			draw_a_img(&c, skin_config.skin_main_bar.english_img,english);
		}
		else 
		{
			switch(iIMIndex)
			{
				case 0:
					//智能拼音
					draw_a_img(&c, skin_config.skin_main_bar.pinyin_img,pinyin);
					break;
				case 1:
					//智能双拼
					draw_a_img(&c, skin_config.skin_main_bar.shuangpin_img,shuangpin);
					break;
				case 2:			
					//区位输入
					draw_a_img(&c, skin_config.skin_main_bar.quwei_img,quwei);
					break;
				case 3:
					//五笔字型
					draw_a_img(&c, skin_config.skin_main_bar.wubi_img,wubi);
					break;
				case 4:
					//五笔拼音
					draw_a_img(&c, skin_config.skin_main_bar.mixpywb_img,mix);
					break;
				case 5:
					//二笔
					draw_a_img(&c, skin_config.skin_main_bar.erbi_img,erbi);
					break;
				case 6:
					//仓颉
					draw_a_img(&c, skin_config.skin_main_bar.cj_img,cangji);
					break;
				case 7:
					//晚风
					draw_a_img(&c, skin_config.skin_main_bar.wanfeng_img,wanfeng);
					break;
				case 8:
					//冰蟾全息
					draw_a_img(&c, skin_config.skin_main_bar.bingchan_img,bingchan);
					break;
				case 9:
					//自然码
					draw_a_img(&c, skin_config.skin_main_bar.ziran_img,ziran);
					break;
				default:
					//其他输入法
					draw_a_img(&c, skin_config.skin_main_bar.otherim_img,otherim);	
					break;
			}
		}
		XCopyArea (dpy, pm_main_bar, mainWindow, gc, 0, 0, skin_config.skin_main_bar.mbbg_img.width, skin_config.skin_main_bar.mbbg_img.height, 0, 0);	
    }
    else
		XUnmapWindow (dpy, mainWindow);
	
	XFreeGC (dpy, gc);
	cairo_destroy(c);
}

void InitMainWindowColor (void)
{
 
}
