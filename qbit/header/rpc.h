#ifndef RPC_H_
#define RPC_H_

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>

#include "util.h"

#define RPC_NOOP	0
#define RPC_FAIL	1
#define RPC_OK		2
#define RPC_CONNECT	3
#define RPC_STOP	4
#define RPC_AUTH	5
#define RPC_AUTH_ERR	6

#define RPC_HEADSIZE	9

typedef struct {
	u_int8 command;
	u_int32_t reqNum;
	u_int32_t bodySize;

	u_int8_t headBuff[rpc_HEADSIZE];
	u_int8_t hasHeadBuff;

	u_int8_t bodyIsFP; //Body is a file pointer
	void* body;
} RpcFrame_t;

int rpcFrame_grab(RpcFrame_t* frame, int sock);
int rpcFrame_send(RpcFrame_t* frame, int sock);
void rpcFrame_destroy(rpcFrame_t* frame);
RpcFrame_t* RpcFrame_create(u_int8_t command, u_int32_t bodySize, u_int32_t reqNum, void* body);

typedef struct RpcSession{
	char* user;
	pthread_t thread;
	pam_handle_t* pamh;
	struct pam_conv	pconv;
	char** pass;
	u_int8_t authed;
	time_t lastAction;
	int	sock;

	struct sockaddr* addr;
	struct RpcSession* next;
	struct RpcSession* prev;
} RpcSession_t;

RpcSession_t* RpcSession_create(struct sockaddr* addr, int sock);
RpcSession_t* RpcSession_destroy(RpcSession_t* session);

typedef struct {
	RpcSession_t* node;
	u_int32_t count;
	pthread_mutex_t* lock;
} RpcSessList_t;

void RpcSessList_add(RpcSessList_t* list, RpcSession_t* session);
RpcSession_t* RpcSessList_remove(RpcSessList_t* list, rpcSession_t* sess);
void RpcSessList_destroy(RpcSessList_t* list);
RpcSessList_t* RpcSessList_create();

int RpcSession_run(RpcSession_t* sess, RpcSessList_t* sess_list);

#define DEAD_SESSION(X) ((time(NULL) - X->lastAction) > CONF_rpc_TIMEOUT || \
						X->sessState == SESS_DEAD || \
						(X->sessState == SESS_FLUSH && !X->outFrames.len))

extern size_t rpcSessionCount;
void* rpc_main(void* listen_sock_ptr);
