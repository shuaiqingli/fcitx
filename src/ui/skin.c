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
#include "skin.h"


//定义全局皮肤配置结构
skin_config_t skin_config;

//指定皮肤类型 在config文件中配置
char  skinType[64];
//指定皮肤所在的文件夹 一般在/usr/share/fcitx/skin目录下面
skin_dir_t skinBuf[10];
int skinCount;

cairo_surface_t *cs_main_bar;
cairo_surface_t *cs_input_bar;

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
cairo_surface_t *  pinyin;
cairo_surface_t *  shuangpin;
cairo_surface_t *  quwei;
cairo_surface_t *  wubi;
cairo_surface_t *  mix;
cairo_surface_t *  erbi;
cairo_surface_t *  cangji;
cairo_surface_t *  wanfeng;
cairo_surface_t *  bingchan;
cairo_surface_t *  ziran;
cairo_surface_t *  dianbao;
cairo_surface_t *  otherim;
cairo_surface_t *  trayActive;
cairo_surface_t *  trayInactive;

//定义全局皮肤配置结构
skin_config_t skin_config;


//指定皮肤类型 在config文件中配置
char  skin_type[64];
//指定皮肤所在的文件夹 一般在/usr/share/fcitx/skin目录下面
skin_dir_t skin_buf[10];
char skin_path[PATH_MAX];
extern Display  *dpy;


void ltrim( char *str)
{
    char* s=str;
    while ( *s )
        if ( isspace(*s) )
            ++s;
        else
            break;
    if ( s>str )
        strcpy( str, s );
}


void rtrim(char *str )
{
    char* s=str;
    while ( *s )
        ++s;
    --s;
    while ( s>=str )
        if ( isspace(*s) )
            --s;
        else
            break;
    *(s+1)=0;
}

void trim(char *str)
{
	ltrim(str) ;
	rtrim(str) ;
}

int GetPrivateProfileString(FILE * fptmp,char *SelectName, char *KeyName,char *DefaultString, char *ReturnString,int Size)
{
	
    char chBuf1[256],chBuf[256],chMsg[256],ch;
    char *pchBegin;
	FILE * fp=fptmp;
    pchBegin = NULL;
    sprintf(chBuf,"[%s]",SelectName);
	
    fseek(fp,0L,SEEK_SET);

    while (1) {
			if( feof(fp) ) {
				strcpy(ReturnString,DefaultString);
			//	fclose(fp) ;
				return -1;
			}
			
			memset(chBuf1,'\0',256);
			fgets(chBuf1,sizeof(chBuf1),fp);
       
      // 过滤注释
      if (chBuf1[0] == '#') continue;
       
		
      // 段名
			pchBegin = strstr( chBuf1, chBuf );
      if( pchBegin == NULL )
				continue;
			else 
				break;
		}

    pchBegin = NULL;
    
    while (1) {
			if( feof(fp) ) {
				strcpy(ReturnString,DefaultString);
			//	fclose(fp) ;
				return -1;
			}
	
			memset(chBuf1,'\0',256);
			fgets(chBuf1,sizeof(chBuf1),fp);
        
      // 过滤注释
      if (chBuf1[0] == '#') continue;
      	trim(chBuf1);
      // 关键字名 
			pchBegin = strstr( chBuf1, KeyName );

			if( pchBegin == NULL )
				continue;
		if( memcmp(KeyName,chBuf1,strlen(KeyName)) != 0)
				continue;
			ch = pchBegin[strlen(KeyName)];
			if ( ch!=' ' && ch!='=' )
	    	continue;
			
			pchBegin = NULL;
			pchBegin = strstr( chBuf1, "=" );

			if( pchBegin == NULL )
				continue;
			else
				break;
		}
    
    
    strcpy( chMsg, pchBegin+1 ); 
    trim(chMsg) ;
    memcpy(ReturnString,chMsg,Size);
    ReturnString[Size] = '\0';

    return 0;
}

int str2skin_img(char * str,skin_img_t * img)
{
	if(sscanf(str, "%s %d %d %d %d %d %d %d %d", img->img_name,&img->position_x,\
		&img->position_y,&img->width,&img->height,&img->response_x,&img->response_y,\
		&img->response_w,&img->response_h) <= 0 )
		return -1;
	
	if( img->response_x ==0 && img->response_y ==0)
	{
		img->response_x=img->position_x;
		img->response_y=img->position_y;
	}
	return 0;
}

