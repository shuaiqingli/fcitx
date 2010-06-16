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

#include <stdio.h>
#include <ctype.h>
#include <X11/xpm.h>
#include <X11/Xatom.h>
#include "ui/skin.h"
#include "MainWindow.h"

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
extern Bool     bSP;
extern Bool     bUseLegend;
extern Bool     bVK;
extern CARD16   connect_id;

extern Bool     bUseGBKT;

extern VKS      vks[];
extern unsigned char iCurrentVK;

GC main_win_gc;
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

	load_main_img();
	    
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
							 skin_config.skin_main_bar.mbbg_img.width, 
							 skin_config.skin_main_bar.mbbg_img.height,
							 0, depth,InputOutput, vs,attribmask, &attrib);

	if(mainWindow == (Window)NULL)
		return False;
		
	xgv.foreground = WhitePixel(dpy, scr);
	//创建pixmap缓冲区,创建主cs
	pm_main_bar = XCreatePixmap(dpy,mainWindow, skin_config.skin_main_bar.mbbg_img.width, skin_config.skin_main_bar.mbbg_img.height, depth);
	gc = XCreateGC(dpy,pm_main_bar, GCForeground, &xgv);
	XFillRectangle(dpy, pm_main_bar, gc, 0, 0,skin_config.skin_main_bar.mbbg_img.width, skin_config.skin_main_bar.mbbg_img.height);	
	cs_main_bar=cairo_xlib_surface_create(dpy, pm_main_bar, vs, skin_config.skin_main_bar.mbbg_img.width, skin_config.skin_main_bar.mbbg_img.height);	
	//cs_main_bar=cairo_xlib_surface_create(dpy, mainWindow, vs, MAINWND_WIDTH, MAINWND_HEIGHT);

	main_win_gc = XCreateGC( dpy, mainWindow, 0, NULL );
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
	Bool btmpPunc;
	char tmpstr[64]={0};

    if ( bMainWindow_Hiden )
    	return;
	
    iIndex = IS_CLOSED;

	//中英标点符号咋就反了?修正	
	btmpPunc=bChnPunc?False:True;
#ifdef _DEBUG
    fprintf (stderr, "DRAW MainWindow\n");
#endif
	//XResizeWindow(dpy, mainWindow, skin_config.skin_main_bar.mbbg_img.width, skin_config.skin_main_bar.mbbg_img.height);

	c=cairo_create(cs_main_bar);
	//把背景清空
	cairo_set_source_rgba(c, 0, 0, 0,0);
	cairo_rectangle (c, 0, 0, SIZEX, SIZEY);
	cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
	cairo_fill(c);
	
	cairo_set_operator(c, CAIRO_OPERATOR_OVER);

	if (hideMainWindow == HM_SHOW || (hideMainWindow == HM_AUTO && (ConnectIDGetState (connect_id) != IS_CLOSED)))
    {	
   // extern mouse_e ms_logo,ms_punc,ms_corner,ms_lx,ms_chs,ms_lock,ms_vk,ms_py;
		draw_a_img(&c, skin_config.skin_main_bar.mbbg_img,bar,RELEASE );
		draw_a_img(&c, skin_config.skin_main_bar.logo_img,logo,ms_logo);
		draw_a_img(&c, skin_config.skin_main_bar.zhpunc_img,punc[btmpPunc],ms_punc);
		draw_a_img(&c, skin_config.skin_main_bar.chs_img,chs_t[bUseGBKT],ms_chs);
		draw_a_img(&c, skin_config.skin_main_bar.half_corner_img,corner[bCorner],ms_corner);
		draw_a_img(&c, skin_config.skin_main_bar.unlock_img,lock[bLocked],ms_lock);
		draw_a_img(&c, skin_config.skin_main_bar.lxoff_img,lx[bUseLegend],ms_lx);
		draw_a_img(&c, skin_config.skin_main_bar.vkhide_img,vk[bVK],ms_vk);
		
		iIndex = ConnectIDGetState (connect_id);
		//printf("[iIndex:%d iIMIndex:%d imName:%s]\n",iIndex,iIMIndex,im[iIMIndex].strName);
		if( iIndex == 1 || iIndex ==0 )
		{
			//英文
			draw_a_img(&c, skin_config.skin_main_bar.english_img,english,ms_py);
		}
		else 
		{
			strcpy(tmpstr,im[iIMIndex].strName);
			
			//默认码表显示
			if     ( strcmp(tmpstr,"智能拼音") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.pinyin_img,pinyin,ms_py);
			else if( strcmp(tmpstr,"智能双拼") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.shuangpin_img,shuangpin,ms_py);
			else if( strcmp(tmpstr,"区位") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.quwei_img,quwei,ms_py);
			else if( strcmp(tmpstr,"五笔字型") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.wubi_img,wubi,ms_py);
			else if( strcmp(tmpstr,"五笔拼音") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.mixpywb_img,mix,ms_py);					
			else if( strcmp(tmpstr,"二笔") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.erbi_img,erbi,ms_py);					
			else if( strcmp(tmpstr,"仓颉") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.cj_img,cangji,ms_py);					
			else if( strcmp(tmpstr,"晚风") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.wanfeng_img,wanfeng,ms_py);					
			else if( strcmp(tmpstr,"冰蟾全息") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.bingchan_img,bingchan,ms_py);	
			else if( strcmp(tmpstr,"自然码") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.ziran_img,ziran,ms_py);
			else if( strcmp(tmpstr,"电报码") == 0)
				draw_a_img(&c, skin_config.skin_main_bar.dianbao_img,dianbao,ms_py);
			else
			{	//如果非默认码表的内容,则临时加载png文件.
				//暂时先不能自定义码表图片.其他码表统一用一种图片。
				//printf("[%s]\n",tmpstr);
				draw_a_img(&c, skin_config.skin_main_bar.otherim_img,otherim,ms_py);
			}
																		
		}
		XCopyArea (dpy, pm_main_bar, mainWindow, main_win_gc, 0, 0, skin_config.skin_main_bar.mbbg_img.width,\
		 															skin_config.skin_main_bar.mbbg_img.height, 0, 0);		
    }
    else
		XUnmapWindow (dpy, mainWindow);
	
	cairo_destroy(c);
}

void InitMainWindowColor (void)
{
 
}
