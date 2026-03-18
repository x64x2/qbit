#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <fcntl.h>

int listen_addr_nonblock(struct sockaddr *bindAddr)
{
	int listenSock = socket(bindAddr->sa_family, SOCK_STREAM, 0);
	if(listenSock == -1) return -1;
	if(fcntl(listenSock, F_SETFL, fcntl(listenSock, F_GETFL) | O_NONBLOCK) == -1) return -1;
	if(bind(listenSock, bindAddr, sizeof(struct sockaddr_in)) == -1) return -1;
	if(listen(listenSock, 32) == -1) return -1;
	return listenSock;
}

int listen_addr(struct sockaddr *bindAddr)
{
	int listenSock = socket(bindAddr->sa_family, SOCK_STREAM, 0);
	if(listenSock == -1) return -1;
	if(bind(listenSock, bindAddr, sizeof(struct sockaddr_in)) == -1) return -1;
	if(listen(listenSock, 32) == -1) return -1;
	return listenSock;
}
