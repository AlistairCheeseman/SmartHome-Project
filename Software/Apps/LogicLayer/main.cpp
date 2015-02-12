/* 
 * File:   main.cpp
 * Author: alistair
 *
 * Created on 22 January 2015, 15:11
 */

#include <cstdlib>
#include <mosquittopp.h>

#include "SQL.h"
#include "mqtt.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    int count;
    if (argc > 1)
    {
        for (count = 1; count < argc; count++)
        {
            printf("argv[%d] = %s\n", count, argv[count]);
        }
    }
    else
    {
        printf("Correct Usage:\n");
        printf("logiclayer <dblocation> <mqtthost> <mqttport>\n");
        printf("where:");
        printf("dblocation is the location of the sqlite db\nmqtthost is the ip address\nmqttport is the mqtt server port\n\n");
        return 0;
    }
    
    
    
    
    
    mosqpp::lib_init();
    class mqtt * mqttstor;
    int rc;
    //for time being mosquitto server and database location are hard coded. todo:get vars from arguments when program called.
    mqttstor = new mqtt("logiclayer", "192.168.3.50", 1883, "/var/db/smarthome");
    while (1) {
       rc = mqttstor->loop();
        if (rc) {
            mqttstor->reconnect();
        } 
    }
    mosqpp::lib_cleanup();
    delete mqttstor;
    return 0;
}

