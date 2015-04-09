/* 
 * File:   UDP.cpp
 * Author: alistair
 * // http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 * Created on 27 February 2015, 11:24
 */

#include "UDP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>



UDP::UDP(const char * host, const char*  port) {
    sock = -1;
    _pendingpacket = false;
    
   sock =  this->createSocket(host, port);
}

UDP::~UDP() {
}

int UDP::createSocket(const char * host, const char* port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct timeval tv;
    int fd, ret;

    // Set options for the resolver
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_DEFAULT; /* Default flags */
    hints.ai_protocol = 0; /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    // Lookup address
    ret = getaddrinfo(host, port, &hints, &result);
    if (ret != 0) {
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket and)
       try the next address. */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1)
            continue;

        // Connect socket to the remote host
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) != -1)
            break; // Success

        close(fd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not connect to remote host.\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    // FIXME: set the Don't Fragment flag

    // Setup timeout on the socket
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (tv)) < 0) {
        perror("Error setting timeout on socket");
    }

    return fd;
}
void UDP::sendpacket(const void* payload)
{
    size_t sent, len = ((uint8_t*)payload)[0];
    sent = send(this->sock, payload, len, 0);
    if (sent != len ) {
      fprintf(stderr,"Warning: only sent %d or %d bytes\n", (int)sent, (int) len);

    }
}
void *UDP::getpacket(uint8_t& read)
{
    static uint8_t buffer[33];
    uint8_t bytes_read;
    
    bytes_read = recv(sock, buffer, 32, 0);
    if (bytes_read <0)
    {
           fprintf(stderr,"recieve failed");
    }
    if (bytes_read > 33)
    {
        bytes_read = 0;
    }
    read = bytes_read;
    return buffer;
}
bool UDP::pendingpacket()
{
   // http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#blocking
      struct timeval tv;
    fd_set readsockfds;
//wait 0.5 seconds
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    FD_ZERO(&readsockfds);
    FD_SET(sock, &readsockfds);

    // don't care about writefds and exceptfds:
    select(sock+1, &readsockfds, NULL, NULL, &tv);

    if (FD_ISSET(sock, &readsockfds))
        return true;
    else
    {
      //  printf("Timed out.\n");
    return false;
    }
    return false;
    //HIGH IMPORTANCE. FIND A WAY TO TELL IF PENDING DATA WITHOUT SLOWING APP DOWN TOO MUCH.
    
}