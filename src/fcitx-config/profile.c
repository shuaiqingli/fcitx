#include <errno.h>

#include "core/fcitx.h"
#include "fcitx-config/fcitx-config.h"
#include "fcitx-config/profile.h"
#include "tools/util.h"

FcitxProfile fcitxProfile;
ConfigFileDesc* fcitxProfileDesc = NULL;

CONFIG_BINDING_BEGIN(FcitxProfile)
CONFIG_BINDING_REGISTER("Profile", "MainWindowOffsetX",  iMainWindowOffsetX)
CONFIG_BINDING_REGISTER("Profile", "MainWindowOffsetY", iMainWindowOffsetY)
CONFIG_BINDING_REGISTER("Profile", "InputWindowOffsetX", iInputWindowOffsetX)
CONFIG_BINDING_REGISTER("Profile", "InputWindowOffsetY", iInputWindowOffsetY)
CONFIG_BINDING_REGISTER("Profile", "Corner", bCorner)
CONFIG_BINDING_REGISTER("Profile", "ChnPunc", bChnPunc)
CONFIG_BINDING_REGISTER("Profile", "TrackCursor", bTrackCursor)
CONFIG_BINDING_REGISTER("Profile", "UseLegend", bUseLegend)
CONFIG_BINDING_REGISTER("Profile", "IMIndex", iIMIndex)
CONFIG_BINDING_REGISTER("Profile", "Locked", bLocked)
CONFIG_BINDING_REGISTER("Profile", "CompactMainWindow", bCompactMainWindow)
CONFIG_BINDING_REGISTER("Profile", "UseGBKT", bUseGBKT)
#ifdef _ENABLE_RECORDING
CONFIG_BINDING_REGISTER("Profile", "Recording", bRecording)
#endif
CONFIG_BINDING_END()

/** 
 * @brief 加载配置文件
 */
void LoadProfile(void)
{
    FILE *fp;
    fp = GetXDGFileConfig( "profile", "rt");
    if (!fp) {
        if (errno == ENOENT)
            SaveProfile();
        return;
    }

    ConfigFileDesc* profileDesc = GetProfileDesc();
    ConfigFile *cfile = ParseConfigFileFp(fp, profileDesc);
    
    FcitxProfileConfigBind(&fcitxProfile, cfile, profileDesc);
    ConfigBindSync((GenericConfig*)&fcitxProfile);

    fclose(fp);
    if (fcitxProfile.gconfig.configFile)
        SaveProfile();
}

ConfigFileDesc* GetProfileDesc()
{
    FcitxLock();
    if (!fcitxProfileDesc)
    {
        FILE *tmpfp;
        tmpfp = GetXDGFileData("profile.desc", "r");
        fcitxProfileDesc = ParseConfigFileDescFp(tmpfp);
		fclose(tmpfp);
    }
    FcitxUnlock();

    return fcitxProfileDesc;
}

void SaveProfile(void)
{
    ConfigFileDesc* profileDesc = GetProfileDesc();
    FcitxLock();
    FILE* fp = GetXDGFileConfig("profile", "wt");
    SaveConfigFileFp(fp, fcitxProfile.gconfig.configFile, profileDesc);
    fclose(fp);
    FcitxUnlock();
}
