
#ifndef LOG_H
#define	LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <unistd.h>

typedef int log_level_t; 
#define LOG_ERROR 0x01
#define LOG_INFO 0x02
#define err_log stderr
#define info_log stdout
#define DEBUG


class log {
    public:
        static void _log(log_level_t level, const char *format, ...)
            __attribute__((format (printf, 2, 3)));
        #define log(level, fmt, ...) _log(level, fmt"\n", ##__VA_ARGS__)
private:



};
#endif	/* LOG_H */