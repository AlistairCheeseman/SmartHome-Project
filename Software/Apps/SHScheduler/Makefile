# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
INSTALL=install


INC_DIR=/data/BBB/targetfs2/include
CFLAGS=-c -Wall -I$(INC_DIR) -I./
LDFLAGS=-lmosquittopp -lsqlite3 

CXX=g++ -g
build: SQL.o main.o MQTT.o
	$(CXX) $(LDFLAGS) SQL.o main.o MQTT.o  -o shscheduler
main.o: 
	$(CXX) $(CFLAGS) main.cpp
SQL.o:
	$(CXX) $(CFLAGS) SQL.cpp 
MQTT.o:
	$(CXX) $(CFLAGS) MQTT.cpp 
clean: 
	rm -fv *.o shscheduler
install: build
	$(INSTALL) shscheduler $(DESTDIR)/bin
