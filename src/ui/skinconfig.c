
#include "ui/skin.h"
#include "ui/MenuWindow.h"
#include "fcitx-config/fcitx-config.h"

CONFIG_BINDING_BEGIN(FcitxSkin);
CONFIG_BINDING_REGISTER("SkinInfo","Name",skinInfo.skinName);
CONFIG_BINDING_REGISTER("SkinInfo","Version",skinInfo.skinVersion);
CONFIG_BINDING_REGISTER("SkinInfo","Author",skinInfo.skinAuthor);
CONFIG_BINDING_REGISTER("SkinInfo","Desc",skinInfo.skinDesc);
	
CONFIG_BINDING_REGISTER("SkinFont","FontSize",skinFont.fontSize);
CONFIG_BINDING_REGISTER("SkinFont","TipColor",skinFont.fontColor[MSG_TIPS]);
CONFIG_BINDING_REGISTER("SkinFont","InputColor",skinFont.fontColor[MSG_INPUT]);
CONFIG_BINDING_REGISTER("SkinFont","IndexColor",skinFont.fontColor[MSG_INDEX]);
CONFIG_BINDING_REGISTER("SkinFont","UserPhraseColor",skinFont.fontColor[MSG_USERPHR]);
CONFIG_BINDING_REGISTER("SkinFont","FirstCandColor",skinFont.fontColor[MSG_FIRSTCAND]);
CONFIG_BINDING_REGISTER("SkinFont","CodeColor",skinFont.fontColor[MSG_CODE]);
CONFIG_BINDING_REGISTER("SkinFont","OtherColor",skinFont.fontColor[MSG_OTHER]);
CONFIG_BINDING_REGISTER("SkinFont","ActiveMenuColor",skinFont.menuFontColor[MENU_ACTIVE]);
CONFIG_BINDING_REGISTER("SkinFont","InactiveMenuColor",skinFont.menuFontColor[MENU_INACTIVE]);
	
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
CONFIG_BINDING_REGISTER("SkinInputBar","BackArrow",skinInputBar.backArrow);
CONFIG_BINDING_REGISTER("SkinInputBar","ForwardArrow",skinInputBar.forwardArrow);

CONFIG_BINDING_REGISTER("SkinTrayIcon","Active",skinTrayIcon.active);
CONFIG_BINDING_REGISTER("SkinTrayIcon","Inactive",skinTrayIcon.inactive);

CONFIG_BINDING_REGISTER("SkinMenu", "BackImg", skinMenu.backImg);
CONFIG_BINDING_REGISTER("SkinMenu", "Resize", skinMenu.resize);
CONFIG_BINDING_REGISTER("SkinMenu", "MarginTop", skinMenu.marginTop);
CONFIG_BINDING_REGISTER("SkinMenu", "MarginBottom", skinMenu.marginBottom);
CONFIG_BINDING_REGISTER("SkinMenu", "MarginLeft", skinMenu.marginLeft);
CONFIG_BINDING_REGISTER("SkinMenu", "MarginRight", skinMenu.marginRight);
CONFIG_BINDING_REGISTER("SkinMenu", "ActiveColor", skinMenu.activeColor);
CONFIG_BINDING_REGISTER("SkinMenu", "LineColor", skinMenu.lineColor);

CONFIG_BINDING_END()

