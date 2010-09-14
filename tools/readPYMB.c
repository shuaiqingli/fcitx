#include <stdio.h>
#include <stdlib.h>

#include "im/pinyin/py.h"
#include "pyTools.h"
#include "fcitx-config/xdg.h"

void usage();

int main(int argc, char **argv)
{
  FILE *fi;
  int i, j;
  char *pyusrphrase_mb = NULL;
  struct _PYMB *PYMB;
  Bool isUser = True;
  char c;

  while((c = getopt(argc, argv, "f:sh")) != -1)
  {
      switch(c)
      {
          case 'f':
              pyusrphrase_mb = strdup(optarg);
              break;
          case 's':
              isUser = False;
              break;
          case 'h':
          default:
              usage();
      }
  }

  if (pyusrphrase_mb)
      fi = fopen(pyusrphrase_mb, "r");
  else
      fi = GetXDGFileUser( PY_USERPHRASE_FILE, "r" , &pyusrphrase_mb);
  if (!fi)
  {
    perror("fopen");
    fprintf(stderr, "Can't open file `%s' for reading\n", pyusrphrase_mb);
    exit(1);
  }
  free(pyusrphrase_mb);

  LoadPYMB(fi, &PYMB, isUser);

  for (i = 0; PYMB[i].HZ[0]; ++i)
  {
    printf("PYFAIndex: %d\n", PYMB[i].PYFAIndex);
    printf("HZ: %s\n", PYMB[i].HZ);
    printf("UserPhraseCount: %d\n", PYMB[i].UserPhraseCount);

    for (j = 0; j < PYMB[i].UserPhraseCount; ++j)
    {
      printf("+-Length: %d\n", PYMB[i].UserPhrase[j].Length);
      printf("| Map: %s\n", PYMB[i].UserPhrase[j].Map);
      printf("| Phrase: %s\n", PYMB[i].UserPhrase[j].Phrase);
      printf("| Index: %d\n", PYMB[i].UserPhrase[j].Index);
      printf("| Hit: %d\n", PYMB[i].UserPhrase[j].Hit);
    }
    printf("\n");
  }

  return 0;
}

void usage()
{
  puts(
"readPYMB - read data from a pinyin .mb file and display its meaning\n"
"\n"
"  usage: readPYMB [OPTION]\n"
"\n"
"  -f <mbfile> MB (MaBiao) file to be read, usually this is\n"
"              ~/.config/fcitx/" PY_USERPHRASE_FILE "\n"
"              if not specified, defaults to\n"
"              ~/.config/fcitx/" PY_USERPHRASE_FILE "\n"
"  -s          Is MB from user or from system (they have different format).\n"
"  -h          display this help\n"
"\n"
"  The MB file can either be a user's MB file (~/.config/fcitx/pyuserphrase.mb),\n"
"  or the system phrase pinyin MB file (/usr/share/fcitx/data/pyphrase.mb.\n"
  );
  exit(1);
  return;
}

