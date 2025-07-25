#ifndef SERVER_H
#define SERVER_H

#include "config.h"
#include "parser.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define MAX_BUF 1024

struct Server{
	
	int sockfd;
	
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;

	unsigned clientaddrlen;
	char* buffer;
	unsigned buffer_length;

};


void fill_server(struct Server*, struct Config*);
void server_loop(struct Server*, struct Config*);

void receive(struct Server*, struct DNSHeader*);

void send_response(struct Server*, struct DNSHeader*, struct Config*);

int forward_to_upstream(struct Server*, struct Config*);
#endif