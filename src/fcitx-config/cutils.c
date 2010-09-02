#include "fcitx-config/cutils.h"
#include "tools/tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

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
