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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ui/ui.h"

#include <X11/Xlocale.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <iconv.h>

#include "core/xim.h"
#include "core/MyErrorsHandlers.h"
#include "ui/MainWindow.h"
#include "ui/InputWindow.h"
#include "ui/MenuWindow.h"
#include "im/special/vk.h"
#include "core/IC.h"
#include "tools/tools.h"
#include "im/pinyin/sp.h"
#include "ui/about.h"
#include "ui/TrayWindow.h"
#include "ui/skin.h"
#include "core/ime.h"

Display        *dpy;
int             iScreen;

GC              dimGC = (GC) NULL;
GC              lightGC = (GC) NULL;

extern Window   mainWindow;
extern int      iMainWindowX;
extern int      iMainWindowY;
extern Window   inputWindow;
extern int      iInputWindowX;
extern int      iInputWindowY;
extern int      iVKWindowX;
extern int      iVKWindowY;
extern Window   ximWindow;
extern VKWindow vkWindow;
extern WINDOW_COLOR mainWindowColor;
extern WINDOW_COLOR inputWindowColor;
extern WINDOW_COLOR VKWindowColor;
extern HIDE_MAINWINDOW hideMainWindow;
extern Bool bMainWindow_Hiden;
extern Bool     bCompactMainWindow;
extern INT8     iIMIndex;
unsigned char   iCurrentVK ;
extern CARD16   connect_id;
extern Bool     bShowVK;
extern Bool     bVK;
extern Bool     bCorner;
extern Bool     bIsDisplaying;

#ifdef _ENABLE_TRAY
extern TrayWindow tray;
#endif

//added by yunfan
extern Window   aboutWindow;
extern Atom     about_protocol_atom;
extern Atom     about_kill_atom;
//**********************************

char strUserLocale[50] = "zh_CN.UTF-8";

Bool InitX (void)
{
    if ((dpy = XOpenDisplay ((char *) NULL)) == NULL) {
    fprintf (stderr, "Error: FCITX can only run under X\n");
    return False;
    }

    SetMyXErrorHandler ();
    iScreen = DefaultScreen (dpy);

    return True;
}

void InitGC (Window window)
{
    XGCValues       values;
    XColor          color;
    int             iPixel;

    if (lightGC)
    XFreeGC (dpy, lightGC);
    lightGC = XCreateGC (dpy, window, 0, &values);
    color.red = LIGHT_COLOR;
    color.green = LIGHT_COLOR;
    color.blue = LIGHT_COLOR;
    if (XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &color))
    iPixel = color.pixel;
    else
    iPixel = WhitePixel (dpy, DefaultScreen (dpy));
    XSetForeground (dpy, lightGC, iPixel);

    if (dimGC)
    XFreeGC (dpy, dimGC);
    dimGC = XCreateGC (dpy, window, 0, &values);
    color.red = DIM_COLOR;
    color.green = DIM_COLOR;
    color.blue = DIM_COLOR;
    if (XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &color))
    iPixel = color.pixel;
    else
    iPixel = BlackPixel (dpy, DefaultScreen (dpy));
    XSetForeground (dpy, dimGC, iPixel);

/*    if (mainWindowColor.backGC)
    XFreeGC (dpy, mainWindowColor.backGC);
    mainWindowColor.backGC = XCreateGC (dpy, window, 0, &values);
    if (XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &(mainWindowColor.backColor)))
    iPixel = mainWindowColor.backColor.pixel;
    else
    iPixel = WhitePixel (dpy, DefaultScreen (dpy));
    XSetForeground (dpy, mainWindowColor.backGC, iPixel);

    if (inputWindowColor.foreGC)
    XFreeGC (dpy, inputWindowColor.foreGC);
    inputWindowColor.foreGC = XCreateGC (dpy, window, 0, &values);
    if (XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &(inputWindowColor.foreColor)))
    iPixel = inputWindowColor.foreColor.pixel;
    else
    iPixel = BlackPixel (dpy, DefaultScreen (dpy));
    XSetForeground (dpy, inputWindowColor.foreGC, iPixel);

    if (inputWindowColor.backGC)
    XFreeGC (dpy, inputWindowColor.backGC);
    inputWindowColor.backGC = XCreateGC (dpy, window, 0, &values);
    if (XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &(inputWindowColor.backColor)))
    iPixel = inputWindowColor.backColor.pixel;
    else
    iPixel = BlackPixel (dpy, DefaultScreen (dpy));
    XSetForeground (dpy, inputWindowColor.backGC, iPixel);
*/
}

