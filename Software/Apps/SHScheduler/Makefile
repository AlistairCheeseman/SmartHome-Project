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
deploy: cross
	scp shscheduler root@192.168.1.237:/usr/bin
