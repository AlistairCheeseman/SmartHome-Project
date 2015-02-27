/* 
 * File:   main.cpp
 * Author: alistair
 *
 * Created on 26 January 2015, 13:36
 */

#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include "SensorNet.h"
#include "UDP.h"
#include "MQTTSNCheck.h"

using namespace std;
SensorNet* net;
UDP* udp;

/*
 * 
 */
int main(int argc, char** argv) {
    net = new SensorNet();
    udp = new UDP("127.0.0.1", "1884");

    int fd = -1;
    int sock = -1;
    uint8_t currentPackLen = 0;
    /*at the moment the radio will send back whatever is sent to it. so what we send here will get sent back (good for testing) currently set to MQTT-SN connect packet.*/
    uint8_t test[] = {0x07, 0x04, 0x00, 0x01, 0x0F, 0x8C, 0x28};
    net->sendpacket(test, 7);
    while (1) {
        net->tick();
         
     /* Process WIRELESS PACKETS*/
        if (net->pendingpacket == true) {
            void *packet = net->getpacket(currentPackLen);
            if (packet && currentPackLen) {
                if (MQTTSNCheck::verifyPacket(packet, currentPackLen) == true) {
                    printf("Wireless --> UDP\n");
                    udp->sendpacket(packet);
                }
            }
        }
        
        
        /*Process UDP Packets */
          //UDP packed recieving is really slowing it down. need to find better way to check packets.
        if (udp->pendingpacket() == true) {
            void *packet = udp->getpacket(currentPackLen);
            if (packet && currentPackLen) {
                if (MQTTSNCheck::verifyPacket(packet, currentPackLen) == true) {
                    printf("UDP --> Wireless\n");
                    net->sendpacket(packet, currentPackLen);
                }
                currentPackLen = 0;
            }
        }
    }
    return 0;

}

