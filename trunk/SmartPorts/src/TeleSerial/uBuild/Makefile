# Copyright (C) 2012  ezlibs.com
#
# platforms:
PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
PLATFORM = linux-am335x

VERSION =	1.3
DIFF_VERSION =	1.0

PROJECT_NAME=TeleSerial

include         Makefile.Defines.$(PLATFORM)

#CFLAGS =	-Wall -g -O2 -MD
#CFLAGS += -D_DEBUG #debugme

##############################################################################
# 代码文件位置
GENERALAGENT_BASEDIR_SRC=..

##############################################################################
#库文件位置
#使用自带库
#EZLIBS_BASEDIR_LIBS=./libs

##使用发布库
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

OBJS_GENERALAGENT = \
		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialLog.o \
		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialHandler.o\
		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialTcpSocket.o \
		$(GENERALAGENT_BASEDIR_SRC)/TeleSerial.o

OBJS_GENERALAGENT += $(GENERALAGENT_BASEDIR_SRC)/DevSerial.o

###############################################################################
### 配置模块
##ezConfig
#CFLAGS += -DEXAMPLE_EZCONFIG
#CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezConfig
#LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezConfig-$(PLATFORM)
##json#thread
#OBJS_CONFIGS	= \
#		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigManager.o \
#		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigGeneral.o \
#		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigTcpSocketServer.o \
#		$(GENERALAGENT_BASEDIR_SRC)/Configs/ConfigTcpSocketConnector.o \
#		$(GENERALAGENT_BASEDIR_SRC)/Configs/ConfigConsoleOverTcp.o \
#		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigDatabase.o \
#		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigWebService.o
##
################################################################################
#### 日志模块
###log4cplus
#CFLAGS += -DEXAMPLE_LOG4CPP
#CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/include
#LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/lib -llog4cplus -lrt

##############################################################################
#以下功能可以随意组合， 但是至少保留一个功能，否则无意义
#不需要的文件可以删除

#CFLAGS += -DTeleSerial_FUNC_CONSOLE_OVERTCP #远程调试接口
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialConsoleOverTcp.o
#
#CFLAGS += -DTeleSerial_FUNC_STATUS_AGENT #tcp状态服务器
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialStatusAgent.o
#
#CFLAGS += -DTeleSerial_FUNC_HTTPKEEPCONNECTOR #http协议的客户端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialHttpKeepConnector.o
#
#CFLAGS += -DTeleSerial_FUNC_DATAPARSE_EXAMPLE #数据解析示例

CFLAGS += -DTeleSerial_FUNC_TCPSOCKETSERVER #TCP服务端
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialTcpSocketServer.o

#CFLAGS += -DTeleSerial_FUNC_TCPSOCKETCONNECTOR #tcp客户端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialTcpSocketConnector.o

#CFLAGS += -DTeleSerial_FUNC_UDPSOCKETSERVER #UDP服务端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialUdpSocket.o
#
#CFLAGS += -DTeleSerial_FUNC_SOCKETPROCESSOR #使用单独的处理线程
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialSocketProcessor.o	\
#		$(GENERALAGENT_BASEDIR_SRC)/SocketMsgQue.o
#
#CFLAGS += -DTeleSerial_FUNC_HTTPSERVER #HTTP服务端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/TeleSerialHttpServer.o

CFLAGS += -DTeleSerial_FUNC_SIGNAL_ONDATA #使用数据回调机制

##############################################################################
#use ezutil library for http_parse
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
#ezUtilCPP
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezUtilCPP/
LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezUtilCPP-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ppSockets
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lppSockets-$(PLATFORM)

## json
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/json
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -ljson-$(PLATFORM)
#zlib
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/zlib
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -lz-$(PLATFORM)
#ezserial
CFLAGS += -I$(HOME)/libs/include/ezserial
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezserial-$(PLATFORM)

#
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)
LIBS +=	-lpthread

#libsigc
CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/libsigc++-$(PLATFORM)/lib/sigc++-2.0/include
CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/libsigc++-$(PLATFORM)/include/sigc++-2.0
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/libsigc++-${PLATFORM}/lib/ -lsigc-2.0
#LIBS += -luuid

#多线程，用ezthread库
OBJS_MULTITHREAD_P = $(GENERALAGENT_BASEDIR_SRC)/mainTeleSerial.o
OBJS_MULTITHREAD_P += $(OBJS_GENERALAGENT)
OBJS_MULTITHREAD_P += $(OBJS_CONFIGS)
DIST_MULTITHREAD = TeleSerial-$(PLATFORM)

DIST =	$(DIST_MULTITHREAD)
#DIST +=	$(DIST_MAINTHREAD)

all: $(DIST)

$(DIST_MULTITHREAD):$(OBJS_MULTITHREAD_P)
		$(CPP) -o $@ $^ $(LIBS)

dbg:$(DIST)
	mv $(DIST) /home/share

tar:	clean
		tar -czf $(PROJECT_NAME).tgz *.h *.cpp Makefile*

-include *.d

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(DIST)
	find $(GENERALAGENT_BASEDIR_SRC) -name "*.o" |xargs rm -f
	find $(GENERALAGENT_BASEDIR_SRC) -name "*.d" |xargs rm -f
	find $(GENERALAGENT_BASEDIR_SRC) -name "*.cfbk" |xargs rm -f
