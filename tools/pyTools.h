#ifndef _PY_TOOLS_H
#define _PY_TOOLS_H

#include "tools/utf8.h"

struct _PYMB
{
  int PYFAIndex;
  char HZ[UTF8_MAX_LENGTH + 1];
  int UserPhraseCount;
  struct
  {
    int Length;
    char *Map;
    char *Phrase;
    int Index;
    int Hit;
  } *UserPhrase;
};

struct _HZMap
{
  char Map[3];
  int BaseCount;
  char **HZ;
  int *Index;
};

int LoadPYBase(FILE *, struct _HZMap **);
void LoadPYMB(FILE *fi, struct _PYMB **pPYMB, int isUser);

#endif /* _PY_TOOLS_H */

