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
#include <string.h>

using namespace std;




bool keep_running = true;
static void termination_handler (int signum)
{
    switch(signum) {
        case SIGHUP:  fprintf(stderr, "Got hangup signal."); break;
        case SIGTERM: fprintf(stderr, "Got termination signal."); break;
        case SIGINT:  fprintf(stderr, "Got interupt signal."); break;
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
    
     while (keep_running) {
    
    }
    return 0;
}

