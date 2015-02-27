/* 
 * File:   UDP.h
 * Author: alistair
 *
 * a UDP socket class to simplify transmission and reception of UDP packets in linux.
 * Created on 27 February 2015, 11:24
 */

#ifndef UDP_H
#define	UDP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>

#ifndef AI_DEFAULT
#define AI_DEFAULT (AI_ADDRCONFIG|AI_V4MAPPED)
#endif

class UDP {
public:
    UDP(const char * host, const char*  port);
    int createSocket(const char * host, const char* port);
    virtual ~UDP();
    bool pendingpacket();
    void sendpacket(const void *payload);
    void *getpacket(uint8_t& read);

private:
    int sock;
    bool _pendingpacket;
};

#endif	/* UDP_H */

