/*
* MQTTSNPacket.h
*
* Created: 01/03/2015 15:23:11
*  Author: Ali
*/


#ifndef MQTTSNPACKET_H_
#define MQTTSNPACKET_H_

#include <stdint.h>
#include <stdlib.h>



/* MESSAGE TYPES */
#define CONNECT			0x04
#define CONNACK			0x05
#define WILLTOPICREQ	0x06
#define WILLTOPIC		0x07
#define WILLMSGREQ		0x08
#define WILLMSG			0x09
#define REGISTER		0x0A
#define REGACK			0x0B
#define PUBLISH			0x0C
#define PUBACK			0x0D
#define PUBCOMP			0x0E
#define PUBREC			0x0F
#define PUBREL			0x10
#define SUBSCRIBE		0x12
#define SUBACK			0x13
#define UNSUBSCRIBE		0x14
#define UNSUBACK		0x15
#define PINGREQ			0x16
#define PINGRESP		0x17
#define DISCONNECT		0x18
#define WILLTOPICUPD	0x1A
#define WILLTOPICRESP	0x1B
#define WILLMSGUPD		0x1C
#define WILLMSGRESP		0x1D

/* MESSAGE FLAGS */
#define DUP				0b10000000
#define QOS_NORMAL		0b00000000
#define QOS_ATLEASTONE	0b00100000
#define QOS_EXACTONCE	0b01000000
#define RETAIN			0b00010000
#define WILL			0b00001000
#define CLEAN_SESSION	0b00000100
#define TOPICID_NUM		0b00000000
#define TOPICID_SET_NUM	0b00000001
#define TOPICID_NAME	0b00000010


/* RETURN CODES */
#define ACCEPTED					0x00
#define REJECTED_CONGESTION			0x01
#define REJECTED_INVALID_TOPIC_ID	0x02
#define REJECTED_NOT_SUPPORTED		0x03




class MQTTSNPacket
{
	//variables
	public:
		/* Message Header (FIXED LENGTH) as maximum RF24 payload is 32 Octets Only the single octect length byte can be supported, where specification suports two bytes length for large packets. */
		uint8_t length;
		uint8_t msgType;
		/*Fixed Length Fields:*/
		uint16_t duration;
		uint8_t flags;
		uint16_t msgId;
		uint8_t protocolId;
		uint8_t radius;
		uint8_t returnCode;
		uint16_t topicId;
		/*variable length fields */
		//have set this to one byte for ease of programming also cannot forsee more than 255 devices on one network for development stages.
		uint8_t clientId;
		//for better ADC resolution the size may have to be increased.
		//to keep message size down the data payload(data) has been restricted to 0-254. this means there are 255 commands but will need to be decoded. There are two main types of data. 1/0 (on/off) or 0-254 (analogue read.)
		unsigned char data[3];
		unsigned char topicname[16];
		unsigned char willMsg[3];
		unsigned char willTopic[16];
	protected:
	private:
	
	//functions
	public:
			/*initializers */
			MQTTSNPacket();
			~MQTTSNPacket();
			
			void sanitise();
			/* in and out functions*/
			/* put all the variables into a assembled packet., return packet size*/
			void gen_packet(unsigned char  (&payloadOut)[20], uint8_t varLength);
			/* Strip down the packet and load it into the class variables. */
			bool load_packet(unsigned char * payload);
	protected:
	private:
		MQTTSNPacket( const MQTTSNPacket &c);
		MQTTSNPacket& operator=(const MQTTSNPacket &c);	
};


#endif /* MQTTSNPACKET_H_ */