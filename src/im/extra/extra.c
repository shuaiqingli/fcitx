/*!
 * \file extra.c
 * \author dgod
 */

#include "im/extra/extra.h"
#include "ui/InputWindow.h"
#include "fcitx-config/configfile.h"
#include "fcitx-config/profile.h"

#include <dlfcn.h>
#include <limits.h>
#include <iconv.h>

#define EIM_MAX		4
extern Display *dpy;

static EXTRA_IM *EIMS[EIM_MAX];
static void *EIM_handle[EIM_MAX];
static char EIM_file[EIM_MAX][PATH_MAX];
static INT8 EIM_index[EIM_MAX];

static char CandTableEngine[10][MAX_CAND_LEN+1];
static char CodeTipsEngine[10][MAX_TIPS_LEN+1];
static char StringGetEngine[MAX_USER_INPUT+1];

extern INT8 iIMCount;
extern INT8 iInCap;

extern int      iCandWordCount;
extern int      iCandPageCount;
extern int	iCurrentCandPage;
extern char     strStringGet[];
extern char     strCodeInput[];
extern int      iCodeInputCount;
extern Bool	bCursorAuto;

static void ResetAll(void)
{
	int i;
	for(i=0;i<EIM_MAX;i++)
	{
		if(EIMS[i])
		{
			if(EIMS[i]->Destroy)
				EIMS[i]->Destroy();
			EIMS[i]=NULL;
		}
		if(EIM_handle[i])
		{
			dlclose(EIM_handle[i]);
			EIM_handle[i]=NULL;
		}
		if(EIM_file[i][0])
		{
			EIM_file[i][0]=0;
		}
		EIM_index[i]=0;
	}
}

static void ExtraReset(void)
{
	int i;
	EXTRA_IM *eim=NULL;
	for(i=0;i<EIM_MAX;i++)
	{
		if(EIM_index[i]==gs.iIMIndex)
		{
			eim=EIMS[i];
			break;
		}
	}
	inputWindow.bShowCursor=False;
	bCursorAuto=False;
	if(!eim) return;
	if(eim->CandWordMax) eim->CandWordMax=fc.iMaxCandWord;
	if(eim->Reset) eim->Reset();
	eim->StringGet[0]=0;
	eim->CodeInput[0]=0;
	eim->CaretPos=-1;
	eim->CandWordMax=fc.iMaxCandWord;
	eim->Reset();
}

static void DisplayEIM(EXTRA_IM *im)
{
	int i;
	char strTemp[3];

	if ( fc.bPointAfterNumber )
	{
		strTemp[1] = '.';
		strTemp[2] = '\0';
	}
	else strTemp[1]='\0';

	SetMessageCount(&messageDown, 0);
	for (i = 0; i < im->CandWordCount; i++)
	{
		strTemp[0] = i + 1 + '0';
		if (i == 9) strTemp[0] = '0';
        AddMessageAtLast(&messageDown, MSG_INDEX, strTemp);

        AddMessageAtLast(&messageDown, (i!=im->SelectIndex)? MSG_OTHER:MSG_FIRSTCAND, im->CandTable[i]);
		if(im->CodeTips && im->CodeTips[i] && im->CodeTips[i][0])
            AddMessageAtLast(&messageDown, MSG_CODE, im->CodeTips[i]);
		if (i != 9)
            MessageConcatLast(&messageDown, " ");
	}

	SetMessageCount(&messageUp, 0);
	if(im->StringGet[0] || im->CodeInput[0])
	{
        AddMessageAtLast(&messageUp, MSG_INPUT, "%s%s", im->StringGet, im->CodeInput);

		inputWindow.bShowCursor=True;
		iCodeInputCount=strlen(im->CodeInput);
		if(im->CaretPos!=-1)
			iCursorPos=im->CaretPos;
		else
			iCursorPos=strlen(im->StringGet) + strlen(im->CodeInput);
		bCursorAuto=True;
	}

	iCandWordCount=im->CandWordCount;
	iCandPageCount=im->CandPageCount;
	iCurrentCandPage=im->CurCandPage;
	if(iCandPageCount) iCandPageCount--;
}

static INPUT_RETURN_VALUE ExtraDoInput(unsigned int sym, unsigned int state, int count)
{
	int i;
	EXTRA_IM *eim=NULL;
	INPUT_RETURN_VALUE ret=IRV_DO_NOTHING;
	for(i=0;i<EIM_MAX;i++)
	{
		if(EIM_index[i]==gs.iIMIndex)
		{
			eim=EIMS[i];
			break;
		}
	}
	if(!eim) return IRV_TO_PROCESS;
	if(eim->DoInput)
		ret=eim->DoInput(sym, state, count);

    unsigned int istate;
    unsigned int key;
    
    istate = state - (state & KEY_NUMLOCK) - (state & KEY_CAPSLOCK) - (state & KEY_SCROLLLOCK);
	if(ret==IRV_GET_CANDWORDS||ret==IRV_GET_CANDWORDS_NEXT)
	{
		strcpy(strStringGet,eim->StringGet);
		eim->StringGet[0]=0;
		if(ret==IRV_GET_CANDWORDS_NEXT)
		{
			DisplayEIM(eim);
		}
		else
		{
			SetMessageCount(&messageDown, 0);
			SetMessageCount(&messageUp, 0);
		}
	}
	else if(ret==IRV_DO_NOTHING && (eim->CandWordCount ||
		eim->StringGet[0] || eim->CodeInput[0]))
	{
		DisplayEIM(eim);
		ret=IRV_DISPLAY_CANDWORDS;
	}
	else if(ret==IRV_DISPLAY_CANDWORDS)
	{
		DisplayEIM(eim);
	}
	else if(ret==IRV_ENG)
	{
		iCodeInputCount=strlen(strCodeInput);
		iInCap=3;
		ret=IRV_DONOT_PROCESS;
	}

	return ret;
}