/*
 * 让指定的区域显示三维效果
 * effect:
 *  _3D_UPPER:显示为凸出效果
 *  _3D_LOWER:显示为凹下效果
 */
void Draw3DEffect (Window window, int x, int y, int width, int height, _3D_EFFECT effect)
{
    if (effect == _3D_UPPER) {
    XDrawLine (dpy, window, lightGC, x, y, x + width - 1, y);
    XDrawLine (dpy, window, lightGC, x, y, x, y + height - 1);
    XDrawLine (dpy, window, dimGC, x + width - 1, y, x + width - 1, y + height - 1);
    XDrawLine (dpy, window, dimGC, x, y + height - 1, x + width - 1, y + height - 1);
    }
    else if (effect == _3D_LOWER) {
    XDrawLine (dpy, window, lightGC, x, y, x + width - 1, y);
    XDrawLine (dpy, window, lightGC, x, y, x, y + height - 1);
    XDrawLine (dpy, window, dimGC, x + width - 1, y, x + width - 1, y + height - 1);
    XDrawLine (dpy, window, dimGC, x, y + height - 1, x + width - 1, y + height - 1);

    XDrawLine (dpy, window, dimGC, x + 1, y + 1, x + width - 2, y + 1);
    XDrawLine (dpy, window, dimGC, x + 1, y + 1, x + 1, y + height - 2);
    XDrawLine (dpy, window, lightGC, x + 1, y + height - 2, x + width - 2, y + height - 2);
    XDrawLine (dpy, window, lightGC, x + width - 2, y + 1, x + width - 2, y + height - 2);
    }
    else {          //   _3D_FLAT
    XDrawRectangle (dpy, window, dimGC, x, y, width - 1, height - 1);
    return;
    }
}

/*
 * 有关界面的消息都在这里处理
 *     有关tray重画的问题，此处的解决方案似乎很dirt
 */
