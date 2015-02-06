/* 
 * File:   SQL.h
 * Author: alistair
 *
 * Created on 02 February 2015, 14:43
 * file to hold SQL Class to handle the DAL Layer of the application
 */

#ifndef SQL_H
#define	SQL_H

#include <sqlite3.h>
#include <stdio.h>

class SQL {
public:
    SQL(const char *filename);
    virtual ~SQL();
private:
    void connectDB(const char *filename);
    sqlite3 *db;
    bool isdbopen;
};

#endif	/* SQL_H */

