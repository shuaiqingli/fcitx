#ifndef FCITX_CONF_H
#define FCITX_CONF_H

#include "fcitx-config/fcitx-config.h"

typedef struct FcitxConfig
{
    GenericConfig gconfig;
    /* program config */
    char *strFontName;
    char *strFontEnName;
    int iFontSize;
    int iMainWindowFontSize;
    int strUserLocale;
    Bool bUseBold;
    char *strRecordingPath;
#ifdef _ENABLE_TRAY
    Bool bUseTrayIcon;
#endif
#ifdef _ENABLE_DBUS
    Bool bUseDBus;
#endif
    /* output config */
    Bool bEngPuncAfterNumber;
    int enterToDo;
    int semicolonToDo;
    Bool bEngAfterCap;
    Bool bConvertPunc;
    Bool bDisablePagingInLegend;

    /* interface config */
    int iMaxCandWord;
    Bool bMainWindow3DEffect;
    Bool bInputWindow3DEffect;
    int hideMainWindow;
    Bool bShowVK;
    Bool bCenterInputWindow;
    Bool bShowInputWindowTriggering;
    int iFixedInputWindowWidth;
    int iOffsetX;
    int iOffsetY;
    Bool bPointAfterNumber;
    Bool bShowVersion;
    XColor cursorColor;
    XColor mainWindowColor;
    XColor mainWindowLineColor;
    XColor mainWindowInputMethodNameColor;
    XColor inputWindowColor;
    XColor messageColor[7];
    XColor inputWindowLineColor;
    XColor arrowColor;
    XColor VKWindowColor;
    XColor VKWindowAlphaColor;
    XColor VKWindowFontColor;

    /* hotkey config */
    HOTKEYS hkTriggerInputMethod;
    HOTKEYS hkChnEngEngSwitch;
    HOTKEYS bDoubleSwitchKey;
    HOTKEYS iTimeInterval;
    HOTKEYS hkCursorFollow;
    HOTKEYS hkHideMainWindow;
    HOTKEYS hkSwitchVK;
    HOTKEYS hkGBKTradSwitch;
    HOTKEYS hkLegend;
    HOTKEYS hkLookupPinyin;
    HOTKEYS hkChnPunc;
    HOTKEYS hkPrev;
    HOTKEYS hkNext;
    HOTKEYS hk2nd3rdCand;
    HOTKEYS hkSaveAll;
#ifdef _ENABLE_RECORDING
    HOTKEYS hkSetRecording;
    HOTKEYS hkResetRecoding;
#endif

    /* im config */
    Bool bUsePY;
    char *strNameOfPY;
    Bool bUseSP;
    char *strNameOfSP;
    char *strDefaultSP;
    Bool bUseQW;
    Bool bUseTable;
    Bool bPhraseTips;
    char *strExternIM;

    /* py config */
    Bool bFullPY;
    Bool bPYCreateAuto;
    Bool bPYSaveAutoAsPhrase;
    HOTKEYS hkAddPYFreqWord;
    HOTKEYS hkDeletePYFreqWord;
    HOTKEYS hkDeletePYUserPhrase;
    HOTKEYS hkPYGetWordFromPhrase;
    int baseOrder;
    int phraseOrder;
    int freqOrder;
    Bool bMHPY_C[6];
    Bool bMHPY_S[5];
} FcitxConfig;

#endif
