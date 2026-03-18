#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

#include "thread_register.h"
#include "error.h"

FILE* stderrlog;
sem_t sem_stderrlog;

void write_error(char* msg, char* func)
{
	log_error(msg, func);
	fprintf(stderr, "%s: %s (%s)\n", func, msg, strerror(errno));
}

void dbg_write_error(char* msg, char* func, char* file, int line)
{
	dbg_log_error(msg, func, file, line);
	fprintf(stderr, "In %s(%d): %s: %s (%s)\n", file, line, func, msg,
		strerror(errno));
}

void log_error(char* msg, char* func)
{
	sem_wait(&sem_stderrlog);
	fprintf(stderrlog, "%s: %s (%s)\n", func, msg, strerror(errno));
	sem_post(&sem_stderrlog);
}

void dbg_log_error(char* msg, char* func, char* file, int line)
{
	sem_wait(&sem_stderrlog);
	fprintf(stderrlog, "In %s(%d): %s: %s (%s)\n", file, line, func, msg,
		strerror(errno));
	sem_post(&sem_stderrlog);
}	

void global_SIGTERM()
{
	exit(0);
}