void MyXEventHandler (XEvent * event)
{
   // unsigned char   iPos;
    
    switch (event->type) {
    case ConfigureNotify:
#ifdef _ENABLE_TRAY
        TrayEventHandler(event);
#endif
        break;
    case ReparentNotify:
#ifdef _ENABLE_TRAY
        TrayEventHandler(event);
#endif
        break;
    case ClientMessage:
    if ((event->xclient.message_type == about_protocol_atom) && ((Atom) event->xclient.data.l[0] == about_kill_atom)) {
        XUnmapWindow (dpy, aboutWindow);
        DrawMainWindow ();
    }
#ifdef _ENABLE_TRAY 
    TrayEventHandler(event);
#endif
    break;
    case Expose:
#ifdef _DEBUG
    fprintf (stderr, "XEvent--Expose\n");
#endif
    if (event->xexpose.count > 0)
        break;
    if (event->xexpose.window == mainWindow)
        DrawMainWindow ();
    else if (event->xexpose.window == vkWindow.window)
        DrawVKWindow ();
    else if (event->xexpose.window == inputWindow)
        DrawInputWindow ();
	else if (event->xexpose.window == mainMenu.menuWindow)
	    DrawXlibMenu(dpy,&mainMenu);
	else if (event->xexpose.window == vkMenu.menuWindow)
	{
		if(iCurrentVK>=0)
			vkMenu.mark=iCurrentVK;
		DrawXlibMenu(dpy,&vkMenu);
	}  
	else if (event->xexpose.window == imMenu.menuWindow)
	{
		if(iIMIndex >=0)
			imMenu.mark=iIMIndex;
	    DrawXlibMenu(dpy,&imMenu);
	}
	else if (event->xexpose.window == skinMenu.menuWindow)
	{
		int i;
		for(i=0;i<12;i++)
		{
			if(strcmp(skinType,skinBuf[i].dirbase) == 0)
				skinMenu.mark=i;
		}
	    DrawXlibMenu(dpy,&skinMenu);
	}
#ifdef _ENABLE_TRAY
    else if (event->xexpose.window == tray.window) {
        TrayEventHandler(event);
    }
#endif
    //added by yunfan
    else if (event->xexpose.window == aboutWindow)
        DrawAboutWindow ();
    //******************************
    break;
    case DestroyNotify:
#ifdef _ENABLE_TRAY 
    TrayEventHandler(event);
#endif
    break;
    case ButtonPress:
    switch (event->xbutton.button) {
    case Button1:
	    set_mouse_status(RELEASE);
        if (event->xbutton.window == inputWindow)
        {
        	int x, y;
        	x = event->xbutton.x;
        	y = event->xbutton.y;
        	MouseClick (&x, &y, inputWindow);
        	DrawInputWindow ();
        }
        else if (event->xbutton.window == mainWindow)
        {

			if (IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.logo_img))
			{
		  		iMainWindowX = event->xbutton.x;
		    	iMainWindowY = event->xbutton.y;
		    	ms_logo=PRESS;
		    	if (!MouseClick (&iMainWindowX, &iMainWindowY, mainWindow))
		    	{
					if (ConnectIDGetState (connect_id) != IS_CHN) {
			   				SetIMState ((ConnectIDGetState (connect_id) == IS_ENG) ? False : True);	
					}
				}
				DrawMainWindow ();
				
#ifdef _ENABLE_TRAY
	            if (ConnectIDGetState (connect_id) == IS_CHN)
	                DrawTrayWindow (ACTIVE_ICON, 0, 0, tray.size, tray.size );
	            else
	                DrawTrayWindow (INACTIVE_ICON, 0, 0, tray.size, tray.size );
#endif
            }
            else if (IsInRspArea(event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.zhpunc_img)
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.enpunc_img))
			{
				ms_punc=PRESS;
			}
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.half_corner_img)
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.full_corner_img) )
			{
				ms_corner=PRESS;
			}
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.lxoff_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.lxon_img ) )
			{
				ms_lx=PRESS;
			}
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.chs_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.cht_img ) )
			{
				ms_chs=PRESS;
			}
			else if (IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.unlock_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.lock_img ) )
		  	{
		  		ms_lock=PRESS;
		  	}
			else if (IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.vkhide_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.vkshow_img ) )
			{
				ms_vk=PRESS;
			}
			else if (!bCorner && IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.pinyin_img ))
			{		
				ms_py=PRESS;
			}
			DrawMainWindow ();
			set_mouse_status(RELEASE);
        }
       	else if(event->xbutton.window == tray.window)
       	{
			SetIMState ((ConnectIDGetState (connect_id) == IS_ENG) ? False : True);
			DrawMainWindow ();   
			  	
#ifdef _ENABLE_TRAY
			if (ConnectIDGetState (connect_id) == IS_CHN)
				DrawTrayWindow (ACTIVE_ICON, 0, 0, tray.size, tray.size );
			else
				DrawTrayWindow (INACTIVE_ICON, 0, 0, tray.size, tray.size );
#endif      	
       	
       	}
        else if (event->xbutton.window == vkWindow.window) 
        {
        	if (!VKMouseKey (event->xbutton.x, event->xbutton.y))
        	{
        	    iVKWindowX = event->xbutton.x;
        	    iVKWindowY = event->xbutton.y;
        	    MouseClick (&iVKWindowX, &iVKWindowY, vkWindow.window);
        	    DrawVKWindow ();
        	}
        }
        //added by yunfan
        else if (event->xbutton.window == aboutWindow)
        {
        	XUnmapWindow (dpy, aboutWindow);
        	DrawMainWindow ();
        }
        else if(event->xbutton.window == mainMenu.menuWindow)
		{
			int i;
			i=selectShellIndex(&mainMenu,event->xbutton.y);
			
			if(i==0)
			{	
				DisplayAboutWindow ();
				XUnmapWindow (dpy, mainMenu.menuWindow);
				DrawMainWindow ();
			}
			//fcitx配置工具接口
			else if(i==6)
			{
				 XUnmapWindow (dpy, inputWindow);
				 XUnmapWindow (dpy, mainMenu.menuWindow);
				//用一个标志位检测fcitx配置程序是否在运行，如果在运行，则跳过，否则可以同时运行多个fcitx_tools程序，会有共享内存方面的bug
				// if( fcitx_tools_status == 0)
				// 	fcitx_tools_interface();	
			}
			else if(i==7)
			{
				clearSelectFlag(&mainMenu);
				XUnmapWindow (dpy, mainMenu.menuWindow);
				XUnmapWindow (dpy, vkMenu.menuWindow);
				XUnmapWindow (dpy, imMenu.menuWindow);
				XUnmapWindow (dpy, skinMenu.menuWindow);
			
			}
	    }
		else if (event->xbutton.window == imMenu.menuWindow) 
		{
			SelectIM(selectShellIndex(&imMenu,event->xbutton.y));
			clearSelectFlag(&mainMenu);
			XUnmapWindow (dpy, imMenu.menuWindow);
			XUnmapWindow (dpy, mainMenu.menuWindow);
	
		}
		else if (event->xbutton.window == skinMenu.menuWindow)
		{
			//皮肤切换在此进行
			int i;
			i=selectShellIndex(&skinMenu,event->xbutton.y);
			//
			if(strcmp(skinType,skinBuf[i].dirbase) !=0 )
			{
				clearSelectFlag(&mainMenu);
				XUnmapWindow (dpy, mainMenu.menuWindow);
				XUnmapWindow (dpy, vkMenu.menuWindow);
				XUnmapWindow (dpy, imMenu.menuWindow);
				XUnmapWindow (dpy, skinMenu.menuWindow);
				DisplaySkin(skinBuf[i].dirbase);	
				SaveConfig();
			}
		}
		else if (event->xbutton.window == vkMenu.menuWindow) {
			SelectVK(selectShellIndex(&vkMenu,event->xbutton.y));
			clearSelectFlag(&mainMenu);
			XUnmapWindow (dpy, mainMenu.menuWindow);
			XUnmapWindow (dpy, vkMenu.menuWindow);
		}
        //****************************
        SaveProfile ();
        break;

		case Button3:
		
		if(event->xbutton.window == mainMenu.menuWindow)
		{
			clearSelectFlag(&mainMenu);
			XUnmapWindow (dpy, mainMenu.menuWindow);
			XUnmapWindow (dpy, vkMenu.menuWindow);
			XUnmapWindow (dpy, imMenu.menuWindow);
			XUnmapWindow (dpy, skinMenu.menuWindow);
		}
