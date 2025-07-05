#include <stdint.h>
#include <arpa/inet.h>

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
	char* hostaddrp;

};

int setup_server_socket(int port);
int send_dns_query(const char* upstream_ip, uint8_t *packet, int len);
int receive_dns_response(int sockfd, uint8_t *buffer, int buflen);

void server_loop();


void fill_server(struct Server);
 