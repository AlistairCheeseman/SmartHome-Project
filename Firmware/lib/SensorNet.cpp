/* 
* SensorNet.cpp
*
* Created: 01/03/2015 21:58:23
* Author: Ali
*/


#include "SensorNet.h"

// default constructor
SensorNet::SensorNet()
{
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

	radio.begin();
	radio.enableDynamicPayloads();
	radio.setRetries(15, 15);
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	radio.startListening();
	radio.printDetails();
}
void SensorNet::tick()
{
	//force a 200ms wait incase we have just transmitted
	_delay_ms(200);
	// if there is data ready
	if ( radio.available() )
	{
		// Dump the payloads until we've gotten everything
		uint8_t len = radio.getDynamicPayloadSize();
					// Fetch the payload, and see if this was the last one.
			radio.read(receive_payload, len );
			this->receive_size = len;
			this->pendingpacket = true;
	}	
}
void SensorNet::sendpacket(const void* payload, const uint8_t len)
{
	// First, stop listening so we can talk
	radio.stopListening();
	// Send the final one back.
	radio.write(payload, len );
	// Now, resume listening so we catch the next packets.
	radio.startListening();
}
void *SensorNet::getpacket(uint8_t *len)
{
	this->pendingpacket = false;
	len = &this->receive_size;
	return this->receive_payload;
}