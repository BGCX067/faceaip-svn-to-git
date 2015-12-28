# Copyright (C) 2012  tiansu-china.com
# 
# platforms:
PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm

VERSION =	1.3
DIFF_VERSION =	1.0

CFLAGS =	-Wall -g -O2 -MD
#CFLAGS += -D_DEBUG #debugme

OBJS_GENERALAGENT = \
		./GeneralAgentHandler.o\
		./GeneralAgent.o

##############################################################################
#以下功能可以随意组合， 但是至少保留一个功能，否则无意义
#不需要的文件可以删除


CFLAGS += -DUSE_GENERALAGENTUDPSOCKETSERVER #UDP服务端
OBJS_GENERALAGENT += \
		./GeneralAgentUdpSocket.o

CFLAGS += -DUSE_GENERALSOCKETPROCESSOR #使用单独的处理线程
OBJS_GENERALAGENT += \
		./GeneralSocketProcessor.o	\
		./SocketMsgQue.o
##############################################################################

LIBS +=	-L$(HOME)/libs/lib -lppSockets-$(PLATFORM)

#多线程，用ezthread库
OBJS_MULTITHREAD_P = main_multi.o
OBJS_MULTITHREAD_P += $(OBJS_GENERALAGENT)
DIST_MULTITHREAD = generalAgentMulti

DIST =	$(DIST_MULTITHREAD)

include         Makefile.version
include         Makefile.Defines.$(PLATFORM)

CFLAGS += -I$(HOME)/libs/include/ppSockets
#
CFLAGS += -I$(HOME)/libs/include/ezthread



CPPFLAGS =	$(CFLAGS)

#use ezutil library for http_parse
CFLAGS += -I$(HOME)/libs/include/ezutil
LIBS +=	-L$(HOME)/libs/lib -lezutil-$(PLATFORM)

LIBS +=	-L$(HOME)/libs/lib -lezthread-$(PLATFORM)
LIBS +=	-lpthread

$(DIST_MULTITHREAD):$(OBJS_MULTITHREAD_P)
		$(CPP) -o $@ $^ $(LIBS)

FILES =	*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)

-include *.d
