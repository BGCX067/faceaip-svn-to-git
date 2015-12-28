#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# tshome-p2p-server-standalone\build\Makefile - \\
#
# Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
#
# $Id: Makefile 1 2012-07-24 09:45:06 WuJunjie Exp $
#
# explain
#      tshome-p2p-server-standalone
#      独立服务器
#
# Update:
#    2012-07-24 09:45 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

#
# platforms:
#PLATFORM = linux-x86-32
#PLATFORM = linux-x86-64
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
# usage: make PLATFORM=linux-x86-64

ifndef PLATFORM
	PLATFORM=linux-x86-32
endif

LIBS_BASEDIR=../lib
#LIBS_BASEDIR=$(HOME)/libs

PLATFORM_LIBS_BASEDIR=${LIBS_BASEDIR}/platform
#PLATFORM_LIBS_BASEDIR=$(HOME)/libs

# Modify this to point to the PJSIP location.
# 使用库里面的脚本安装的文件： install-ts-p2p-lib.sh
#PJSIP_BASEDIR=${HOME}/.libs/tshome-p2p-lib
# 使用自带库文件
PJSIP_BASEDIR=..
PJSIP_BASEDIR_LIB=${PJSIP_BASEDIR}/lib/tshome-p2p-lib-${PLATFORM}/lib
PJSIP_BASEDIR_INC=${PJSIP_BASEDIR}/lib/tshome-p2p-lib-${PLATFORM}/include

PJSIP_BASEDIR_SRC=../src

#CROSS   = arm-linux-
CC      = $(CROSS)gcc
CPP     = $(CROSS)g++
LD      = $(CROSS)ld
AR      = $(CROSS)ar
RANLIB  = $(CROSS)ranlib
STRIP   = $(CROSS)strip

#use ezutil library
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/ezutil 
LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -lezutil-$(PLATFORM)
#ezUtilCPP
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/ezUtilCPP/
LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -lezUtilCPP-$(PLATFORM)
#use ezsocket library
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/ezsocket 
LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -lezsocket-$(PLATFORM)
#use ezthread library
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/ezthread 
OBJS_MAIN	= \
		${PJSIP_BASEDIR_SRC}/main.o \
		${PJSIP_BASEDIR_SRC}/Solar.o \
		${PJSIP_BASEDIR_SRC}/LifeDetectionTimer.o 

LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -lezthread-$(PLATFORM)

## json
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/json
LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -ljson-$(PLATFORM)
## 配置模块
#zlib
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/zlib
LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -lz-$(PLATFORM)
#ezConfig
CFLAGS += -DUSE_EZCONFIG_LIBS
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/ezConfig
LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -lezConfig-$(PLATFORM)
#json#thread
OBJS_CONFIGS	= \
		${PJSIP_BASEDIR_SRC}/Configs/ConfigManager.o \
		${PJSIP_BASEDIR_SRC}/Configs/ConfigGeneral.o \
		${PJSIP_BASEDIR_SRC}/Configs/ConfigIPC.o \
		${PJSIP_BASEDIR_SRC}/Configs/ConfigP2P.o \
		${PJSIP_BASEDIR_SRC}/Configs/ConfigDB.o

#use ppsockets library
CFLAGS += -I$(PLATFORM_LIBS_BASEDIR)/include/ppSockets
LIBS +=	-L$(PLATFORM_LIBS_BASEDIR)/lib -lppSockets-$(PLATFORM)
OBJS_GENERALAGENT = \
		${PJSIP_BASEDIR_SRC}/IpcServer/GeneralAgentHandler.o\
		${PJSIP_BASEDIR_SRC}/common/DoubleOutLog.o\
		${PJSIP_BASEDIR_SRC}/IpcServer/GeneralAgent.o
CFLAGS += -DUSE_GENERALAGENTUDPSOCKETSERVER #UDP服务端
OBJS_GENERALAGENT += \
		${PJSIP_BASEDIR_SRC}/IpcServer/GeneralAgentUdpSocket.o
CFLAGS += -DUSE_GENERALSOCKETPROCESSOR #使用单独的处理线程
OBJS_GENERALAGENT += \
		${PJSIP_BASEDIR_SRC}/IpcServer/GeneralSocketProcessor.o	\
		${PJSIP_BASEDIR_SRC}/IpcServer/SocketMsgQue.o

CFLAGS += -DUSE_GENERALCONSOLE_OVERTCP #远程调试接口
OBJS_GENERALAGENT += \
		${PJSIP_BASEDIR_SRC}/IpcServer/GeneralConsoleOverTcp.o

