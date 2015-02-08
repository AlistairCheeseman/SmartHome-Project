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


static int c_callback(void *param, int argc, char **argv, char **azColName) 
{
    SQL* sql = reinterpret_cast<SQL*>(param);
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
    const char *statement = "INSERT INTO log (topic,payload) VALUES ('%s', '%s' ); ";
    char *sql = (char *) malloc((strlen(statement) + strlen(topic) + strlen(payload)) + 1);
    int rc;
    char *zErrMsg = 0;
    /* Create SQL statement */
    sprintf(sql, statement, topic, payload);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records stored successfully\n");
    }
    delete[] sql;
    delete zErrMsg;
}

void SQL::execute(char* statement)
{
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
void SQL::writePower(float pf, float pow)
{
    const char *statement = "INSERT INTO PowerLog (moment, powerReading, powerFactor) VALUES ('%s', '%f', '%f' ); ";
    char *sql = new char[300];
    int rc;
    char *zErrMsg = 0;   
    time_t now = time(0);
    tm *ltm = localtime(&now); 
    char *buffer = new char[22];
    strftime(buffer, 22, "%Y-%m-%d %H:%M:%S", ltm);
    /* Create SQL statement */
    sprintf(sql, statement,buffer, pow, pf);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, c_callback, this, &zErrMsg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        printf( "Records stored successfully\n");
    }
    delete sql;
    delete[] buffer;
}
void SQL::get_data()
{
    char *zErrMsg = 0;  
    sqlite3_exec(db, "SELECT * FROM PowerLog", c_callback, this, &zErrMsg);
}
int SQL::callback(int argc, char** argv, char** azColName)
{
    /* just print returned results for time being.*/
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
