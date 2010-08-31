DEFS = -DPKGDATADIR=\"$(pkgdatadir)\"

toolsdir = $(pkgdatadir)/tools
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/lib
EXTRA_DIST = $(tools_DATA)

bin_PROGRAMS = mb2txt txt2mb createPYMB readPYMB readPYBase mb2org fcitx-remote 
readPYMB_SOURCES = pyTools.c pyTools.h readPYMB.c
readPYBase_SOURCES = pyTools.c pyTools.h readPYBase.c
fcitx_remote_SOURCES = client-test.c

createPYMB_LDADD = \
				   ../src/im/pinyin/pyParser.o \
				   ../src/im/pinyin/pyMapTable.o \
				   ../src/tools/utf8.o \
				   ../src/im/pinyin/PYFA.o \
				   ../src/im/pinyin/sp.o \
				   ../src/tools/xdg.o
mb2org_LDADD = ../src/im/pinyin/pyParser.o ../src/im/pinyin/pyMapTable.o ../src/im/pinyin/PYFA.o ../src/im/pinyin/sp.o pyTools.o ../src/tools/xdg.o

txt2mb_LDADD = ../src/tools/utf8.o
