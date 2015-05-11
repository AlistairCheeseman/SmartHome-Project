/*
* MQTTSN.h
*
* Created: 28/02/2015 07:53:11
* Author: Ali
* //this needs to manage connection, keepalive, disconnection, sleep automatically.
* //it also needs to process the incoming packets sending the relevant response.
*  There are two types of message processing available.  
*		cached - a look up table of topics to ID is stored in RAM or EEPROM.
*		non-cached - every time a publish ( in either direction) is issued a poll to the server to get the id or topic name must also be sent before or after the publish.
*			current plans are for non cached, caching will be a potential future feature.
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
#define STATE_WAIT_MASK			0xF0
//NOMINAL STATES
#define STATE_ASLEEP			0x01
#define STATE_DISCONNECTED		0x02
#define STATE_ACTIVE			0x03


#define TOPIC_STATUS_UPDATE "S"
#define TOPIC_REQUEST "R"
#define TOPIC_STATUS_REQUEST "P"


#include "SensorNet.h"
#include "MQTTSNPacket.h"
#include "Timing.h"
#include <string.h>

class MQTTSN
{
	// need to create a table in ram with each messageId with its corresponding status, incase requests get mixed up or wanything which is a big issue right now. for time being the main program handles this.
	
	
	//variables
	public:
	uint8_t currentState;
	protected:
	private:
	SensorNet* network;
	uint8_t clientId;
	MQTTSNPacket packet;
	unsigned long lastTransmission;
	uint8_t msgid;
	uint8_t topicIdResp;
	uint32_t macId;
uint32_t  destId;
	//functions
	public:
	MQTTSN(SensorNet& snetwork, uint8_t clientId, uint32_t MAC);
	~MQTTSN();
	//called every few seconds to handle any pending data.
		void tick(void);
		//connect to broker
		void connect(void);
		//disconnect from broker
		void disconnect(bool isresponse);
		//subscribe to a topic
		uint16_t subscribe(unsigned char *topicName, uint8_t topicNameLen);
		//unsubscribe from a topic
		void unsubscribe(unsigned char *topicName, uint8_t topicNameLen);
		//publish data to a topic by the topic name
		void publish(unsigned char *topicName,unsigned  char payload[3], uint8_t topicNameLen, uint8_t payloadlen);
		//publish data to a topic by Id
		void publish(uint16_t topicid,unsigned char *payloaddata, uint8_t payloadlen);
		//set a routine to handle when a subscribed message is recieved.
		void setCallback(void(*)(uint16_t topicId, uint8_t *payload,unsigned int payloadLen));
			//get topic id hopes that there are no new messages in between the messages to get the id
			uint16_t gettopicid(unsigned char *topicNameIn, uint8_t length);
			//wait for a response from the server, internally used for gettopicid().
			uint16_t waitResponse(unsigned char  (&payload)[20]);
	protected:
	private:
	//variable to hold the callback function
	void (*callback)(uint16_t topicId, uint8_t *payload,unsigned int payloadLen);
	MQTTSN( const MQTTSN &c );
	MQTTSN& operator=( const MQTTSN &c );
	//send a ping to the server to prevent timeout
	void ping(void);
	//respond to a ping request.
	void pingresponse(void);
	//get a unique message Id.
	int getmsgid();
}; //MQTTSN

#endif //__MQTTSN_H__
