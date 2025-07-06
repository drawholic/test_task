#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "parser.h"
#include "Config.h"
#include "blacklist.h"

#define BUFSIZE 1232 

void fill_server(struct Server* s){

	s->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	s->portno = 54;
	if(s->sockfd < 0)
	{
		printf("Failure creating server\n");
		exit(-1);
	};

	s->optval = 1;

	setsockopt(s->sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&s->optval, sizeof(int));


	bzero((char*)&s->serveraddr, sizeof(s->serveraddr));
	s->serveraddr.sin_family = AF_INET;
	s->serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	s->serveraddr.sin_port = htons((unsigned short)s->portno);

	if(bind(s->sockfd, (struct sockaddr*)&s->serveraddr, sizeof(s->serveraddr)) < 0){
		perror(0);
		printf("Failure binding\n");
		close(s->sockfd);
		exit(-1);
	};
	s->clientlen = sizeof(s->clientaddr);		

	inet_ntop(AF_INET, &(s->serveraddr.sin_addr), s->ip, sizeof(s->ip));
};

void server_loop(struct Server* s, struct Config* c)
{
	char* buffer = malloc(BUFSIZE);
	struct DNSHeader* h = malloc(sizeof (struct DNSHeader));

	while(1)
	{
		s->n = recvfrom(s->sockfd, buffer, BUFSIZE, 0, (struct sockaddr*)&s->clientaddr, &s->clientlen);
		if(s->n < 0)
		{ 
			continue;
		};

		parse_header(buffer, h);
		char domain[256];
		const uint8_t* question_ptr = (const uint8_t*)(buffer + 12);
		const uint8_t* after_name = get_domain_name((const uint8_t*)buffer, question_ptr, domain, sizeof(domain));

		printf("Requested domain: %s\n", domain);

		// its an answer
		if(h->flags & QR)
		{
			printf("its an answer\n");
			struct DNSResourceRecordTail answer;
			parse_answer(buffer, &answer);

			forward_answer(s, buffer, s->n);
			// form a response to send to client

		}else{
			printf("its a question\n");
			if(is_blacklist(domain, c)){
				printf("in blacklist\n");
				form_answer( buffer, h);

				forward_answer(s, buffer, s->n);
			}else{
				printf("not in blacklist\n");
				forward_request(s, buffer);

				receive_answer(s, buffer);

				forward_answer(s, buffer, s->n);
			}
			// its a question
			struct DNSQuestionTail question;
			parse_question(buffer, &question);
		
			const uint8_t* ptr = buffer + sizeof(struct DNSHeader);

			// check the list

			// if not present, forward to upstream dns

			// receive response

			// forward response

		}; 	
	};
}; 


void forward_answer(struct Server* s, char* query, int query_len) {
  uint8_t response[512]; // Standard DNS message max size
    memset(response, 0, sizeof(response));

    struct DNSHeader* req_header = (struct DNSHeader*) query;
    struct DNSHeader* res_header = (struct DNSHeader*) response;

    // Copy the ID from request
    res_header->id = req_header->id;

    // Set QR=1 (response), AA=1 (authoritative), RA=1 (recursion available), RCODE=3 (NXDOMAIN)
    res_header->flags = htons(QR | AA | RA | 3);

    // Copy QDCOUNT from request (usually 1)
    res_header->qdcount = req_header->qdcount;

    // No answers, authority or additional records
    res_header->ancount = 0;
    res_header->nscount = 0;
    res_header->arcount = 0;

    // Copy question section from query after the header (12 bytes)
    memcpy(response + sizeof(struct DNSHeader), query + sizeof(struct DNSHeader), query_len - sizeof(struct DNSHeader));

    int total_len = query_len; // Same length as request

    int sent = sendto(s->sockfd, response, total_len, 0, (struct sockaddr*)&s->clientaddr, s->clientlen);
    if (sent < 0) {
        perror("sendto failed");
    }
}

void forward_request(struct Server *s, char* buffer, struct Config* c){
	struct sockaddr_in upstreamaddr;

	upstreamaddr.sin_family = AF_INET;
	upstreamaddr.sin_addr.s_addr = htonl(c->upstream_ip);
	upstreamaddr.sin_port = htons(53);

	int n = sendto(s->sockfd, buffer, s->n, (struct sockaddr*)&upstreamaddr, sizeof(upstreamaddr));

	if(n<0)
	{
		perror("failure sending to upstream\n");
		return;
	};

};

void receive_answer(struct Server* s, char* buffer, struct Config* c)
{


	int n = recvfrom(s->sockfd, buffer, s->n, );


};