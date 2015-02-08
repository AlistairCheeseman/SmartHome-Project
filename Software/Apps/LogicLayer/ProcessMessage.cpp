/* 
 * File:   ProcessMessage.cpp
 * Author: Alistair
 * 
 * Created on 08 February 2015, 10:35
 */

#include "ProcessMessage.h"
#include "SQL.h"
#include "mqtt.h"
#include <cstring>
ProcessMessage::ProcessMessage(char *dbloc) {
   sqldb = new SQL(dbloc);
}

ProcessMessage::~ProcessMessage() {
    delete sqldb;
}

 void ProcessMessage::messageReceived(char *topic, char *payload, int payloadlen, class mqtt *sender)
 {
     //log all values to log table so we have a log of all messages.
     sqldb->logVal(topic,payload);
     /* below is a sample logic to convert celsius to farenheit this will work for message transport testing. */
     double temp_celsius, temp_farenheit;

	if(!strcmp(topic, "temperature/celsius"))
        {
		temp_celsius = atof(payload);
		temp_farenheit = temp_celsius*9.0/5.0 + 32.0;
		snprintf(payload, 50, "%f", temp_farenheit);
		sender->publish(NULL, "temperature/farenheit", strlen(payload), payload);
	}
     
     
 }