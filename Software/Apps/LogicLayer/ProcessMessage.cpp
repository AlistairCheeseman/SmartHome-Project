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
    if (!strcmp(devtype, "d")) // if it is a physical device.
    {

        char* mac;
        char* id;
        char* type;
        mac = strtok(NULL, "/"); // this is the unique ID of the device, it is also the key in the sensor db table.
        id = strtok(NULL, "/"); //  the id of the item attached to the device.
        type = strtok(NULL, "/"); // the type of request.

        fprintf(stdout, "======================================\n");
        fprintf(stdout, "Device Transmission Recieved\n");
        fprintf(stdout, "-------------------------------------\n");
        fprintf(stdout, "Originating MAC Address: %s\n", mac);
        fprintf(stdout, "Output ID: %s\n", id);
        if (!strcmp(type, "R"))
            fprintf(stdout, "Message Type: %s\n", "Request");
        else if (!strcmp(type, "P"))
            fprintf(stdout, "Message Type: %s\n", "State Change Request");
        else if (!strcmp(type, "S"))
            fprintf(stdout, "Message Type: %s\n", "State Update");

        fprintf(stdout, "Payload: %s\n", payload);
        fprintf(stdout, "-------------------------------------\n");

        if (!strcmp(type, "R")) {
            //a request has been initiated.
            //look up the database and find what device it corresponds to. then publish a State Request to that device.

            char* publishtopic = sqldb->getSRDEVTopic(mac, id);
            //publishtopic is now a ; seperated string of all the topics the payload must be delievered to.




            char* currentTopic;
            currentTopic = strtok(publishtopic, ";");
            while (currentTopic != NULL) {
                fprintf(stdout, "ACTION: Publishing State Request to DEV topic: %s \n", currentTopic);
                sender->publish(NULL, currentTopic, strlen(currentTopic), payload);
               currentTopic = strtok(NULL, ";");
            }





        } else if (!strcmp(type, "S")) {
            //the state has been updated.
            //look up the device in the database to update the database to this value.
            //(update the logical mapping (state) too?)

            char* publishtopic = sqldb->getMAPDevtopic(mac, id);
            fprintf(stdout, "ACTION: Publishing new DEV state to MAP Layer: %s \n", publishtopic);
            sender->publish(NULL, publishtopic, strlen(publishtopic), payload);
            //check if there are any pending rules
            char* sensorId = new char[5];
            strcpy(sensorId, sqldb->getSensorId(mac, id));
                fprintf(stdout, "Sensor Id: %s\n", sensorId);
            bool pendingRules = sqldb->checkRules(sensorId);
            if (pendingRules == true)
            {
                fprintf(stdout, "Found some rules to process with this request!\n");
                automationRule * ARs = new automationRule[10];
              int ruleCount =   sqldb->getRules(sensorId, ARs);
              for (int t = 0;t<ruleCount;t++)
              {
                  fprintf(stdout, "Rule Id: %d\n", ARs[t].id);
              }
            }
            
        } else if (!strcmp(type, "P")) {
            //nothing needs to be done for a state request, this is handled by the device.
            // the state request is solely on the device layer for the actual request of the state change
            fprintf(stdout, "ACTION: State Request..........\nnot doing anything with packet.\n");
        }
        fprintf(stdout, "======================================\n\n");
    } else // if it is a logical mapping.
    {
        //   /map/room/device/setting/{S SR R}

        char* room = strtok(NULL, "/");
        char* device = strtok(NULL, "/");
        char* setting = strtok(NULL, "/");
        char* type = strtok(NULL, "/");
        fprintf(stdout, "======================================\n");
        fprintf(stdout, "Mapping Transmission Recieved\n");
        fprintf(stdout, "-------------------------------------\n");
        fprintf(stdout, "Room: %s\n", room);
        fprintf(stdout, "Device: %s\n", device);
        fprintf(stdout, "Setting on Device: %s\n", setting);
        if (!strcmp(type, "R"))
            fprintf(stdout, "Message Type: %s\n", "Request");
        else if (!strcmp(type, "P"))
            fprintf(stdout, "Message Type: %s\n", "State Change Request");
        else if (!strcmp(type, "S"))
            fprintf(stdout, "Message Type: %s\n", "State Update");

        fprintf(stdout, "Payload: %s\n", payload);
        fprintf(stdout, "-------------------------------------\n");



        //todo: look up logical mapping and translate to physical device.
        if (!strcmp(type, "S")) {
            //logical device has been updated. ensure db holds up-to-date value by writing to it.
            fprintf(stdout, "ACTION: writing new State value to DB.\n");
            sqldb->setSMAPVal(room, device, setting, payload);
        }
        if (!strcmp(type, "R")) {
            char* publishtopic = sqldb->getSRMAPTopic(room, device, setting);
            fprintf(stdout, "ACTION: Publishing State Request to DEV topic: %s \n", publishtopic);
            sender->publish(NULL, publishtopic, strlen(publishtopic), payload);
            //a request has been published. look up what device it corresponds to and publish a request on the physical layer.
        }
        fprintf(stdout, "======================================\n\n");
    }
}