#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

struct DNSHeader
{
	uint16_t id;

	uint16_t flags;
	
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;

};

struct DNSQuestion
{
	char *qname;
	uint16_t qtype;
	uint16_t qclass;

};

struct DNSResourceRecord
{
	char *name;
	uint16_t type;
	uint16_t classname;
	uint32_t ttl;
	uint16_t rdlength;
	uint8_t rdata;

};


void fill_header(struct DNSHeader*, char*);

void get_domain_name(char*, char*);

#endif