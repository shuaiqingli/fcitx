INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/src
DEFS = -DPKGDATADIR=\"$(pkgdatadir)\" -DDATADIR=\"$(datadir)\" -DLOCALEDIR=\"$(localedir)\"

bin_PROGRAMS=fcitx
fcitx_SOURCES= IC.c IC.h\
			   MyErrorsHandlers.c MyErrorsHandlers.h\
			   ime.c ime.h \
			   internalVersion.c \
			   xim.c xim.h \
			   main.c  \
			   fcitx.h

fcitx_LDADD=../../lib/libXimd.a \
			../im/pinyin/libimpinyin.a \
			../im/table/libimtable.a \
			../im/qw/libimqw.a \
			../im/extra/libimextra.a \
			../im/special/libimspecial.a \
			../tools/libtools.a \
			../ui/libui.a \
			../interface/libinterface.a \
            ../fcitx-config/libfcitx-config.la

fcitx_LDFLAGS = -ldl -lpthread
