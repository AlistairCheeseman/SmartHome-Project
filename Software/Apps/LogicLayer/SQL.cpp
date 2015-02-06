/* 
 * File:   SQL.cpp
 * Author: alistair
 * 
 * Created on 02 February 2015, 14:43
 */

#include "SQL.h"
#include <sqlite3.h>


SQL::SQL(const char *filename) {
    connectDB(filename);
}

SQL::~SQL() {
    if (isdbopen) {
        sqlite3_close(db);
    }
    db =NULL;
    isdbopen=NULL;
}

void SQL::connectDB(const char *filename) {
    int rc;

    rc = sqlite3_open(filename, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        //exit(0);
        isdbopen=false;
    } else {
        fprintf(stderr, "Opened database successfully\n");
        isdbopen = true;
    }


}

