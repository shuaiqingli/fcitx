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
 *   t3swing  t3swing@sina.com    20091117                                 *
 ***************************************************************************/

#include "MenuWindow.h"

#include <ctype.h>

#include <iconv.h>
#include <X11/Xatom.h>
#include "skin.h"
#include "fcitx-config/configfile.h"


extern int      iScreen;

xlibMenu mainMenu,imMenu,vkMenu,skinMenu; 
					
void InitMenuDefault(xlibMenu * Menu)
{
	Menu->pos_x=100;
	Menu->pos_y=100;
	Menu->width=130;
//	Menu->bgcolor.r=0xDCDCDC;
//	Menu->bgselectcolor=0x0A2465;
//	Menu->charcolor=0x111111;
//	Menu->charselectcolor=(0xFFFFFF-0x111111);
	Menu->font_size=14;
	strcpy(Menu->font,gs.fontZh);
	Menu->mark=-1;
}

void SetMeueShell(menuShell * shell,char * tips,int isselect,shellType type)
{
	strcpy(shell->tipstr,tips);
	shell->isselect=isselect;
	shell->type=type;
}

int CreateXlibMenu(Display * dpy,xlibMenu * Menu)
{
	XSetWindowAttributes attrib;
	unsigned long   attribmask;
	int             iBackPixel;
	WINDOW_COLOR     menuWindowColor = { NULL, NULL, {0, 220 << 8, 220 << 8, 220 << 8}};
					
    if (XAllocColor (dpy, DefaultColormap (dpy, DefaultScreen (dpy)), &(menuWindowColor.backColor)))
		iBackPixel = menuWindowColor.backColor.pixel;
    else
		iBackPixel = WhitePixel (dpy, DefaultScreen (dpy));
	//开始只创建一个简单的窗口不做任何动作
    Menu->menuWindow =XCreateSimpleWindow (dpy, DefaultRootWindow (dpy), 
									0, 0,
									MENU_WINDOW_WIDTH,MENU_WINDOW_HEIGHT,
									0,WhitePixel (dpy, DefaultScreen (dpy)),iBackPixel);	
    if (Menu->menuWindow == (Window) NULL)
	{
		//printf("CreateMenuWindow failed\n");
		return False;
	}
	
  	attrib.override_redirect = True;
	attribmask = CWOverrideRedirect;
	XChangeWindowAttributes (dpy, Menu->menuWindow, attribmask, &attrib);
	
	Menu->menu_cs=cairo_xlib_surface_create(dpy, Menu->menuWindow, DefaultVisual(dpy, iScreen), MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT);
	
    XSelectInput (dpy, Menu->menuWindow, ExposureMask | ButtonPressMask | ButtonReleaseMask  | PointerMotionMask );
    //XFlush( dpy);

    return True;
}

void GetMenuHeight(Display * dpy,xlibMenu * Menu)
{
	int i=0;
	int winheight=0;
	int fontheight=0;

	winheight = 8+8;//菜单头和尾都空8个pixel
	fontheight=FontHeight(Menu->font);
	for(i=0;i<Menu->useItemCount;i++)
	{
		//printf("%d %d %x %x %x %s\n",winheight,Menu->width,Menu->bgcolor,Menu->charcolor,Menu->charselectcolor,Menu->shell[i].tipstr);
		if( Menu->shell[i].type == menushell)
			winheight=winheight+6+fontheight;
		else if( Menu->shell[i].type == divline)
			winheight+=5;
	}
	Menu->height=winheight;
	

}
//根据Menu内容来绘制菜单内容
void DrawXlibMenu(Display * dpy,xlibMenu * Menu)
{
	int i=0;
	int fontheight;
	int pos_y=0;

	fontheight=FontHeight(Menu->font);

	GetMenuHeight(dpy,Menu);
	//printf("%d %d\n",Menu->height,Menu->useItemCount);
	//从第8像素点处开始绘图
	pos_y=8;
	for (i=0;i<Menu->useItemCount;i++)
	{
		if( Menu->shell[i].type == menushell)
		{
			DisplayText( dpy,Menu,i,pos_y);
			if(Menu->mark == i)//void menuMark(Display * dpy,xlibMenu * Menu,int y,int i)
				menuMark(dpy,Menu,pos_y,i);
			pos_y=pos_y+6+fontheight;
		}
		else if( Menu->shell[i].type == divline)
		{
			DrawDivLine(dpy,Menu,pos_y);
			pos_y+=5;
		}
	}
	XMoveResizeWindow(dpy, Menu->menuWindow, Menu->pos_x,Menu->pos_y,Menu->width, Menu->height);
}

