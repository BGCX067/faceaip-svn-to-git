# Copyright (C) 2012  WuJunjie(Joy.Woo@hotmail.com)

# platforms:
#  linux-x86-32
#  uclinux-s3c2510

PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm

VERSION =	1.0
DIFF_VERSION =	0.9

DIST =		tt 

CFLAGS =	-Wall -g -O2 -MD
CFLAGS += -D_DEBUG #debugme

CFLAGS += -I$(HOME)/libs/include/ezthread
CLEAN_FILES = $(DIST) *.o *.gdb *.d *.cfbk ../*.d ../*.cfbk *.tgz
CLEAN_FILES += *actual* *rewrite* a.txt

#特殊功能区
#

CPPFLAGS =	$(CFLAGS) 

LIBS =		-L$(HOME)/libs/lib -lezthread-$(PLATFORM) -lpthread

OBJS_MAIN	= \
		main.o

# EnOcean模块
#CFLAGS += -DDEV_ENOCEAN_SUPPORT
CFLAGS += -I$(HOME)/libs/include/ezserial
OBJS_ENOCEAN = \
	../enocean_mgr.o  \
	../esp.o
LIBS +=	-L$(HOME)/libs/lib -lezserial-$(PLATFORM)

OBJS_DIST += $(OBJS_ENOCEAN)
OBJS_DIST += $(OBJS_MAIN)
CLEAN_FILES += $(OBJS_ENOCEAN)

DIST = tt-$(PLATFORM)

$(DIST): $(OBJS_DIST)
	$(CPP) -o $@ $^ $(LIBS)

# --static

run: $(DIST)
	sudo ./$(DIST)

FILES =		*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)
include         Makefile.version
include         Makefile.Defines.$(PLATFORM)

-include	*.d

