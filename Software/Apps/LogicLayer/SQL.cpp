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

    rc = sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        isdbopen = false;
    } else {
        fprintf(stderr, "Opened database successfully\n");
        isdbopen = true;
    }
}

void SQL::logVal(char *topic, char *payload) {
    const char *statement = "INSERT INTO 'log' (moment, topic,payload) VALUES ('%s','%s', '%s' ); ";
    char *sql = (char *) malloc((strlen(statement) + strlen(topic) + strlen(payload)) + 1 + 22);
    int rc;
    char *zErrMsg = 0;
    /* Create SQL statement */
    char *buffer = getCurrentMoment();
    sprintf(sql, statement, buffer, topic, payload);
    /* Execute SQL statement */
    do {
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s whilst logging raw value\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records stored successfully\n");
        break;
    }
    }while(1);
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
                if (sqlite3_column_text(ppStmt, 4) == NULL) {
                    sprintf(SRTopic, "%s/%s", mac, id);
                } else {
                    strcpy(SRTopic, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 4)));
                }
            }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
        }
    }
    sqlite3_finalize(ppStmt);


    char *output = new char[200]; // final output variable
    output[0] = 0x00; //ensure that it is sanitised before processing.
    char* currentTopic = strtok(SRTopic, ";"); // variable to hold each iteration of the raw publish topic.
    while (currentTopic != NULL) { // loop for each publish topic
        int i, j;
        char * formattedTopic = new char[15];
        sprintf(formattedTopic, "d/%s/P;", currentTopic); // format the string and add the ; back in.

        for (j = 0; formattedTopic[j] != NULL; j++) // get the length of the formatted string
            ;
        for (i = 0; output[i] != NULL; i++) // get the current length of the output.
            ;
        char * newout = (char*) calloc(i + j + 1, sizeof (char*)); // create a var to hold the new output string.
        memcpy(newout, output, i * sizeof (char*));
        memcpy(newout + i, formattedTopic, j * sizeof (char*));
        newout[i + j] = 0x00;
        memcpy(output, newout, (i + j + 1) * sizeof (char*));
        currentTopic = strtok(NULL, ";");
    }




    // sprintf(output, "d/%s/P", SRTopic);
    fprintf(stdout, "%s\n", output);
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
    sprintf(sql, statement, room, device, setting);
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
    sqlite3_finalize(ppStmt);
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

    char *SMapTopic = new char[50];
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
    sqlite3_finalize(ppStmt);
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
    const char *InsStatement = "INSERT INTO Sensor_Histories (SensorId, moment, value) VALUES ('%d', '%s', '%s' );UPDATE Sensors SET CurrentValue ='%s' WHERE Id = '%d';";
    char *sql = new char[300];
    int SensorId;
    sprintf(sql, SelStatement, room, device, setting);
    sqlite3_stmt *ppStmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_BUSY) {
                continue; // it's busy, attempt a step again.
            }
            if (res == SQLITE_ROW) {
                SensorId = (sqlite3_column_int(ppStmt, 0));
            }
            if (res == SQLITE_DONE) {
                break;
            }
            if (res == SQLITE_ERROR) {
                fprintf(stdout, "SQL Error : %s\n", sqlite3_errmsg(db));
                break;
            }
        }
    }
    sqlite3_finalize(ppStmt);
    sprintf(sql, InsStatement, SensorId, getCurrentMoment(), MAPVal, MAPVal, SensorId);
    int rc;
    char *zErrMsg = 0;
    rc = sqlite3_exec(db, sql, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s in SensorHistory Insert and Current Value Update\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        // fprintf(stdout,"Records stored successfully\n");
    }
}
bool SQL::checkRules(char* id)
{
    // get any active rule, that we can process that can mentions the sensor.
    const char *Statement1 = "SELECT count(*) FROM automation where Condition LIKE '%";
    const char *Statement2 = "SENSOR=%s";
    const char *Statement3 = "%' AND (stateId = '1' OR stateId = '4' OR stateId = '5') AND (typeId = '3' OR typeId = '5' OR typeId = '6' );"; // rule active and linked to sensor id.
    char *sql = new char[300];
    char *newStatement2 = new char[20];
    sprintf(newStatement2, Statement2, id);
    sprintf(sql, "%s%s%s", Statement1,newStatement2 , Statement3);
    int resCount = 0;
    sqlite3_stmt *ppStmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_BUSY) {
                continue; // it's busy, attempt a step again.
            }
            if (res == SQLITE_ROW) {
               resCount = (sqlite3_column_int(ppStmt, 0));
            }
            if (res == SQLITE_DONE) {
                break;
            }
            if (res == SQLITE_ERROR) {
                fprintf(stdout, "SQL Error : %s\n", sqlite3_errmsg(db));
                break;
            }
        }
    }
    sqlite3_finalize(ppStmt);
    if (resCount >0)
        return true;
    else
        return false;
}
int SQL::getRules(char* id, automationRule *(&AR))
{
    const char *Statement1 = "SELECT Id,Payload, Topic, Condition, TypeId  FROM automation where Condition LIKE '%";
    const char *Statement2 = "SENSOR=%s";
    const char *Statement3 = "%' AND (stateId = '1' OR stateId = '4' OR stateId = '5') AND (typeId = '3' OR typeId = '5' OR typeId = '6' );"; // rule active and linked to sensor id.
    char *sql = new char[300];
    char *newStatement2 = new char[20];
    sprintf(newStatement2, Statement2, id);
    sprintf(sql, "%s%s%s", Statement1,newStatement2 , Statement3);
    int res = 0;
    sqlite3_stmt *ppStmt;
    int currentrule = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_ROW) {
                // for each row returned.
                AR[currentrule].id = sqlite3_column_int(ppStmt, 0);
                strcpy(AR[currentrule].payload, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 1)));
                strcpy(AR[currentrule].topic, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 2)));
                char * conditionsBuffer = new char[200];
                strcpy(conditionsBuffer, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 3)));
                int typeId =  sqlite3_column_int(ppStmt, 4);
                if (typeId == 4)
                {
                    AR[currentrule].isTemporary = true;
                }
                else
                {
                    AR[currentrule].isTemporary = false;
                }
                
                char* currentCondition = strtok(conditionsBuffer, ";"); // variable to hold each iteration of the conditi
                int currentConditionCount = 0;
                while (currentCondition != NULL) { // loop for each publish topic
                    memcpy(AR[currentrule].conditions[currentConditionCount], currentCondition, strlen(currentCondition));
                    AR[currentrule].conditions[currentConditionCount][strlen(currentCondition)] = 0x00;
                    currentConditionCount++;
                    currentCondition = strtok(NULL, ";");
                }
                AR[currentrule].conditionCount = currentConditionCount;
                AR[currentrule].TypeId =  sqlite3_column_int(ppStmt, 4);
            }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
            if (res == SQLITE_BUSY) {
                fprintf(stdout, "database busy\n");
            }
            currentrule++;
        }
    }
    return currentrule;
}
char* SQL::getSensorId(char* mac, char* id) {
    const char *statement = "SELECT Id FROM Sensors WHERE DevId = '%s' AND controlId = '%s'";
    char *sql = new char[300];
    char *sensorId = new char[4];
    sensorId[0] = 0x00;
    sprintf(sql, statement, mac, id);
    sqlite3_stmt *ppStmt;
    int res = 0;
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_ROW) {
                strcpy(sensorId, reinterpret_cast<const char*> (sqlite3_column_text(ppStmt, 0)));
            }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
        }
    }
    sqlite3_finalize(ppStmt);
    return sensorId;
}