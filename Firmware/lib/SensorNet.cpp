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

void SensorNet::setup()
{
	uint64_t pipes[2];
	pipes[0] = 0xF0F0F0F0E1LL;
	pipes[1] = 0xF0F0F0F0D2LL;

	radio->begin();
	radio->enableDynamicPayloads();
	radio->setRetries(15, 15);
	radio->openWritingPipe(pipes[1]);
	radio->openReadingPipe(1, pipes[0]);
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