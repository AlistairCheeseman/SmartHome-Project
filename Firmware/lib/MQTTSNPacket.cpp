/*
* MQTTSNPacket.cpp
*
* Created: 01/03/2015 15:23:28
*  Author: Ali
*/

#include "MQTTSNPacket.h"


MQTTSNPacket::MQTTSNPacket()
{
	//clean all variables, initialise everything to zero.
	sanitise();
}
MQTTSNPacket::~MQTTSNPacket()
{
}
//this takes in a raw mqtt packet and loads it into the relevant variables in this class depending on what packet type it is.
bool MQTTSNPacket::load_packet(unsigned char * payload)
{
	int t;
	this->msgType = payload[1];
	this->length = payload[0];
	if (payload[1] == CONNECT) // find what packet type it is.
	{
		if (length != 7)
		return false;
		/*VARIABLE LENGTH MESSAGE (FORCE FIXED) */
		//mask off unused flags ( only need CleanSession and Will)
		this->flags = 	payload[2];
		this->flags = this->flags & 0b0000110;
		this->protocolId =	payload[3];
		this->duration  = (	payload[4]<< 8)| payload[5];
		this->clientId =	payload[6];
		return true; // no point looping through rest of routine.
	}
	else if (payload[1] == CONNACK)
	{
		this->returnCode = payload[2];
		return true;
	}
	else if ((payload[1] == WILLTOPICREQ) | (payload[1] == WILLMSGREQ))
	{
		return true;
	}
	else if (payload[1] == WILLTOPIC)
	{
		if (payload[0] > 0x2)
		{
			this->flags =	payload[2];
			this->flags = (this->flags & 0b01110000);
			for (t= 3; t< length; t++)
			{
				this->willTopic[t - 3] =	payload[t];
			}
		}
		return true;
	}
	else if (payload[1] == WILLMSG)
	{
		for (int t= 2; t< length; t++)
		{
			this->willMsg[t - 2] =	payload[t];
		}
		return true;
	}
	else if (payload[1] == REGISTER)
	{
		this->topicId = (payload[2] << 8)| 	payload[3];
		this->msgId	 = (payload[4] << 8)| payload[5];
		for (t=6;t<length;t++)
		{
			this->willMsg[t - 6] =	payload[t];
		}
		return true;
	}
	else if (payload[1] == REGACK)
	{
		this->topicId = ( payload[2] << 8)| payload[3];
		this->msgId	 = (payload[4] << 8)| payload[5] ;
		this->returnCode = 	payload[6];
		return true;
	}
	else if (payload[1] == PUBLISH)
	{
		this->flags = payload[2];
		this->flags = (this->flags & 0b11110011);
		this->topicId = (payload[3] <<8)|	payload[4];
		this->msgId = (payload[5] <<8)|payload[6];
		for (t = 7; t< length; t++)
		{
			this->data[t - 7] =	payload[t];
		}
		return true;
	}
	else if (payload[1] == PUBACK)
	{
		this->topicId	= (payload[2] << 8)| payload[3];
		this->msgId = (payload[4] << 8)| payload[5];
		this->returnCode = payload[6];
		return true;
	}
	/* QOS L2 */
	else if ((payload[1] == PUBCOMP) | (payload[1] == PUBREC) | (payload[1] == PUBREL))
	{
		this->msgId = (payload[2] << 8 ) | payload[3];
		return true;
	}
	/* END QOS L2 */
	else if ((payload[1] == SUBSCRIBE)|(payload[1] == UNSUBSCRIBE))
	{
		//todo FIX!
		//CANNOT USE TOPIC ID FOR SUBS/UNSUBS!!!!
		//only works for topicID Not topic name. this is to keep packet length short.
		this->flags = 	payload[2];
		if (payload[1] == SUBSCRIBE)
		this->flags = this->flags & 0b11100011;
		if (payload[1] == UNSUBSCRIBE)
		this->flags = this->flags & 0b00000011;
		this->msgId	 = (payload[3] << 8)|payload[4];
		
		this->topicId	 = (payload[5] << 8)|payload[6] ;
		
		return true;
	}
	else if (payload[1] == SUBACK)
	{
		this->flags = 	payload[2];
		this->flags = this->flags & 0b01100000;
		this->topicId	 = (payload[3] << 8)|	payload[4];
		this->msgId = (payload[5] << 8)| payload[6] ;
		this->returnCode = payload[7];
		return true;
	}
	else if (payload[1] == UNSUBACK)
	{
		this->msgId	 = (payload[2] << 8)|payload[3];
		return true;
	}
	else if (payload[1] == PINGREQ)
	{
		/*VARIABLE LENGTH MESSAGE (FORCE FIXED) */
		this->clientId = payload[2];
		return true;
	}
	else if (payload[1] == PINGRESP)
	{
		return true;
	}
	else if (payload[1] == DISCONNECT)
	{
		if (length > 0x02)
		{
			this->duration = (((uint8_t)payload[2] << 8)|(uint8_t)payload[3]);
		}
		return true;
	}
	else if (payload[1] == WILLTOPICUPD)
	{
		if (length > 0x02)
		{
			this->flags = payload[2];
			this->flags = this->flags & 0b01110000;
			for (t= 3; t<(uint8_t)length; t++)
			{
				this->willTopic[t - 3] =	payload[t];
			}
		}
		return true;
	}
	else if (payload[1] == WILLMSGUPD)
	{
		if (length > 0x02)
		{
			for (t= 2; t< ((uint8_t)length) ; t++)
			{
				this->willMsg[t - 3] =	payload[t];
			}
			
		}
		return true;
	}
	else if (((uint8_t)payload[1] == WILLTOPICRESP)| ((uint8_t)payload[1] == WILLMSGRESP))
	{
		this->returnCode = 	(uint8_t)payload[2];
		return true;
	}
	return false;
}
/* varlength is the length of the variable part */
// generate the MQTT packet from what class variables have been set.
void MQTTSNPacket::gen_packet(unsigned char  (&payloadOut)[20],uint8_t varLength)
{
	
	if (msgType == CONNECT) // check what packet it is to load correct packet payload data.
	{
		/*VARIABLE LENGTH MESSAGE (FORCE FIXED) */
		//mask off unused flags ( only need CleanSession and Will)
		this->flags = this->flags & 0b0000110;
		payloadOut[0] = 0x07;
		payloadOut[1] = this->msgType;
		payloadOut[2] = this->flags;
		payloadOut[3] = this->protocolId;
		payloadOut[4] = (this->duration >> 8);
		payloadOut[5] = this->duration;
		payloadOut[6] = this->clientId;
		return; // no poiint looping through rest of routine.
	}
	else if (msgType == CONNACK)
	{
		payloadOut[0] = 0x03;
		payloadOut[1] = this->msgType;
		payloadOut[2] = this->returnCode;
		return;
	}
	else if ((msgType == WILLTOPICREQ) | (msgType == WILLMSGREQ))
	{
		payloadOut[0] = 0x02;
		payloadOut[1] = this->msgType;
		return;
	}
	else if (msgType == WILLTOPIC)
	{
		this->flags = (this->flags & 0b01110000);
		if (this->flags && this->willTopic) //if flags and willtopic are empty then it is a reset packet length 2.
		{
			payloadOut[0] =0x02;
		}
		else
		{
			payloadOut[0] =varLength + 3;
			payloadOut[2] = this->flags;
			for (int t= 0; t< varLength; t++)
			{
				payloadOut[3+t] = this->willTopic[t];
			}
		}
		payloadOut[1] = this->msgType;
		return;
	}
	else if (msgType == WILLMSG)
	{
		payloadOut[0] = varLength +2;
		payloadOut[1] = this->msgType;
		for (int t= 0; t< varLength; t++)
		{
			payloadOut[2+t] = this->willMsg[t];
		}
		return;
	}
	else if (msgType == REGISTER)
	{
		payloadOut[0] = varLength + 0x06;
		payloadOut[1] = this->msgType;
		payloadOut[2] = (this->topicId >> 8);
		payloadOut[3] = this->topicId;
		payloadOut[4] = (this->msgId >> 8);
		payloadOut[5] = this->msgId;
		for (int t= 0; t< varLength; t++)
		{
			payloadOut[6+t] = this->topicname[t];
		}
		return;
	}
	else if (msgType == REGACK)
	{
		payloadOut[0] = 0x07;
		payloadOut[1] = this->msgType;
		payloadOut[2] = (this->topicId >> 8);
		payloadOut[3] = this->topicId;
		payloadOut[4] = (this->msgId >>8);
		payloadOut[5] = this->msgId;
		payloadOut[6] = this->returnCode;
		return;
	}
	else if (msgType == PUBLISH)
	{
		this->flags = (this->flags & 0b11110011);
		payloadOut[0] = 0x07 + varLength;
		payloadOut[1] = this->msgType;
		payloadOut[2] = this->flags;
		payloadOut[3] = (this-> topicId >>8);
		payloadOut[4] = this->topicId;
		payloadOut[5] = (this->msgId >>8);
		payloadOut[6] = (this->msgId);
		for (int t= 0; t< varLength; t++)
		{
			payloadOut[7+t] = this->data[t];
		}
		return;
	}
	else if (msgType == PUBACK)
	{
		payloadOut[0] = 0x07;
		payloadOut[1] = this->msgType;
		payloadOut[2] = (this->topicId >>8);
		payloadOut[3] = this->topicId;
		payloadOut[4] = (this->msgId >> 8);
		payloadOut[5] = this->msgId;
		payloadOut[6] = returnCode;
		return;
	}
	/* QOS L2 */
	else if ((msgType == PUBCOMP) | (msgType == PUBREC) | (msgType == PUBREL))
	{
		payloadOut[0] = 0x04;
		payloadOut[1] = this->msgType;
		payloadOut[2] = (this->msgId >>8 );
		payloadOut[3] = this->msgId;
		return;
	}
	/* END QOS L2 */
	else if ((msgType == SUBSCRIBE)|(msgType == UNSUBSCRIBE))
	{//CANNOT USE TOPIC ID FOR SUBS/UNSUBS!!!!

		int t;
		if (msgType == SUBSCRIBE)
		this->flags = this->flags & 0b11100011;
		if (msgType == UNSUBSCRIBE)
		this->flags = this->flags & 0b00000011;
		
		payloadOut[0] = 0x05 + varLength;
		payloadOut[1] = this->msgType;
		payloadOut[2] = this->flags;
		payloadOut[3] = (this->msgId >>8);
		payloadOut[4] = (this->msgId);
		for (t= 0; t< varLength; t++)
		{
			payloadOut[5+t] = this->topicname[t];
		}
		
		return;
	}
	else if (msgType == SUBACK)
	{
		this->flags = this->flags & 0b01100000;
		payloadOut[0] = 0x08;
		payloadOut[1] = this->msgType;
		payloadOut[2] = this->flags;
		payloadOut[3] = (this->topicId>>8);
		payloadOut[4] = this->topicId;
		payloadOut[5] = (this->msgId >> 8);
		payloadOut[6] = this->msgId;
		payloadOut[7] = this->returnCode;
		return;
	}
	else if (msgType == UNSUBACK)
	{
		payloadOut[0] = 0x04;
		payloadOut[1] = this->msgType;
		payloadOut[2] = (this->msgId >>8);
		payloadOut[3] = this->msgId;
		return;
	}
	else if (msgType == PINGREQ)
	{
		/*VARIABLE LENGTH MESSAGE (FORCE FIXED) */
		payloadOut[0] = 0x03;
		payloadOut[1] = this->msgType;
		payloadOut[2] = this->clientId;
		return;
	}
	else if (msgType == PINGRESP)
	{
		payloadOut[0] = 0x2;
		payloadOut[1] = this->msgType;
		return;
	}
	else if (msgType == DISCONNECT)
	{
		if (this->duration)
		{
			payloadOut[0] = 0x04;
			payloadOut[2] = (this->duration >>8);
			payloadOut[3] = this->duration;
		}
		else
		{
			payloadOut[0] = 0x02;
		}
		payloadOut[1] = this->msgType;
		return;
	}
	else if (msgType == WILLTOPICUPD)
	{
		this->flags = this->flags & 0b01110000;
		if (this->flags && this->willTopic)
		{
			payloadOut[0] = 0x02;
		}
		else
		{
			payloadOut[0] = 3+ varLength;
			payloadOut[2] = this->flags;
			for (int t= 0; t< varLength; t++)
			{
				payloadOut[3+t] = this->willTopic[t];
			}
			
		}
		payloadOut[1] = this->msgType;
		return;
	}
	else if (msgType == WILLMSGUPD)
	{
		payloadOut[0] = 2 + varLength;
		payloadOut[1] = this->msgType;
		for (int t= 0; t< varLength; t++)
		{
			payloadOut[2+t] = this->willMsg[t];
		}
		return;
	}
	else if ((msgType == WILLTOPICRESP)| (msgType == WILLMSGRESP))
	{
		payloadOut[0] = 0x03;
		payloadOut[1] = this->msgType;
		payloadOut[2] = this->returnCode;
		return;
	}
}
// clean slate the whole class, to ensure everything is initialised to zero. this is called between packet generation/decoding.
void MQTTSNPacket::sanitise()
{
	
	this->length = 0x00;
	this->msgType = 0x00;
	this->duration = 0x00;
	this->flags = 0x00;
	this->msgId = 0x00;
	this->protocolId = 0x01; // this is always this number
	this->radius = 0x00;
	this->returnCode = 0x00;
	this->topicId = 0x00;
	this->clientId = 0x00;
	this->data[0] = 0x00;
	this->data[1] = 0x00;
	this->data[2] = 0x00;
	this->data[3] = 0x00;
	this->data[4] = 0x00;
	this->data[5] = 0x00;
	this->data[6] = 0x00;
	this->data[7] = 0x00;
	this->data[8] = 0x00;
	this->data[9] = 0x00;
	this->data[10] = 0x00;
	
	this->topicname[0] = 0x00;
	this->topicname[1] = 0x00;
	this->topicname[2] = 0x00;
	this->topicname[3] = 0x00;
	this->topicname[4] = 0x00;
	this->topicname[5] = 0x00;
	this->topicname[6] = 0x00;
	this->topicname[7] = 0x00;
	this->topicname[8] = 0x00;
	this->topicname[9] = 0x00;
	this->topicname[10] = 0x00;
	this->topicname[11] = 0x00;
	this->topicname[12] = 0x00;
	this->topicname[13] = 0x00;
	this->topicname[14] = 0x00;
	this->topicname[15] = 0x00;
	this->willMsg[0] = 0x00;
	this->willMsg[1] = 0x00;
	this->willMsg[2] = 0x00;
	this->willTopic[0] = 0x00;
	this->willTopic[1] = 0x00;
	this->willTopic[2] = 0x00;
	this->willTopic[3] = 0x00;
	this->willTopic[4] = 0x00;
	this->willTopic[5] = 0x00;
	this->willTopic[6] = 0x00;
	this->willTopic[7] = 0x00;
	this->willTopic[8] = 0x00;
	this->willTopic[9] = 0x00;
	this->willTopic[10] = 0x00;
	this->willTopic[11] = 0x00;
	this->willTopic[12] = 0x00;
	this->willTopic[13] = 0x00;
	this->willTopic[14] = 0x00;
	this->willTopic[15] = 0x00;
}