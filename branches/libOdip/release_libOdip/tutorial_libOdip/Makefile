# Copyright (C) 2012  WuJunjie(Joy.Woo@hotmail.com)

# platforms:
#  linux-x86-32
#  uclinux-s3c2510

PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
PLATFORM = linux-arm-none-gnueabi

VERSION =	1.0
DIFF_VERSION =	0.9

DIST =		getStream

CFLAGS =	-Wall -g -O2 -MD
CFLAGS += -D_DEBUG #debugme

include         Makefile.version
include         Makefile.Defines.$(PLATFORM)


CLEAN_FILES += *actual* *rewrite* a.txt

#特区
# 动态库和静态库选择，注意二选一注释干净
###############################################################################
#Use Static Library
LIBS_HOME=../lib
LIBS  = -L$(LIBS_HOME) -lOdip-$(PLATFORM)
LIBS +=	-lppSockets-$(PLATFORM)
LIBS +=	-lezUtilCPP-$(PLATFORM)
LIBS +=	-lezutil-$(PLATFORM)
LIBS +=	-lezthread-$(PLATFORM)


#Use Dynamic Library
#can not use in android -- wujj at 2012-12-19 14:32:21
#LIBS  = -L../lib -lOdip-$(PLATFORM)
###############################################################################

LIBS +=	-lpthread

#CFLAGS += -I$(HOME)/libs/include/Odip
CFLAGS += -I../include

#CPPFLAGS =	$(CFLAGS) 

getStream:	getStream.o
	$(CPP) $(PLATFORM_LINK_FLAGS) -o $@ $^ $(LIBS)

FILES =		*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)

-include	*.d
