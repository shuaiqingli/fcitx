/***************************************************************************
 *   Copyright (C) 20010~2010 by CSSlayer                                  *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/** 
 * @file util.c
 * @brief 通用函数
 * @author CSSlayer
 * @version 4.0.0
 * @date 2010-05-02
 */
#include "core/fcitx.h"

#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <pthread.h>
#include <sys/stat.h>
#include <libgen.h>

#include "interface/DBus.h"

#include "tools/util.h"
#include "fcitx-config/sprintf.h"

pthread_mutex_t fcitxMutex;

/** 
 * @brief 去除字符串首末尾空白字符
 * 
 * @param s
 * 
 * @return malloc的字符串，需要free
 */
char *trim(char *s)
{
    register char *end;
    register char csave;
    
    while (isspace(*s))                 /* skip leading space */
        ++s;
    end = strchr(s,'\0') - 1;
    while (end >= s && isspace(*end))               /* skip trailing space */
        --end;
    
    csave = end[1];
    end[1] = '\0';
    s = strdup(s);
    end[1] = csave;
    return (s);
}

/** 
 * @brief 获得xdg路径的数据文件
 * 
 * @param 文件名
 * @param 模式
 * 
 * @return 文件指针
 */
FILE *GetXDGFileData(const char *fileName, const char *mode)
{
    size_t len;
    char ** path = GetXDGPath(&len, "XDG_CONFIG_HOME", ".config", "fcitx" , DATADIR, "fcitx/data" );

    FILE* fp = GetXDGFile(fileName, path, mode, len);

    FreeXDGPath(path);

    return fp;
}

/** 
 * @brief 获得xdg路径的配置文件
 * 
 * @param 文件名
 * @param 模式
 * 
 * @return 文件指针
 */
FILE *GetXDGFileConfig(const char *fileName, const char *mode)
{
    size_t len;
    char ** path = GetXDGPath(&len, "XDG_CONFIG_HOME", ".config", "fcitx", DATADIR,
                                        "fcitx/data");

    FILE* fp = GetXDGFile(fileName, path, mode, len);

    FreeXDGPath(path);

    return fp;

}

/** 
 * @brief 根据Path按顺序查找第一个符合要求的文件，在mode包含写入时，这个函数会尝试创建文件夹
 * 
 * @param 文件名
 * @param 路径数组
 * @param 模式
 * @param 路径数值长度
 * 
 * @return 文件指针
 */
FILE *GetXDGFile(const char *fileName, char **path, const char *mode, size_t len)
{
    char buf[PATH_MAX];
    int i;
    FILE *fp = NULL;

    if (len <= 0)
        return NULL;
    for(i=0;i<len;i++)
    {
        snprintf(buf, sizeof(buf), "%s/%s", path[i], fileName);
        buf[sizeof(buf) - 1] = '\0';

        fp = fopen (buf, mode);
        if (fp)
            break;

    }

    if (!fp)
    {
        if (strchr(mode, 'w'))
        {
            snprintf(buf, sizeof(buf), "%s/%s", path[0], fileName);
            buf[sizeof(buf) - 1] = '\0';

            char *dirc = strdup(buf);
            char *dir = dirname(dirc);
            if (access(dir, 0))
                mkdir(dir, S_IRWXU);
            fp = fopen (buf, mode);
            free(dirc);
        }
    }
    return fp;
}

/** 
 * @brief 释放路径数组的内存
 * 
 * @param 路径数组
 */
void FreeXDGPath(char **path)
{
    free(path[0]);
    free(path);
}

/** 
 * @brief 获得字符串数组，返回对应的XDGPath
 * 
 * @param 用于返回字符串数组长度的指针
 * @param XDG_*_HOME环境变量名称
 * @param 默认XDG_*_HOME路径
 * @param XDG_*_DIRS环境变量名称
 * @param 默认XDG_*_DIRS路径
 * 
 * @return 字符串数组
 */
