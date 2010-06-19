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

#include <string.h>
#include <ctype.h>
#include "version.h"
#include <time.h>
#include <X11/xpm.h>
#include <X11/Xatom.h>

#include "ui/InputWindow.h"
#include "ui/ui.h"
#include "core/ime.h"

#include "tools/tools.h"

#include "interface/DBus.h"
#include "skin.h"

Window          inputWindow;
int             iInputWindowX = INPUTWND_STARTX;
int             iInputWindowY = INPUTWND_STARTY;

uint            iInputWindowHeight = INPUTWND_HEIGHT;
int		iFixedInputWindowWidth = 0;
uint            iInputWindowWidth = INPUTWND_WIDTH;
uint            iInputWindowUpWidth = INPUTWND_WIDTH;
uint            iInputWindowDownWidth = INPUTWND_WIDTH;

uint            INPUTWND_START_POS_UP = 8;

// *************************************************************
MESSAGE         messageUp[32];	//输入条上部分显示的内容
uint            uMessageUp = 0;

// *************************************************************
MESSAGE         messageDown[32];	//输入条下部分显示的内容
uint            uMessageDown = 0;

XImage         *pNext = NULL, *pPrev = NULL;

Bool            bShowPrev = False;
Bool            bShowNext = False;
Bool            bTrackCursor = True;
Bool            bCenterInputWindow = True;
Bool            bShowInputWindowTriggering = True;
Bool		bIsDisplaying = False;

int             iCursorPos = 0;
Bool            bShowCursor = False;

_3D_EFFECT      _3DEffectInputWindow = _3D_LOWER;

//这两个变量是GTK+ OverTheSpot光标跟随的临时解决方案
/* Issue 11: piaoairy: 为适应generic_config_integer(), 改INT8 为int */
int		iOffsetX = 0;
int		iOffsetY = 16;

extern Display *dpy;

extern Bool     bUseGBKT;
extern int iScreen;
//计算速度
extern Bool     bStartRecordType;
extern Bool     bShowUserSpeed;
extern Bool     bShowVersion;
extern time_t   timeStart;
extern uint     iHZInputed;
extern Bool	bUseDBus;

extern CARD16	connect_id;

extern int	iClientCursorX;
extern int	iClientCursorY;

#ifdef _DEBUG
extern char     strUserLocale[];
extern char     strXModifiers[];
#endif

#ifdef _ENABLE_RECORDING
extern FILE	*fpRecord;
extern Bool	bRecording;
#endif

Pixmap pm_input_bar;

Bool CreateInputWindow (void)
{
    XSetWindowAttributes	attrib;
    unsigned long	attribmask;
    XTextProperty	tp;
    char		strWindowName[]="Fcitx Input Window";
	int depth;
	Colormap cmap;
	Visual * vs;
	int scr;
	GC gc;
	XGCValues xgv;
	
	load_input_img();
    CalculateInputWindowHeight ();   
	scr=DefaultScreen(dpy);
   	vs=find_argb_visual (dpy, scr);
   	cmap = XCreateColormap (dpy, RootWindow(dpy, scr),vs, AllocNone);
   	
    attrib.override_redirect = True;//False;
	attrib.background_pixel = 0;
	attrib.border_pixel = 0;
	attrib.colormap =cmap;
	attribmask = (CWBackPixel|CWBorderPixel|CWOverrideRedirect |CWColormap); 
	depth = 32;
	
	inputWindow=XCreateWindow (dpy, RootWindow(dpy, scr),iInputWindowX, iInputWindowY, INPUT_BAR_MAX_LEN, iInputWindowHeight, 0, depth,InputOutput, vs,attribmask, &attrib);

	if(mainWindow == (Window)NULL)
		return False;
		
	xgv.foreground = WhitePixel(dpy, scr);

	pm_input_bar=XCreatePixmap(dpy,inputWindow, INPUT_BAR_MAX_LEN, iInputWindowHeight, depth);
	gc = XCreateGC(dpy,pm_input_bar, GCForeground, &xgv);
	XFillRectangle(dpy, pm_input_bar, gc, 0, 0,INPUT_BAR_MAX_LEN, iInputWindowHeight);
	cs_input_bar=cairo_xlib_surface_create(dpy, pm_input_bar, vs, INPUT_BAR_MAX_LEN, iInputWindowHeight);
	
	load_input_msg();
    XSelectInput (dpy, inputWindow, ButtonPressMask | ButtonReleaseMask  | PointerMotionMask | ExposureMask);

    //Set the name of the window
    tp.value = (void *)strWindowName;
    tp.encoding = XA_STRING;
    tp.format = 16;
    tp.nitems = strlen(strWindowName);
    XSetWMName (dpy, inputWindow, &tp);

    return True;
}