#ifdef _ENABLE_TRAY	
		else if(event->xbutton.window == tray.window)
		{
			
			loadSkinDir();

			GetMenuHeight(dpy,&mainMenu);
			
			if( event->xbutton.x_root-event->xbutton.x+mainMenu.width >=DisplayWidth(dpy, iScreen) )
				mainMenu.pos_x=DisplayWidth(dpy, iScreen)-mainMenu.width-event->xbutton.x;	
			else			
				mainMenu.pos_x=event->xbutton.x_root-event->xbutton.x;
			
			//面板的高度是可以变动的，需要取得准确的面板高度，才能准确确定右键菜单位置。
			if( event->xbutton.y_root+mainMenu.height-event->xbutton.y >=DisplayHeight(dpy, iScreen) )
				mainMenu.pos_y=DisplayHeight(dpy, iScreen)-mainMenu.height-event->xbutton.y-15;	
			else			
				mainMenu.pos_y=event->xbutton.y_root-event->xbutton.y+25;//+skin_config.skin_tray_icon.active_img.height;
				
			/*printf("%d %d :%d %d :%d \n",event->xbutton.x,event->xbutton.y,event->xbutton.x_root,\
								event->xbutton.y_root,skin_config.skin_tray_icon.active_img.height);*/
			DrawXlibMenu( dpy,&mainMenu);
			DisplayXlibMenu(dpy,&mainMenu);		
		}
