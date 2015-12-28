# Copyright (C) 2012  WuJunjie(Joy.Woo@hotmail.com)

# platforms:
#  linux-x86-32
#  uclinux-s3c2510

PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm

VERSION =	1.0
DIFF_VERSION =	0.9

DIST = tt-$(PLATFORM)

CFLAGS =	-Wall -g -O2 -MD -D__UNIT_TEST
#CFLAGS += -D_DEBUG #debugme
ifeq ($(PLATFORM),linux-x86-32)
	CFLAGS += -DUSE_USB_INTERFACE
else
	MYSQL_LIBS_PREFIX=lib
endif

#MIDDLE_WARE=../../libs
MIDDLE_WARE=$(HOME)/libs

#CFLAGS += -I$(HOME)/libs/include/ezthread
CFLAGS += -I${MIDDLE_WARE}/include/ezThread
CLEAN_FILES = $(DIST) tt-* *.o *.gdb *.d *.cfbk ../*.d ../*.cfbk *.tgz
CLEAN_FILES += *actual* *rewrite* a.txt

#特殊功能区
#

CPPFLAGS =	$(CFLAGS)

LIBS =		-L${MIDDLE_WARE}/lib -lezThread-$(PLATFORM) -lpthread

OBJS_MAIN	= \
		main.o

# EnOcean模块
CFLAGS += -DDEV_ENOCEAN_SUPPORT
#CFLAGS += -DUSE_PROCESS_THREAD

CFLAGS += -I${MIDDLE_WARE}/include/ezserial
OBJS_ENOCEAN = \
	../enocean_mgr.o  \
	../esp.o
LIBS +=	-L${MIDDLE_WARE}/lib -lezserial-$(PLATFORM)

CFLAGS += -I${MIDDLE_WARE}/include/ezutil
LIBS +=	-L${MIDDLE_WARE}/lib -lezutil-$(PLATFORM)

OBJS_DIST += $(OBJS_ENOCEAN)
OBJS_DIST += $(OBJS_MAIN)
CLEAN_FILES += $(OBJS_ENOCEAN)


$(DIST): $(OBJS_DIST)
	$(CPP) -o $@ $^ $(LIBS)

# --static

run: $(DIST)
	sudo ./$(DIST)
	
dbg: $(DIST)
	mv ./$(DIST) /home/share

FILES =		*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)
include         Makefile.version
include         Makefile.Defines.$(PLATFORM)

-include	*.d

