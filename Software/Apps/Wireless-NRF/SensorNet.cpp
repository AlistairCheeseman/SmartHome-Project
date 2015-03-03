/* 
 * File:   SensorNet.cpp
 * Author: alistair
 * 
 * Created on 26 February 2015, 19:11
 */
#include <unistd.h>
#include "SensorNet.h"
#include "Timing.h"

SensorNet::SensorNet() {
    timing = new Timing();
    radio = new Radio((char*) "/dev/spidev1.0", 115, 117, false);
    pipes[0] = 0xF0F0F0F0E1LL;
    pipes[1] = 0xF0F0F0F0D2LL;
    receive_size = 0;
    debug = false;
    setup();

}

SensorNet::~SensorNet() {
}

void SensorNet::setup(void) {
    radio->begin();
    radio->enableDynamicPayloads();
    radio->setRetries(15, 15);
    radio->openWritingPipe(pipes[0]);
    radio->openReadingPipe(1, pipes[1]);
    radio->startListening();
    radio->printDetails();
}

void SensorNet::tick() {
    // if there is data ready
    if (radio->available()) {
        //note if the previous packet has not been processed, it WILL get overwritten.
        // Grab the response, compare, and send to debugging spew
        uint8_t len = radio->getDynamicPayloadSize();
        radio->read(receive_payload, len);
        this->receive_size = len;
        // Put a zero at the end for easy printing
        receive_payload[len] = 0;


        if (debug)
         fprintf(stdout,"Got response size=%i value=%s\n\r", len, receive_payload);

        this->pendingpacket = true;

    }
}

void SensorNet::sendpacket(const void* payload, const uint8_t len) {
    //handle the logic that decides which node to send the packet.
    //send it. (temp solution to only send it to the test node)

    this->send(pipes[0], payload, len);
}

void *SensorNet::getpacket(uint8_t& len) {
    this->pendingpacket = false;
    len = this->receive_size;
    return this->receive_payload;

}

void SensorNet::send(uint64_t txaddr, const void* send_payload, uint8_t len) {
    // First, stop listening so we can talk.
    radio->stopListening();

    // Take the time, and send it.  This will block until complete
    radio->write(send_payload, len);

    // Now, continue listening
    radio->startListening();
}