/* 
 * File:   main.cpp
 * Author: alistair
 *
 * Created on 22 January 2015, 15:11
 */

#include <cstdlib>
#include <mosquittopp.h>
#include <string.h>

#include "SQL.h"
#include "mqtt.h"


using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    int count;
    char *database = new char[50];
    char *mqttserver = new char[50];
    int port = 1883;
/*    if (argc > 1)
    {
        for (count = 1; count < argc; count++)
        {
            printf("argv[%d] = %s\n", count, argv[count]);
        }
    }*/
     if (argc == 4)
    {
         strcpy(database, argv[1]);
        printf("Database: %s\n", database);
        strcpy(mqttserver, argv[2]);
                 printf("Mqtt Server: %s\n", mqttserver);
                 port = atoi(argv[3]);
                 printf("Port: %d\n", port);
    }  
     else if (argc == 3)
    {
         strcpy(database, argv[1]);
        printf("Database: %s\n", database);
        strcpy(mqttserver, argv[2]);
                 printf("Mqtt Server: %s\n", mqttserver);      
                 printf("Port: %d\n", port);
    }    
    else
    {
        printf("Correct Usage:\n");
        printf("logiclayer <dblocation> <mqtthost> [<mqttport>]\n");
        printf("where:\n");
        printf("dblocation is the location of the sqlite db\nmqtthost is the ip address\nmqttport is the mqtt server port\n\n");
        printf("The values are normally:\n");
        printf("Database: /var/db/smarthome\n");
        printf("MQTT Server: the local IP Address\n");
        printf("MQTT Port: 1883\n");
        return 0;
    }
    
   
    
    
    
    mosqpp::lib_init();
    class mqtt * mqttstor;
    int rc;
    //for time being mosquitto server and database location are hard coded. todo:get vars from arguments when program called.
    mqttstor = new mqtt("logiclayer", mqttserver, port, database);
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

