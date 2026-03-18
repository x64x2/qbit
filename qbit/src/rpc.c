#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <poll.h>
#include <sys/wait.h>

#include "thread_register.h"
#include "error.h"
#include "tdconf.h"
#include "rpc.h"
#include "util.h"

int respond_frame(RpcFrame_t* frame, RpcSession_t* sess);
int respond_msg_connect(RpcSession_t* session, RpcFrame_t* frame);
int respond_msg_stop(RpcSession_t* session, RpcFrame_t* frame);
inline int respond_msg_noop(RpcSession_t* session, RpcFrame_t* frame);
inline int respond_msg_when_unauthed(RpcSession_t* session,
	RpcFrame_t* frame);
int respond_msg_auth(RpcSession_t* session, RpcFrame_t* frame);
inline int respond_msg_when_bad_req(RpcSession_t* session,
	RpcFrame_t* frame);

void* rpc_main(void* listen_sock_ptr)
{
	int listen_sock = *(int*)listen_sock_ptr;
	RpcSessList_t* sess_list = RpcSessList_create();
	while(1) {
		if(sess_list == NULL) break;
		socklen_t addr_size = sizeof(struct sockaddr);
		struct sockaddr* address = malloc(addr_size);
		RpcSession_t* new_sess;

		if(address == NULL) break;
		int new = accept(listen_sock, address, &addr_size);

		if(new == -1) {
			LOG_ERROR("Connection error");
			pthread_exit(&sess_list);
		}

		new_sess = RpcSession_create(address, new);
		if(new_sess == NULL) break;

		RpcSessList_add(sess_list, new_sess);
		if(RpcSession_run(new_sess, sess_list) == 0) break;
	}
	LOG_ERROR("Out of memory.");
	RpcSessList_destroy(sess_list);
	pthread_exit(sess_list);
}

int respond_frame(RpcFrame_t* frame, RpcSession_t* sess)
{
	switch(frame->command) {
		case RPC_NOOP:
			return respond_msg_noop(sess, frame);
		case RPC_CONNECT:
			return respond_msg_connect(sess, frame);
		case RPC_AUTH:
			return respond_msg_auth(sess, frame);
		case RPC_STOP:
			return respond_msg_stop(sess, frame);
		default:
			return respond_msg_when_bad_req(sess, frame);
	}
}

int respond_msg_stop(RpcSession_t* session, RpcFrame_t* frame)
{
	if(session->user == NULL) goto cleanup;
	shutdown(session->sock, SHUT_RD);
	char* msg = str_buf("Bye!", 4);
	RpcFrame_send(RpcFrame_create(rpc_OK, 4, frame->reqNum, msg),
		session->sock);
	rpcFrame_destroy(frame);
	return 0;
}

int respond_msg_auth(RpcSession_t* session, RpcFrame_t* frame)
{
	if(session->user == NULL) return 0;
	int retval = 0;
	char* pw = malloc(frame->bodySize + 1);
	if(pw == NULL) {
		retval = -1;
		return 0;
	}
	pw[frame->bodySize] = '\0';
	memcpy(pw, frame->body, frame->bodySize);
	session->pass = &pw;
		RpcFrame_send(RpcFrame_create(rpc_OK, 0, frame->reqNum, NULL),
			session->sock);
	session->pass = NULL;
	memset(frame->body, 0, frame->bodySize);
	free(pw);
	RpcFrame_destroy(frame);
	return retval;
}

int respond_msg_connect(RpcSession_t* session, RpcFrame_t* frame)
{
	char* msg;
	int retval = 0;
	session->lastAction = time(NULL);
	if(session->user != NULL) {
		msg = str_buf("Already Connected", 17);
		RpcFrame_send(
			RpcFrame_create(rpc_FAIL, 17, frame->reqNum, msg),
			session->sock);
		return 0;
	}
	session->user = malloc(frame->bodySize + 1);
	if(session->user == NULL) {
		retval = -1;
		return 0;
	}
}

static inline int respond_msg_noop(RpcSession_t* session, RpcFrame_t* frame)
{
	if(session->user == NULL) return 0;
	session->lastAction = time(NULL);
	rpcFrame_send(rpcFrame_create(rpc_NOOP, 0, frame->reqNum, NULL),
		session->sock);
	rpcFrame_destroy(frame);
	return 0;
}

static inline int respond_msg_when_bad_req(RpcSession_t* session, RpcFrame_t* frame)
{
	if(session->user == NULL) return 0;
	char* msg = str_buf("Bad Request", 11);
	RpcFrame_send(RpcFrame_create(RPC_FAIL, 11, frame->reqNum, msg),
		session->sock);
	RpcFrame_destroy(frame);
	return 0;
}

static inline int respond_msg_when_unauthed(RpcSession_t* session, RpcFrame_t* frame)
{
	if(session->user == NULL) return 0;
	session->lastAction = time(NULL);
	RpcFrame_send(
		RpcFrame_create(rpc_FAIL, 25, frame->reqNum,
			str_buf("Session not authenticated", 25)),
		session->sock);
	RpcFrame_destroy(frame);
	return 0;
}
