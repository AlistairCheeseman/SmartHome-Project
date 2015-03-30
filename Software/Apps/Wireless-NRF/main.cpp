/* 
 * File:   main.cpp
 * Author: alistair
 *
 * Created on 26 January 2015, 13:36
 */

#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include "SensorNet.h"
#include "UDP.h"
#include "MQTTSNCheck.h"

using namespace std;
SensorNet* net;
UDP* udp[6];
uint32_t clients[6];
uint8_t nexthop[6];

/*
 * 
 */
int main(int argc, char** argv) {
    net = new SensorNet();
    //create all client connections.
    udp[0] = new UDP("127.0.0.1", "1884");
    udp[1] = new UDP("127.0.0.1", "1884");
    udp[2] = new UDP("127.0.0.1", "1884");
    udp[3] = new UDP("127.0.0.1", "1884");
    udp[4] = new UDP("127.0.0.1", "1884");
    udp[5] = new UDP("127.0.0.1", "1884");
    // these will dynamically populate however, setting to fixed values for the time being to allow simple routing.
    clients[0] = 0x87865E; // power outlet
    clients[1] = 0xAEAEAE; // energy sensor
    clients[2] = 0x898989; // heating controller
    clients[3] = 0x6E6E6E; // environment sensor
    clients[4] = 0x000000;
    clients[5] = 0x000000;


    //this is hardcoded. will be dynamic in future, assume turned on in correct order for the clients[] assignments.
    nexthop[0] = 1;
    nexthop[1] = 2;
    nexthop[2] = 3;
    nexthop[3] = 4;







    int fd = -1;
    int sock = -1;
    uint8_t currentPackLen = 0;
    uint32_t destId = 0;
    uint32_t sourceId = 0;
    while (1) {
        net->tick();

        /* Process WIRELESS PACKETS*/
        if (net->pendingpacket == true) {
            void *packet = net->getpacket(currentPackLen, sourceId, destId);
            if (packet && currentPackLen && destId == 0x000000) { //check packet is for the central server has a length and has some data.
                if (MQTTSNCheck::verifyPacket(packet, currentPackLen) == true) { //check it is a valid mqtt packet.
                    fprintf(stdout, "Wireless --> UDP\n");
                    fprintf(stdout, "Source: %.6x\nDest  :%.6x\n", sourceId, destId);
                    uint8_t connection = 0;
                    for (int t = 0; t <= 5; t++) {
                        if (clients[t] == 0x000000) {
                            fprintf(stdout, "added new client to table.\n");
                            // if the client address is 0x000000 it is un used! - make it in use!

                            clients[t] = sourceId;
                        }
                        // loop through all the connections to find which client we should be connecting to.
                        if (clients[t] == sourceId) {
                            connection = t;
                            break;
                        }
                    }
                    udp[connection]->sendpacket(packet);
                    fprintf(stdout, "\n");
                }
            }
        }
        /*Process UDP Packets */ // loop through every udp connection
        for (int t = 0; t <= 5; t++) {
            currentPackLen = 0;
            //UDP packed recieving is really slowing it down. need to find better way to check packets.
            if (udp[t]->pendingpacket() == true) {
                void *packet = udp[t]->getpacket(currentPackLen); // get the headerless mqttsn packet
                if (packet && currentPackLen) { // check it's a mqtt packet.
                    if (MQTTSNCheck::verifyPacket(packet, currentPackLen) == true) {
                        fprintf(stdout, "UDP --> Wireless\n");
                        fprintf(stdout, "Source: %.6x\nDest  : %.6x\n", 0x000000, clients[t]);
                        fprintf(stdout, "Next Hop : %3d\n", nexthop[t]);
                        net->sendpacket(packet, currentPackLen, 0x000000, clients[t],nexthop[t]);
                        fprintf(stdout, "\n");
                    }
                    currentPackLen = 0;
                }
            }
        }
    }
    return 0;

}

