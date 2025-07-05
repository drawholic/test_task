#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 1024

int setup_server_socket(int port){};


int send_dns_query(const char* upstream_ip, uint8_t *packet, int len){};


int receive_dns_response(int sockfd, uint8_t *buffer, int buflen){};

void fill_server(struct Server s){

	s.sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if(s.sockfd < 0)
	{
		printf("Failure creating server\n");
		exit(-1);
	};

	s.optval = 1;

	setsockopt(s.sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&s.optval, sizeof(int));


	bzero((char*)&s.serveraddr, sizeof(s.serveraddr));
	s.serveraddr.sin_family = AF_INET;
	s.serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	s.serveraddr.sin_port = htons((unsigned short)s.portno);

	if(bind(s.sockfd, (struct sockaddr*)&s.serveraddr, sizeof(s.serveraddr)) < 0){
		printf("Failure binding\n");
		close(s.sockfd);
		exit(-1);
	};
	s.clientlen = sizeof(s.clientaddr);
};

void server_loop()
{
	char* buffer = malloc(BUFSIZE);

	while(1)
	{
		n = recvfrom(s.sockfd, buffer, BUFSIZE, 0, (struct sockaddr*)&s.clientaddr, &s.clientlen);
		if(n < 0)
		{
			printf("Failure receiving\n");
			continue;
		};

		s.hostp = gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr, sizeof(s.clientaddr.sin_addr.s_addr), AF_INET);
		if(s.hostp == NULL)
		{
			printf("Failure gethostbyaddr\n");
			continue;
		};
		s.hostaddrp = inet_ntoa(s.clientaddr.sin_addr);
		if(s.hostaddrp == NULL)
		{
			printf("Failure on inet_ntoa\n");
			continue;
		}

		
	};
};