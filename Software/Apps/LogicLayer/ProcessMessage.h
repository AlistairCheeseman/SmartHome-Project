/* 
 * File:   ProcessMessage.h
 * Author: Alistair
 *
 * Created on 08 February 2015, 10:35
 */

#ifndef PROCESSMESSAGE_H
#define	PROCESSMESSAGE_H

class ProcessMessage {
public:
    // this processes any messages, this performs the actual logic layer operation.
    ProcessMessage(const char *dbloc);
    virtual ~ProcessMessage();
    void messageReceived(char *topic, char *payload, int payloadlen,  class mqtt *sender);
private:
    class SQL *sqldb;
};

#endif	/* PROCESSMESSAGE_H */

