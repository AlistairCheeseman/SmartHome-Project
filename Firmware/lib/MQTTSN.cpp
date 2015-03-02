/* 
* MQTTSN.cpp
*
* Created: 28/02/2015 07:53:11
* Author: Ali
*/


#include "MQTTSN.h"

// default constructor
MQTTSN::MQTTSN(SensorNet& snetwork, uint8_t clientId)
{
	this->clientId = clientId;
	this->network = &snetwork;
	this->currentState = STATE_DISCONNECTED;
} //MQTTSN

// default destructor
MQTTSN::~MQTTSN()
{
} //~MQTTSN
void MQTTSN::ping()
{
	packet.sanitise();
	packet.msgType = PINGREQ;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0]);
	this->currentState = STATE_WAIT_PINGRESP;
	
	this->lastTransmission = Timing::millis();
}
void MQTTSN::pingresponse()
{
packet.sanitise();
		packet.msgType = PINGRESP;
		unsigned char payload[20];
		packet.gen_packet(payload, 0x00);
		network->sendpacket(payload, payload[0]);
		
		this->lastTransmission = Timing::millis();
}

void MQTTSN::tick()
{
	network->tick();
}
void MQTTSN::connect()
{
	packet.sanitise();
		packet.msgType = CONNECT;
		packet.flags = (CLEAN_SESSION);
		packet.duration = KEEPALIVE;
		packet.clientId =this->clientId;
		unsigned char payload[20];
		packet.gen_packet(payload, 0x00);
		network->sendpacket(payload, payload[0]);
		this->currentState = STATE_WAIT_CONNACK;
		
		this->lastTransmission = Timing::millis();
}
void MQTTSN::disconnect(bool isresponse)
{
	packet.sanitise();
	packet.msgType = DISCONNECT;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0]);
	if (isresponse != true)
	this->currentState = STATE_WAIT_DISCONNECT;
	
		this->lastTransmission = Timing::millis();
}
void MQTTSN::subscribe()
{
	
}
void MQTTSN::unsubscribe()
{
	
}
void MQTTSN::publish()
{
	
}