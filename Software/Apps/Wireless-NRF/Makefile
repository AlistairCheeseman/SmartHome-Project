# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
INSTALL=install


INC_DIR=/data/BBB/targetfs2/include
CFLAGS=-c -Wall -I$(INC_DIR) -I./
LDFLAGS=-lmosquittopp -lsqlite3 

CXX=g++ -g
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