#endif	
		else if (event->xbutton.window == mainWindow )
		 {
		 	loadSkinDir();

			GetMenuHeight(dpy,&mainMenu);
				
			mainMenu.pos_x = iMainWindowX;
			mainMenu.pos_y = iMainWindowY+skin_config.skin_main_bar.mbbg_img.height+5;
			if( (mainMenu.pos_y+mainMenu.height) > DisplayHeight(dpy, iScreen) )
					mainMenu.pos_y= iMainWindowY-5-mainMenu.height;
			
			DrawXlibMenu( dpy,&mainMenu);
			DisplayXlibMenu(dpy,&mainMenu);
		}
		break;
    }
    break;
    case ButtonRelease:
    if (event->xbutton.window == mainWindow)
    {
        switch (event->xbutton.button)
        {
        case Button1:  		
			set_mouse_status(RELEASE);
			if(IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.logo_img))
				DrawMainWindow();
		    else if (IsInRspArea(event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.zhpunc_img)
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.enpunc_img))
						ChangePunc ();
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.half_corner_img)
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.full_corner_img) )
						ChangeCorner ();
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.lxoff_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.lxon_img ) )
						ChangeLegend ();
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.chs_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.cht_img ) )
						ChangeGBKT ();
			else if (IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.unlock_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.lock_img ) )
		  				ChangeLock ();
			else if (IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.vkhide_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.vkshow_img ) )
			   			SwitchVK ();
			else if (!bCorner && IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.pinyin_img ))
			{		
				SwitchIM (-1);
			}

		break;

        //added by yunfan
        case Button2:
        DisplayAboutWindow ();
        break;
        //********************
        case Button3:

#ifdef _ENABLE_TRAY
 		 if (event->xbutton.window == tray.window) {
        switch (event->xbutton.button) {
        case Button1:
        if (ConnectIDGetState (connect_id) != IS_CHN) {
            SetIMState (True);
            DrawMainWindow ();

            DrawTrayWindow (ACTIVE_ICON, 0, 0, tray.size, tray.size );
        }
        else {
            SetIMState (False);
            DrawTrayWindow (INACTIVE_ICON, 0, 0, tray.size, tray.size );
            }

        break;
        case Button2:
        if (IsWindowVisible(mainWindow)) {
            bMainWindow_Hiden = True;
            XUnmapWindow(dpy,mainWindow);
        }
        else {
            bMainWindow_Hiden = False;
            DisplayMainWindow();
            DrawMainWindow();
        }
        break;
        }
    }
    }
    }
#endif
    break;
    case FocusIn:
    if (ConnectIDGetState (connect_id) == IS_CHN)
        DisplayInputWindow ();
    if (hideMainWindow != HM_HIDE)
        XMapRaised (dpy, mainWindow);
    break;
    default:
    break;

	case MotionNotify:
		if(event->xany.window == mainWindow)
		{
			if(IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.logo_img))
			{
				ms_logo=MOTION;
			}
			else if (IsInRspArea(event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.zhpunc_img)
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.enpunc_img))
			{
				ms_punc=MOTION;
			}
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.half_corner_img)
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.full_corner_img) )
			{
				ms_corner=MOTION;
			}
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.lxoff_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.lxon_img ) )
			{
				ms_lx=MOTION;
			}
		    else if (IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.chs_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.cht_img ) )
			{
				ms_chs=MOTION;
			}
			else if (IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.unlock_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.lock_img ) )
		  	{
		  		ms_lock=MOTION;
		  	}
			else if (IsInRspArea (event->xbutton.x, event->xbutton.y, skin_config.skin_main_bar.vkhide_img )
						||IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.vkshow_img ) )
			{
				ms_vk=MOTION;
			}
			else if (!bCorner && IsInRspArea (event->xbutton.x, event->xbutton.y,skin_config.skin_main_bar.pinyin_img ))
			{		
				ms_py=MOTION;
			}
			DrawMainWindow ();
			set_mouse_status(RELEASE);
		}
		else if(event->xany.window == mainMenu.menuWindow)
		{	
			MainMenuEvent(event->xmotion.x,event->xmotion.y);
		}
		else if(event->xany.window == imMenu.menuWindow)	
		{	
			IMMenuEvent(event->xmotion.x,event->xmotion.y);
		}	
		else if(event->xany.window == vkMenu.menuWindow)	
		{	
			VKMenuEvent(event->xmotion.x,event->xmotion.y);
		}	
		else if(event->xany.window == skinMenu.menuWindow)	
		{	
			SkinMenuEvent(event->xmotion.x,event->xmotion.y);
		}	
					break;
    }
}

