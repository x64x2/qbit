#ifndef COMMS_H_
#define COMMS_H_

int listen_addr_nonblock(struct sockaddr *bindAddr);
int listen_addr(struct sockaddr *bindAddr);