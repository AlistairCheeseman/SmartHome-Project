#include "MQTTSNCheck.h"
#include <stdint.h>
#include <stdio.h>
//this is a very simple packet checker. it simply checks the actual length and the expected length.
bool MQTTSNCheck::verifyPacket(void* payload, uint8_t length) {
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
       fprintf(stderr, "ERROR: PACKET INVALID: read %d bytes but header states length is %d\n", (int)length, (int) buf[0]);
        return false;
    }
    for (int q = 0; q < length; q++)
       fprintf(stdout, "0x%x ", buf[q]);
    return true;

}
