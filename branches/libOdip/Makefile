#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile - \library\libezthread\Build\
#
# Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
#
# $Id: Makefile 1 2011-10-22 08:14:10 WuJunjie Exp $
#
# explain
#      for joy.woo only, use build/Makefile pls
#
# Update:
#    2011-10-22 08:14 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
PUB_MAKE_FILE_PREFIX =	../../library/Makefile.Defines
include		$(PUB_MAKE_FILE_PREFIX)/Makefile.Batch.Build

# define default platform
ifndef PLATFORM
PLATFORM = linux-x86-32
endif
VERSION="1.0.0"

# CPP, CFLAGS, LIBS
include		$(PUB_MAKE_FILE_PREFIX)/Makefile.Defines.$(PLATFORM)

LIB_OBJS =	\
		./src/api_exa.o

PREFIX =	$(HOME)/libs
INS_DIR_LIB = $(PREFIX)/lib
INS_DIR_INC = $(PREFIX)/include/ezman

LIB_TARGET = libezman-$(PLATFORM).a

TARGET	= $(LIB_TARGET)
all: $(TARGET)
	cp $(TARGET) ./lib
	cp ./src/*.h ./include

$(LIB_TARGET): $(LIB_OBJS)
	#mkdir -p $(LIBDIR)
	$(RM) $@;
	$(AR) $(AR_FLAG) $@ $^
	$(RANLIB) $@

RELEASE_TAR=release_libOdip.tgz

install:	all 
		@mkdir -p $(INS_DIR_LIB)
		cp $(LIB_TARGET) $(INS_DIR_LIB)
		@mkdir -p $(INS_DIR_INC)
		cp ./src/*.h $(INS_DIR_INC)
release:
	rm -rf release_libOdip.tgz release_libOdip/*
	cp -rf tutorial_libOdip include  lib  release_libOdip/
	cp readme-user.txt release_libOdip/readme.txt
	tar -czvf $(RELEASE_TAR) release_libOdip

.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGET) $(LIB_OBJS) $(RELEASE_TAR) ./lib/* ./include/*
	find . -name "*.a" |xargs rm -f
	find . -name "*.cfbk" |xargs rm -f
	find . -name "*.d" |xargs rm -f
	find . -name "*.cfbk" |xargs rm -f
	make -C tutorial_libOdip clean
