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
 * @file   skin.c
 * @author Yuking yuking_net@sohu.com  t3swing  t3swing@sina.com
 *        
 * @date   2009-10-9
 * 
 * @brief  皮肤设置相关定义及初始化加载工作
 * 
 * 
 */
#include "ui/ui.h"
#include "ui/skin.h"
#include "ui/font.h"
#include "tools/tools.h"
#include "tools/xdg.h"
#include "fcitx-config/configfile.h"
#include "fcitx-config/fcitx-config.h"
#include <pthread.h>
#ifdef _ENABLE_TRAY
#include "ui/TrayWindow.h"
#endif

extern pthread_rwlock_t plock;
//定义全局皮肤配置结构
FcitxSkin sc;

//指定皮肤类型 在config文件中配置
//指定皮肤所在的文件夹 一般在/usr/share/fcitx/skin目录下面
StringHashSet skinDir;

cairo_surface_t *  bar;
cairo_surface_t *  logo;
cairo_surface_t *  punc[2];
cairo_surface_t *  corner[2];	
cairo_surface_t *  lx[2];	
cairo_surface_t *  chs_t[2];	
cairo_surface_t *  lock[2];	
cairo_surface_t *  vk[2];
cairo_surface_t *  input;
cairo_surface_t *  prev;
cairo_surface_t *  next;
cairo_surface_t *  english;
cairo_surface_t *  otherim;
cairo_surface_t *  trayActive;
cairo_surface_t *  trayInactive;

//定义全局皮肤配置结构
FcitxSkin sc;

MouseE ms_logo,ms_punc,ms_corner,ms_lx,ms_chs,ms_lock,ms_vk,ms_py;
//指定皮肤类型 在config文件中配置
//指定皮肤所在的文件夹 一般在/usr/share/fcitx/skin目录下面
UT_array *skinBuf;
extern Display  *dpy;
extern CARD16 connect_id;
ConfigFileDesc * fcitxSkinDesc = NULL;

ConfigFileDesc* GetSkinDesc()
{
    FcitxLock();
    if (!fcitxSkinDesc)
    {
        FILE *tmpfp;
        tmpfp = GetXDGFileData("skin.desc", "r", NULL);
        fcitxSkinDesc = ParseConfigFileDescFp(tmpfp);
		fclose(tmpfp);
    }
    FcitxUnlock();

    return fcitxSkinDesc;
}


/**
@加载皮肤配置文件
*/
int LoadSkinConfig()
{	
	FILE    *fp;
  	char  buf[PATH_MAX]={0};
    if (sc.config.configFile)
    {
        FreeConfigFile(sc.config.configFile);
        free(sc.skinInfo.skinName);
        free(sc.skinInfo.skinVersion);
        free(sc.skinInfo.skinAuthor);
        free(sc.skinInfo.skinDesc);
    }
    memset(&sc, 0, sizeof(FcitxSkin));

reload:
    //获取配置文件的绝对路径
    {
        snprintf(buf, PATH_MAX, "%s/fcitx_skin.conf", fc.skinType);
        buf[PATH_MAX-1] ='\0';
        size_t len;
        char ** path = GetXDGPath(&len, "XDG_CONFIG_HOME", ".config", "fcitx/skin" , DATADIR, "fcitx/skin" );

        fp = GetXDGFile(buf, path, "r", len, NULL);

        FreeXDGPath(path);
    }

    if (fp)
    {
        ConfigFile *cfile;
        ConfigFileDesc* skinDesc = GetSkinDesc();
        if (sc.config.configFile == NULL)
        {
            cfile = ParseConfigFileFp(fp, skinDesc);
        }
        else
        {
            cfile = sc.config.configFile;
            cfile = ParseIniFp(fp, cfile);
        }
        if (!cfile)
        {
            fclose(fp);
            fp = NULL;
        }
        else
        {
            FcitxSkinConfigBind(&sc, cfile, skinDesc);
            ConfigBindSync((GenericConfig*)&sc);
        }
    }
	
    if (!fp)
    {
        if (strcmp(fc.skinType, "default") == 0)
        {
            FcitxLog(FATAL, _("Can not load default skin, is installion correct?"));
            perror("fopen");
            exit(1);	// 如果安装目录里面也没有配置文件，那就只好告诉用户，无法运行了
        }
        perror("fopen");
        FcitxLog(WARNING, _("Can not load skin %s, return to default"), fc.skinType);
        if (fc.skinType)
            free(fc.skinType);
        fc.skinType = strdup("default");
        goto reload;
    }


	fclose(fp);
	
	return 0;

}