void DisplayXlibMenu(Display * dpy,xlibMenu * Menu)
{
	//clearSelectFlag(Menu);
	XMapRaised (dpy, Menu->menuWindow);	     
}

void DrawDivLine(Display * dpy,xlibMenu * Menu,int line_y)
{
	//printf("DrawDivLine============\n");
	//分割线的颜色写死了，浅灰色
	cairo_t * cr;
	cr=cairo_create(Menu->menu_cs);
	cairo_set_source_rgb(cr, 0.2, 0.2, 0.1);
	cairo_set_line_width (cr, 0.2);
	cairo_move_to(cr, 5, line_y+3);
	cairo_line_to(cr, Menu->width-10, line_y+3);
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void menuMark(Display * dpy,xlibMenu * Menu,int y,int i)
{
	GC gc ;
	gc=XCreateGC( dpy, Menu->menuWindow, 0, NULL );
	if(Menu->shell[i].isselect == 0)
		XSetForeground(dpy,gc,0x0A2465 );
	else
		XSetForeground(dpy,gc,0xDCDCDC );
	XFillArc(dpy, Menu->menuWindow, gc,  3, y+5, 11, 11, 0, 360*64);
	XFreeGC (dpy, gc);
}

/*
* 显示菜单上面的文字信息,只需要指定窗口,窗口宽度,需要显示文字的上边界,字体,显示的字符串和是否选择(选择后反色)
* 其他都固定,如背景和文字反色不反色的颜色,反色框和字的位置等
*/
void DisplayText(Display * dpy,xlibMenu * Menu,int shellindex,int line_y)
{
	cairo_t *  cr;
	cr=cairo_create(Menu->menu_cs);
	if(Menu->shell[shellindex].isselect ==0)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgb (cr, 0.86, 0.86, 0.86);
        cairo_rectangle (cr, 2,line_y,Menu->width-4,Menu->font_size+4);
        cairo_fill (cr);
      
		cairo_set_source_rgb(cr, 0.067, 0.067, 0.067);
		cairo_select_font_face(cr, Menu->font,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, Menu->font_size);
	
        OutputStringWithContext(cr, Menu->shell[shellindex].tipstr , 20,line_y+Menu->font_size);
	
		if(Menu->shell[shellindex].next == 1)
		{	
			cairo_set_source_rgb (cr, 0.04, 0.14, 0.4);
			cairo_move_to(cr,Menu->width-11,line_y+4);
			cairo_line_to(cr,Menu->width-11,line_y+16);
			cairo_line_to(cr,Menu->width-5,line_y+10);
			cairo_line_to(cr,Menu->width-11,line_y+4);
			cairo_fill (cr);
		}
	}
	else
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgb (cr, 0.04, 0.14, 0.4);
        cairo_rectangle (cr, 2,line_y,Menu->width-4,Menu->font_size+4);
        cairo_fill (cr);
	
       
        cairo_set_source_rgb(cr, 0.93, 0.93, 0.93);
		cairo_select_font_face(cr, Menu->font,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, Menu->font_size);
	
        OutputStringWithContext(cr, Menu->shell[shellindex].tipstr , 20,line_y+Menu->font_size);
		
		if(Menu->shell[shellindex].next == 1)
		{			
			cairo_set_source_rgb (cr,0.93, 0.93, 0.93);
			cairo_move_to(cr,Menu->width-11,line_y+5);
			cairo_line_to(cr,Menu->width-11,line_y+15);
			cairo_line_to(cr,Menu->width-5,line_y+10);
			cairo_line_to(cr,Menu->width-11,line_y+5);
			cairo_fill (cr);
		}		
	}
		cairo_destroy(cr);
}

/**
*返回鼠标指向的菜单在menu中是第多少项
*/
int selectShellIndex(xlibMenu * Menu,int y)
{
	int i;
	int winheight=8;
	int fontheight;
	
	fontheight=FontHeight(Menu->font);
	for(i=0;i<Menu->useItemCount;i++)
	{
		if( Menu->shell[i].type == menushell)
		{
			if(y>winheight+1 && y<winheight+6+fontheight-1)
				return i;
			winheight=winheight+6+fontheight;
		}
		else if( Menu->shell[i].type == divline)
			winheight+=5;
	}
	return -1;
}