static INPUT_RETURN_VALUE ExtraGetCandWords(SEARCH_MODE sm)
{
	int i;
	EXTRA_IM *eim=NULL;
	INPUT_RETURN_VALUE ret=IRV_DO_NOTHING;
	for(i=0;i<EIM_MAX;i++)
	{
		if(EIM_index[i]==gs.iIMIndex)
		{
			eim=EIMS[i];
			break;
		}
	}
	if(!eim) return IRV_TO_PROCESS;
	if(eim->GetCandWords)
		ret=eim->GetCandWords(sm);
	if(ret==IRV_DISPLAY_CANDWORDS)
		DisplayEIM(eim);
	return ret;
}

static char *ExtraGetCandWord(int index)
{
	int i;
	EXTRA_IM *eim=NULL;
	for(i=0;i<EIM_MAX;i++)
	{
		if(EIM_index[i]==gs.iIMIndex)
		{
			eim=EIMS[i];
			break;
		}
	}
	if(!eim) return 0;
	if(eim->GetCandWord)
	{
		SetMessageCount(&messageDown, 0);
        SetMessageCount(&messageUp, 0);
		return eim->GetCandWord(index);
	}
	return 0;
}

char *ExtraGetPath(char *type)
{
	static char ret[256];
	ret[0]=0;
	if(!strcmp(type,"DATA"))
	{
		strcpy(ret,PKGDATADIR"/data");
	}
	else if(!strcmp(type,"LIB"))
	{
		strcpy(ret,LIBDIR);
	}
	else if(!strcmp(type,"BIN"))
	{
		strcpy(ret,PKGDATADIR"/../../bin");
	}
	return ret;
}

/* the result is slow, why? */
char *GetClipboardString(Display *disp)
{
	Atom sel;
	Window w;
	int ret;
	Atom target;
	Atom type;
	int fmt;
	unsigned long n,after;
	unsigned char *pret=0;
	static char result[1024];

	sel = XInternAtom(disp, "PRIMARY", 0);
	target = XInternAtom(disp,"UTF8_STRING",0);
	w=XGetSelectionOwner(disp,sel);
	if(w==None)
	{
		//printf("None owner\n");
		return NULL;
	}
	XConvertSelection(disp,sel,target,sel,w,CurrentTime);
	ret=XGetWindowProperty(disp,w,sel,0,1023,False,target,&type,&fmt,&n,&after,&pret);
	if(ret!=Success || !pret || fmt!=8)
	{
		//printf("Error %d\n",ret);
		return NULL;
	}
	memcpy(result,pret,n);
	XFree(pret);
	result[n]=0;
	return result;
}

int InitExtraIM(EXTRA_IM *eim,char *arg)
{
	eim->CodeInput=strCodeInput;
	eim->StringGet=StringGetEngine;
	eim->CandTable=CandTableEngine;
	eim->CodeTips=CodeTipsEngine;
	eim->GetPath=ExtraGetPath;
	eim->CandWordMax=fc.iMaxCandWord;
	eim->CaretPos=-1;
    eim->fc = (void*)&fc;
    eim->profile = (void*)&fcitxProfile;

	if(eim->Init(arg))
	{
		printf("eim: init fail\n");
		return -1;
	}

	return 0;
}

void LoadExtraIM(char *fn)
{
	void *handle;
	int i;
	EXTRA_IM *eim;
	char path[PATH_MAX];
	char temp[256];
	char *arg;
	char fnr[256];

	for(i=0;i<EIM_MAX;i++)
	{
		if(EIM_file[i][0] && !strcmp(EIM_file[i],path))
		{
			ResetAll();
			break;
		}
	}
	for(i=0;i<EIM_MAX;i++)
	{
		if(!EIMS[i])
		{
			break;
		}
	}
	if(i==EIM_MAX)
	{
		printf("eim: no space\n");
		return;
	}

	strcpy(fnr,fn);
	arg=strstr(fnr,".so ");
	if(arg)
	{
		arg[3]=0;
		arg+=4;
	}
	if(fnr[0]=='~' && fnr[1]=='/')
		sprintf(temp,"%s/%s",getenv("HOME"),fnr+2);
	else if(strchr(fnr,'/'))
		strcpy(temp,fnr);
	else
		sprintf(temp,"%s/%s",ExtraGetPath("LIB"),fnr);

	handle=dlopen(temp,RTLD_LAZY);

	if(!handle)
	{
		printf("eim: open %s fail %s\n",temp,dlerror());
		return;
	}
	eim=dlsym(handle,"EIM");
	if(!eim || !eim->Init)
	{
		printf("eim: bad im\n");
		dlclose(handle);
		return;
	}
	if(InitExtraIM(eim,arg))
	{
		dlclose(handle);
		return;
	}
	RegisterNewIM(eim->Name, eim->IconName,ExtraReset,ExtraDoInput,ExtraGetCandWords,ExtraGetCandWord,NULL,NULL,NULL,NULL);
	EIMS[i]=eim;
	EIM_handle[i]=handle;
	strcpy(EIM_file[i],fn);
	EIM_index[i]=iIMCount-1;
}
