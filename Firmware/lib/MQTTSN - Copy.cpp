/*
* MQTTSN.cpp
*
* Created: 28/02/2015 07:53:11
*  Author: Ali
*/
void MQTTSN::connect()
{
	packet.msgType = CONNECT;
	packet.flags = (CLEAN_SESSION);
	packet.duration = KEEPALIVE;
	packet.clientId =nodeid;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0]);
	this->currentState = STATE_WAIT_CONNACK;
}
void MQTTSN::tick()
{
	network->tick();
	if (network->pendingpacket)
	{
		void *payload;
		uint8_t len;
		payload = network->getpacket(&len);
		uint8_t* current = reinterpret_cast<uint8_t*> (payload);
		packet.load_packet(current);
		switch (packet.msgType)
		{
			case CONNECT:
			//a client should not receive a connect request so leaving out for time being.
			break;
			case CONNACK:
			this->currentState= STATE_ACTIVE;
			break;
			case REGISTER:
			//
			
			break;
			case REGACK:
			//write the topic id to the look up table. for the topic.
			
			this->currentState = STATE_ACTIVE;
			break;
			case PUBLISH:
			
			break;
			case PUBACK:
			
			break;
			case SUBSCRIBE:
			//should not recieve a subscribe re
			break;
			case SUBACK:
			
			break;
			case UNSUBSCRIBE:
			
			break;
			case UNSUBACK:
			
			break;
			case PINGREQ:
			// we have been asked for a ping repsponse, send it.
			this->pingresponse();
			break;
			case PINGRESP:
			// do not do anything. the response has been recieved, if this was a broker we would update the active devices list.
			this->currentState = STATE_ACTIVE;
			break;
			case DISCONNECT:
			this->currentState = STATE_DISCONNECTED;
			//if we have sent a disconnect and are waiting for confirmation, we dont need to send a response.
			if (this->currentState != STATE_WAIT_DISCONNECT)
			{
				//for some reason the broker wants us to disconnect
				//send confirmation
				this->disconnect(true);
			}
			break;
			/* unimplemented message types.  */
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
	
 /*	uint32_t lasttrans = Timing::millis();
lasttrans = 	lasttrans - this->lastTransmission;
	if (lasttrans > KEEPALIVE_THRESHOLD)
	{
		this->ping();
	}*/
}
MQTTSN::~MQTTSN()
{
}
/*
void MQTTSN::setcallback(void(*)(char*,uint8_t*,unsigned int))
{
	 this->callback = callback;	
}*/
void MQTTSN::disconnect(bool isresponse)
{
	packet.msgType = DISCONNECT;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0]);
	this->lastTransmission = Timing::millis();
	if (isresponse != true)
	this->currentState = STATE_WAIT_DISCONNECT;
}
void MQTTSN::ping()
{
	packet.msgType = PINGREQ;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0]);
	this->lastTransmission = Timing::millis();
	this->currentState = STATE_WAIT_PINGRESP;
}
void MQTTSN::pingresponse()
{
	packet.msgType = PINGRESP;
	unsigned char payload[20];
	packet.gen_packet(payload, 0x00);
	network->sendpacket(payload, payload[0]);
	this->lastTransmission = Timing::millis();
}




