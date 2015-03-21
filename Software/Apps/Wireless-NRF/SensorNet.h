/* 
 * File:   SensorNet.h
 * Author: alistair
 *
 * Handles the transmission and routing of the wireless network.
 * Created on 26 February 2015, 19:11
 */

#ifndef SENSORNET_H
#define	SENSORNET_H

#include <unistd.h>
#include "Radio.h"
#include "Timing.h"

class SensorNet {
public:
    SensorNet();

    virtual ~SensorNet();
    void tick(void);
    void setup(void);
        bool pendingpacket;
    void sendpacket(const void* payload, const uint8_t len, uint32_t sourceId, uint32_t destId);
    void *getpacket(uint8_t& len, uint32_t& sourceId, uint32_t& destId);
    
private:
    void send(uint64_t txaddr,  const void* send_payload, uint8_t len);
    Timing* timing;
    Radio* radio;
   
  uint64_t pipes[2];
   static const int min_payload_size = 4;
   static const int max_payload_size = 32;
   static const int payload_size_increments_by = 2;
    char receive_payload[max_payload_size + 1]; // +1 to allow room for a terminating NULL char
   
 uint8_t receive_size; 
 uint32_t sourceId;
 uint32_t destId;
 bool debug;


};

#endif	/* SENSORNET_H */

