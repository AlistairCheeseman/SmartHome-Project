/* 
 * File:   MQTT.h
 * Author: Ali
 *
 * Created on 09 April 2015, 16:09
 */

#ifndef MQTT_H
#define	MQTT_H
#include <mosquittopp.h>
#include <cstdio>
class MQTT : public mosqpp::mosquittopp {
public:
    MQTT(const char *id, const char *host, int port);
    virtual ~MQTT();
        void on_connect(int rc);
            void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);
private:

};

#endif	/* MQTT_H */

