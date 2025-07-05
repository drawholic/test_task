#include <arpa/inet.h>

// #define INET_ADDRSTRLEN 20 
#define BUFLEN 256
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

struct Config
{
	char upstream_ip[INET_ADDRSTRLEN];
	int upstream_port;
	char **blacklist;
	int blacklist_count;
	enum ResponseType{
		NXDOMAIN, REFUSED, CUSTOM_IP
	} response_type;
	char custom_ip[INET_ADDRSTRLEN];
};

void read_config(struct Config c);

