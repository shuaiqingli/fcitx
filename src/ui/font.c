#include <fontconfig/fontconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui/skin.h"

extern char strUserLocale[];

void InitFont()
{
    if (!FcInit())
    {
        fprintf(stderr, "Error: Load fontconfig failed");
        exit(1);
    }
}

void CreateFont()
{
    FcFontSet	*fs;
    FcPattern   *pat;
    FcObjectSet *os;

    char locale[3];

    if (strUserLocale[0])
    {
        strncpy(locale, strUserLocale, 2);
        setlocale (LC_CTYPE, strUserLocale);
    }
    else
    {
        strcpy(locale, "zh");
        setlocale (LC_CTYPE, "zh_CN.UTF-8");
    }
    locale[2]='\0';
reloadfont:
    if (strcmp(skin_config.skin_font.font_zh, "") == 0)
    {
        FcChar8 strpat[9];
        sprintf((char*)strpat, ":lang=%s", locale);
        pat = FcNameParse(strpat);
    }
    else
    {
        pat = FcNameParse ((FcChar8*)skin_config.skin_font.font_zh);
    }
    
    os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, (char*)0);
    fs = FcFontList(0, pat, os);
    if (os)
        FcObjectSetDestroy(os);
    
    FcPatternDestroy(pat);

    if (!fs || fs->nfont <= 0)
        goto nofont;

    FcChar8* family;
    if (FcPatternGetString (fs->fonts[0], FC_FAMILY, 0, &family) != FcResultMatch)
        goto nofont;
    strcpy(skin_config.skin_font.font_zh, (const char*) family);

    FcFontSetDestroy(fs);

    setlocale (LC_CTYPE, "");
    return;

nofont:
    if (strcmp(skin_config.skin_font.font_zh, "") != 0)
    {
        strcpy(skin_config.skin_font.font_zh, "");
        if (pat)
            FcPatternDestroy(pat);
        if (os)
            FcObjectSetDestroy(os);
        if (fs)
            FcFontSetDestroy(fs);

        goto reloadfont;
    }
        
    fprintf(stderr, "no valid font.");
    exit(1);
}