/*
 * 根据字体的大小来调整窗口的高度
 */
void CalculateInputWindowHeight (void)
{
	iInputWindowHeight=skin_config.skin_input_bar.ibbg_img.height;
	//printf("iInputWindowHeight:%d \n",iInputWindowHeight);
}

void DisplayInputWindow (void)
{
#ifdef _DEBUG
    fprintf (stderr, "DISPLAY InputWindow\n");
#endif
    CalInputWindow();
    MoveInputWindow(connect_id);
    if (uMessageUp || uMessageDown)
	{
		if (!bUseDBus)
			XMapRaised (dpy, inputWindow);
#ifdef _ENABLE_DBUS
		else
			updateMessages();
#endif
	}
}

void InitInputWindowColor (void)
{

}

void ResetInputWindow (void)
{
    uMessageDown = 0;
    uMessageUp = 0;
}

void CalInputWindow (void)
{

#ifdef _DEBUG
    fprintf (stderr, "CAL InputWindow\n");
#endif

    if (!uMessageUp && !uMessageDown) {
	bShowCursor = False;
	if (bShowVersion) {
	    uMessageUp = 1;
	    strcpy (messageUp[0].strMsg, "FCITX ");
	    strcat (messageUp[0].strMsg, FCITX_VERSION);
	    messageUp[0].type = MSG_TIPS;
	}

#ifdef _DEBUG
	uMessageDown = 1;
	strcpy (messageDown[0].strMsg, strUserLocale);
	strcat (messageDown[0].strMsg, " - ");
	strcat (messageDown[0].strMsg, strXModifiers);
	messageDown[0].type = MSG_CODE;
#else
	//显示打字速度
	if (bStartRecordType && bShowUserSpeed) {
	    double          timePassed;

	    timePassed = difftime (time (NULL), timeStart);
	    if (((int) timePassed) == 0)
		timePassed = 1.0;

	    uMessageDown = 6;
	    strcpy (messageDown[0].strMsg, "打字速度：");
	    messageDown[0].type = MSG_OTHER;
	    sprintf (messageDown[1].strMsg, "%d", (int) (iHZInputed * 60 / timePassed));
	    messageDown[1].type = MSG_CODE;
	    strcpy (messageDown[2].strMsg, "/分  用时：");
	    messageDown[2].type = MSG_OTHER;
	    sprintf (messageDown[3].strMsg, "%d", (int) timePassed);
	    messageDown[3].type = MSG_CODE;
	    strcpy (messageDown[4].strMsg, "秒  字数：");
	    messageDown[4].type = MSG_OTHER;
	    sprintf (messageDown[5].strMsg, "%u", iHZInputed);
	    messageDown[5].type = MSG_CODE;
	}
#endif
    }

#ifdef _ENABLE_RECORDING
    if ( bRecording && fpRecord ) {
	if ( uMessageUp > 0 ) {
	    if ( messageUp[uMessageUp-1].type==MSG_RECORDING )
	        uMessageUp --;
	}
	strcpy(messageUp[uMessageUp].strMsg,"  [记录模式]");
	messageUp[uMessageUp].type = MSG_RECORDING;
	uMessageUp ++;
    }
#endif
}

void DrawInputWindow(void)
{
	int i;
	char up_str[MESSAGE_MAX_LENGTH]={0};
	char first_str[MESSAGE_MAX_LENGTH]={0};
	char down_str[MESSAGE_MAX_LENGTH]={0};
	char * strGBKT=NULL;
	GC gc = XCreateGC( dpy, inputWindow, 0, NULL );
	
	for (i = 0; i < uMessageUp; i++)
	{
//		printf("messageUp:%s\n",messageUp[i].strMsg);
		strGBKT = bUseGBKT ? ConvertGBKSimple2Tradition (messageUp[i].strMsg) : messageUp[i].strMsg;
		strcat(up_str,strGBKT);
		
		if (bUseGBKT)
	   		free (strGBKT);
	}
	
	strGBKT=NULL;

    if ( uMessageDown <= 0)
        strcpy(first_str, "");
    else
    {
        strGBKT = bUseGBKT ? ConvertGBKSimple2Tradition (messageDown[0].strMsg) : messageDown[0].strMsg;
        strcpy(first_str,strGBKT);
        if (bUseGBKT)
            free(strGBKT);
        if (uMessageDown >= 2)
        {
            strGBKT = bUseGBKT ? ConvertGBKSimple2Tradition (messageDown[1].strMsg) : messageDown[1].strMsg;
            strcat(first_str,strGBKT);
            if (bUseGBKT)
                free(strGBKT);
        }
        strGBKT=NULL;
    }
	
	for (i = 2; i < uMessageDown; i++) 
	{
//		printf("%d:%s\n",i, messageDown[i].strMsg);
	   	strGBKT = bUseGBKT ? ConvertGBKSimple2Tradition (messageDown[i].strMsg) : messageDown[i].strMsg;
	   	strcat(down_str,strGBKT);
	   	
	   	if (bUseGBKT)
	   		free (strGBKT);
	}
  
	draw_input_bar(up_str,first_str,down_str,&iInputWindowWidth);
//  printf("%s: %s: %s\n",up_str,first_str,down_str);
	XResizeWindow(dpy,inputWindow,iInputWindowWidth, iInputWindowHeight);	
	XCopyArea (dpy, pm_input_bar, inputWindow, gc, 0, 0, iInputWindowWidth, iInputWindowHeight, 0, 0);
    XFreeGC(dpy, gc);
}

