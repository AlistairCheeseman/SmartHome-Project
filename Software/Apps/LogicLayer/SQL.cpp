/* 
 * File:   SQL.cpp
 * Author: alistair
 * 
 * Created on 02 February 2015, 14:43
 */

#include "SQL.h"
#include <sqlite3.h>


SQL::SQL() {
    connectDB();
}

SQL::SQL(const SQL& orig) {
}

SQL::~SQL() {
    if (isdbopen) {
        sqlite3_close(db);
    }

}

void SQL::connectDB() {
    int rc;

    rc = sqlite3_open("/var/db/smarthome", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        //exit(0);
        isdbopen=false;
    } else {
        fprintf(stderr, "Opened database successfully\n");
        isdbopen = true;
    }
    if (isdbopen) {
        sqlite3_close(db);
    }
}

