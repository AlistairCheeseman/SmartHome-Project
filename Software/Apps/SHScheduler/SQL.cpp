/* 
 * File:   SQL.cpp
 * Author: Ali
 * 
 * Created on 09 April 2015, 10:53
 */

#include "SQL.h"

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

char* SQL::getCurrentMoment() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char *buffer = new char[22];
    strftime(buffer, 22, "%Y-%m-%d %H:%M:%S", ltm);
    return buffer;
}

void SQL::getActiveAutomationRules(automationRule *(&AR), int &len) {
    int res = 0;
    sqlite3_stmt *ppStmt;

    int ruleCount = 0;
    //get all start/stop actions that are active or temporary active 
    const char *statementCount = "SELECT count(*) FROM automation where ((stateId = 1) OR (stateId = 4) OR (stateId = 5)) AND (TypeId = 1);"; // get number of rules.
    if (sqlite3_prepare_v2(db, statementCount, strlen(statementCount), &ppStmt, 0) == SQLITE_OK) {
        while (1) {
            res = sqlite3_step(ppStmt);
            if (res == SQLITE_ROW) {
                // single result returned.
                ruleCount = (sqlite3_column_int(ppStmt, 0));
            }
            if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                break;
            }
        }
    }
    len = ruleCount;

    if (ruleCount == 0) {

        return;
    }

    int currentrule = 0;

    const char *statement = "SELECT Id, Payload, Topic, Condition , stateId FROM automation where ((stateId = 1) OR (stateId = 4) OR (stateId = 5)) AND (TypeId = 1);"; // get all active or temporary rules.
    if (sqlite3_prepare_v2(db, statement, strlen(statement), &ppStmt, 0) == SQLITE_OK) {
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
}

void SQL::updateLastRunTime(int ruleId) {
    const char *statement = "UPDATE 'automation' SET lastExec = '%s' WHERE Id = '%d'; ";
    char *sql = (char *) malloc((strlen(statement) + 1 + 30));
    int rc;
    char *zErrMsg = 0;
    /* Create SQL statement */
    char *buffer = getCurrentMoment();
    sprintf(sql, statement, buffer, ruleId);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, this, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //   fprintf(stdout, "Records stored successfully\n");
    }
    delete[] sql;
    delete zErrMsg;
}

void SQL::deleteRule(int ruleId) {
   const char *statement = "DELETE FROM automation where Id = %d;";
    char *sql = (char *) malloc(strlen(statement) + 1);
    int rc;
    char *zErrMsg = 0;
    /* Create SQL statement */
    sprintf(sql, statement,ruleId);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, this, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        //   fprintf(stdout, "Records stored successfully\n");
    }
    delete[] sql;
    delete zErrMsg;
}

