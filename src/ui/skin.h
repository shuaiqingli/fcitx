/***************************************************************************
 *   Copyright (C) 2009-2010 by t3swing                                    *
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
 * @file   skin.h
 * @author t3swing  t3swing@sina.com
 *        
 * @date   2009-10-9
 * 
 * @brief  皮肤设置相关定义及初始化加载工作
 * 
 * 
 */

#ifndef _SKIN_H
#define _SKIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <X11/xpm.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/extensions/Xrender.h>
#include <dirent.h>
#include <sys/stat.h>
#include "ui.h"
#include "MainWindow.h"
#include "InputWindow.h"

#define SIZEX 800
#define SIZEY 200
//输入条最大长度(缓冲区大小由这个决定)
#define INPUT_BAR_MAX_LEN 1500

typedef struct
{
	char img_name[32];
	//图片绘画区域
	int  position_x;
	int  position_y;
	int  width;
	int  height;
	//按键响应区域
	int  response_x;
	int  response_y;
	int  response_w;
	int  response_h;
}skin_img_t;


typedef struct 
{
	char skin_name[64];
	char skin_version[32];
	char skin_author[64];
	char shin_remark[128];
}skin_info_t;

typedef struct 
{
	int font_size;
	char font_en[32];
	char font_zh[32];
	char input_char_color[16];
	char output_char_color[16];
	char char_no_color[16];
	char first_char_color[16];
}skin_font_t;

typedef struct 
{
	skin_img_t mbbg_img;
	skin_img_t logo_img;
	skin_img_t zhpunc_img;
	skin_img_t enpunc_img;
	skin_img_t chs_img;
	skin_img_t cht_img;
	skin_img_t half_corner_img;
	skin_img_t full_corner_img;
	skin_img_t unlock_img;
	skin_img_t lock_img;
	skin_img_t lxoff_img;
	skin_img_t lxon_img;
	skin_img_t vkhide_img;
	skin_img_t vkshow_img;
	skin_img_t english_img;
	skin_img_t pinyin_img;
	skin_img_t shuangpin_img;
	skin_img_t quwei_img;
	skin_img_t wubi_img;
	skin_img_t mixpywb_img;
	skin_img_t erbi_img;
	skin_img_t cj_img;
	skin_img_t wanfeng_img;
	skin_img_t bingchan_img;
	skin_img_t ziran_img;
	skin_img_t dianbao_img;
	skin_img_t otherim_img;
}skin_main_bar_t;

typedef struct 
{	
	skin_img_t ibbg_img;
	unsigned char resize;
	int	resize_pos;
	int resize_w;
	int input_pos;
	int output_pos;
	int layout_left;
	int layout_right;
	char cursor_color[16];
	skin_img_t back_arrow_img;
	skin_img_t forward_arrow_img;
}skin_input_bar_t;


typedef struct 
{
	skin_img_t active_img;
	skin_img_t inactive_img;
}skin_tray_icon_t;

/** 
* 配置文件结构,方便处理,结构固定
*/
typedef struct 
{	
		skin_info_t skin_info;
		skin_font_t skin_font;
		skin_main_bar_t skin_main_bar;
		skin_input_bar_t skin_input_bar;
		skin_tray_icon_t skin_tray_icon;	
}skin_config_t;

/**
*
*/
typedef struct
{
	char  dirbuf[128];
	char  dirbase[64];
}skin_dir_t;

typedef struct
{
	double r;
	double b;
	double g;
}cairo_color_t;

extern cairo_surface_t *cs_main_bar;
extern cairo_surface_t *cs_input_bar;
//extern cairo_surface_t *cs_main_menu;
//extern cairo_surface_t *cs_vk_menu;
//extern cairo_surface_t *cs_im_menu;
//extern cairo_surface_t *cs_skin_menu;

extern cairo_surface_t *  bar;
extern cairo_surface_t *  logo;
extern cairo_surface_t *  punc[2];
extern cairo_surface_t *  corner[2];	
extern cairo_surface_t *  lx[2];	
extern cairo_surface_t *  chs_t[2];	
extern cairo_surface_t *  lock[2];	
extern cairo_surface_t *  vk[2];
extern cairo_surface_t *  input;
extern cairo_surface_t *  prev;
extern cairo_surface_t *  next;
extern cairo_surface_t *  english;
extern cairo_surface_t *  pinyin;
extern cairo_surface_t *  shuangpin;
extern cairo_surface_t *  quwei;
extern cairo_surface_t *  wubi;
extern cairo_surface_t *  mix;
extern cairo_surface_t *  erbi;
extern cairo_surface_t *  cangji;
extern cairo_surface_t *  wanfeng;
extern cairo_surface_t *  bingchan;
extern cairo_surface_t *  ziran;
extern cairo_surface_t *  dianbao;
extern cairo_surface_t *  otherim;
extern cairo_surface_t *  trayActive;
extern cairo_surface_t *  trayInactive;

extern Display *dpy;
extern Window  mainWindow;
extern Window   inputWindow;

extern Bool bShowCursor;
extern int skinCount;
extern skin_config_t skin_config;
extern char  skinType[64];
extern skin_dir_t skinBuf[10];
extern HIDE_MAINWINDOW hideMainWindow;
/**
*解析skin_xpm结构的值
*/
extern int str2skin_img(char * str,skin_img_t * img);
extern Visual * find_argb_visual (Display *dpy, int scr);
extern void load_main_img();
void load_tray_img();
void load_input_img();
void load_input_msg();
void draw_a_img(cairo_t **c,skin_img_t img,cairo_surface_t * png);
extern void draw_main_bar();
void draw_input_bar(char * up_str,char *first_str,char * down_str,unsigned int * iwidth);
/**
* 加载皮肤配置文件
*/
int load_skin_config();

/*
**
*/
int WritePrivateProfileString(char *SelectName, char *KeyName, char *String, char *FileName);
int GetPrivateProfileString(FILE * fptmp,char *SelectName, char *KeyName,char *DefaultString, char *ReturnString,int Size);
int fill_skin_config_int(FILE * fptmp,char * SelectName,char * KeyName);
int fill_skin_config_str(FILE * fptmp,char * SelectName,char * KeyName,char * str);

void DisplayInputBar(int barlen);
void DisplaySkin(char * skinname);
int loadSkinDir();

extern Bool IsInBox (int x0, int y0, int x1, int y1, int x2, int y2);
extern Bool IsInRspArea(int x0,int y0,skin_img_t img);

extern Visual * find_argb_visual (Display *dpy, int scr);

extern void SelectIM(int imidx);
extern void SelectVK(int );
#endif


