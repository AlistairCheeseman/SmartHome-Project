#include "log.h"


void log::_log(log_level_t level, const char *format, ...)
{
    va_list arg;
    
    FILE * fp = fopen ("/var/log/logiclayer.log", "a"); // append to end or create.
        
    FILE *console = (level == LOG_ERROR) ? err_log : info_log;
    
    /* Write the error message */
    va_start(arg, format);
    vfprintf(fp, format, arg); // write to log file
    vfprintf(console, format, arg); // write to console (stdout or stderr)
    va_end(arg);

#ifdef DEBUG
    fflush(console);
    fsync(fileno(console));
    fflush(fp);
    fsync(fileno(fp));
#endif
}



