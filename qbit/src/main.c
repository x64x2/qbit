/* main.c - main is responsible for spawning two threads, one for qbits, one for
 * rpc sessions. It also prepares the environment by parsing command line args
 * and config files, and initiating the listen connections.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <pthread.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#include "rpc.h"
#include "qbit.h"
#include "error.h"
#include "comms.h"
#include "thread_register.h"

#include "util.h"

int main(int argc, char** argv)
{
	//!TODO:Check for root, surrender where necessary
	//!Check for lockfile
	//!Read configs
	stderrlog = fopen("/usr/x64x2/vimrc/qbitd/qbit/Debug/dbglog", "a");
	if(stderrlog == NULL) {
		fprintf(stderr, "Failed to open logfile. Aborting...\n");
		return 1;
	}
	sem_init(&sem_stderrlog, 0, 1);
	pthread_attr_t* ptattr = malloc(sizeof(pthread_attr_t));
	if(ptattr == NULL) {
		WRITE_ERROR("Out of memory");
		return 1;
	}
	pthread_attr_init(ptattr);
	pthread_attr_setdetachstate(ptattr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setstacksize(ptattr, 512);
	
	struct sockaddr *bindAddr = CONF_rpc_ADDR();
	
	int rpc_sock = listen_addr(bindAddr);
	if(rpc_sock == -1) {
		WRITE_ERROR("Failed to listen to socket");
		return 1;
	}
	free(bindAddr);

	if(daemon(0,0) == -1) {
		//We are not in the 8th dimension, we are over Texas
		WRITE_ERROR("Failed to detatch");
		return 1;
	}

	if((errno = pthread_create(&rpc_thread, ptattr, rpc_main, &rpc_sock)) != 0) {
		LOG_ERROR("Failed to initialize thread");
		return 1;
	}

	if((errno = pthread_create(&qbit_thread, ptattr, qbit_main, NULL)) != 0) {
		LOG_ERROR("Failed to initialize thread");
		return 1;
	}

	pthread_join(qbit_thread, NULL);
	pthread_join(rpc_thread, NULL);
	free(ptattr);
}
