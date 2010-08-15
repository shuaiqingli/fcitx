#include <stdio.h>
#include <stdarg.h>

#ifndef HAVE_VASPRINTF
extern int vasprintf(char **ptr, const char *format, va_list ap);
#endif

#ifndef HAVE_ASPRINTF
extern int asprintf(char **ptr, const char *format, ...);
#endif
