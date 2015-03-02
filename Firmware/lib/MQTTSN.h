/*
* MQTTSN.h
*
* Created: 28/02/2015 07:53:11
* Author: Ali
//this needs to manage connection, keepalive, disconnection, sleep automatically.
//it also needs to process the incoming packets sending the relevant response.
*/


#ifndef __MQTTSN_H__
#define __MQTTSN_H__


//SET TO ROUGH RECCOMMENDED VALUES
//MAXIMUM TIME TO WAIT IF CONGESTION STATUS RECIEVED BEFORE RETRYING. SECONDS
#define T_WAIT 0d600
//retry count.
#define N_RETRY 4
//maximum time between transmissions
#define KEEPALIVE 0xC8
//time threshold to attempt a ping.
#define KEEPALIVE_THRESHOLD KEEPALIVE/2


//the states a device can be in.
//WAITING FOR ACK STATES.
#define STATE_WAIT_CONNACK		0x10
#define STATE_WAIT_REGISTER		0x20
#define STATE_WAIT_PUBACK		0x30
#define STATE_WAIT_SUBACK		0x40
#define STATE_WAIT_UNSUBACK		0x50
#define STATE_WAIT_PINGRESP		0x60
#define STATE_WAIT_DISCONNECT	0x70
//NOMINAL STATES
#define STATE_ASLEEP			0x01
#define STATE_DISCONNECTED		0x02
#define STATE_ACTIVE			0x03


#include "SensorNet.h"
#include "MQTTSNPacket.h"
#include "Timing.h"

class MQTTSN
{
	//variables
	public:
	protected:
	private:
	SensorNet* network;
	uint8_t clientId;
	MQTTSNPacket packet;
	uint8_t currentState;
	unsigned long lastTransmission;

	//functions
	public:
	MQTTSN(SensorNet& snetwork, uint8_t clientId);
	~MQTTSN();
		void tick(void);
		void connect(void);
		void disconnect(bool isresponse);
		void subscribe();
		void unsubscribe();
		void publish();
	protected:
	private:
	MQTTSN( const MQTTSN &c );
	MQTTSN& operator=( const MQTTSN &c );
	void ping(void);
	void pingresponse(void);
}; //MQTTSN

#endif //__MQTTSN_H__
