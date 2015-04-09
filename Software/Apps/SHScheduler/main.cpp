/* 
 * File:   main.cpp
 * Author: Ali
 *
 * Created on 28 March 2015, 13:03
 */

#include <cstdlib>
#include <cstdio>
#include <signal.h>
#include <mosquittopp.h>
#include <vector>
#include <string.h>
#include <ctype.h>

#include "MQTT.h"
#include "SQL.h"
#include "automationRule.h"
using namespace std;


char* getDayofWeek();
char* getTimeofDay();

bool keep_running = true;

static void termination_handler(int signum) {
    switch (signum) {
        case SIGHUP: fprintf(stderr, "Got hangup signal.");
            break;
        case SIGTERM: fprintf(stderr, "Got termination signal.");
            break;
        case SIGINT: fprintf(stderr, "Got interupt signal.");
            break;
    }

    // Signal the main thead to stop
    keep_running = false;
}

/*
 * 
 */
int main(int argc, char** argv) {
    // this program will be called through cron.
    //mode 1 this will check the database for all rules and find any that apply to the current condition and post that current condition.
    //mode 2 It will analyse the logs and look for any patterns, if found it will load them into the suggested rules db for the user to make an active rule.

    // Setup signal handlers
    signal(SIGTERM, termination_handler);
    signal(SIGINT, termination_handler);
    signal(SIGHUP, termination_handler);

    mosqpp::lib_init();
    SQL* sql = new SQL("/var/db/smarthome");
   MQTT* mqtt = new MQTT("Scheduler", "192.168.3.50", 1883);

    while (keep_running) {
        mqtt->loop();
        automationRule* aRS;
        int ruleCount;
        aRS = new automationRule[100]; // shouldn't have any more than this.
        sql->getActiveAutomationRules(aRS, ruleCount);
        fprintf(stdout, "found %d Rules\n", ruleCount);
        if (ruleCount < 1)
            break; // leave program, no need to hang around.
        for (int t = 0; t < ruleCount; t++) {

            fprintf(stdout, "================\n");
            fprintf(stdout, "Automation Rule!\n");
            fprintf(stdout, "Topic:   %s\n", aRS[t].topic);
            fprintf(stdout, "Payload: %s\n", aRS[t].payload);
            fprintf(stdout, "Key\tValue\n");
            bool runRule = true;
            bool pendingPeriod = false;
            //sort the rule conditions and if it is good, run it.
            for (unsigned int i = 0; i < aRS[t].conditionCount; ++i) {
                char* conditionKey = strtok(aRS[t].conditions[i], "=");
                char* conditionValue = strtok(NULL, "=");
                fprintf(stdout, "%s\t%s\n", conditionKey, conditionValue);
                if (strcmp(conditionKey, "TIME") == 0) {
                    if (!(strcmp(conditionValue,getTimeofDay()) == 0)) // force the time to be valid for testing
                    {
                        //it's not now. ( 5 min window)
                        fprintf(stdout, "Excluded Rule, wrong time\n");
                        runRule = false;
                    }
                } else if (strcmp(conditionKey, "PERIOD") == 0) {
                    if (strcmp(conditionValue, "DAILY") == 0) {
                        // run every day, no need to check the day.
                        pendingPeriod = false;
                    } else {
                        pendingPeriod = true; // if we need a day before we can work out if the rule is valid.
                    }
                } else if ((strcmp(conditionKey, "DAY") == 0) && pendingPeriod == true) {\
                    if (!(strcmp(conditionValue, getDayofWeek()) == 0)) {
                        //it's not today.
                        fprintf(stdout, "Excluded Rule, wrong day of week\n");
                        runRule = false;
                    }

                }
            }
            if (runRule == true) {
                // send the payload to the topic.
                fprintf(stdout, "RuleValid!\n");
                            mqtt->loop();
                mqtt->publish(NULL, aRS[t].topic, strlen(aRS[t].topic),  aRS[t].payload);
                 mqtt->loop();
                 mqtt->disconnect();
                  
                fprintf(stdout,"Publishing %s to %s\n", aRS[t].payload,aRS[t].topic);
            } else {
                fprintf(stdout, "Skipping Rule\n");
            }
            fprintf(stdout, "================\n");
        }
        keep_running = false;
    }
    delete sql;
    mosqpp::lib_cleanup();
    return 0;
}

char* getDayofWeek() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char *buffer = new char[22];
    strftime(buffer, 22, "%A", ltm);
    int i=0;
    char c;
  while (buffer[i])
  {
    c=buffer[i];
   buffer[i] = (toupper(c));
    i++;
  }
    return buffer;
}
char* getTimeofDay() {
     time_t now = time(0);
    tm *ltm = localtime(&now);

    ltm->tm_min =  ltm->tm_min - ( ltm->tm_min % 5);
    char *buffer = new char[22];
    strftime(buffer, 22, "%H:%M", ltm);
    fprintf(stdout, "Time is: %s\n", buffer);
    return buffer;
}