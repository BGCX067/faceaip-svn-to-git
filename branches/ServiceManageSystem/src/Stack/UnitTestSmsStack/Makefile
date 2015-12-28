#
# A example of Unit Test whth CppUnit
#
# you cppunit should config as: ./configure --prefix=`pwd`/inc --exec-prefix=`pwd`/bin 
# your cppunit path: tools/cppunit-1.12.1

EXEC = cppt
#add all your cpp files here
CPPS = main.cpp TestMan.cpp ../SmsStack.cpp

CXX = g++
PREFIX = ${HOME}/libs/cppunit-1.12.1-linux-x86-32

#INCLUDEDIR = ${PREFIX}/inc/include
# for cppunit-1.12.1
INCLUDEDIR = ${PREFIX}/include/
LDFLAGS = -L${PREFIX}/bin/lib
#CXXFLAGS = -static
LDLIBS  = -lcppunit 
LDLIBS += -ldl  #if in cygwin, pls comment this


PLATFORM = linux-x86-32
LDLIBS +=          -L$(HOME)/libs/lib -ljson-$(PLATFORM)
CFLAGS += -I$(HOME)/libs/include/json

all : ${EXEC}
	@echo "Test result ->"
	@./${EXEC}

${EXEC} : ${CPPS}
	@echo "Making ..."
	${CXX} -I${INCLUDEDIR} ${LDFLAGS} ${CFLAGS} -o ${EXEC} ${CPPS} ${LDLIBS}

clean:
	rm -f ${EXEC} ${EXEC}.exe *.o *.cfbk
