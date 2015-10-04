/* 
 * File:   MQTT.cpp
 * Author: Ali
 * 
 * Created on 09 April 2015, 16:09
 */

#include "MQTT.h"

MQTT::MQTT(const char *id, const char *host, int port): mosquittopp(id) {
    int keepalive = 60;
	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive);  
}

MQTT::~MQTT() {
}


void MQTT::on_connect(int rc)
{
	fprintf(stdout,"Connected with code %d.\n", rc);
}
void MQTT::on_message(const struct mosquitto_message *message)
{
   fprintf(stdout,"got Message.\n"); // don't need to handle any messages really as this program will only generate mesasages
}

void MQTT::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	fprintf(stdout,"Subscription succeeded.\n"); // don't need to handle any messages really as this program will only generate mesasages
}