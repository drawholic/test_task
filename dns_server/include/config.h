#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAX_BLACKLIST 100

enum ResponseType {
    RESPONSE_REFUSED,
    RESPONSE_NXDOMAIN,
    RESPONSE_NOERROR,
    RESPONSE_CUSTOM_IP  // optional if you ever go back to IP
};

struct Config
{
	char pathname[256];

	char* blacklist[MAX_BLACKLIST];
	unsigned blacklist_len;
	char upstream_ip[64];
	enum ResponseType response_type;

};

void read_config(struct Config*);

int is_in_blacklist(struct Config*, char*);
#endif