int fill_skin_config_img(FILE * fptmp,char * SelectName ,char * KeyName, skin_img_t * img)
{	
	FILE * fp=fptmp;
	char    valuebuf[128]={0};
	GetPrivateProfileString(fp,SelectName,KeyName,"",(char*)valuebuf,sizeof(valuebuf));
	if(strlen(valuebuf) != 0 )
		str2skin_img(valuebuf,img);
	
	//测试解析是否正常
	/*printf("%s: %s %d %d %d %d %d %d %d %d\n",KeyName, img->img_name,img->position_x,\
		img->position_y,img->width,img->height,img->response_x,img->response_y,\
		img->response_w,img->response_h);*/
	
	return 0;
}

int fill_skin_config_str(FILE * fptmp,char * SelectName,char * KeyName,char * str)
{	
	FILE * fp=fptmp;
	char    valuebuf[128]={0};
	GetPrivateProfileString(fp,SelectName,KeyName,"",(char*)valuebuf,sizeof(valuebuf));
	if(strlen(valuebuf) != 0)
		strcpy(str,valuebuf);
	//printf("%s:%s\n",KeyName,str);
	return 0;
}

int fill_skin_config_int(FILE * fptmp,char * SelectName,char * KeyName)
{	
	FILE * fp=fptmp;
	char    valuebuf[128]={0};
	GetPrivateProfileString(fp,SelectName,KeyName,"",(char*)valuebuf,sizeof(valuebuf));
	if(strlen(valuebuf) != 0)
	{	
		//printf("%s:%d\n",KeyName,atoi(valuebuf));
		return atoi(valuebuf);
	}
	
	return 0;
}

/*
*
*
*/
int convert_str2color(cairo_color_t * color,char * str)
{
	int r,g,b;
	if(strlen <= 0)
	{
		r=255;
		g=0;
		b=0;
	}
	else if(sscanf( str, "%d %d %d",&r,&g,&b) !=3)
	{
		printf("%s \n",str);
		printf("color format error!\n");
		return -1;
	}
	color->r=(double)r/255;
	color->g=(double)g/255;
	color->b=(double)b/255;

	return 0;   
}

