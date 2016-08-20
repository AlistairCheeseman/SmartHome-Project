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
    //connect to the database
    sqldb = new SQL(dbloc);
}

ProcessMessage::~ProcessMessage() {
    //close the database connection
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

        log::log(LOG_INFO,"======================================");
        log::log(LOG_INFO,"Device Transmission Recieved");
        log::log(LOG_INFO,"-------------------------------------");
        log::log(LOG_INFO,"Originating MAC Address: %s", mac);
        log::log(LOG_INFO,"Output ID: %s", id);
        if (!strcmp(type, "R"))
            log::log(LOG_INFO,"Message Type: %s", "Request");
        else if (!strcmp(type, "P"))
            log::log(LOG_INFO,"Message Type: %s", "State Change Request");
        else if (!strcmp(type, "S"))
            log::log(LOG_INFO,"Message Type: %s", "State Update");

        log::log(LOG_INFO,"Payload: %s", payload);
        log::log(LOG_INFO,"-------------------------------------");

        if (!strcmp(type, "R")) {
            //a request has been initiated.
            //look up the database and find what device it corresponds to. then publish a State Request to that device.

            char* publishtopic = sqldb->getSRDEVTopic(mac, id);
            //publishtopic is now a ; seperated string of all the topics the payload must be delievered to.




            char* currentTopic;
            currentTopic = strtok(publishtopic, ";");
            while (currentTopic != NULL) {
                log::log(LOG_INFO,"ACTION: Publishing State Request to DEV topic: %s", currentTopic);
                sender->publish(NULL, currentTopic, strlen(currentTopic), payload);
               currentTopic = strtok(NULL, ";");
            }





        } else if (!strcmp(type, "S")) {
            //the state has been updated.
            //look up the device in the database to update the database to this value.
            //(update the logical mapping (state) too?)

            char* publishtopic = sqldb->getMAPDevtopic(mac, id);
            log::log(LOG_INFO,"ACTION: Publishing new DEV state to MAP Layer: %s", publishtopic);
            sender->publish(NULL, publishtopic, strlen(publishtopic), payload);
            //check if there are any pending rules
            char* sensorId = new char[5];
            strcpy(sensorId, sqldb->getSensorId(mac, id));
                log::log(LOG_INFO,"Sensor Id: %s", sensorId);
            bool pendingRules = sqldb->checkRules(sensorId);
            if (pendingRules == true)
            {
                log::log(LOG_INFO,"Found some rules to process with this request!");
                automationRule * ARs = new automationRule[10];
              int ruleCount =   sqldb->getRules(sensorId, ARs);
              for (int t = 0;t<ruleCount;t++)
              {
                  log::log(LOG_INFO,"Rule Id: %d", ARs[t].id);
                  log::log(LOG_INFO,"Rule TypeId: %d", ARs[t].TypeId);
                  if (ARs[t].TypeId == 5)
                  {
                      log::log(LOG_INFO,"Mirror Rule!");
                      sender->publish(NULL,ARs[t].topic, strlen(ARs[t].topic),payload); 
                      
                  }
              }
            }
            
        } else if (!strcmp(type, "P")) {
            //nothing needs to be done for a state request, this is handled by the device.
            // the state request is solely on the device layer for the actual request of the state change
            log::log(LOG_INFO,"ACTION: State Request..........\nnot doing anything with packet.");
        }
        log::log(LOG_INFO,"======================================");
    } else // if it is a logical mapping.
    {
        //   /map/room/device/setting/{S SR R}

        char* room = strtok(NULL, "/");
        char* device = strtok(NULL, "/");
        char* setting = strtok(NULL, "/");
        char* type = strtok(NULL, "/");
        log::log(LOG_INFO,"======================================");
        log::log(LOG_INFO,"Mapping Transmission Recieved");
        log::log(LOG_INFO,"-------------------------------------");
        log::log(LOG_INFO,"Room: %s", room);
        log::log(LOG_INFO,"Device: %s", device);
        log::log(LOG_INFO,"Setting on Device: %s", setting);
        if (!strcmp(type, "R"))
            log::log(LOG_INFO,"Message Type: %s", "Request");
        else if (!strcmp(type, "P"))
            log::log(LOG_INFO,"Message Type: %s", "State Change Request");
        else if (!strcmp(type, "S"))
            log::log(LOG_INFO,"Message Type: %s", "State Update");

        log::log(LOG_INFO,"Payload: %s", payload);
        log::log(LOG_INFO,"-------------------------------------");



        //todo: look up logical mapping and translate to physical device.
        if (!strcmp(type, "S")) {
            //logical device has been updated. ensure db holds up-to-date value by writing to it.
            log::log(LOG_INFO,"ACTION: writing new State value to DB.");
            sqldb->setSMAPVal(room, device, setting, payload);
        }
        if (!strcmp(type, "R")) {
            char* publishtopic = sqldb->getSRMAPTopic(room, device, setting);
            log::log(LOG_INFO,"ACTION: Publishing State Request to DEV topic: %s", publishtopic);
            sender->publish(NULL, publishtopic, strlen(publishtopic), payload);
            //a request has been published. look up what device it corresponds to and publish a request on the physical layer.
        }
        log::log(LOG_INFO,"======================================");
    }
}