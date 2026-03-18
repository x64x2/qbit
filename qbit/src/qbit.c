#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "thread_register.h"
#include "error.h"

/* This function runs in its own thread and is responsible for continuing all
 * qbit transactions and seeding.
 */
void* qbit_main(void* param)
{
	return 0;
}

