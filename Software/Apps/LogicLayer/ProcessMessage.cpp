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

ProcessMessage::ProcessMessage(const char *dbloc) {
    sqldb = new SQL(dbloc);
}

ProcessMessage::~ProcessMessage() {
    delete sqldb;
}

void ProcessMessage::messageReceived(char *topic, char *payload, int payloadlen, class mqtt *sender) {
    //log all values to log table so we have a log of all messages.
    sqldb->logVal(topic, payload);

    char longtopic[100];
    strcpy(longtopic, topic);
    char* devtype;
    devtype = strtok(longtopic, "/"); // this indicates if the topic is addressing a device or a logical mapping
    if (!strcmp(devtype, "dev")) // if it is a physical device.
    {

        char* mac;
        char* id;
        char* type;
        mac = strtok(NULL, "/"); // this is the unique ID of the device, it is also the key in the sensor db table.
        id = strtok(NULL, "/"); //  the id of the item attached to the device.
        type = strtok(NULL, "/"); // the type of request.

        printf("======================================\n");
        printf("Device Transmission Recieved\n");
        printf("-------------------------------------\n");
        printf("Originating MAC Address: %s\n", mac);
        printf("Output ID: %s\n", id);
        if (!strcmp(type, "R"))
            printf("Message Type: %s\n", "Request");
        else if (!strcmp(type, "SR"))
            printf("Message Type: %s\n", "State Change Request");
        else if (!strcmp(type, "S"))
            printf("Message Type: %s\n", "State Update");

        printf("Payload: %s\n", payload);
        printf("-------------------------------------\n");

        if (!strcmp(type, "R")) {
            //a request has been initiated.
            //look up the database and find what device it corresponds to. then publish a State Request to that device.

            char* publishtopic = sqldb->getSRDEVTopic(longtopic);
            printf("ACTION: Publishing State Request to DEV topic: %s \n", publishtopic);
            sender->publish(NULL, publishtopic, strlen(publishtopic), payload);
        } else if (!strcmp(type, "S")) {
            //the state has been updated.
            //look up the device in the database to update the database to this value.
            //(update the logical mapping (state) too?)

            char* publishtopic = sqldb->getMAPDevtopic(mac, id);
            printf("ACTION: Publishing new DEV state to MAP Layer: %s \n", publishtopic);
            sender->publish(NULL, publishtopic, strlen(publishtopic), payload);
        } else if (!strcmp(type, "SR")) {
            //nothing needs to be done for a state request, this is handled by the device.
            // the state request is solely on the device layer for the actual request of the state change
            printf("ACTION: State Request..........\nnot doing anything with packet.\n");
        }
        printf("======================================\n\n");
    } else // if it is a logical mapping.
    {
        //   /map/room/device/setting/{S SR R}

        char* room = strtok(NULL, "/");
        char* device = strtok(NULL, "/");
        char* setting = strtok(NULL, "/");
        char* type = strtok(NULL, "/");
        printf("======================================\n");
        printf("Mapping Transmission Recieved\n");
        printf("-------------------------------------\n");
        printf("Room: %s\n", room);
        printf("Device: %s\n", device);
        printf("Setting on Device: %s\n", setting);
        if (!strcmp(type, "R"))
            printf("Message Type: %s\n", "Request");
        else if (!strcmp(type, "SR"))
            printf("Message Type: %s\n", "State Change Request");
        else if (!strcmp(type, "S"))
            printf("Message Type: %s\n", "State Update");

        printf("Payload: %s\n", payload);
        printf("-------------------------------------\n");



        //todo: look up logical mapping and translate to physical device.
        if (!strcmp(type, "S")) {
            //logical device has been updated. ensure db holds up-to-date value by writing to it.
            printf("ACTION: writing new State value to DB.\n");
            sqldb->setSMAPVal(longtopic, payload);
        }
        if (!strcmp(type, "R")) {
            char* publishtopic = sqldb->getSRMAPTopic(longtopic);
            printf("ACTION: Publishing State Request to DEV topic: %s \n", publishtopic);
            sender->publish(NULL, publishtopic, strlen(publishtopic), payload);
            //a request has been published. look up what device it corresponds to and publish a request on the physical layer.
        }
        printf("======================================\n\n");
    }
}