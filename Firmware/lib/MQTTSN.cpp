/*
* MQTTSN.cpp
*
* Created: 28/02/2015 07:53:11
* Author: Ali
*/


#include "MQTTSN.h"

// default constructor
MQTTSN::MQTTSN(SensorNet& snetwork, uint8_t clientId, uint32_t MAC)
{
	this->clientId = clientId;
	this->network = &snetwork;
	this->currentState = STATE_DISCONNECTED;
	this->msgid = 0x00;
	this->macId = MAC;
	this->destId = 0x000000; //default base station id.
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
	network->sendpacket(payload, payload[0], macId, destId);
	this->currentState = STATE_WAIT_PINGRESP;
	
	this->lastTransmission = Timing::millis();
}
void MQTTSN::pingresponse()
{
	packet.sanitise();
	packet.msgType = PINGRESP;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0], macId, destId);
	
	this->lastTransmission = Timing::millis();
}

void MQTTSN::tick()
{
	//are these delays needed?
	//	_delay_ms(50);
	network->tick();
	_delay_ms(50);
	uint32_t destId;
	uint32_t sourceId;
	if (network->pendingpacket)
	{//if there is a pending app layer packet that needs to be processed.
		packet.sanitise();
		void *payload;
		uint8_t len;
		payload = network->getpacket(&len, &sourceId, &destId);
		
		
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
			case PUBLISH:
			// there has been an update to a registered topic. callback to the main program with the data and the topic id.
			if ((packet.flags & 0b00000011) == TOPICID_NUM)
			{
				//at the moment only working off ID numbers.
				if (callback)
				{
					int len = packet.length - 0x07;
					callback(packet.topicId, packet.data, len);
				}
			}
			//only QOS0 supported so we do not need to send a reply.
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
			case SUBACK:
			case REGACK:
			//write the topic id to the look up table. for the topic.
			this->topicIdResp = packet.topicId;
			this->currentState = STATE_ACTIVE;
			break;
			case UNSUBSCRIBE:
			
			break;
			case UNSUBACK:
			
			break;
			case SUBSCRIBE:
			//should not recieve a subscribe request as a client.
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
	if ((this->currentState == STATE_ACTIVE) | (this->currentState==STATE_WAIT_PINGRESP))
	{
		
		unsigned long timenow = Timing::millis();
		timenow = timenow - this->lastTransmission;
		timenow = timenow / 1000;
		if (timenow > KEEPALIVE_THRESHOLD && this->currentState==STATE_ACTIVE)
		{
			//sendping
			this->ping();
		}
		if (timenow > KEEPALIVE && this->currentState==STATE_WAIT_PINGRESP) // the timeout period has expired. the server now believes we are disconnected, update to reflect this.
		{
			this->currentState = STATE_DISCONNECTED;
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
	network->sendpacket(payload, payload[0], macId, destId);
	this->currentState = STATE_WAIT_CONNACK;
	this->lastTransmission = Timing::millis();
}
void MQTTSN::disconnect(bool isresponse)
{
	packet.sanitise();
	packet.msgType = DISCONNECT;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0], macId, destId);
	
	if (isresponse != true)
	this->currentState = STATE_WAIT_DISCONNECT;
	
	this->lastTransmission = Timing::millis();
}
uint16_t MQTTSN::subscribe(unsigned char *topicName, uint8_t topicNameLen)
{
	int t;
	//send subscribe, wait for SUBACK
	packet.sanitise();
	packet.msgType = SUBSCRIBE;
	//THE TOPICID FLAGS ARE CUSTOM FOR SUBSCRIBE!!!!!/UNSUBSCRIBE!!!!!	//THE TOPICID FLAGS ARE CUSTOM FOR SUBSCRIBE!!!!!/UNSUBSCRIBE!!!!!
	packet.flags = (QOS_NORMAL|SUBS_TOPICID_NAME);
	packet.msgId = getmsgid();
	for (t=0;t<topicNameLen; t++)
	{
		packet.topicname[t] = topicName[t];
	}
	
	
	unsigned char payload[20];
	packet.gen_packet(payload, topicNameLen);
	network->sendpacket(payload, payload[0], macId, destId);
	this->currentState = STATE_WAIT_SUBACK;
	this->lastTransmission = Timing::millis();
	return this->waitResponse(payload);
}
void MQTTSN::unsubscribe(unsigned char *topicName, uint8_t topicNameLen)
{
	//lookup the topic id.( send REGISTER)
	
	//THE TOPICID FLAGS ARE CUSTOM FOR SUBSCRIBE!!!!!/UNSUBSCRIBE!!!!!	//THE TOPICID FLAGS ARE CUSTOM FOR SUBSCRIBE!!!!!/UNSUBSCRIBE!!!!!
	//wait for response with ID (REGACK)
	uint8_t topicid = this->gettopicid(topicName, topicNameLen);
	//send unsubscribe, wait for UNSUBACK
	packet.sanitise();
	packet.topicId = topicid;

	this->lastTransmission = Timing::millis();
}
void MQTTSN::publish(unsigned char *topicName,unsigned char *payloaddata, uint8_t topicNameLen, uint8_t payloadlen)
{

	//lookup the topic id.( send REGISTER)
	//wait for response with ID (REGACK)
	uint16_t topicid = this->gettopicid(topicName, topicNameLen);
	//send publish, wait for PUBACK
	this->publish(topicid, payloaddata, payloadlen);
}
void MQTTSN::publish(uint16_t topicid,unsigned char *payloaddata, uint8_t payloadlen)
{
	int t = 0;
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
	network->sendpacket(payload, payload[0], macId, destId);
	this->currentState = STATE_ACTIVE; // only if >QOS0 do we have to set waiting reply.
	this->lastTransmission = Timing::millis();
	
}
int MQTTSN::getmsgid()
{
	this->msgid ++;
	return this->msgid;
}
uint16_t MQTTSN::gettopicid(unsigned char *topicNameIn, uint8_t length)
{
	int t = 0;
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
	this->currentState = STATE_WAIT_REGISTER;
	network->sendpacket(payload, payload[0], macId, destId);
	uint16_t id = waitResponse(payload);
	this->topicIdResp = 0; //stop it polluting other things.
	return id;
}
void MQTTSN::setCallback(void(*callbackfunc)(uint16_t topicId, uint8_t *payload,unsigned int payloadLen))
{
	this->callback = callbackfunc;
}
uint16_t MQTTSN::waitResponse(unsigned char  (&payload)[20])
{
	unsigned long timenow = 	Timing::millis();

	bool hadresponse = false;
	int count = 0;
	while (!hadresponse)
	{
		_delay_ms(250); // increase to allow for response before resend as looping too quick and missing response..
		this->tick();
		if (this->currentState == STATE_ACTIVE)
		{
			//WE HAVE RESPONSE !;
			hadresponse = true;
			break;
		}
		count++;
		if (count > 9)
		{
			//if we have waited for a bit resend the packet.
			network->sendpacket(payload, payload[0], macId, destId);
			count = 0;
			//if recieved disconnect packet or it has timeout (60 seconds = 10,000milliseconds)
			if ((this->currentState == STATE_DISCONNECTED) | ((Timing::millis() - timenow) > 60000))
			{
				//dont get stuck if connectivity is lost.
				return 0;
			}
		}
	}
	// this is sometimes zero?
	uint16_t id = this->topicIdResp;
	return id;
}