##如果编译电信版本， 请打开下列宏
#CFLAGS += -DSMARTHOME_AGENT_JSTELECOM
#OBJS_CONFIGS	+= \
#		${PJSIP_BASEDIR_SRC}/SmartHomeTelcom/TeHomeIF.o		\
#		${PJSIP_BASEDIR_SRC}/Configs/ConfigSmartHomeTelcom.o 
#######################################################################################################
###电信http连接器 需打开SMARTHOME_AGENT_JSTELECOM
#CFLAGS += -DUSE_HTTPKEEPCONNECTORSMARTHOME
#OBJS_GENERALAGENT += \
#		${PJSIP_BASEDIR_SRC}/SmartHomeTelcom/AgentSmartHomeTelcom.o    \
#		${PJSIP_BASEDIR_SRC}/SmartHomeTelcom/HttpKeepConnectorSmartHomeTelcom.o	\
#		${PJSIP_BASEDIR_SRC}/SmartHomeTelcom/HandlerSmartHomeTelcom.o
#######################################################################################################
###控制服务器 需打开SMARTHOME_AGENT_JSTELECOM， 但是不依赖USE_HTTPKEEPCONNECTORSMARTHOME
#CFLAGS += -DUSE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
#OBJS_GENERALAGENT += \
#		${PJSIP_BASEDIR_SRC}/SmartHomeTelcom/SmartHomeTelcomCtrlServer/AgentSmartHomeTelcomCtrlServer.o\
#		${PJSIP_BASEDIR_SRC}/SmartHomeTelcom/SmartHomeTelcomCtrlServer/HandlerSmartHomeTelcomCtrlServer.o\
#		${PJSIP_BASEDIR_SRC}/SmartHomeTelcom/SmartHomeTelcomCtrlServer/HttpServerSmartHomeTelcomCtrlServer.o


CFLAGS+=-I${LIBS_BASEDIR}/platform/log4cplus-${PLATFORM}/include

ifeq ($(PLATFORM),linux-x86-64)
	MYSQL_LIBS_PREFIX=lib64
	CFLAGS+=-DLINUX_X86_64
else
	MYSQL_LIBS_PREFIX=lib
	CFLAGS+=-DLINUX_X86_32
endif

CFLAGS+=-g
CFLAGS+=-ggdb3
CFLAGS+=-Wall -DPJ_AUTOCONF=1 -O2 -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1 -DTSHOME\
	-I/usr/include/mysql -I/usr/local/ssl/include/\
	-I../include \
	-I${PJSIP_BASEDIR_INC}/pjnath \
	-I${PJSIP_BASEDIR_INC}/pjlib \
	-I${PJSIP_BASEDIR_INC}/pjlib-util

CPPFLAGS=${CFLAGS}

LDFLAGS=\
	${LIBS_BASEDIR}/platform/log4cplus-${PLATFORM}/lib/liblog4cplus.a			\
	${PJSIP_BASEDIR_LIB}/libpjnath-${PLATFORM}.a 					\
	${PJSIP_BASEDIR_LIB}/libpjlib-util-${PLATFORM}.a  \
	${PJSIP_BASEDIR_LIB}/libpj-${PLATFORM}.a
LDLIBS=${LIBS} -L../lib -L/usr/${MYSQL_LIBS_PREFIX}/mysql -lmysqlclient -ldl -L/usr/local/ssl/lib -lm -lnsl -lrt -lpthread  -lcrypto -lssl

###服务通知模块
OBJS_SERVICE = \
		${PJSIP_BASEDIR_SRC}/service/notification/myAPNS.o

PJTURN_SRV_OBJS += ${OBJS_MAIN}
PJTURN_SRV_OBJS += ${OBJS_CONFIGS}
PJTURN_SRV_OBJS += ${OBJS_GENERALAGENT}
PJTURN_SRV_OBJS += ${OBJS_SERVICE}

PJTURN_SRV_OBJS +=	${PJSIP_BASEDIR_SRC}/pj/allocation.o  					 \
					${PJSIP_BASEDIR_SRC}/pj/auth.o                           \
					${PJSIP_BASEDIR_SRC}/pj/listener_udp.o                   \
					${PJSIP_BASEDIR_SRC}/common/aes_tools.o                  \
					${PJSIP_BASEDIR_SRC}/datagram/report_mgr.o               \
					${PJSIP_BASEDIR_SRC}/common/md5_tools.o                  \
					${PJSIP_BASEDIR_SRC}/common/mybyteswap.o                 \
					${PJSIP_BASEDIR_SRC}/common/rsa_tools.o                  \
					${PJSIP_BASEDIR_SRC}/datagram/IPCProtocolManager.o       \
					${PJSIP_BASEDIR_SRC}/server/mysqlpool.o                  \
					${PJSIP_BASEDIR_SRC}/pj/listener_tcp.o                   \
					${PJSIP_BASEDIR_SRC}/pj/server.o                         \
					${PJSIP_BASEDIR_SRC}/pj/snd_rcv.o                        \
					${PJSIP_BASEDIR_SRC}/server/ipc_handler.o                \
					${PJSIP_BASEDIR_SRC}/common/c_thread.o                   \
					${PJSIP_BASEDIR_SRC}/server/pj_server.o                  \
					${PJSIP_BASEDIR_SRC}/server/tsp_hander.o                 \
					${PJSIP_BASEDIR_SRC}/server/udb_mgr.o

PJTURN_SRV_EXE:=../bin/tshome-p2p-server-$(PLATFORM)

all: $(PJTURN_SRV_EXE)
	@echo $(PJTURN_SRV_EXE)" done."

$(PJTURN_SRV_EXE): $(PJTURN_SRV_OBJS)
	$(CPP) -o $@ $(PJTURN_SRV_OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PJTURN_SRV_OBJS) $(PJTURN_SRV_EXE)

.c.o:
	$(CPP) -c $(CPPFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $< -o $@
