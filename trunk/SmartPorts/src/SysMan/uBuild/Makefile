# Copyright (C) 2012  WuJunjie(Joy.Woo@hotmail.com)

# platforms:
#  linux-x86-32
#  uclinux-s3c2510

PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
PLATFORM = linux-am335x

VERSION =	1.0
DIFF_VERSION =	0.9

BASE_DIR_SRC = ..
DIST =		mainSysMan-$(PLATFORM)

OBJ_SYS_MAN = \
		$(BASE_DIR_SRC)/mainSysMan.o \
		$(BASE_DIR_SRC)/Maintenance.o \
		$(BASE_DIR_SRC)/../Configs/ConfigNet.o \
		$(BASE_DIR_SRC)/ConfigManagerNetInit.o \
		$(BASE_DIR_SRC)/../Configs/ConfigNetworkInterface.o

OBJ_SYS_MAN = \
		$(GENERALAGENT_BASEDIR_SRC)/DoubleOutLog.o \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentHandler.o\
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentTcpSocket.o \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgent.o
CFLAGS += -DEXAMPLE_GENERALAGENTTCPSOCKETSERVER #TCP服务端
OBJ_SYS_MAN += \
		$(BASE_DIR_SRC)/GeneralAgentTcpSocketServer.o

include         Makefile.version
include         Makefile.Defines.$(PLATFORM)
CFLAGS +=	-Wall -O2 -MD
CFLAGS += -g -D_DEBUG #debugme
CFLAGS += -DSTANDLONE_UTIL
#pass the ../src/Solar.h:54: warning: ??CSolar::m_TimerMaintain?ˉ will be initialized after
CFLAGS += -Wno-reorder

#CFLAGS += -I../include
CLEAN_FILES += *actual* *rewrite* a.txt $(OBJ_SYS_MAN)

#特殊功能区
#
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

CPPFLAGS =	$(CFLAGS) 

CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezplatform
LIBS +=		-L$(HOME)/libs/lib -lezplatform-$(PLATFORM) -lpthread
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)
LIBS += -lpthread
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

test_net_work_interface-$(PLATFORM):	test_net_work_interface.o
	$(CPP) -o $@ $^ $(LIBS)

mainSysMan-$(PLATFORM):	$(OBJ_SYS_MAN)
	$(CPP) -o $@ $^ $(LIBS)
	
FILES =		*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)

-include	*.d

dbg:$(DIST)
	mv $(DIST) /home/share
