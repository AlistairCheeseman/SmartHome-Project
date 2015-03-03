/* 
 * File:   mqtt.cpp
 * Author: Alistair
 * 
 * Created on 07 February 2015, 11:24
 */
#include <cstdio>
#include <cstring>
#include <mosquittopp.h>

#include "ProcessMessage.h"

#include "mqtt.h"




mqtt::mqtt(const char *id, const char *host, int port,const char *dbloc) : mosquittopp(id)
{
  	int keepalive = 60;
	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive);  
        messageProcessor = new ProcessMessage(dbloc);
        
}

mqtt::~mqtt() {
}


void mqtt::on_connect(int rc)
{
	fprintf(stdout,"Connected with code %d.\n", rc);
	if(rc == 0){
		/* Only attempt to subscribe on a successful connect. */
            //subscribe to every topic as all requests will pass through this program.
		subscribe(NULL, "#",1); //set QOS to deliver at least once.
	}
}

void mqtt::on_message(const struct mosquitto_message *message)
{
    if (message->payloadlen > 0)
    {
    char buffer[101];
    memset(buffer, 0,101*sizeof(char));
    memcpy(buffer, message->payload, 100*sizeof(char));
    messageProcessor->messageReceived(message->topic, buffer, message->payloadlen, this);
    }
}

void mqtt::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	fprintf(stdout,"Subscription succeeded.\n");
}