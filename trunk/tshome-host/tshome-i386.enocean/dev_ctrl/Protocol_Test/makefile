
MAIN_PATH = /opt/tshome-host
COMMON_PATH = ${MAIN_PATH}/include
SOURCE_PATH = ./src

DEV_LIB_PATH = ${MAIN_PATH}/lib/dev_ctrl
DEV_LIB_NAME = GatewayManager

INCLUDE = -I${COMMON_PATH}

CFLAGS = -Wall -g3 $(INCLUDE)
CPPFLAGS = $(CFLAGS)

STATIC_LIBS = -L$(DEV_LIB_PATH) -l$(DEV_LIB_NAME) -ldl
DYNAMIC_LIBS = -ldl

PROGS = test
PROGS_S = test-s

all:	$(PROGS)
test-d: $(PROGS)

.c.o:
	gcc $(CFLAGS) -o $@ -c $<

test: $(SOURCE_PATH)/Protocol_Test.o
	g++ -o $@  $^ $(DYNAMIC_LIBS)

test-s: $(SOURCE_PATH)/Protocol_Test.o
	g++ -o $@  $^ $(STATIC_LIBS)

clean:
	rm -rf $(PROGS) $(PROGS_S) $(SOURCE_PATH)/*.o
