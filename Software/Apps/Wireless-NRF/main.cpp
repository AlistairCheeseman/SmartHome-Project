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