void MoveInputWindow(CARD16 connect_id)
{
	iInputWindowWidth=(iInputWindowWidth>skin_config.skin_input_bar.ibbg_img.width)?iInputWindowWidth:skin_config.skin_input_bar.ibbg_img.width;
	iInputWindowWidth=(iInputWindowWidth>=INPUT_BAR_MAX_LEN)?INPUT_BAR_MAX_LEN:iInputWindowWidth;
	
    if (ConnectIDGetTrackCursor (connect_id) && bTrackCursor)
    {
        int iTempInputWindowX, iTempInputWindowY;

	if (iClientCursorX < 0)
	    iTempInputWindowX = 0;
	else
	    iTempInputWindowX = iClientCursorX + iOffsetX;

	if (iClientCursorY < 0)
	    iTempInputWindowY = 0;
	else
	    iTempInputWindowY = iClientCursorY + iOffsetY;

	if ((iTempInputWindowX + iInputWindowWidth) > DisplayWidth (dpy, iScreen))
	    iTempInputWindowX = DisplayWidth (dpy, iScreen) - iInputWindowWidth;

	if ((iTempInputWindowY + iInputWindowHeight) > DisplayHeight (dpy, iScreen)) {
	    if ( iTempInputWindowY > DisplayHeight (dpy, iScreen) )
	        iTempInputWindowY = DisplayHeight (dpy, iScreen) - 2 * iInputWindowHeight;
	    else
	        iTempInputWindowY = iTempInputWindowY - 2 * iInputWindowHeight;
	}

	if (!bUseDBus)
	{
		//printf("iInputWindowWidth:%d\n",iInputWindowWidth);
		XMoveWindow (dpy, inputWindow, iTempInputWindowX, iTempInputWindowY);
	}
#ifdef _ENABLE_DBUS
	else
	{
	if (iClientCursorX < 0)
	    iTempInputWindowX = 0;
	else
	    iTempInputWindowX = iClientCursorX + iOffsetX;

	if (iClientCursorY < 0)
	    iTempInputWindowY = 0;
	else
	    iTempInputWindowY = iClientCursorY + iOffsetY;

	    KIMUpdateSpotLocation(iTempInputWindowX, iTempInputWindowY);
	}
#endif
	ConnectIDSetPos (connect_id, iTempInputWindowX - iOffsetX, iTempInputWindowY - iOffsetY);
    }
    else
    {
		position * pos = ConnectIDGetPos(connect_id);
		if (bCenterInputWindow) {
		    iInputWindowX = (DisplayWidth (dpy, iScreen) - iInputWindowWidth) / 2;
		    if (iInputWindowX < 0)
			iInputWindowX = 0;
		}
		else
		    iInputWindowX = pos ? pos->x : iInputWindowX;
	
		if (!bUseDBus)
		{
			XMoveResizeWindow (dpy, inputWindow, iInputWindowX, pos ? pos->y : iInputWindowY, iInputWindowWidth, iInputWindowHeight); 
		}
#ifdef _ENABLE_DBUS
		else
		    KIMUpdateSpotLocation(iInputWindowX, pos ? pos->y : iInputWindowY);
#endif
    }
    
}

void CloseInputWindow()
{
	XUnmapWindow (dpy, inputWindow);
#ifdef _ENABLE_DBUS
	if (bUseDBus)
	{
		KIMShowAux(False);
		KIMShowPreedit(False);
		KIMShowLookupTable(False);
	}
#endif
}

