#ifndef FCITX_PROFILE_H
#define FCITX_PROFILE_H

#include <fcitx-config/fcitx-config.h>

typedef struct FcitxProfile
{
    GenericConfig gconfig;
    int iMainWindowOffsetX;
    int iMainWindowOffsetY;
    int iInputWindowOffsetX;
    int iInputWindowOffsetY;
    Bool bCorner;
    Bool bChnPunc;
    Bool bTrackCursor;
    Bool bUseLegend;
    int iIMIndex;
    Bool bLocked;
    Bool bCompactMainWindow;
    Bool bUseGBKT;
    Bool bRecording;
} FcitxProfile;

extern FcitxProfile fcitxProfile;

void LoadProfile();
void SaveProfile();

#endif
