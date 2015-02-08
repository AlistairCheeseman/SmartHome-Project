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
    mosqpp::lib_init();
    class mqtt * mqttstor;
    int rc;
    //for time being mosquitto server and database location are hard coded. todo:get vars from arguments when program called.
    mqttstor = new mqtt("logiclayer", "192.168.3.50", 1883);
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

