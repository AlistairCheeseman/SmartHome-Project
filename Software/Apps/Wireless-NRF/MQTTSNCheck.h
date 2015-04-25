/* 
 * File:   MQTTSNCheck.h
 * Author: alistair
 *
 * simple static functions that help out with error checking and statistics for the MQTTSN Protocol
 * Created on 27 February 2015, 18:21
 */

#ifndef MQTTSN_H
#define	MQTTSN_H
#include <stdint.h>
    namespace MQTTSNCheck
    {
        bool verifyPacket(void *(&payload), uint8_t &len);
    }




#endif	/* MQTTSN_H */

