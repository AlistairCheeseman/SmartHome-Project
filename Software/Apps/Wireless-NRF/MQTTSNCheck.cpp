#include "MQTTSNCheck.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//this is a very simple packet checker. it simply checks the actual length and the expected length.

bool MQTTSNCheck::verifyPacket(void *(&payload), uint8_t &length) {
    const uint8_t* buf = reinterpret_cast<const uint8_t*> (payload);
    if (buf[0] == 0x00) {
        fprintf(stderr, "ERROR: PACKET INVALID: packet length header is not valid\n");
        return false;
    }
    if (buf[0] == 0x01) {
        fprintf(stderr, "ERROR: PACKET INVALID: packet length is too long to process\n");
        return false;
    }
    if (buf[0] != length) {
        fprintf(stderr, "ERROR: PACKET INVALID: read %d bytes but header states length is %d\n", (int) length, (int) buf[0]);
        return false;
    }

    /*route to fix a bug in RSMB Server. where publish is padded with a LOT of unneeded bits causing packet size to be more than doubled.*/
    if (buf[1] == 0x0C) {
        if (buf[0] > 8) {
            //if it is bigger than the minmum length
            int end = 0;
            for (int t = 7; t < length; t++) // loop from the 2nd data byte to the last.
            {
                if (buf[t] == 0x00) {
                    //ITS A NULL BYTE!
                    break;
                }
                end++;
            }
            if (end > 0) {
                //we have found some null bits starting at end + 7.
                uint8_t newlen = 7 + end; // set new packet length.

                char newbuf[newlen];
                newbuf[0] = newlen;
                for (int q = 1; q < newlen; q++) {
                    newbuf[q] = buf[q];
                }
                memcpy(payload, newbuf, newlen);
                //      payload = newbuf;
                length = newlen;
            }
        }
    }

    fprintf(stdout, "MQTT PACKET :0x");
    for (int q = 0; q < length; q++)
        fprintf(stdout, "%.2x", buf[q]);
    fprintf(stdout, " \n");
    char* topic = new char[20];

    switch (buf[1]) {
        case 0x00:
            topic = "ADVERTISE\n";
            break;
        case 0x01:
            topic = "SEARCHGW\n";
            break;
        case 0x02:
            topic = "GWINFO\n";
            break;
        case 0x04:
            topic = "CONNECT\n";
            break;
        case 0x05:
            topic = "CONNACK\n";
            break;
        case 0x06:
            topic = "WILLTOPICREQ\n";
            break;
        case 0x07:
            topic = "WILLTOPIC\n";
            break;
        case 0x08:
            topic = "WILLMSGREQ\n";
            break;
        case 0x09:
            topic = "WILLMSG\n";
            break;
        case 0x0A:
            topic = "REGISTER\n";
            break;
        case 0x0B:
            topic = "REGACK\n";
            break;
        case 0x0C:
            topic = "PUBLISH\n";
            break;
        case 0x0D:
            topic = "PUBACK\n";
            break;
        case 0x0E:
            topic = "PUBCOMP\n";
            break;
        case 0x0F:
            topic = "PUBREC\n";
            break;
        case 0x10:
            topic = "PUBREL\n";
            break;
        case 0x12:
            topic = "SUBSCRIBE\n";
            break;
        case 0x13:
            topic = "SUBACK\n";
            break;
        case 0x14:
            topic = "UNSUBSCRIBE\n";
            break;
        case 0x15:
            topic = "UNSUBACK\n";
            break;
        case 0x16:
            topic = "PINGREQ\n";
            break;
        case 0x17:
            topic = "PINGRESP\n";
            break;
        case 0x18:
            topic = "DISCONNECT\n";
            break;
        case 0x1A:
            topic = "WILLTOPICUPD\n";
            break;
        case 0x1B:
            topic = "WILLTOPICRESP\n";
            break;
        case 0x1C:
            topic = "WILLMSGUPD\n";
            break;
        case 0x1D:
            topic = "WILLMSGRESP\n";
            break;
        default:
            break;
    }


    printf("%c[%d;%d;%dm%s", 0x1B, 1, 32, 40, topic);
    printf("%c[%dm", 0x1B, 0);


    return true;

}