Visual * find_argb_visual (Display *dpy, int scr)
{
    XVisualInfo		*xvi;
    XVisualInfo		template;
    int			nvi;
    int			i;
    XRenderPictFormat	*format;
    Visual		*visual;

    template.screen = scr;
    template.depth = 32;
    template.class = TrueColor;
    xvi = XGetVisualInfo (dpy,  VisualScreenMask |VisualDepthMask |VisualClassMask,&template,&nvi);
    if (!xvi)
	return 0;
    visual = 0;
    for (i = 0; i < nvi; i++)
    {
	format = XRenderFindVisualFormat (dpy, xvi[i].visual);
	if (format->type == PictTypeDirect && format->direct.alphaMask)
	{
	    visual = xvi[i].visual;
	    break;
	}
    }

    XFree (xvi);
    return visual;
}

int load_a_img(FcitxImage * img,cairo_surface_t ** png)
{
    char buf[PATH_MAX];
	if( strlen(img->img_name) > 0 && strcmp( img->img_name ,"NONE.img") !=0)
	{
        size_t len;
        char ** path = GetXDGPath(&len, "XDG_CONFIG_HOME", ".config", "fcitx/skin" , DATADIR, "fcitx/skin" );
        char *name;
        snprintf(buf, PATH_MAX, "%s/%s", fc.skinType, img->img_name); 
        buf[PATH_MAX-1] ='\0';
        
        FILE* fp = GetXDGFile(buf, path, "r", len, &name);
        FreeXDGPath(path);

        if (fp)
        {
            fclose(fp);
        
            *png=cairo_image_surface_create_from_png(name);
            if( img->width ==0 || img->height== 0)
            {
                img->width=cairo_image_surface_get_width(*png);
                img->height=cairo_image_surface_get_height(*png);
                //printf("width:%d  height:%d\n",img->width,img->height);
                img->response_w=img->width;
                img->response_h=img->height;
            }
        }
        else
        {
            *png = NULL;
        }
        free(name);
	}
	return 0;
}


void load_main_img()
{
	load_a_img( &sc.skinMainBar.backImg, &bar );
	load_a_img( &sc.skinMainBar.logo, &logo);
	load_a_img( &sc.skinMainBar.zhpunc, &punc[0]);
	load_a_img( &sc.skinMainBar.enpunc, &punc[1]);
	load_a_img( &sc.skinMainBar.chs, &chs_t[0]);
	load_a_img( &sc.skinMainBar.cht, &chs_t[1]);
	load_a_img( &sc.skinMainBar.halfcorner, &corner[0]);
	load_a_img( &sc.skinMainBar.fullcorner, &corner[1]);
	load_a_img( &sc.skinMainBar.unlock, &lock[0]);
	load_a_img( &sc.skinMainBar.lock, &lock[1]);
	load_a_img( &sc.skinMainBar.nolegend, &lx[0]);
	load_a_img( &sc.skinMainBar.legend, &lx[1]);
	load_a_img( &sc.skinMainBar.novk, &vk[0]);
	load_a_img( &sc.skinMainBar.vk, &vk[1]);
	load_a_img( &sc.skinMainBar.eng, &english);
	load_a_img( &sc.skinMainBar.chn, &otherim);
    int i = 0;
    for (; i < iIMCount; i ++)
    {
        im[i].image.position_x = sc.skinMainBar.chn.position_x;
        im[i].image.position_y = sc.skinMainBar.chn.position_y;
        im[i].image.response_x = sc.skinMainBar.chn.response_x;
        im[i].image.response_y = sc.skinMainBar.chn.response_y;
        im[i].image.response_w = sc.skinMainBar.chn.response_w;
        im[i].image.response_h = sc.skinMainBar.chn.response_h;
        load_a_img(&im[i].image, &im[i].icon);
    }
}

