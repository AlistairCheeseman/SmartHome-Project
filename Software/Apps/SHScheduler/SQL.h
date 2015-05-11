/* 
 * File:   SQL.h
 * Author: Ali
 *
 * Created on 09 April 2015, 10:53
 * this handles the bulk sql data function names are self explanatory.
 */

#ifndef SQL_H
#define	SQL_H

#include <sqlite3.h>
#include <stdio.h>
#include <ctime>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "automationRule.h"
class SQL {
public:
    SQL(const char *filename);
    virtual ~SQL();
     void getActiveAutomationRules(automationRule *(&AR), int &len);
     void updateLastRunTime(int ruleId);
     void deleteRule(int ruleId);
     void createSuggestedRules();
private:
    void connectDB(const char *filename);
    sqlite3 *db;
    bool isdbopen;
   char* getCurrentMoment();

};

#endif	/* SQL_H */

