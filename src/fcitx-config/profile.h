#ifndef FCITX_PROFILE_H
#define FCITX_PROFILE_H

#include "config.h"

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
#ifdef _ENABLE_RECORDING
    Bool bRecording;
#endif
} FcitxProfile;

extern FcitxProfile fcitxProfile;

void LoadProfile();
void SaveProfile();

#endif
