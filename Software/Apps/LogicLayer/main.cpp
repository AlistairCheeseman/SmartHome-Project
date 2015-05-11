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


using namespace std;

// if the program revieces a SIGHUP signal process it and shut down the program, this allows for a more graceful shutdown.
bool keep_running = true;
static void termination_handler (int signum)
{
    switch(signum) {
        case SIGHUP:  fprintf(stderr, "Got hangup signal."); break;
        case SIGTERM: fprintf(stderr, "Got termination signal."); break;
        case SIGINT:  fprintf(stderr, "Got interupt signal."); break;
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
        fprintf(stdout,"Database: %s\n", database);
        strcpy(mqttserver, argv[2]);
                 fprintf(stdout,"Mqtt Server: %s\n", mqttserver);
                 port = atoi(argv[3]);
                 fprintf(stdout,"Port: %d\n", port);
    }  
     else if (argc == 3)
    {
         strcpy(database, argv[1]);
        fprintf(stdout,"Database: %s\n", database);
        strcpy(mqttserver, argv[2]);
                 fprintf(stdout,"Mqtt Server: %s\n", mqttserver);      
                 fprintf(stdout,"Port: %d\n", port);
    }    
    else
    {
        fprintf(stdout,"Correct Usage:\n");
        fprintf(stdout,"logiclayer <dblocation> <mqtthost> [<mqttport>]\n");
        fprintf(stdout,"where:\n");
        fprintf(stdout,"dblocation is the location of the sqlite db\nmqtthost is the ip address\nmqttport is the mqtt server port\n\n");
        fprintf(stdout,"The values are normally:\n");
        fprintf(stdout,"Database: /var/db/smarthome\n");
        fprintf(stdout,"MQTT Server: the local IP Address\n");
        fprintf(stdout,"MQTT Port: 1883\n");
        return 0;
    }
    
   
    
    
    // initialise the MQTT Library
    mosqpp::lib_init();
    class mqtt * mqttstor;
    int rc;
    // create the MQTT class to handle everything.
    mqttstor = new mqtt("logiclayer", mqttserver, port, database);
    while (keep_running) {
       rc = mqttstor->loop();
        if (rc) {
            mqttstor->reconnect();
        } 
    }
    //tidy up for a graceful shutdown.
    mosqpp::lib_cleanup();
    return 0;
}

