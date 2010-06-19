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
    FcResult	result;

    if (strUserLocale[0])
        setlocale (LC_CTYPE, strUserLocale);
    else
        setlocale (LC_CTYPE, "zh_CN.UTF-8");
reloadfont:
    if (strcmp(skin_config.skin_font.font_zh, "") == 0)
        pat = FcPatternCreate ();
    else
    {
        pat = FcNameParse ((FcChar8*)skin_config.skin_font.font_zh);
    }
    FcConfigSubstitute (0, pat, FcMatchPattern);
    FcDefaultSubstitute (pat);
    fs = FcFontSetCreate ();
    
    FcPattern   *match;
    match = FcFontMatch (0, pat, &result);
    if (match)
        FcFontSetAdd (fs, match);
    else
        goto nofont;
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
        if (fs)
            FcFontSetDestroy(fs);

        goto reloadfont;
    }
        
    fprintf(stderr, "no valid font.");
    exit(1);
}
