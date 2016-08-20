# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
INSTALL=install


INC_DIR=/data/BBB/targetfs2/include
CFLAGS=-c -Wall -I$(INC_DIR) -I./
LDFLAGS=-lmosquittopp -lsqlite3 

ifeq ($(MAKELEVEL), 0)
all: 
	bash -c "source /data/smarthome/Software/LFS/buildscript.sh; \
	for var in \$$(compgen -v); do export \$$var; done; \
    $(MAKE) $@"
else
all: build
endif
#CXX=${CROSS_COMPILE}-g++ -g
build: SQL.o main.o mqtt.o ProcessMessage.o log.o
	$(CXX) $(LDFLAGS) SQL.o main.o mqtt.o ProcessMessage.o log.o -o logiclayer
main.o: 
	$(CXX) $(CFLAGS) main.cpp
SQL.o:
	$(CXX) $(CFLAGS) SQL.cpp 
mqtt.o:
	$(CXX) $(CFLAGS) mqtt.cpp 
ProcessMessage.o:
	$(CXX) $(CFLAGS) ProcessMessage.cpp
log.o:
	$(CXX) $(CFLAGS) log.cpp   
clean: 
	rm -fv *.o logiclayer
install: build
	$(INSTALL) logiclayer $(DESTDIR)/bin
deploy: build
	scp logiclayer root@192.168.1.237/usr/bin/