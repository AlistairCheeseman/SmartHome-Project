/* 
 * File:   Timing.h
 * Author: alistair
 *
 * some simple timing features.
 * Created on 26 February 2015, 15:14
 */

#ifndef TIMING_H
#define	TIMING_H
#include <stddef.h>
#include <time.h>
#include <sys/time.h>


class Timing {
public:
    Timing();
   void __msleep(int milisec);
  void __usleep(int milisec);
    void __start_timer();
    long __millis();
    virtual ~Timing();
private:
  struct timeval start, end;
     long mtime, seconds, useconds;
};

#endif	/* TIMING_H */

