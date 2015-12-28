#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile - Z:\Dropbox\stock\trunk\code\appmodule\SmartWebs\
#
# Copyright (C) 2013 ezlibs.com, All Rights Reserved.
#
# $Id: Makefile 1 2013-06-25 09:16:40 WuJunjie Exp $
#
# explain
#      exp
#
# Update:
#    2013-06-25 09:16 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

# platforms:
#  linux-x86-32
#  uclinux-s3c2510

PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
PLATFORM = linux-am335x

VERSION =	1.0
DIFF_VERSION =	0.9

DIST =		smartwebs-$(PLATFORM) 
MIDDLE_WARE=$(HOME)/libs

CFLAGS =	-Wall -g -O2 -MD
CFLAGS += -D_DEBUG #debugme

include         Makefile.Defines.$(PLATFORM)

CFLAGS	+= -DWEBS -DOS="LINUX" -DLINUX $(UMSW) $(DASW) $(SSLSW) $(IFMODSW)
CFLAGS += -I$(HOME)/libs/include/aspwebs
LIBS +=		-L$(HOME)/libs/lib -laspwebs-$(PLATFORM) -lpthread

CFLAGS += -I$(HOME)/libs/include/ezplatform
LIBS +=          -L$(HOME)/libs/lib -lezplatform-$(PLATFORM)

CLEAN_FILES += *actual* *rewrite* a.txt

#特殊功能区
#单元测试，单独编译
CFLAGS += -DSTANDLONE_UTIL

CPPFLAGS =	$(CFLAGS) 
BASEDIR_SRC=..
CONFIGS_BASEDIR_SRC=../../Configs

ARCH	= libaspwebs.a

# User Management switch
UMSW	= -DUSER_MANAGEMENT_SUPPORT

# Digest Access switch
DASW	= -DDIGEST_ACCESS_SUPPORT

OBJS_DEP = $(BASEDIR_SRC)/main.o $(BASEDIR_SRC)/smart_webs.o $(BASEDIR_SRC)/websFunction.o

CFLAGS  += -DUSE_USB_INTERFACE -D__UNIT_TEST -DDEV_ENOCEAN_SUPPORT
#CFLAGS  += -DUSE_PROCESS_THREAD
CFLAGS  += -DCHECK_STATUS_THREAD
CFLAGS += -I${MIDDLE_WARE}/include/ezserial
LIBS += -L${MIDDLE_WARE}/lib -lezserial-$(PLATFORM)
CFLAGS += -I${MIDDLE_WARE}/include/ezutil
LIBS += -L${MIDDLE_WARE}/lib -lezutil-$(PLATFORM)

OBJS_DEP +=$(BASEDIR_SRC)/enocean/enocean_mgr.o $(BASEDIR_SRC)/enocean/esp.o
OBJS_DEP +=\
		$(BASEDIR_SRC)/../Configs/ConfigGeneral.o \
		$(BASEDIR_SRC)/../Configs/ConfigNetworkInterface.o
#特殊功能区
#
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

#ezConfig
CFLAGS += -DUSE_EZCONFIG
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezConfig
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -lezConfig-$(PLATFORM)
## json
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/json
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -ljson-$(PLATFORM)
#zlib
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/zlib
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -lz-$(PLATFORM)
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)
LIBS += -lpthread

smartwebs-$(PLATFORM):$(OBJS_DEP)
	$(CPP) -o $@ $(DFLAGS) \
	$(CFLAGS) $(IFLAGS) \
	$(OBJS_DEP) $(LIBS) $(LDFLAGS) 

	
FILES =		*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)
		
dbg:smartwebs-$(PLATFORM)
	cp smartwebs-$(PLATFORM) /home/share
	rm -rf /home/share/spages
	cp spages /home/share -rf

CLEAN_FILES+=$(OBJS_DEP)
include         Makefile.version
-include	*.d
