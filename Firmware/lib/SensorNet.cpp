/*
* SensorNet.cpp
*
* Created: 01/03/2015 21:58:23
* Author: Ali
*/


#include "SensorNet.h"

// default constructor
SensorNet::SensorNet(RF24& radio)
{
	this->radio = &radio;
} //SensorNet

// default destructor
SensorNet::~SensorNet()
{
} //~SensorNet

void SensorNet::setup(uint8_t level, uint8_t id)// the level helps us find the level this node operates on. the id is Level0(doesn't matter) Level1(1-5) Level2(11-55) Level3(111- 255)
{
	radio->begin();
	radio->enableDynamicPayloads();
	radio->setRetries(15, 15);
	
	
	uint8_t ADCount = 0;
	ADCount = getAddresses(level, id);
	radio->openWritingPipe(pipes[0]); //parent
	radio->openReadingPipe(1, pipes[1]);
	
	if (ADCount != 2)
	{
		radio->openReadingPipe(2, pipes[2]);
		radio->openReadingPipe(3, pipes[3]);
		if (ADCount != 4)
		{
			radio->openReadingPipe(4, pipes[4]);
			radio->openReadingPipe(5, pipes[5]);
		}
	}
	radio->startListening();
	_delay_ms(1000); //wait for a bit.
	radio->printDetails();
}
void SensorNet::tick()
{
	//force a 200ms wait incase we have just transmitted
	_delay_ms(150); //increased as avergage packet size has increased by six bytes.
	// if there is data ready
	if ( radio->available() )
	{
		printf("Message Received!\n");
		char raw_payload[max_payload_size + 1];
		// Dump the payloads until we've gotten everything
		uint8_t len = radio->getDynamicPayloadSize();
		// Fetch the payload, and see if this was the last one.
		radio->read(raw_payload, len );
		uint32_t shifter;
		shifter = raw_payload[0];
		this->destId = (shifter << 16);
		shifter = raw_payload[1];
		this->destId |= (shifter << 8);
		shifter = raw_payload[2];
		this->destId |= (shifter);

		
		shifter = raw_payload[3];
		this->sourceId = (shifter << 16);
		shifter = raw_payload[4];
		this->sourceId |= (shifter<< 8);
		shifter = raw_payload[5];
		this->sourceId |= (shifter);
		
		printf("Source : %.2x%.2x%.2x\n", (uint8_t)(sourceId >> 16),(uint8_t)(sourceId >> 8),(uint8_t)(sourceId));
		printf("Dest   : %.2x%.2x%.2x\n",(uint8_t)(destId >> 16),(uint8_t)(destId >> 8),(uint8_t)(destId));
		printf("0x");
		for (int pkt = 6; pkt < len; pkt++) {
			this->receive_payload[pkt - 6] = raw_payload[pkt];
			printf("%.2x", raw_payload[pkt]);
		}
		printf("\n");
		this->receive_size = len - 6;
		
		this->pendingpacket = true;
	}
}
void SensorNet::sendpacket(const void* payload, const uint8_t len,const uint32_t sourceId,const uint32_t destId)
{
	uint8_t newlen = len + 6;
	const uint8_t* tempPacket = reinterpret_cast<const uint8_t*> (payload);
	uint8_t newPacket[newlen];
	newPacket[0] = (destId >> 16);
	newPacket[1] = (destId >> 8);
	newPacket[2] = (destId);
	newPacket[3] = (sourceId >> 16);
	newPacket[4] = (sourceId >> 8);;
	newPacket[5] = (sourceId);;
	for (int t = 0; t<len; t++)
	{
		newPacket[t + 6] = tempPacket[t];
	}
	
	
	
	//		_delay_ms(200); //slow down transmission to allow recieve
	//think this is causing lost packets as sensor data doesn't end up processed.
	//this->tick(); //process any pending packets
	
	// First, stop listening so we can talk
	radio->stopListening();
	//radio->openWritingPipe();
	// Send the final one back.
	radio->write(newPacket, newlen );
	// Now, resume listening so we catch the next packets.
	radio->startListening();
}
void *SensorNet::getpacket(uint8_t *len, uint32_t *sourceId, uint32_t *destId)
{
	this->pendingpacket = false;
	len = &this->receive_size;
	sourceId = &this->sourceId;
	destId = &this->destId;
	return this->receive_payload;
}
uint8_t SensorNet::getAddresses(uint8_t level, uint8_t id)
{
	//level is the sensor level we are on.
	//id is the id of this sensor on its level.
	//if level is 1 we are a root node.
	uint64_t address = 0xF0F0F0F000LL;
	uint8_t len = 0;
	if (level == 0)
	{
		//this means we are the central node, the server.// listen to 001, 002, 003, 004 we are 000
		pipes[0] = address;
		pipes[1] = address;
		pipes[2] = address | 0x01 ;
		pipes[3] = address | 0x02 ;
		pipes[4] = address | 0x03 ;
		pipes[5] = address | 0x04 ;
		len = 0x06;
	}
	if (level == 1 )
	{ // we send to the directly to the central device // we are 001-004 listen on 01x, 02x, 03x, 04x.
		pipes[0] = address;
		pipes[1] = address | id;
		pipes[2] = address | (id + 10);
		pipes[3] = address | (id + 20);
		pipes[4] = address | (id + 30);
		pipes[5] = address | (id + 40);
		len =0x06;
	}
	if (level ==2) // we are 01x, 02x etc listen on 1xx,2xx,3xx,4xx.
	{
		uint8_t parent = id % 10;
		pipes[0] = address | parent;
		pipes[1] = address | id;
		pipes[2] = address | (id + 100);
		pipes[3] = address | (id + 200);
		len = 0x04;
	}
	if (level == 3)
	{
		uint8_t parent = id % 100;
		// we have no clients.
		//only send to parent. we are 1xx,2xx,3xx,4xx.
		pipes[0] = address | parent;
		pipes[1] = address | id;
		len =0x02;
	}
	return len;
}