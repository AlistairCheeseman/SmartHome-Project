/* 
 * File:   SQL.cpp
 * Author: alistair
 * 
 * Created on 02 February 2015, 14:43
 */

#include "SQL.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctime>

static int c_callback(void *param, int argc, char **argv, char **azColName) {
    SQL* sql = reinterpret_cast<SQL*> (param);
    return sql->callback(argc, argv, azColName);
}

SQL::SQL(const char *filename) {
    connectDB(filename);
}

SQL::~SQL() {
    if (isdbopen) {
        sqlite3_close(db);
    }
    db = NULL;
    isdbopen = NULL;
}

void SQL::connectDB(const char *filename) {
    int rc;

    rc = sqlite3_open(filename, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        isdbopen = false;
    } else {
        fprintf(stderr, "Opened database successfully\n");
        isdbopen = true;
    }
}

void SQL::logVal(char *topic, char *payload) {
    const char *statement = "INSERT INTO log (moment, topic,payload) VALUES ('%s','%s', '%s' ); ";
    char *sql = (char *) malloc((strlen(statement) + strlen(topic) + strlen(payload)) + 1 + 22);
    int rc;
    char *zErrMsg = 0;
    /* Create SQL statement */
    char *buffer = getCurrentMoment();
    sprintf(sql, statement, buffer, topic, payload);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //   fprintf(stdout, "Records stored successfully\n");
    }
    delete[] sql;
    delete zErrMsg;
}

void SQL::execute(char* statement) {
    int rc;
    char *zErrMsg = 0;
    /* Execute SQL statement */
    rc = sqlite3_exec(db, statement, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records stored successfully\n");
    }
    delete zErrMsg;
}

void SQL::writePower(float pf, float pow) {
    const char *statement = "INSERT INTO PowerLog (moment, powerReading, powerFactor) VALUES ('%s', '%f', '%f' ); ";
    char *sql = new char[300];
    int rc;
    char *zErrMsg = 0;
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char *buffer = new char[22];
    strftime(buffer, 22, "%Y-%m-%d %H:%M:%S", ltm);
    /* Create SQL statement */
    sprintf(sql, statement, buffer, pow, pf);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
         fprintf(stdout, "Records stored successfully\n");
    }
    delete sql;
    delete[] buffer;
}

void SQL::get_data() {
    char *zErrMsg = 0;
    sqlite3_exec(db, "SELECT * FROM PowerLog", c_callback, this, &zErrMsg);
}

int SQL::callback(int argc, char** argv, char** azColName) {
    /* just print returned results for time being.*/
    int i;
    for (i = 0; i < argc; i++) {
          fprintf(stdout, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
      fprintf(stdout, "\n");
    return 0;
}

char* SQL::getCurrentMoment() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char *buffer = new char[22];
    strftime(buffer, 22, "%Y-%m-%d %H:%M:%S", ltm);
    return buffer;
}




// get the topic that the SR Request must be published to for the Dev Request.
//multiple Requests can map to one ServiceRequest. (two light switches in a room control one light from different locations.)
// an incoming string should look like: dev/<mac address>/<device output id>/R
// an outgoing string should look like: dev/<mac address>/<device output id>/SR

char* SQL::getSRDEVTopic(char* mac, char* id) {
    //check the sensor table and if the sr topic is null republish to the same source with SR
    //else 
    //get the topic from the table.
    //todo: if missing record in sql table?
    const char *statement = "SELECT * FROM Sensors WHERE DevId = '%s' AND controlId = '%s'";
    char *sql = new char[300];
    char *SRTopic = new char[50];
    sprintf(sql, statement, mac, id);
    sqlite3_stmt *ppStmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_ROW) {
                if (sqlite3_column_text(ppStmt,4) == NULL)
                {
                    sprintf(SRTopic, "%s/%s", mac, id);
                }
                else
                {
                strcpy(SRTopic, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 4)));
                }
            }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
        }
    }
    char *output = new char[200];
    sprintf(output, "d/%s/P", SRTopic);
    return output;
}






// get the topic that the SR Request must be published to for the MAP Request.
// a mapping layer maps one to one to a device. the map is unique in the sensor table
// an incoming string should look like: map/<room>/<device>/<named device output>/R
// an outgoing string should look like: dev/<mac address>/<device output id>/SR

char* SQL::getSRMAPTopic(char* room, char* device, char* setting) {
    //try and find the map topic in the Sensor table.
    //get the device and publish the state request there.
    //todo:if the topic is not in the table?
     const char *statement = "SELECT * FROM Sensors WHERE MapTopic = '%s/%s/%s'";
    char *sql = new char[300];
    char *DevId = new char[50];
    char *ControlId = new char[50];
    sprintf(sql, statement, room,device, setting);
    sqlite3_stmt *ppStmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_ROW) {    
                strcpy(DevId, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 1)));
                  strcpy(ControlId, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 2)));
            }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
        }
    }
    char *output = new char[200];
    sprintf(output, "d/%s/%s/P", DevId, ControlId);
    return output;
}






//get the Mapping layer path from the mac and Id.
// an incoming string should look like: dev/<mac address>/<device output id>/S
// an outgoing string should look like: map/<room>/<device>/<named device output>/S

char* SQL::getMAPDevtopic(char* mac, char* id) {
    const char *statement = "SELECT * FROM Sensors WHERE DevId = '%s' AND controlId = '%s'";
    char *sql = new char[300];

    char *SMapTopic= new char[50];
    sprintf(sql, statement, mac, id);
    sqlite3_stmt *ppStmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_ROW) {
                strcpy(SMapTopic, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 3)));
              }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
        }
    }

    char *output = new char[200];
    sprintf(output, "map/%s/S", SMapTopic);

    return output;


    // return (char*) "/map/global/heating/hotwater/S"; 
}


//Set the current value into the db and log the change.
// an incoming string should look like: mmap/<room>/<device>/<named device output>/S
//it does not need to return anything.
void SQL::setSMAPVal(char* room, char* device, char* setting, char* MAPVal) {
//get the sensorid for the map.
    //log the new value in the sensor log table with with the FK SensorId.
    //update the sensor table the the sensors current state.
    const char *SelStatement = "SELECT Id FROM Sensors WHERE MapTopic = '%s/%s/%s'";
    const char *InsStatement = "INSERT INTO Sensor_Histories (SensorId, moment, value) VALUES ('%d', '%s', '%s' )";
    const char *UpdStatement = "UPDATE Sensors SET CurrentValue ='%s' WHERE Id = '%d'";
    
        char *sql = new char[300];
    
        int SensorId;
    sprintf(sql, SelStatement, room, device, setting);
    sqlite3_stmt *ppStmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_ROW) {
              SensorId  = (sqlite3_column_int(ppStmt, 0));
              }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
        }
    }
 sprintf(sql, InsStatement, SensorId, getCurrentMoment(), MAPVal);
 int rc;
  char *zErrMsg = 0;
     rc = sqlite3_exec(db, sql, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
       // fprintf(stdout,"Records stored successfully\n");
    }
    
     sprintf(sql, UpdStatement,MAPVal, SensorId);
     rc=0;
     zErrMsg = 0;
     rc = sqlite3_exec(db, sql, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
       // fprintf(stdout,"Records stored successfully\n");
    }
    
    
}