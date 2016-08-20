/* 
 * File:   main.cpp
 * Author: alistair
 *
 * Created on 22 January 2015, 15:11
 */

#include <cstdlib>
#include <signal.h>
#include <mosquittopp.h>
#include <string.h>

#include "SQL.h"
#include "mqtt.h"
#include "log.h"

using namespace std;

// if the program revieces a SIGHUP signal process it and shut down the program, this allows for a more graceful shutdown.
bool keep_running = true;
static void termination_handler (int signum)
{
    switch(signum) {
        case SIGHUP:  log::log(LOG_ERROR, "Got hangup signal."); break;
        case SIGTERM: log::log(LOG_ERROR, "Got termination signal."); break;
        case SIGINT:  log::log(LOG_ERROR, "Got interupt signal."); break;
    }

    // Signal the main thead to stop
    keep_running = false;
}








/*
 * 
 */
int main(int argc, char** argv) {
    int count;
    char *database = new char[50];
    char *mqttserver = new char[50];
    int port = 1883;
    
    // Setup signal handlers
    signal(SIGTERM, termination_handler);
    signal(SIGINT, termination_handler);
    signal(SIGHUP, termination_handler);

    
    // check if we have the configuration arguments necessary to start the program.
     if (argc == 4)
    {
         strcpy(database, argv[1]);
        log::log(LOG_INFO,"Database: %s", database);
        strcpy(mqttserver, argv[2]);
                 log::log(LOG_INFO,"Mqtt Server: %s", mqttserver);
                 port = atoi(argv[3]);
                 log::log(LOG_INFO,"Port: %d", port);
    }  
     else if (argc == 3)
    {
         strcpy(database, argv[1]);
        log::log(LOG_INFO,"Database: %s", database);
        strcpy(mqttserver, argv[2]);
                 log::log(LOG_INFO,"Mqtt Server: %s", mqttserver);      
                 log::log(LOG_INFO,"Port: %d", port);
    }    
    else
    {
        log::log(LOG_INFO,"Correct Usage:");
        log::log(LOG_INFO,"logiclayer <dblocation> <mqtthost> [<mqttport>]");
        log::log(LOG_INFO,"where:");
        log::log(LOG_INFO,"dblocation is the location of the sqlite db\nmqtthost is the ip address\nmqttport is the mqtt server port\n");
        log::log(LOG_INFO,"The values are normally:");
        log::log(LOG_INFO,"Database: /var/db/smarthome");
        log::log(LOG_INFO,"MQTT Server: the local IP Address");
        log::log(LOG_INFO,"MQTT Port: 1883");
        return 0;
    }
    
   
    
    
    // initialise the MQTT Library
    mosqpp::lib_init();
    class mqtt * mqttstor;
    int rc;
    // create the MQTT class to handle everything.
    mqttstor = new mqtt("logiclayer", mqttserver, port, database);
    while (keep_running) {
        fflush(stdout);
       rc = mqttstor->loop();
        if (rc) {
            mqttstor->reconnect();
        } 
    }
    //tidy up for a graceful shutdown.
    mosqpp::lib_cleanup();
    return 0;
}

