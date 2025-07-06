#include <stdint.h>
#include <arpa/inet.h>
#include "Config.h"
struct Server
{
	int sockfd;
	int portno;
	int clientlen;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int n;
	int optval;
	struct hostent *hostp;
	char ip[INET_ADDRSTRLEN];

};

void server_loop(struct Server*, struct Config*);


void fill_server(struct Server*);
 
void forward_answer(struct Server*, char*, int);

void forward_request(struct Server*, char*, struct Config*);
void receive_answer(struct Server*, char*, struct Config*);