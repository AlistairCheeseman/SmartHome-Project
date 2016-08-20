/* 
 * File:   Timing.cpp
 * Author: alistair
 * 
 * Created on 26 February 2015, 15:14
 */

#include "Timing.h"
#include <time.h>
#include <sys/time.h>

Timing::Timing() {
    this->__start_timer();
    this->mtime = 0;
    this->seconds = 0;
    this->useconds = 0;
}

Timing::~Timing() {
}

void Timing::__msleep(int milisec)
{
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = milisec * 1000000L;
	nanosleep(&req, (struct timespec *)NULL);	
}

void Timing::__usleep(int milisec)
{
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = milisec * 1000L;
	nanosleep(&req, (struct timespec *)NULL);	
}

/**
 * This function is added in order to simulate arduino millis() function
 */
void Timing::__start_timer()
{
	gettimeofday(&start, NULL);
}

long Timing::__millis()
{
	gettimeofday(&end, NULL);
    this->seconds  = this->end.tv_sec  - this->start.tv_sec;
    this->useconds = this->end.tv_usec - this->start.tv_usec;

    this->mtime = ((seconds) * 1000 + this->useconds/1000.0) + 0.5;	
	return this->mtime;
}

