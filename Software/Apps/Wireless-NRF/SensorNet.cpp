/* 
 * File:   SensorNet.cpp
 * Author: alistair
 * 
 * Created on 26 February 2015, 19:11
 */
#include <unistd.h>
#include <stdint.h>
#include "SensorNet.h"
#include "Timing.h"

SensorNet::SensorNet() {
    timing = new Timing();
    radio = new Radio((char*) "/dev/spidev1.0", 115, 117, false);
    pipes[0] = 0xF0F0F0F001LL; // the default writing pipe, this will get set to the node needed by the dest addr.
    pipes[1] = 0xF0F0F0F000LL;
    pipes[2] = 0xF0F0F0F002LL;
    pipes[3] = 0xF0F0F0F003LL;
    pipes[4] = 0xF0F0F0F004LL;
    pipes[5] = 0xF0F0F0F005LL;
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
    radio->openReadingPipe(2, pipes[2]);
    radio->openReadingPipe(3, pipes[3]);
    radio->openReadingPipe(4, pipes[4]);
    radio->openReadingPipe(5, pipes[5]);
    radio->startListening();
    radio->printDetails();
}

void SensorNet::tick() {
    // if there is data ready
    if (radio->available()) {
        char raw_payload[max_payload_size + 1];
        //note if the previous packet has not been processed, it WILL get overwritten.
        // Grab the response, compare, and send to debugging spew
        uint8_t len = radio->getDynamicPayloadSize();
        radio->read(raw_payload, len);
        // Put a zero at the end for easy printing
        raw_payload[len] = 0;
        // central node ID is 0x00 0x00 0x00.
        this->destId = (raw_payload[0] << 16) | (raw_payload[1] << 8) | raw_payload[2];
        // this is used to know what UDP socket to send from.
        this->sourceId = (raw_payload[3] << 16) | (raw_payload[4] << 8) | raw_payload[5];
        for (int pkt = 6; pkt < len; pkt++) {
            this->receive_payload[pkt - 6] = raw_payload[pkt];
        }
        this->receive_size = len - 6; //( 6 bytes, 3 for sender, 3 for recieve Addr)
        this->receive_payload[len] = 0;
        if (debug)
            fprintf(stdout, "Got message size=%i value=%s\n\r", len, receive_payload);
        this->pendingpacket = true;
    }
}

void SensorNet::sendpacket(const void* payload, const uint8_t len, uint32_t sourceId, uint32_t destId, uint8_t nextHopId) {
    //handle the logic that decides which node to send the packet.
    //send it. (temp solution to only send it to the test node)
    uint8_t newlen = len + 6;
    const uint8_t* tempPacket = reinterpret_cast<const uint8_t*> (payload);
    uint8_t* newPacket = new uint8_t[newlen];
    newPacket[0] = (destId >> 16);
    newPacket[1] = (destId >> 8);
    newPacket[2] = (destId);
    newPacket[3] = (sourceId >> 16);
    newPacket[4] = (sourceId >> 8);
    newPacket[5] = (sourceId);
    for (int t = 0; t < len; t++) {
        newPacket[t + 6] = tempPacket[t];
    }
    //todo: perform tanslation of the destId to the radio Address, fixed at a single exit for time being.
    // until address translation can properly occur will simply use this:
    uint64_t nexthop = 0;
    if (nextHopId == 1) // 0 and 1 are switched.
        nexthop = pipes[0];
    else
        nexthop = pipes[(nextHopId)];


     printf("Sending to: 0x%llX for routing\n", nexthop);
    this->send(nexthop, newPacket, newlen);
}

void *SensorNet::getpacket(uint8_t& len, uint32_t& sourceId, uint32_t& destId) {
    this->pendingpacket = false;
    len = this->receive_size;
    sourceId = this->sourceId;
    destId = this->destId;
    return this->receive_payload;

}

void SensorNet::send(uint64_t txaddr, const void* send_payload, uint8_t len) {
    // First, stop listening so we can talk.
    radio->stopListening();
    radio->openWritingPipe(txaddr);
    // Take the time, and send it.  This will block until complete
    radio->write(send_payload, len);

    // Now, continue listening
    radio->startListening();
}