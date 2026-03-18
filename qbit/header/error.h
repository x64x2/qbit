#include <stdio.h>
#include <semaphore.h>

#ifndef ERROR_H_
#define ERROR_H_

void write_error(char* msg, char* func);
void log_error(char* msg, char* func);
void dbg_write_error(char* msg, char* func, char* file, int line);
void dbg_log_error(char* msg, char* func, char* file, int line);

#define WRITE_ERROR(x) dbg_write_error(x,(char*)__FUNCTION__,\
	(char*)__FILE__,__LINE__)
#define LOG_ERROR(x) dbg_log_error(x,(char*)__FUNCTION__,\
	(char*)__FILE__,__LINE__)

#define WRITE_ERROR(x) write_error(x,(char*)__FUNCTION__)
#define LOG_ERROR(x) log_error(x,(char*)__FUNCTION__)


void global_SIGTERM();
extern FILE* stderrlog;
extern sem_t sem_stderrlog;
