#include <stdlib.h>
#include "fcitx-config/sprintf.h"

#ifndef HAVE_VASPRINTF
int vasprintf(char **ptr, const char *format, va_list ap)
{
    int ret;
    va_list ap2;
    
    va_copy(ap2, ap);
    
    ret = vsnprintf(NULL, 0, format, ap2);
    if (ret <= 0) return ret;
    
    (*ptr) = (char *)malloc(ret+1);
    if (!*ptr) return -1;
    
    va_copy(ap2, ap);
    
    ret = vsnprintf(*ptr, ret+1, format, ap2);
    
    return ret;
}
#endif

#ifndef HAVE_ASPRINTF
int asprintf(char **ptr, const char *format, ...)
{
    va_list ap;
    int ret;
    
    *ptr = NULL;
    va_start(ap, format);
    ret = vasprintf(ptr, format, ap);
    va_end(ap);
    
    return ret;
}
#endif

