/* 
 * File:   mqtt.h
 * Author: Alistair
 *
 * Created on 07 February 2015, 11:24
 */

#ifndef MQTT_H
#define	MQTT_H

#include <mosquittopp.h>
// this handles the mqtt processing, used to send and recieve mqtt messages with the use of libmosquittopp included in mosquitto.
class mqtt : public mosqpp::mosquittopp {
public:
    mqtt(const char *id, const char *host, int port,const char *dbloc);
    virtual ~mqtt();
// on connect ack
    void on_connect(int rc);
    //on message revieved
    void on_message(const struct mosquitto_message *message);
    //on sucessful subscription.
    void on_subscribe(int mid, int qos_count, const int *granted_qos);


private:
  class  ProcessMessage *messageProcessor;
    
};

#endif	/* MQTT_H */

