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
	this->msgid = 0x00;
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
	
	if (network->pendingpacket)
	{//if there is a pending app layer packet that needs to be processed.
		packet.sanitise();
		void *payload;
		uint8_t len;
		payload = network->getpacket(&len);
		uint8_t* current = reinterpret_cast<uint8_t*> (payload);
		packet.load_packet(current);
		switch (packet.msgType)
		{//to optimize the more frequent the packet tye the higher up it should be in the select statement.
			case CONNACK:
			if (packet.returnCode ==  ACCEPTED)
			{
				this->currentState= STATE_ACTIVE;
			}
			//todo: handle other return codes but for being this will do for testing.
			break;
			case PINGRESP:
			// do not do anything. the response has been recieved, if this was a broker we would update the active devices list.
			this->currentState = STATE_ACTIVE;
			break;
			case PINGREQ:
			// we have been asked for a ping response, send it. we dont need to do anything on the cient end.
			this->pingresponse();
			break;
			case DISCONNECT:
			//if server initiates, respond with disconnect.
			//if client initiates - do nothing.

			//if we have sent a disconnect and are waiting for confirmation, we dont need to send a response.
			if ((this->currentState != STATE_WAIT_DISCONNECT) && (this->currentState != STATE_DISCONNECTED))
			{
				//for some reason the broker wants us to disconnect
				//send confirmation
				this->disconnect(true);
			}
			this->currentState = STATE_DISCONNECTED;
			break;
			case PUBACK:
			this->currentState = STATE_ACTIVE;
			break;
			case REGACK:
			//write the topic id to the look up table. for the topic.
			this->topicIdResp = packet.topicId;
			this->currentState = STATE_ACTIVE;
			break;
			case REGISTER:
			// for the time being we are ignoring this util cached mode is enabled.
			break;
			case CONNECT:
			//a client should not receive a connect request so leaving out for time being.
			break;
			/* unimplemented message types. not implemented QOS or WILLS  */
			case WILLTOPICUPD:
			case WILLMSGUPD:
			case WILLTOPICRESP:
			case WILLMSGRESP:
			case PUBREC:
			case PUBREL:
			case PUBCOMP:
			case WILLMSG:
			case WILLMSGREQ:
			case WILLTOPIC:
			case WILLTOPICREQ:
			break;
		}
	}
	
	//after any packet processing has finished. check that the timeout is not near, if it is, buzz the server.
	if (this->currentState == STATE_ACTIVE)
	{
		unsigned long timenow = Timing::millis();
		timenow = timenow - this->lastTransmission;
		timenow = timenow / 1000;
		if (timenow > KEEPALIVE_THRESHOLD)
		{
			//sendping
			this->ping();
		}
	}
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
void MQTTSN::subscribe(unsigned char *topicName, uint8_t topicNameLen)
{
	//lookup the topic id.( send REGISTER)
	//wait for response with ID (REGACK)
	uint8_t topicid = this->gettopicid(topicName, topicNameLen);
	//send subscribe, wait for SUBACK
	packet.sanitise();
	packet.msgType = SUBSCRIBE;
	packet.flags = (QOS_NORMAL|TOPICID_NUM);
	packet.msgId = getmsgid();
	packet.topicId = topicid;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0]);
	this->currentState = STATE_WAIT_SUBACK;
	this->lastTransmission = Timing::millis();
}
void MQTTSN::unsubscribe(unsigned char *topicName, uint8_t topicNameLen)
{
	//lookup the topic id.( send REGISTER)
	//wait for response with ID (REGACK)
	uint8_t topicid = this->gettopicid(topicName, topicNameLen);
	//send unsubscribe, wait for UNSUBACK
	packet.sanitise();
	

	this->lastTransmission = Timing::millis();
}
void MQTTSN::publish(unsigned char *topicName,unsigned char *payloaddata, uint8_t topicNameLen, uint8_t payloadlen)
{
	int t = 0;
	//lookup the topic id.( send REGISTER)
	//wait for response with ID (REGACK)
	uint8_t topicid = this->gettopicid(topicName, topicNameLen);
	//send publish, wait for PUBACK
	printf("begin main publish\n");
	packet.sanitise();
	packet.msgType = PUBLISH;
	packet.flags = (QOS_NORMAL|TOPICID_NUM);
	packet.topicId = topicid;
	packet.msgId = 0x0000; //no qos
		for (t = 0; t< payloadlen; t++)
		{
			packet.data[t] = payloaddata[t];
		}
	unsigned char payload[20];
	packet.gen_packet(payload, payloadlen);
printf("PACKET GENERATED\n");
	network->sendpacket(payload, payload[0]);
printf("PUBLISH SENT\n");
	this->currentState = STATE_ACTIVE; // only if >QOS0 do we have to set waiting reply.
	this->lastTransmission = Timing::millis();
}
int MQTTSN::getmsgid()
{
	this->msgid ++;
	return this->msgid;
	
}
int MQTTSN::gettopicid(unsigned char *topicNameIn, uint8_t length)
{
	int t = 0;
	bool hadresponse = false;
	packet.sanitise();
	packet.msgType = REGISTER;
	packet.topicId = 0x00;
	packet.msgId = getmsgid();
	for (t = 0; t< length; t++)
	{
		packet.topicname[t] = topicNameIn[t];
	}
	unsigned char payload[20];
	packet.gen_packet(payload, length);
	network->sendpacket(payload, payload[0]);
	while (!hadresponse)
	{
		this->tick();
		if (this->currentState == STATE_ACTIVE)
		{
			//WE HAVE RESPONSE !;
			hadresponse = true;
		}
	}
	uint8_t id = 1;
	printf("Topic ID: %d\n", id);
	this->topicIdResp = 0; //stop it polluting other things.
	return id;
}