void colorRevese(xlibMenu * Menu,int shellIndex)
{
	int i;
	for(i=0;i<Menu->useItemCount;i++)
	{
		Menu->shell[i].isselect=0;
	}
	if(shellIndex>=0)
		Menu->shell[shellIndex].isselect=1;
}

void clearSelectFlag(xlibMenu * Menu)
{
	int i;
	for(i=0;i<16;i++)
	{
		Menu->shell[i].isselect=0;
	}
}


//以上为菜单的简单封装，下面为对菜单的操作部分
//=========================================================================================
//创建菜单窗口
Bool CreateMenuWindow( )
{
	Bool ret;
	int i;
	InitMenuDefault(&mainMenu);
	mainMenu.width=MENU_WINDOW_WIDTH;
	SetMeueShell(&mainMenu.shell[0],"关于fcitx",0,menushell);
	SetMeueShell(&mainMenu.shell[1],"",0,divline);
	SetMeueShell(&mainMenu.shell[2],"皮肤切换",0,menushell);
	SetMeueShell(&mainMenu.shell[3],"输入法切换",0,menushell);
	SetMeueShell(&mainMenu.shell[4],"软键盘切换",0,menushell);
	SetMeueShell(&mainMenu.shell[5],"",0,divline);
	SetMeueShell(&mainMenu.shell[6],"fcitx配置     ...",0,menushell);
	SetMeueShell(&mainMenu.shell[7],"退出",0,menushell);
	mainMenu.useItemCount=8;
	for(i=0;i<mainMenu.useItemCount;i++)
	{
		mainMenu.shell[i].next=0;
	}
	mainMenu.shell[2].next=1;
	mainMenu.shell[3].next=1;
	mainMenu.shell[4].next=1;
	ret=CreateXlibMenu(dpy,&mainMenu);
	return 0;
}

//创建输入法选择菜单窗口
Bool CreateImMenuWindow() 
{	
	Bool ret;

	InitMenuDefault(&imMenu);
	ret=CreateXlibMenu(dpy,&imMenu);
	return ret;
}  
 
//创建皮肤选择菜单窗口,皮肤菜单由于在窗口创建之初,信息不全,菜单结构在菜单显示之前再填充
Bool CreateSkinMenuWindow()
{
	Bool ret;
	InitMenuDefault(&skinMenu);
	ret=CreateXlibMenu(dpy,&skinMenu);
	return ret;
} 
	
//创建软键盘布局选择菜单窗口
Bool CreateVKMenuWindow()
{
	Bool ret;
	InitMenuDefault(&vkMenu);
	SetMeueShell(&vkMenu.shell[0],"1.西文半角",0,menushell);
	SetMeueShell(&vkMenu.shell[1],"2.全角符号",0,menushell);
	SetMeueShell(&vkMenu.shell[2],"3.希腊字母",0,menushell);
	SetMeueShell(&vkMenu.shell[3],"4.俄文字母",0,menushell);
	SetMeueShell(&vkMenu.shell[4],"5.数字序号",0,menushell);
	SetMeueShell(&vkMenu.shell[5],"6.数学符号",0,menushell);
	SetMeueShell(&vkMenu.shell[6],"7.数字符号",0,menushell);
	SetMeueShell(&vkMenu.shell[7],"8.特殊符号",0,menushell);
	SetMeueShell(&vkMenu.shell[8],"9.日文平假名",0,menushell);
	SetMeueShell(&vkMenu.shell[9],"10.日文片假名",0,menushell);
	SetMeueShell(&vkMenu.shell[10],"11.制表符",0,menushell);
		
	vkMenu.useItemCount=11;
	ret=CreateXlibMenu(dpy,&vkMenu);
	return ret;
	
}

