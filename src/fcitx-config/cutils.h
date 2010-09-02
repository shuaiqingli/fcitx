#include <stdlib.h>

typedef enum ErrorLevel
{
    DEBUG,
    ERROR,
    INFO,
    FATAL,
    WARNING
} ErrorLevel;

char *trim(char *s);
void *malloc0(size_t bytes);

#define FcitxLog(e, fmt, arg...) FcitxLogFunc(e, __FILE__, __LINE__, fmt, ##arg)
void FcitxLogFunc(ErrorLevel, const char* filename, const int line, const char* fmt, ...);
