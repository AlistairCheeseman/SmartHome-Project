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

build: main.o UDP.o Timing.o SensorNet.o SPI.o Radio.o MQTTSNCheck.o GPIO.o
	$(CXX) $(LDFLAGS) main.o UDP.o Timing.o SensorNet.o SPI.o Radio.o MQTTSNCheck.o GPIO.o  -o wireless-nrf
main.o: 
	$(CXX) $(CFLAGS) main.cpp
UDP.o:
	$(CXX) $(CFLAGS) UDP.cpp 
Timing.o:
	$(CXX) $(CFLAGS) Timing.cpp 
SensorNet.o:
	$(CXX) $(CFLAGS) SensorNet.cpp
SPI.o:
	$(CXX) $(CFLAGS) SPI.cpp
Radio.o:
	$(CXX) $(CFLAGS) Radio.cpp
MQTTSNCheck.o:
	$(CXX) $(CFLAGS) MQTTSNCheck.cpp
GPIO.o:
	$(CXX) $(CFLAGS) GPIO.cpp
	
	
	
clean: 
	rm -fv *.o wireless-nrf
install: build
	$(INSTALL) wireless-nrf $(DESTDIR)/bin
deploy: cross
	scp wireless-nrf root@192.168.1.237:/usr/bin



