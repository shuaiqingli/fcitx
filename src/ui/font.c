#include <fontconfig/fontconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui/skin.h"
#include "tools/util.h"

extern char strUserLocale[];

void InitFont()
{
    if (!FcInit())
    {
        FcitxLog(FATAL, _("Error: Load fontconfig failed"));
        exit(1);
    }
}

void CreateFont()
{
    FcFontSet	*fs = NULL;
    FcPattern   *pat = NULL;
    FcObjectSet *os = NULL;

    char locale[3];

    if (strUserLocale[0])
        strncpy(locale, strUserLocale, 2);
    else
        strcpy(locale, "zh");
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
    os = NULL;
    
    FcPatternDestroy(pat);
    pat = NULL;

    if (!fs || fs->nfont <= 0)
        goto nofont;

    FcChar8* family;
    if (FcPatternGetString (fs->fonts[0], FC_FAMILY, 0, &family) != FcResultMatch)
        goto nofont;
    strcpy(skin_config.skin_font.font_zh, (const char*) family);

    FcFontSetDestroy(fs);

    FcitxLog(INFO, _("your current font is: %s"), skin_config.skin_font.font_zh);

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
        
    FcitxLog(FATAL, _("no valid font."));
    exit(1);
}