void load_input_img()
{
	load_a_img( &sc.skinInputBar.backImg, &input);
	load_a_img( &sc.skinInputBar.backArrow, &prev);
	load_a_img( &sc.skinInputBar.forwardArrow, &next);
}

void load_tray_img()
{
    load_a_img( &sc.skinTrayIcon.active, &trayActive);
    load_a_img( &sc.skinTrayIcon.inactive, &trayInactive);
}

void destroy_a_img(cairo_surface_t ** png)
{
	if(png != NULL)
		cairo_surface_destroy(*png);
	*png=NULL;
}

void destroy_img()
{
	destroy_a_img(&bar);
	destroy_a_img(&logo);
	destroy_a_img(&punc[0]);
	destroy_a_img(&punc[1]);
	destroy_a_img(&corner[0]);	
	destroy_a_img(&corner[1]);	
	destroy_a_img(&lx[0]);
	destroy_a_img(&lx[1]);
	destroy_a_img(&chs_t[0]);	
	destroy_a_img(&chs_t[1]);	
	destroy_a_img(&lock[0]);
	destroy_a_img(&lock[1]);
	destroy_a_img(&vk[0]);
	destroy_a_img(&vk[1]);
	destroy_a_img(&english);
	destroy_a_img(&otherim);
	
	destroy_a_img(&input);
	destroy_a_img(&prev);
	destroy_a_img(&next);
    destroy_a_img(&trayActive);
    destroy_a_img(&trayInactive);

    int i = 0;
    for (; i < iIMCount; i ++)
        destroy_a_img(&im[i].icon);
}

