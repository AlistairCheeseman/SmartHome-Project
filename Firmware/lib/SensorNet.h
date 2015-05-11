/* 
* SensorNet.h
*
* Created: 01/03/2015 21:58:23
* Author: Ali
*/


#ifndef __SENSORNET_H__
#define __SENSORNET_H__

#include "RF24.h"
class SensorNet
{
//variables
public:
protected:
private:
RF24* radio;
uint64_t pipes[6];
static const int max_payload_size = 32;
char receive_payload[max_payload_size + 1]; // +1 to allow room for a terminating NULL char
uint8_t receive_size;
 uint32_t sourceId;
 uint32_t destId;
 uint32_t myId;
bool debug;

//functions
public:
	SensorNet(RF24& radio, uint32_t MAC);
	~SensorNet();
	//called to handle pending data
	void tick(void);
	//configure everything
	void setup(uint8_t level, uint8_t id);
	//is there any work to do?
	bool pendingpacket;
	//send data
	void sendpacket(const void* payload, const uint8_t len,const uint32_t sourceId,const uint32_t destId);
	//get data
	void *getpacket(uint8_t *len, uint32_t *sourceId, uint32_t *destId);
protected:
private:
void send(
//internal send data function
uint64_t txaddr,  const void* send_payload, uint8_t len);
//get all recieve and tx addresses from the configuration
uint8_t getAddresses(uint8_t level, uint8_t id);
//route a packet down the network.
void routePacket(void);

}; //SensorNet

#endif //__SENSORNET_H__
