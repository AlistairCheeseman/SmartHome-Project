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
                end++;
                if (buf[t] == 0x00) {
                    //ITS A NULL BYTE!
                    break;
                }
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

    switch (buf[1]) {
        case 0x00:
            fprintf(stdout, "ADVERTISE\n");
            break;
        case 0x01:
            fprintf(stdout, "SEARCHGW\n");
            break;
        case 0x02:
            fprintf(stdout, "GWINFO\n");
            break;
        case 0x04:
            fprintf(stdout, "CONNECT\n");
            break;
        case 0x05:
            fprintf(stdout, "CONNACK\n");
            break;
        case 0x06:
            fprintf(stdout, "WILLTOPICREQ\n");
            break;
        case 0x07:
            fprintf(stdout, "WILLTOPIC\n");
            break;
        case 0x08:
            fprintf(stdout, "WILLMSGREQ\n");
            break;
        case 0x09:
            fprintf(stdout, "WILLMSG\n");
            break;
        case 0x0A:
            fprintf(stdout, "REGISTER\n");
            break;
        case 0x0B:
            fprintf(stdout, "REGACK\n");
            break;
        case 0x0C:
            fprintf(stdout, "PUBLISH\n");
            break;
        case 0x0D:
            fprintf(stdout, "PUBACK\n");
            break;
        case 0x0E:
            fprintf(stdout, "PUBCOMP\n");
            break;
        case 0x0F:
            fprintf(stdout, "PUBREC\n");
            break;
        case 0x10:
            fprintf(stdout, "PUBREL\n");
            break;
        case 0x12:
            fprintf(stdout, "SUBSCRIBE\n");
            break;
        case 0x13:
            fprintf(stdout, "SUBACK\n");
            break;
        case 0x14:
            fprintf(stdout, "UNSUBSCRIBE\n");
            break;
        case 0x15:
            fprintf(stdout, "UNSUBACK\n");
            break;
        case 0x16:
            fprintf(stdout, "PINGREQ\n");
            break;
        case 0x17:
            fprintf(stdout, "PINGRESP\n");
            break;
        case 0x18:
            fprintf(stdout, "DISCONNECT\n");
            break;
        case 0x1A:
            fprintf(stdout, "WILLTOPICUPD\n");
            break;
        case 0x1B:
            fprintf(stdout, "WILLTOPICRESP\n");
            break;
        case 0x1C:
            fprintf(stdout, "WILLMSGUPD\n");
            break;
        case 0x1D:
            fprintf(stdout, "WILLMSGRESP\n");
            break;
        default:
            break;
    }



    return true;

}
