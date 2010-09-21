#ifndef UTF8_H
#  define UTF8_H

#include <stdlib.h>

#define UTF8_MAX_LENGTH 6

#define ISUTF8_CB(c)  (((c)&0xc0) == 0x80)

extern size_t utf8_strlen(const char *s);
extern char* utf8_get_char(const char *in, int *chr);
extern int utf8_strncmp(const char *s1, const char *s2, int n);
extern int utf8_char_len(const char *in);
extern char* utf8_get_nth_char(char* s, unsigned int n);
extern int utf8_check_string(const char *s);
extern int utf8_get_char_extended (const char *p, int max_len);
extern int utf8_get_char_validated (const char *p, int max_len);

#endif /* ifndef UTF8_H */

