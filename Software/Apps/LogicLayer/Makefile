# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
INSTALL=install



CFLAGS=-g -c -Wall -I./ $(INC_DIR)
LDFLAGS=-lmosquittopp -lsqlite3 

all: clean build

ifeq ($(MAKELEVEL), 0)
cross: 
	bash -c "source ../../LFS/buildscript.sh; \
	for var in \$$(compgen -v); do export \$$var; done; \
	export INC_DIR=-I\$$TARGETFS/include; \
    $(MAKE) $@"
else
cross: clean build
endif



# the actual build 
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
deploy: cross
	scp logiclayer root@192.168.1.237:/usr/bin