/*
 * 判断鼠标点击处是否处于指定的区域内
 */
Bool IsInBox (int x0, int y0, int x1, int y1, int x2, int y2)
{
    if (x0 >= x1 && x0 <= x2 && y0 >= y1 && y0 <= y2)
    return True;

    return False;
}

Bool IsInRspArea(int x0,int y0,skin_img_t img)
{
	return IsInBox (x0,y0,img.response_x , img.response_y, img.response_x+img.response_w, img.response_y+img.response_h);
}

int StringWidth (char *str, int fontSize)
{
    cairo_text_extents_t extents;
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 10, 10);
    cairo_t* c = cairo_create(surface);
    cairo_select_font_face(c, skin_config.skin_font.font_zh, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(c, fontSize);

    cairo_text_extents(c, str, &extents);

    int width = extents.width;

	cairo_destroy(c);
    cairo_surface_destroy(surface);

    return width;
}

int FontHeight ()
{
    char            str[] = "Ay中";
    cairo_text_extents_t extents;
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 10, 10);
    cairo_t* c = cairo_create(surface);
    cairo_select_font_face(c, skin_config.skin_font.font_zh, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(c, skin_config.skin_font.font_size);

    cairo_text_extents(c, str, &extents);

    int height = extents.height;

	cairo_destroy(c);
    cairo_surface_destroy(surface);
    return height;
}

/*
 * 以指定的颜色在窗口的指定位置输出字串
 */
void OutputString (cairo_t* c, char *str, int fontSize, int x, int y, cairo_color_t* color)
{
    if (!str)
    return;

    cairo_save(c);
    
    cairo_set_source_rgb(c, color->r, color->g, color->b);
    cairo_select_font_face(c, skin_config.skin_font.font_zh, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(c, fontSize);
    cairo_move_to(c, x, y);
    cairo_show_text(c, str);

    cairo_restore(c);
}

Bool IsWindowVisible(Window window)
{
    XWindowAttributes attrs;

    XGetWindowAttributes( dpy, window, &attrs );

    if ( attrs.map_state==IsUnmapped)
    return False;

    return True;
}

Bool MouseClick (int *x, int *y, Window window)
{
    XEvent          evtGrabbed;
    Bool            bMoved = False;

    // To motion the  window
    while (1) {
    XMaskEvent (dpy, PointerMotionMask | ButtonReleaseMask | ButtonPressMask, &evtGrabbed);
    if (ButtonRelease == evtGrabbed.xany.type) {
        if (Button1 == evtGrabbed.xbutton.button)
        break;
    }
    else if (MotionNotify == evtGrabbed.xany.type) {
        static Time     LastTime;

        if (evtGrabbed.xmotion.time - LastTime < 20)
        continue;

        XMoveWindow (dpy, window, evtGrabbed.xmotion.x_root - *x, evtGrabbed.xmotion.y_root - *y);
        XRaiseWindow (dpy, window);

        bMoved = True;
        LastTime = evtGrabbed.xmotion.time;
    }
    }

    *x = evtGrabbed.xmotion.x_root - *x;
    *y = evtGrabbed.xmotion.y_root - *y;

    return bMoved;
}