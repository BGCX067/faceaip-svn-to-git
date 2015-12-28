INCLUDE =	-I$(HOME)/libs/include/ppSockets

CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD
CPPFLAGS =	$(CFLAGS) 
LIBS =		-L$(HOME)/libs/lib -lppSockets-linux-x86-32 -lpthread

PROGS =		GeneralAgent

all: 		$(PROGS)

GeneralAgent:	GeneralAgentHandler.o GeneralAgentTcpSocket.o GeneralAgent.o GeneralAgentTcpSocket.o GeneralAgentTcpSocketConnector.o GeneralAgentTcpSocketServer.o
		g++ -o $@ $^ $(LIBS)

clean:
		rm -f *.o *~ slask $(PROGS) */*~ *.d *.bak *.cfbk */*.o */*~

FILES =		GeneralAgent*.h GeneralAgent*.cpp	\
		Makefile

tar:	clean
		tar -czf GeneralAgent.tar.gz $(FILES)

-include	*.d
