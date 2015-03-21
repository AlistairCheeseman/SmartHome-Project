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
uint64_t pipes[2];
static const int max_payload_size = 32;
char receive_payload[max_payload_size + 1]; // +1 to allow room for a terminating NULL char
uint8_t receive_size;
 uint32_t sourceId;
 uint32_t destId;
bool debug;

//functions
public:
	SensorNet(RF24& radio);
	~SensorNet();
	void tick(void);
	void setup(void);
	bool pendingpacket;
	void sendpacket(const void* payload, const uint8_t len,const uint32_t sourceId,const uint32_t destId);
	void *getpacket(uint8_t *len, uint32_t *sourceId, uint32_t *destId);
protected:
private:
void send(uint64_t txaddr,  const void* send_payload, uint8_t len);
}; //SensorNet

#endif //__SENSORNET_H__