/**
@加载皮肤配置文件
*/
int load_skin_config()
{	
	FILE    *fp;
  	char  buf[256]={0};
	memset(&skin_config,0,sizeof(skin_config_t));
  
    //获取配置文件的绝对路径
    sprintf(skin_path,"%s/skin/%s/",PKGDATADIR,skinType);
    sprintf(buf,"%s/fcitx_skin.conf",skin_path);

	fp = fopen(buf, "r");
	
	if(!fp){
		perror("fopen");
		exit(1);	// 如果安装目录里面也没有配置文件，那就只好告诉用户，无法运行了
	}

	fill_skin_config_str(fp,"skin_info","skin_name",skin_config.skin_info.skin_name);
	fill_skin_config_str(fp,"skin_info","skin_version",skin_config.skin_info.skin_version);
	fill_skin_config_str(fp,"skin_info","skin_author",skin_config.skin_info.skin_author);
	fill_skin_config_str(fp,"skin_info","shin_remark",skin_config.skin_info.shin_remark);
	
	skin_config.skin_font.font_size = fill_skin_config_int(fp,"skin_font","font_size");
	fill_skin_config_str(fp,"skin_font","font_en",skin_config.skin_font.font_en);
	fill_skin_config_str(fp,"skin_font","font_zh",skin_config.skin_font.font_zh);
	fill_skin_config_str(fp,"skin_font","input_char_color",skin_config.skin_font.input_char_color);
	fill_skin_config_str(fp,"skin_font","output_char_color",skin_config.skin_font.output_char_color);
	fill_skin_config_str(fp,"skin_font","char_no_color",skin_config.skin_font.char_no_color);
	fill_skin_config_str(fp,"skin_font","first_char_color",skin_config.skin_font.first_char_color);
	
	fill_skin_config_img(fp,"skin_main_bar","mbbg_img",&skin_config.skin_main_bar.mbbg_img);
	fill_skin_config_img(fp,"skin_main_bar","logo_img",&skin_config.skin_main_bar.logo_img);
	fill_skin_config_img(fp,"skin_main_bar","zhpunc_img",&skin_config.skin_main_bar.zhpunc_img);
	fill_skin_config_img(fp,"skin_main_bar","enpunc_img",&skin_config.skin_main_bar.enpunc_img);
	fill_skin_config_img(fp,"skin_main_bar","chs_img",&skin_config.skin_main_bar.chs_img);
	fill_skin_config_img(fp,"skin_main_bar","cht_img",&skin_config.skin_main_bar.cht_img);
	fill_skin_config_img(fp,"skin_main_bar","halfcorner_img",&skin_config.skin_main_bar.half_corner_img);
	fill_skin_config_img(fp,"skin_main_bar","fullcorner_img",&skin_config.skin_main_bar.full_corner_img);
	fill_skin_config_img(fp,"skin_main_bar","unlock_img",&skin_config.skin_main_bar.unlock_img);
	fill_skin_config_img(fp,"skin_main_bar","lock_img",  &skin_config.skin_main_bar.lock_img);
	fill_skin_config_img(fp,"skin_main_bar","lxoff_img",&skin_config.skin_main_bar.lxoff_img);
	fill_skin_config_img(fp,"skin_main_bar","lxon_img",&skin_config.skin_main_bar.lxon_img);
	fill_skin_config_img(fp,"skin_main_bar","vkhide_img",&skin_config.skin_main_bar.vkhide_img);
	fill_skin_config_img(fp,"skin_main_bar","vkshow_img",&skin_config.skin_main_bar.vkshow_img);
	fill_skin_config_img(fp,"skin_main_bar","english_img",&skin_config.skin_main_bar.english_img);
	fill_skin_config_img(fp,"skin_main_bar","pinyin_img",&skin_config.skin_main_bar.pinyin_img);
	fill_skin_config_img(fp,"skin_main_bar","shuangpin_img",&skin_config.skin_main_bar.shuangpin_img);
	fill_skin_config_img(fp,"skin_main_bar","quwei_img",&skin_config.skin_main_bar.quwei_img);
	fill_skin_config_img(fp,"skin_main_bar","wubi_img",&skin_config.skin_main_bar.wubi_img);
	fill_skin_config_img(fp,"skin_main_bar","mixpywb_img",&skin_config.skin_main_bar.mixpywb_img);
	fill_skin_config_img(fp,"skin_main_bar","erbi_img",&skin_config.skin_main_bar.erbi_img);
	fill_skin_config_img(fp,"skin_main_bar","cj_img",&skin_config.skin_main_bar.cj_img);
	fill_skin_config_img(fp,"skin_main_bar","wanfeng_img",&skin_config.skin_main_bar.wanfeng_img);
	fill_skin_config_img(fp,"skin_main_bar","bingchan_img",&skin_config.skin_main_bar.bingchan_img);
	fill_skin_config_img(fp,"skin_main_bar","ziran_img",&skin_config.skin_main_bar.ziran_img);
	fill_skin_config_img(fp,"skin_main_bar","dianbao_img",&skin_config.skin_main_bar.dianbao_img);
	fill_skin_config_img(fp,"skin_main_bar","otherim_img",&skin_config.skin_main_bar.otherim_img);
	
	fill_skin_config_img(fp,"skin_input_bar","ibbg_img",&skin_config.skin_input_bar.ibbg_img);
	skin_config.skin_input_bar.resize          =fill_skin_config_int(fp,"skin_input_bar","resize");
	skin_config.skin_input_bar.resize_pos=fill_skin_config_int(fp,"skin_input_bar","resize_pos");
	skin_config.skin_input_bar.resize_w=fill_skin_config_int(fp,"skin_input_bar","resize_w");
	skin_config.skin_input_bar.input_pos   =fill_skin_config_int(fp,"skin_input_bar","input_pos");
	skin_config.skin_input_bar.output_pos  =fill_skin_config_int(fp,"skin_input_bar","output_pos");
	skin_config.skin_input_bar.layout_left   =fill_skin_config_int(fp,"skin_input_bar","layout_left");
	skin_config.skin_input_bar.layout_right  =fill_skin_config_int(fp,"skin_input_bar","layout_right");
	fill_skin_config_str(fp,"skin_input_bar","cursor_color",skin_config.skin_input_bar.cursor_color);
	fill_skin_config_img(fp,"skin_input_bar","back_arrow_img",&skin_config.skin_input_bar.back_arrow_img);
	fill_skin_config_img(fp,"skin_input_bar","forward_arrow_img",&skin_config.skin_input_bar.forward_arrow_img);

	fill_skin_config_img(fp,"skin_tray_icon","active_img",&skin_config.skin_tray_icon.active_img);
	fill_skin_config_img(fp,"skin_tray_icon","inactive_img",&skin_config.skin_tray_icon.inactive_img);

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

int load_a_img(skin_img_t * img,cairo_surface_t ** png)
{
	char  buf[256]={0};

	sprintf(buf,"%s/%s",skin_path,img->img_name);
	//printf("%s ",buf);
	if( strlen(img->img_name) > 0 && strcmp( img->img_name ,"NONE.img") !=0)
	{
		*png=cairo_image_surface_create_from_png(buf);
		if( img->width ==0 || img->height== 0)
		{
			img->width=cairo_image_surface_get_width(*png);
			img->height=cairo_image_surface_get_height(*png);
			//printf("width:%d  height:%d\n",img->width,img->height);
			img->response_w=img->width;
			img->response_h=img->height;
		}
	}
	return 0;
}


void load_main_img()
{
	load_a_img( &skin_config.skin_main_bar.mbbg_img, &bar );
	load_a_img( &skin_config.skin_main_bar.logo_img, &logo);
	load_a_img( &skin_config.skin_main_bar.zhpunc_img, &punc[0]);
	load_a_img( &skin_config.skin_main_bar.enpunc_img, &punc[1]);
	load_a_img( &skin_config.skin_main_bar.chs_img, &chs_t[0]);
	load_a_img( &skin_config.skin_main_bar.cht_img, &chs_t[1]);
	load_a_img( &skin_config.skin_main_bar.half_corner_img, &corner[0]);
	load_a_img( &skin_config.skin_main_bar.full_corner_img, &corner[1]);
	load_a_img( &skin_config.skin_main_bar.unlock_img, &lock[0]);
	load_a_img( &skin_config.skin_main_bar.lock_img, &lock[1]);
	load_a_img( &skin_config.skin_main_bar.lxoff_img, &lx[0]);
	load_a_img( &skin_config.skin_main_bar.lxon_img, &lx[1]);
	load_a_img( &skin_config.skin_main_bar.vkhide_img, &vk[0]);
	load_a_img( &skin_config.skin_main_bar.vkshow_img, &vk[1]);
	load_a_img( &skin_config.skin_main_bar.english_img, &english);
	load_a_img( &skin_config.skin_main_bar.pinyin_img, &pinyin);
	load_a_img( &skin_config.skin_main_bar.shuangpin_img, &shuangpin);
	load_a_img( &skin_config.skin_main_bar.quwei_img, &quwei);
	load_a_img( &skin_config.skin_main_bar.wubi_img, &wubi);
	load_a_img( &skin_config.skin_main_bar.mixpywb_img, &mix);
	load_a_img( &skin_config.skin_main_bar.erbi_img, &erbi);
	load_a_img( &skin_config.skin_main_bar.cj_img, &cangji);
	load_a_img( &skin_config.skin_main_bar.wanfeng_img, &wanfeng);
	load_a_img( &skin_config.skin_main_bar.bingchan_img, &bingchan);
	load_a_img( &skin_config.skin_main_bar.ziran_img, &ziran);
	load_a_img( &skin_config.skin_main_bar.dianbao_img, &dianbao);
	load_a_img( &skin_config.skin_main_bar.otherim_img, &otherim);
}

void load_input_img()
{
	load_a_img( &skin_config.skin_input_bar.ibbg_img, &input);
	load_a_img( &skin_config.skin_input_bar.ibbg_img, &prev);
	load_a_img( &skin_config.skin_input_bar.ibbg_img, &next);
}

void load_tray_img()
{
    load_a_img( &skin_config.skin_tray_icon.active_img, &trayActive);
    load_a_img( &skin_config.skin_tray_icon.inactive_img, &trayInactive);
}
void distroy_a_img(cairo_surface_t ** png)
{
	if(png != NULL)
		cairo_surface_destroy(*png);
	*png=NULL;
}
void distroy_img()
{
	distroy_a_img(&bar);
	distroy_a_img(&logo);
	distroy_a_img(&punc[0]);
	distroy_a_img(&punc[1]);
	distroy_a_img(&corner[0]);	
	distroy_a_img(&corner[1]);	
	distroy_a_img(&lx[0]);
	distroy_a_img(&lx[1]);
	distroy_a_img(&chs_t[0]);	
	distroy_a_img(&chs_t[1]);	
	distroy_a_img(&lock[0]);
	distroy_a_img(&lock[1]);
	distroy_a_img(&vk[0]);
	distroy_a_img(&vk[1]);
	distroy_a_img(&english);
	distroy_a_img(&pinyin);
	distroy_a_img(&shuangpin);
	distroy_a_img(&quwei);
	distroy_a_img(&wubi);
	distroy_a_img(&mix);
	distroy_a_img(&erbi);
	distroy_a_img(&cangji);
	distroy_a_img(&wanfeng);
	distroy_a_img(&bingchan);
	distroy_a_img(&ziran);
	distroy_a_img(&dianbao);
	distroy_a_img(&otherim);
	
	distroy_a_img(&input);
	distroy_a_img(&prev);
	distroy_a_img(&next);
}

cairo_t *c ,*c2,*c3,*c4,*c5;
/**
*输入条的绘制非常注重效率,画笔在绘图过程中不释放
*/
void load_input_msg()
{
	cairo_color_t in_char,first_char,out_char,cursor_color;
	int font_size;
	
	font_size=skin_config.skin_font.font_size;
	
	convert_str2color(&in_char,skin_config.skin_font.input_char_color);
	convert_str2color(&out_char,skin_config.skin_font.output_char_color);
	convert_str2color(&first_char,skin_config.skin_font.first_char_color);
	convert_str2color(&cursor_color,skin_config.skin_input_bar.cursor_color);
//输入条背景图画笔
//	c=cairo_create(cs_input_bar);
	
//拼音画笔
	c2=cairo_create(cs_input_bar);
	cairo_set_source_rgb(c2, in_char.r, in_char.g, in_char.b);
	cairo_select_font_face(c2, skin_config.skin_font.font_zh,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(c2, font_size);


//第一个字画笔
	c3=cairo_create(cs_input_bar);
	cairo_set_source_rgb(c3,first_char.r, first_char.g, first_char.b);
	cairo_select_font_face(c3, skin_config.skin_font.font_zh,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(c3, font_size);

//其他字画笔	
	c4=cairo_create(cs_input_bar);
	cairo_set_source_rgb(c4, out_char.r, out_char.g, out_char.b);
	cairo_select_font_face(c4, skin_config.skin_font.font_zh,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(c4, font_size);

//光标画笔
	c5=cairo_create(cs_input_bar);
	cairo_set_source_rgb(c5, cursor_color.r, cursor_color.g, cursor_color.b);
	cairo_set_line_width (c5, 1);
}

void free_input_msg()
{
//	cairo_destroy(c);
	cairo_destroy(c2);
	cairo_destroy(c3);
	cairo_destroy(c4);
	cairo_destroy(c5);
}

void draw_a_img(cairo_t **c,skin_img_t img,cairo_surface_t * png)
{
/*	printf("%s=%d %d %d %d %d %d %d %d\n",img.img_name,img.position_x, img.position_y,img.width,img.height,
										                img.response_x,img.response_y,img.response_w,img.response_h);*/
	if( strlen(img.img_name) == 0 || strcmp( img.img_name ,"NONE.img") ==0)
		return;	

	cairo_set_source_surface(*c,png, img.position_x, img.position_y);
	cairo_paint(*c);
}

void draw_input_bar(char * up_str,char *first_str,char * down_str,unsigned int * iwidth)
{

	cairo_text_extents_t extents;
	int png_width,png_height;
	int repaint_times=0,remain_width=0;
	int input_bar_len=0;
	int resize_pos=0;
	int resize_w=0;
	int flag=0;
	int cursor_pos=0;
	int up_len,down_len;
	int down_str_pos;

	resize_pos=skin_config.skin_input_bar.resize_pos;
	resize_w=(skin_config.skin_input_bar.resize_w==0)?20:skin_config.skin_input_bar.resize_w;
	flag=skin_config.skin_input_bar.resize;
	
	cairo_text_extents(c2,up_str,&extents);
	up_len=(int)extents.width;
	cursor_pos=up_len+skin_config.skin_font.font_size+5;	

	cairo_text_extents(c3,first_str,&extents);
	down_len= (int)extents.width;
	down_str_pos=down_len+10;
	
	cairo_text_extents(c4,down_str,&extents);
	down_len+= (int)extents.width;
	
	input_bar_len=(up_len<down_len)?down_len:up_len;
	input_bar_len+=skin_config.skin_input_bar.layout_left+skin_config.skin_input_bar.layout_right;
	//输入条长度应该比背景图长度要长,比最大长度要短
	input_bar_len=(input_bar_len>skin_config.skin_input_bar.ibbg_img.width)?input_bar_len:skin_config.skin_input_bar.ibbg_img.width;
	input_bar_len=(input_bar_len>=INPUT_BAR_MAX_LEN)?INPUT_BAR_MAX_LEN:input_bar_len;
	*iwidth=input_bar_len;


	png_width=skin_config.skin_input_bar.ibbg_img.width;
	png_height=skin_config.skin_input_bar.ibbg_img.height;
		
	//重绘的次数
	repaint_times=(input_bar_len - png_width)/resize_w+1;
	//不够汇一次的剩余的长度
	remain_width=(input_bar_len - png_width)%resize_w;
	
	
	//把背景清空
	c=cairo_create(cs_input_bar);
	cairo_set_source_rgba(c, 0, 0, 0,0);
	cairo_rectangle (c, 0, 0, INPUT_BAR_MAX_LEN, skin_config.skin_input_bar.ibbg_img.height);
	//cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
	cairo_set_operator(c, CAIRO_OPERATOR_SOURCE);
	cairo_fill(c);
	
	//把cr设定位png图像,并保存
	cairo_set_source_surface(c, input, 0, 0);
	cairo_save(c);
	
	//画输入条第一部分(从开始到重复或者延伸段开始的位置)
	cairo_set_operator(c, CAIRO_OPERATOR_OVER);
	cairo_rectangle	(c,0,0,resize_pos,png_height);
	cairo_clip(c);
	cairo_paint(c);
	
	//再画输入条的第三部分(因为第二部分可变,最后处理会比较方便)
	cairo_restore(c);
	cairo_save(c);
	cairo_set_operator(c, CAIRO_OPERATOR_OVER);
	cairo_translate(c, input_bar_len-(png_width-resize_pos-resize_w), 0);
	//printf("%d %d %d\n",input_bar_len,png_width-resize_pos-resize_w,input_bar_len-(png_width-resize_pos-resize_w));
	cairo_set_source_surface(c, input, -resize_pos-resize_w, 0);
	cairo_rectangle	(c,0,0,png_width-resize_pos-resize_w,png_height);
	cairo_clip(c);
	cairo_paint(c);

	//画第二部分,智能变化有两种方式
	if( flag == 1) //
	{
		int i=0;
			
		//先把整段的都绘上去
		for(i=0;i<repaint_times;i++)
		{
			cairo_restore(c);
			cairo_save(c);
			cairo_translate(c,resize_pos+i*resize_w, 0);
			cairo_set_source_surface(c, input,-resize_pos, 0);
			cairo_set_operator(c, CAIRO_OPERATOR_OVER);
			cairo_rectangle	(c,0,0,resize_w,png_height);
			cairo_clip(c);
			cairo_paint(c);
		}
		
	//绘制剩余段
		if(remain_width != 0)
		{
			cairo_restore(c);
			cairo_translate(c,resize_pos+resize_w*repaint_times, 0);
			cairo_set_source_surface(c, input,-resize_pos, 0);
			cairo_set_operator(c, CAIRO_OPERATOR_OVER);
			cairo_rectangle	(c,0,0,remain_width,png_height);
			cairo_clip(c);
			cairo_paint(c);
		}
	}
	else if( flag == 0)
	{
		cairo_restore(c);
		cairo_translate(c,resize_pos, 0);
		cairo_scale(c, (double)(input_bar_len-png_width+resize_w)/(double)resize_w,1);
		//printf("%f\n",(double)(input_bar_len-png_width+resize_w)/(double)resize_w);
		cairo_set_source_surface(c, input,-resize_pos, 0);
		cairo_set_operator(c, CAIRO_OPERATOR_OVER);
		cairo_rectangle	(c,0,0,resize_w,png_height);
		cairo_clip(c);
		cairo_paint(c);
	}
	else
	{
		cairo_restore(c);
		cairo_translate(c,resize_pos, 0);
		cairo_set_source_surface(c, input,-resize_pos, 0);
		cairo_set_operator(c, CAIRO_OPERATOR_OVER);
		cairo_rectangle	(c,0,0,resize_w,png_height);
		cairo_clip(c);
		cairo_paint(c);
	}
	
	cairo_move_to(c2,skin_config.skin_input_bar.layout_left,skin_config.skin_input_bar.input_pos);
	cairo_show_text(c2, up_str); 

 	cairo_move_to(c3, skin_config.skin_input_bar.layout_left,skin_config.skin_input_bar.output_pos);
	cairo_show_text(c3, first_str);
	
	cairo_move_to(c4, skin_config.skin_input_bar.layout_left+down_str_pos,skin_config.skin_input_bar.output_pos);
	cairo_show_text(c4, down_str);
	
	//画光标
	if(bShowCursor )
	{
		cairo_move_to(c5,cursor_pos,skin_config.skin_input_bar.input_pos+2);
		cairo_line_to(c5,cursor_pos,skin_config.skin_input_bar.input_pos-skin_config.skin_font.font_size);
		cairo_stroke(c5);
	}
	
	cairo_destroy(c);
}



void DisplaySkin(char * skinname)
{
	memset(skinType,0,sizeof(skinType));//buf长度太短，注意改进
	strcpy(skinType,skinname);

	XUnmapWindow (dpy, mainWindow);
	XUnmapWindow (dpy, inputWindow);
	
	free_input_msg();
	distroy_img();

	XDestroyWindow(dpy, mainWindow);
	XDestroyWindow(dpy, inputWindow);
	
	load_skin_config();
	CreateMainWindow ();
	CreateInputWindow ();

	DrawMainWindow ();
	DrawInputWindow ();
	
	XMapRaised(dpy,mainWindow);
}

//图片文件加载函数完成
/*-------------------------------------------------------------------------------------------------------------*/
//skin目录下读入skin的文件夹名

int loadSkinDir()
{
	DIR * dir;
	struct dirent * drt;
	struct stat file_stat;
	char pathbuf[128]={0};	
	int i=0;
	skinCount=0;

	for(i=0;i<10;i++)
	{
		memset(&skinBuf[i],0,sizeof(skin_dir_t));
	}

	sprintf(pathbuf, "%s/skin/", PKGDATADIR);

	dir= opendir(pathbuf);

	if (dir == NULL)
	{	
		perror("can't open the dir");
		return -1;
	}
	
	//strcpy(skinBuf[0].dirbase,"default");
	i=0;
	while((drt = readdir(dir)) != NULL)
	{
		memset(pathbuf,0,sizeof(pathbuf));
		if( strcmp( drt->d_name, ".") == 0 || strcmp(drt->d_name,"..") == 0)
			continue;
		sprintf(pathbuf,"%s/skin/%s",PKGDATADIR,drt->d_name);
		//printf("filename:%s\n",pathbuf);
		if( stat(pathbuf,&file_stat) == -1)
		{
			printf("can't get dirinfo\n");
			//return -1;
		}
	    //如果是目录的话则读入到dirbuf中
	
		if ( file_stat.st_mode & S_IFDIR)
		{	
			
			if (strlen(pathbuf) != 0)
			{	
				strcpy(skinBuf[i].dirbuf,pathbuf);
				strcpy(skinBuf[i].dirbase,drt->d_name);
				i++;
				//printf("*******%s\n",pathbuf);
			}	
		}	
	}
	
	
	for(i=0 ;i<10;i++)
	{	
		if(strlen(skinBuf[i].dirbuf) == 0)
			continue;
		skinCount++;	
	}
	//printf("skinCount:%d \n",skinCount);
	closedir(dir);

	return 0;
}


