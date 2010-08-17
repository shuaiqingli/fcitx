
#include "ui/skin.h"
#include "fcitx-config/fcitx-config.h"

CONFIG_BINDING_BEGIN(FcitxSkin);
CONFIG_BINDING_REGISTER("SkinInfo","Name",skinInfo.skinName);
CONFIG_BINDING_REGISTER("SkinInfo","Version",skinInfo.skinVersion);
CONFIG_BINDING_REGISTER("SkinInfo","Author",skinInfo.skinAuthor);
CONFIG_BINDING_REGISTER("SkinInfo","Desc",skinInfo.shinDesc);
	
CONFIG_BINDING_REGISTER("SkinFont","FontSize",skinFont.fontSize);
CONFIG_BINDING_REGISTER("SkinFont","FontEn",skinFont.fontEn);
CONFIG_BINDING_REGISTER("SkinFont","FontZh",skinFont.fontZh);
CONFIG_BINDING_REGISTER("SkinFont","InputCharColor",skinFont.inputCharColor);
CONFIG_BINDING_REGISTER("SkinFont","OutputCharColor",skinFont.outputCharColor);
CONFIG_BINDING_REGISTER("SkinFont","CharNoColor",skinFont.charNoColor);
CONFIG_BINDING_REGISTER("SkinFont","FirstCharColor",skinFont.firstCharColor);
	
CONFIG_BINDING_REGISTER("SkinMainBar","BackImg",skinMainBar.backImg);
CONFIG_BINDING_REGISTER("SkinMainBar","Logo",skinMainBar.logo);
CONFIG_BINDING_REGISTER("SkinMainBar","ZhPunc",skinMainBar.zhpunc);
CONFIG_BINDING_REGISTER("SkinMainBar","EnPunc",skinMainBar.enpunc);
CONFIG_BINDING_REGISTER("SkinMainBar","Chs",skinMainBar.chs);
CONFIG_BINDING_REGISTER("SkinMainBar","Cht",skinMainBar.cht);
CONFIG_BINDING_REGISTER("SkinMainBar","HalfCorner",skinMainBar.halfcorner);
CONFIG_BINDING_REGISTER("SkinMainBar","FullCorner",skinMainBar.fullcorner);
CONFIG_BINDING_REGISTER("SkinMainBar","Unlock",skinMainBar.unlock);
CONFIG_BINDING_REGISTER("SkinMainBar","Lock",  skinMainBar.lock);
CONFIG_BINDING_REGISTER("SkinMainBar","Legend",skinMainBar.legend);
CONFIG_BINDING_REGISTER("SkinMainBar","NoLegend",skinMainBar.nolegend);
CONFIG_BINDING_REGISTER("SkinMainBar","VK",skinMainBar.vk);
CONFIG_BINDING_REGISTER("SkinMainBar","NoVK",skinMainBar.novk);
CONFIG_BINDING_REGISTER("SkinMainBar","Eng",skinMainBar.eng);
CONFIG_BINDING_REGISTER("SkinMainBar","Chn",skinMainBar.chn);
CONFIG_BINDING_REGISTER("SkinInputBar","BackImg",skinInputBar.backImg);
CONFIG_BINDING_REGISTER("SkinInputBar","Resize", skinInputBar.resize);
CONFIG_BINDING_REGISTER("SkinInputBar","ResizePos", skinInputBar.resizePos);
CONFIG_BINDING_REGISTER("SkinInputBar","ResizeWidth", skinInputBar.resizeWidth);
CONFIG_BINDING_REGISTER("SkinInputBar","InputPos", skinInputBar.inputPos);
CONFIG_BINDING_REGISTER("SkinInputBar","OutputPos",skinInputBar.outputPos);
CONFIG_BINDING_REGISTER("SkinInputBar","LayoutLeft", skinInputBar.layoutLeft);
CONFIG_BINDING_REGISTER("SkinInputBar","LayoutRight", skinInputBar.layoutRight);
CONFIG_BINDING_REGISTER("SkinInputBar","CursorColor",skinInputBar.cursorColor);
CONFIG_BINDING_REGISTER("SkinInputBar","BackArrowImg",skinInputBar.backArrow);
CONFIG_BINDING_REGISTER("SkinInputBar","ForwardArrowImg",skinInputBar.forwardArrow);

CONFIG_BINDING_REGISTER("SkinTrayIcon","Active",skinTrayIcon.active);
CONFIG_BINDING_REGISTER("SkinTrayIcon","Inactive",skinTrayIcon.inactive);
CONFIG_BINDING_END()

