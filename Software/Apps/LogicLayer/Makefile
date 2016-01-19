# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
INSTALL=install


INC_DIR=/data/BBB/targetfs2/include
CFLAGS=-c -Wall -I$(INC_DIR) -I./
LDFLAGS=-lmosquittopp -lsqlite3 

CXX=g++ -g
build: SQL.o main.o mqtt.o ProcessMessage.o
	$(CXX) $(LDFLAGS) SQL.o main.o mqtt.o ProcessMessage.o  -o logiclayer
main.o: 
	$(CXX) $(CFLAGS) main.cpp
SQL.o:
	$(CXX) $(CFLAGS) SQL.cpp 
mqtt.o:
	$(CXX) $(CFLAGS) mqtt.cpp 
ProcessMessage.o:
	$(CXX) $(CFLAGS) ProcessMessage.cpp
clean: 
	rm -fv *.o logiclayer
install: build
	$(INSTALL) logiclayer $(DESTDIR)/bin