/**
*输入条的绘制非常注重效率,画笔在绘图过程中不释放
*/
void load_input_msg()
{
	ConfigColor in_char,first_char,out_char,cursorColor;
	int fontSize;
	
	fontSize=sc.skinFont.fontSize;
	
	in_char = sc.skinFont.inputCharColor;
	out_char = sc.skinFont.outputCharColor;
	first_char = sc.skinFont.firstCharColor;
	cursorColor = sc.skinInputBar.cursorColor;
//输入条背景图画笔
//	c=cairo_create(cs_input_bar);
	inputWindow.c_back = cairo_create(inputWindow.cs_input_bar);
	
//拼音画笔
	inputWindow.c_eng = cairo_create(inputWindow.cs_input_bar);
	cairo_set_source_rgb(inputWindow.c_eng, in_char.r, in_char.g, in_char.b);
	cairo_select_font_face(inputWindow.c_eng, gs.fontZh,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(inputWindow.c_eng, fontSize);


//第一个字画笔
	inputWindow.c_first=cairo_create(inputWindow.cs_input_bar);
	cairo_set_source_rgb(inputWindow.c_first,first_char.r, first_char.g, first_char.b);
	cairo_select_font_face(inputWindow.c_first, gs.fontZh,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(inputWindow.c_first, fontSize);

//其他字画笔	
	inputWindow.c_other = cairo_create(inputWindow.cs_input_bar);
	cairo_set_source_rgb(inputWindow.c_other, out_char.r, out_char.g, out_char.b);
	cairo_select_font_face(inputWindow.c_other, gs.fontZh,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(inputWindow.c_other, fontSize);

//光标画笔
	inputWindow.c_cursor=cairo_create(inputWindow.cs_input_bar);
	cairo_set_source_rgb(inputWindow.c_cursor, cursorColor.r, cursorColor.g, cursorColor.b);
	cairo_set_line_width (inputWindow.c_cursor, 1);
}

void draw_a_img(cairo_t **c,FcitxImage img,cairo_surface_t * png,MouseE mouse)
{
	cairo_t * cr;
/*	printf("%s=%d %d %d %d %d %d %d %d\n",img.img_name,img.position_x, img.position_y,img.width,img.height,
										                img.response_x,img.response_y,img.response_w,img.response_h);*/
	if( strlen(img.img_name) == 0 || strcmp( img.img_name ,"NONE.img") ==0 || !png)
		return;	

    if(mouse == MOTION)
    {
	    cairo_set_source_surface(*c,png, img.position_x, img.position_y);
		cairo_paint_with_alpha(*c,0.7);
		
	}
	else if(mouse == PRESS)
	{
		cr=cairo_create(mainWindow.cs_main_bar);
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		cairo_translate(cr, img.position_x+(int)(img.width*0.2/2), img.position_y+(int)(img.height*0.2/2));
		cairo_scale(cr, 0.8, 0.8);
		cairo_set_source_surface(cr,png,0,0);
		cairo_paint(cr);
		cairo_destroy(cr);
	}
	else //if( img.mouse == RELEASE)
	{
		cairo_set_source_surface(*c,png, img.position_x, img.position_y);
		cairo_paint(*c);
	}
}

void draw_input_bar(char * up_str,char *first_str,char * down_str,unsigned int * iwidth)
{
    char p[MESSAGE_MAX_LENGTH];
	int png_width,png_height;
	int repaint_times=0,remain_width=0;
	int input_bar_len=0;
	int resizePos=0;
	int resizeWidth=0;
	RESIZERULE flag=0;
	int cursor_pos=0;
	int up_len,down_len;
	int down_str_pos;
    int iChar = iCursorPos;
    cairo_t *c;

	resizePos=sc.skinInputBar.resizePos;
	resizeWidth=(sc.skinInputBar.resizeWidth==0)?20:sc.skinInputBar.resizeWidth;
	flag=sc.skinInputBar.resize;
	
	up_len=StringWidthWithContext(inputWindow.c_eng ,up_str);

    if (inputWindow.bShowCursor)
    {
        strncpy(p, up_str, iChar);
        p[iChar] = '\0';
        cursor_pos=sc.skinInputBar.layoutLeft
            + StringWidthWithContext(inputWindow.c_eng, p) + 2;	
    }

	down_len= StringWidthWithContext(inputWindow.c_first ,first_str);
	down_str_pos=down_len+10;
	
	down_len+= StringWidthWithContext(inputWindow.c_other, down_str);
	
	input_bar_len=(up_len<down_len)?down_len:up_len;
	input_bar_len+=sc.skinInputBar.layoutLeft+sc.skinInputBar.layoutRight;
	//输入条长度应该比背景图长度要长,比最大长度要短
	input_bar_len=(input_bar_len>sc.skinInputBar.backImg.width)?input_bar_len:sc.skinInputBar.backImg.width;
	input_bar_len=(input_bar_len>=INPUT_BAR_MAX_LEN)?INPUT_BAR_MAX_LEN:input_bar_len;
	*iwidth=input_bar_len;


	png_width=sc.skinInputBar.backImg.width;
	png_height=sc.skinInputBar.backImg.height;
		
	//重绘的次数
	repaint_times=(input_bar_len - png_width)/resizeWidth+1;
	//不够汇一次的剩余的长度
	remain_width=(input_bar_len - png_width)%resizeWidth;
	
	
	//把背景清空
	c=cairo_create(inputWindow.cs_input_bar);
	cairo_set_source_rgba(c, 0, 0, 0,0);
	cairo_rectangle (c, 0, 0, INPUT_BAR_MAX_LEN, sc.skinInputBar.backImg.height);
	//cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
	cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
	cairo_fill(c);
	
	//把cr设定位png图像,并保存
	cairo_set_source_surface(c, input, 0, 0);
	cairo_save(c);
	
	//画输入条第一部分(从开始到重复或者延伸段开始的位置)
	cairo_set_operator(c, CAIRO_OPERATOR_OVER);
	cairo_rectangle	(c,0,0,resizePos,png_height);
	cairo_clip(c);
	cairo_paint(c);
	
	//再画输入条的第三部分(因为第二部分可变,最后处理会比较方便)
	cairo_restore(c);
	cairo_save(c);
	cairo_set_operator(c, CAIRO_OPERATOR_OVER);
	cairo_translate(c, input_bar_len-(png_width-resizePos-resizeWidth), 0);
	//printf("%d %d %d\n",input_bar_len,png_width-resizePos-resizeWidth,input_bar_len-(png_width-resizePos-resizeWidth));
	cairo_set_source_surface(c, input, -resizePos-resizeWidth, 0);
	cairo_rectangle	(c,0,0,png_width-resizePos-resizeWidth,png_height);
	cairo_clip(c);
	cairo_paint(c);

    //画第二部分,智能变化有两种方式
	if( flag == R_COPY) //
	{
		int i=0;
			
		//先把整段的都绘上去
		for(i=0;i<repaint_times;i++)
		{
			cairo_restore(c);
			cairo_save(c);
			cairo_translate(c,resizePos+i*resizeWidth, 0);
			cairo_set_source_surface(c, input,-resizePos, 0);
			cairo_set_operator(c, CAIRO_OPERATOR_OVER);
			cairo_rectangle	(c,0,0,resizeWidth,png_height);
			cairo_clip(c);
			cairo_paint(c);
		}
		
	//绘制剩余段
		if(remain_width != 0)
		{
			cairo_restore(c);
			cairo_translate(c,resizePos+resizeWidth*repaint_times, 0);
			cairo_set_source_surface(c, input,-resizePos, 0);
			cairo_set_operator(c, CAIRO_OPERATOR_OVER);
			cairo_rectangle	(c,0,0,remain_width,png_height);
			cairo_clip(c);
			cairo_paint(c);
		}
	}
	else if( flag == R_RESIZE)
	{
		cairo_restore(c);
		cairo_translate(c,resizePos, 0);
		cairo_scale(c, (double)(input_bar_len-png_width+resizeWidth)/(double)resizeWidth,1);
		cairo_set_source_surface(c, input,-resizePos, 0);
		cairo_set_operator(c, CAIRO_OPERATOR_OVER);
		cairo_rectangle	(c,0,0,resizeWidth,png_height);
		cairo_clip(c);
		cairo_paint(c);
	}
	else
	{
		cairo_restore(c);
		cairo_translate(c,resizePos, 0);
		cairo_set_source_surface(c, input,-resizePos, 0);
		cairo_set_operator(c, CAIRO_OPERATOR_OVER);
		cairo_rectangle	(c,0,0,resizeWidth,png_height);
		cairo_clip(c);
		cairo_paint(c);
	}
	
	if(inputWindow.bShowCursor )
	{	
		//画向前向后箭头
		
		cairo_set_source_surface(inputWindow.c_back, prev,
							input_bar_len-(sc.skinInputBar.backImg.width-sc.skinInputBar.backArrow.position_x) , 
							sc.skinInputBar.backArrow.position_y);				
		if(inputWindow.bShowNext)
			cairo_paint(inputWindow.c_back);
		else
			cairo_paint_with_alpha(inputWindow.c_back,0.5);
		
		//画向前箭头
		cairo_set_source_surface(inputWindow.c_back, next,
							input_bar_len-(sc.skinInputBar.backImg.width-sc.skinInputBar.forwardArrow.position_x) , 
							sc.skinInputBar.forwardArrow.position_y);
		if(inputWindow.bShowPrev)
			cairo_paint(inputWindow.c_back);
		else
			cairo_paint_with_alpha(inputWindow.c_back,0.5);
	}

    OutputStringWithContext(inputWindow.c_eng, up_str, sc.skinInputBar.layoutLeft, sc.skinInputBar.inputPos );

    OutputStringWithContext(inputWindow.c_first , first_str, sc.skinInputBar.layoutLeft,sc.skinInputBar.outputPos);

    OutputStringWithContext(inputWindow.c_other, down_str, sc.skinInputBar.layoutLeft+down_str_pos,sc.skinInputBar.outputPos);
	
	//画光标
	if(inputWindow.bShowCursor )
	{
		cairo_move_to(inputWindow.c_cursor,cursor_pos,sc.skinInputBar.inputPos+2);
		cairo_line_to(inputWindow.c_cursor,cursor_pos,sc.skinInputBar.inputPos-sc.skinFont.fontSize);
		cairo_stroke(inputWindow.c_cursor);
	}
	
	cairo_destroy(c);
}

/*
*把鼠标状态初始化为某一种状态.
*/
void set_mouse_status(MouseE m)
{
	 ms_logo=m;
	 ms_punc=m;
	 ms_corner=m;
	 ms_lx=m;
	 ms_chs=m;
	 ms_lock=m;
	 ms_vk=m;
	 ms_py=m;
}


void DisplaySkin(char * skinname)
{
    if (fc.bUseDBus)
        return;
    char *pivot = fc.skinType;
    fc.skinType= strdup(skinname);
    if (pivot)
        free(pivot);

	XUnmapWindow (dpy, mainWindow.window);
    CloseInputWindow();
	
	destroy_img();

    DestroyMainWindow();
	DestroyInputWindow();
	
	LoadSkinConfig();
    CreateFont();
	CreateMainWindow ();
	CreateInputWindow ();

	DrawMainWindow ();
	DrawInputWindow ();
#ifdef _ENABLE_TRAY
    load_tray_img();
    if (ConnectIDGetState (connect_id) == IS_CHN)
        DrawTrayWindow (ACTIVE_ICON, 0, 0, tray.size, tray.size );
    else
        DrawTrayWindow (INACTIVE_ICON, 0, 0, tray.size, tray.size );
#endif
	
	XMapRaised(dpy,mainWindow.window);
}

//图片文件加载函数完成
/*-------------------------------------------------------------------------------------------------------------*/
//skin目录下读入skin的文件夹名

int loadSkinDir()
{
    if (!skinBuf)
    {
        skinBuf = malloc(sizeof(UT_array));
        utarray_init(skinBuf, &ut_str_icd);
    }
    else
    {
        utarray_clear(skinBuf);
    }
    int i ; 
    DIR *dir;
    struct dirent *drt;
    struct stat fileStat;
    size_t len;
    char pathBuf[PATH_MAX];
    char **skinPath = GetXDGPath(&len, "XDG_CONFIG_HOME", ".config", "fcitx/skin" , DATADIR, "fcitx/skin" );
    for(i = 0; i< len; i++)
    {
        dir = opendir(skinPath[i]);
        if (dir == NULL)
            continue;

        while((drt = readdir(dir)) != NULL)
        {
            if (strcmp(drt->d_name , ".") == 0 || strcmp(drt->d_name, "..") == 0)
                continue;
            sprintf(pathBuf,"%s/%s",skinPath[i],drt->d_name);

            if( stat(pathBuf,&fileStat) == -1)
            {
                continue;
            }
            if ( fileStat.st_mode & S_IFDIR)
            {
                /* check duplicate name */
                int j = 0;
                for(;j<skinBuf->i;j++)
                {
                    char **name = (char**) utarray_eltptr(skinBuf, j);
                    if (strcmp(*name, drt->d_name) == 0)
                        break;
                }
                if (j == skinBuf->i)
                {
                    char *temp = drt->d_name;
                    utarray_push_back(skinBuf, &temp);
                }
            }
        }

        closedir(dir);
    }

    FreeXDGPath(skinPath);

	return 0;
}