//主菜单事件处理
void MainMenuEvent(int x,int y)
{
	int i,j;
	char tmpstr[64]={0};
	i=selectShellIndex(&mainMenu, y);
	//printf("i=%d\n",i);
	colorRevese(&mainMenu,i);
	DrawXlibMenu( dpy,&mainMenu);
	DisplayXlibMenu(dpy,&mainMenu);	
	
	switch(i)
	{
	//显示皮肤菜单
		case 2:
			skinMenu.useItemCount=skinBuf->i;
			for(j=0;j<skinBuf->i;j++)
			{
                char **sskin = (char**)utarray_eltptr(skinBuf, j);
				strcpy(skinMenu.shell[j].tipstr, *sskin);
			}
			skinMenu.pos_x=mainMenu.pos_x;
			skinMenu.pos_y=mainMenu.pos_y;
		
			if( skinMenu.pos_x+ mainMenu.width+skinMenu.width > DisplayWidth(dpy,iScreen))
				skinMenu.pos_x=skinMenu.pos_x-skinMenu.width+4;
			else
				skinMenu.pos_x=skinMenu.pos_x+ mainMenu.width-4;
			
			if( skinMenu.pos_y+y+skinMenu.height >DisplayHeight(dpy, iScreen))
				skinMenu.pos_y=DisplayHeight(dpy, iScreen)-skinMenu.height-10;
			else 
				skinMenu.pos_y=y+skinMenu.pos_y-10;
			
			clearSelectFlag(&skinMenu);
			DrawXlibMenu( dpy,&skinMenu);
			DisplayXlibMenu(dpy,&skinMenu);	
			break;
		case 3:
			for(i=0;i<iIMCount;i++)
			{
				memset(tmpstr,0,sizeof(tmpstr));
				sprintf(tmpstr,"%d.%s",i+1,_(im[i].strName));
				//printf("%s\n",tmpstr);
				SetMeueShell(&imMenu.shell[i],tmpstr,0,menushell);
			}
			imMenu.useItemCount=iIMCount;
			
			imMenu.pos_x=mainMenu.pos_x;
			imMenu.pos_y=mainMenu.pos_y;
		
			if( imMenu.pos_x+ mainMenu.width+imMenu.width > DisplayWidth(dpy,iScreen))
				imMenu.pos_x=imMenu.pos_x-imMenu.width+4;
			else
				imMenu.pos_x=imMenu.pos_x+ mainMenu.width-4;
			
			if( imMenu.pos_y+y+imMenu.height >DisplayHeight(dpy, iScreen))
				imMenu.pos_y=DisplayHeight(dpy, iScreen)-imMenu.height-10;
			else 
				imMenu.pos_y=y+imMenu.pos_y-10;
			
			clearSelectFlag(&imMenu);
			DrawXlibMenu( dpy,&imMenu);
			DisplayXlibMenu(dpy,&imMenu);	
			break;
		case 4:
			vkMenu.pos_x=mainMenu.pos_x;
			vkMenu.pos_y=mainMenu.pos_y;
		
			if( vkMenu.pos_x+ mainMenu.width+vkMenu.width > DisplayWidth(dpy,iScreen))
				vkMenu.pos_x=vkMenu.pos_x-vkMenu.width+4;
			else
				vkMenu.pos_x=vkMenu.pos_x+ mainMenu.width-4;
			
			if( vkMenu.pos_y+y+vkMenu.height >DisplayHeight(dpy, iScreen))
				vkMenu.pos_y=DisplayHeight(dpy, iScreen)-vkMenu.height-10;
			else 
				vkMenu.pos_y=y+vkMenu.pos_y-10;
			
			clearSelectFlag(&vkMenu)	;
			DrawXlibMenu( dpy,&vkMenu);
			DisplayXlibMenu(dpy,&vkMenu);	
			break;
		default :
			break;
	}
	
	if(mainMenu.shell[2].isselect == 0)
		XUnmapWindow (dpy, skinMenu.menuWindow);
	if(mainMenu.shell[3].isselect == 0)
		XUnmapWindow (dpy, imMenu.menuWindow);
	if(mainMenu.shell[4].isselect == 0)
		XUnmapWindow (dpy, vkMenu.menuWindow);
}

//输入法菜单事件处理
void IMMenuEvent(int x,int y)
{
	int i;
	i=selectShellIndex(&imMenu, y);

	//printf("i=%d\n",i);
	colorRevese(&imMenu,i);
	DrawXlibMenu(dpy,&imMenu);
	DisplayXlibMenu(dpy,&imMenu);	
}

//虚拟键盘菜单事件处理
void VKMenuEvent(int x,int y)
{
	int i;
	i=selectShellIndex(&vkMenu, y);
	//printf("i=%d\n",i);
	colorRevese(&vkMenu,i);
	DrawXlibMenu(dpy,&vkMenu);
	DisplayXlibMenu(dpy,&vkMenu);	
}

//皮肤菜单事件处理
void SkinMenuEvent(int x,int y)
{
	int i;
	i=selectShellIndex(&skinMenu, y);
	//printf("i=%d\n",i);
	colorRevese(&skinMenu,i);
	DrawXlibMenu(dpy,&skinMenu);
	DisplayXlibMenu(dpy,&skinMenu);	
}

