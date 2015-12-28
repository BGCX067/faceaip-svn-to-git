#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile - \library\libezthread\Build\
#
# Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
#
# $Id: Makefile 1 2011-10-22 08:14:10 WuJunjie Exp $
#
# explain
#      exp
#
# Update:
#    2011-10-22 08:14 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
PUB_MAKE_FILE_PREFIX =	.
#./Makefile.Defines

# define default platform
ifndef PLATFORM
PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm

#for android
PLATFORM = linux-arm-none-gnueabi

endif
VERSION="1.0.0"

# CPP, CFLAGS, LIBS
include		$(PUB_MAKE_FILE_PREFIX)/Makefile.Defines.$(PLATFORM)

SRC_PATH=../src

LIB_OBJS =	\
		$(SRC_PATH)/comm/DoubleOutLog.o \
		$(SRC_PATH)/comm/GeneralAgentHandler.o \
		$(SRC_PATH)/comm/GeneralAgentTcpSocket.o \
		$(SRC_PATH)/comm/GeneralAgent.o \
		$(SRC_PATH)/nvrnetapi.o \
		$(SRC_PATH)/comm/GeneralAgentTcpSocketConnector.o \
		$(SRC_PATH)/comm/GeneralSocketProcessor.o \
		$(SRC_PATH)/comm/SocketMsgQue.o

CFLAGS += -DUSE_GENERALSOCKETPROCESSOR #使用单独的处理线程
CFLAGS += -DUSE_GENERALAGENTTCPSOCKETCONNECTOR #客户端
CFLAGS += -DUSE_DATAPARSE_EXAMPLE #数据解析示例

CFLAGS += -I$(SRC_PATH) -I$(SRC_PATH)/comm
CFLAGS += -I$(HOME)/libs/include/ppSockets
CFLAGS += -I$(HOME)/libs/include/ezThread
CFLAGS += -I$(HOME)/libs/include/ezUtilCPP
CFLAGS += -I$(HOME)/libs/include/ezutil

ARCH_LIBS +=	$(HOME)/libs/lib/libppSockets-$(PLATFORM).a
ARCH_LIBS +=	$(HOME)/libs/lib/libezUtilCPP-$(PLATFORM).a
ARCH_LIBS +=	$(HOME)/libs/lib/libezutil-$(PLATFORM).a
ARCH_LIBS +=	$(HOME)/libs/lib/libezThread-$(PLATFORM).a

#LIBS +=	-lpthread

CPPFLAGS =	$(CFLAGS)

PREFIX =	$(HOME)/libs
INS_DIR_LIB = $(PREFIX)/lib
INS_DIR_INC = $(PREFIX)/include/Odip

LIB_TARGET = libOdip-$(PLATFORM).a
LIB_TARGET_DYN = libOdip-$(PLATFORM).so

TARGET	= $(LIB_TARGET)
all: $(TARGET)
	cp $(TARGET) ../lib
	cp ../src/*.h ../include

dyn: $(LIB_TARGET_DYN)
	cp $(LIB_TARGET_DYN) ../lib
	cp ../src/*.h ../include

$(LIB_TARGET): $(LIB_OBJS)
	#mkdir -p $(LIBDIR)
	$(RM) $@;
	$(AR) $(AR_FLAG) $@ $^
	# $(ARCH_LIBS)
	$(RANLIB) $@

$(LIB_TARGET_DYN): $(LIB_OBJS)
	#mkdir -p $(LIBDIR)
	$(RM) $@;
	$(CC) $(PLATFORM_LINK_FLAGS) -shared $^ $(ARCH_LIBS) -lc -o $@

install:	all
		@mkdir -p $(INS_DIR_LIB)
		cp $(LIB_TARGET) $(INS_DIR_LIB)
		@mkdir -p $(INS_DIR_INC)
		cp ../src/*.h $(INS_DIR_INC)
.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGET) $(LIB_TARGET_DYN) $(LIB_OBJS) *.a *.so *.cfbk ./lib/* ./include/*
	find .. -name "*.d" |xargs rm -f
	find .. -name "*.cfbk" |xargs rm -f
	make -C ../tutorial_libOdip clean
