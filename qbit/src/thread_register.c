#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "thread_register.h"
#include "error.h"

pthread_t rpc_thread = 0;
pthread_t qbit_thread = 0;