char **GetXDGPath(
        size_t *len,
        const char* homeEnv,
        const char* homeDefault,
        const char* suffixHome,
        const char* dirsDefault,
        const char* suffixGlobal)
{
    char* dirHome;
    const char *xdgDirHome = getenv(homeEnv);
    if (xdgDirHome && xdgDirHome[0])
    {
        dirHome = strdup(xdgDirHome);
    }
    else
    {
        const char *home = getenv("HOME");
        dirHome = malloc(strlen(home) + 1 + strlen(homeDefault) + 1);
        sprintf(dirHome, "%s/%s", home, homeDefault);
    }

    char *dirs;
    asprintf(&dirs, "%s/%s:%s/%s", dirHome, suffixHome , dirsDefault, suffixGlobal);
    
    free(dirHome);
    
    /* count dirs and change ':' to '\0' */
    size_t dirsCount = 1;
    char *tmp = dirs;
    while (*tmp) {
        if (*tmp == ':') {
            *tmp = '\0';
            dirsCount++;
        }
        tmp++;
    }
    
    /* alloc char pointer array and puts locations */
    size_t i;
    char **dirsArray = malloc(dirsCount * sizeof(char*));
    for (i = 0; i < dirsCount; ++i) {
        dirsArray[i] = dirs;
        while (*dirs) {
            dirs++;
        }
        dirs++;
    }
    
    *len = dirsCount;
    return dirsArray;
}

/** 
 * @brief 返回申请后的内存，并清零
 * 
 * @param 申请的内存长度
 * 
 * @return 申请的内存指针
 */
void *malloc0(size_t bytes)
{
    void *p = malloc(bytes);
    if (!p)
        return NULL;

    memset(p, 0, bytes);
    return p;
}

/** 
 * @brief 自定义的二分查找，和bsearch库函数相比支持不精确位置的查询
 * 
 * @param key
 * @param base
 * @param nmemb
 * @param size
 * @param accurate
 * @param compar
 * 
 * @return 
 */
void *custom_bsearch(const void *key, const void *base,
        size_t nmemb, size_t size, int accurate,
        int (*compar)(const void *, const void *))
{
    if (accurate)
        return bsearch(key, base, nmemb, size, compar);
    else
    {
        size_t l, u, idx;
        const void *p;
        int comparison;
        
        l = 0;
        u = nmemb;
        while (l < u)
        {
            idx = (l + u) / 2;
            p = (void *) (((const char *) base) + (idx * size));
            comparison = (*compar) (key, p);
            if (comparison <= 0)
                u = idx;
            else if (comparison > 0)
                l = idx + 1;
        }

        if (u >= nmemb)
            return NULL;
        else
            return (void *) (((const char *) base) + (l * size));
    }
}

void FcitxInitThread()
{
#ifdef _ENABLE_DBUS
    dbus_threads_init_default();
#endif
    pthread_mutexattr_t   mta;
    int rc;
    
    rc = pthread_mutexattr_init(&mta);
    if (rc != 0)
        FcitxLog(ERROR, _("pthread mutexattr init failed"));

    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

    rc = pthread_mutex_init(&fcitxMutex, &mta);
    gs.bMutexInited = True;
    if (rc != 0)
        FcitxLog(ERROR, _("pthread mutex init failed"));
}

int FcitxLock()
{
    if (gs.bMutexInited)
        return pthread_mutex_lock(&fcitxMutex); 
    return 0;
}

int FcitxUnlock()
{
    if (gs.bMutexInited)        
        return pthread_mutex_unlock(&fcitxMutex);
    return 0;
}

/** 
 * @brief Fcitx记录Log的函数
 * 
 * @param ErrorLevel
 * @param fmt
 * @param ...
 */
void FcitxLogFunc(ErrorLevel e, const char* filename, const int line, const char* fmt, ...)
{
#ifndef _DEBUG
    if (e == DEBUG)
        return;
#endif
    switch (e)
    {
        case INFO:
            fprintf(stderr, _("Info:"));
            break;
        case ERROR:
            fprintf(stderr, _("Error:"));
            break;
        case DEBUG:
            fprintf(stderr, _("Debug:"));
            break;
        case WARNING:
            fprintf(stderr, _("Warning:"));
            break;
        case FATAL:
            fprintf(stderr, _("Fatal:"));
            break;
    }
    va_list ap;
    fprintf(stderr, "%s:%u-", filename, line);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}
