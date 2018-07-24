/*********************
Gliss CPU simulator validator
log.c : logging subsystem
**********************/

#define LOG_C
#include "all_inc.h"

int open_log_file(char * fpath)
	{
	logfile = fopen(fpath, "w");
	if ( logfile == NULL ) 
		{
		perror("Opening log file");
		exit(1);
		}
	else return 0;	
	}
	
void close_log_file()
	{
	fclose(logfile);
	}
	
void log_msg(char * fmt, ...)
	{
	if ( ! do_logging ) return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(logfile, fmt, ap);
	va_end(ap);
	}
#undef LOG_C
