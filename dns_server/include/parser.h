
#include <stdint.h>
#include <stdlib.h>

#define QR 0b1000000000000000 
#define OPCODE 0b0111100000000000 
#define AA 0b0000010000000000 
#define TC 0b0000001000000000 
#define RD 0b0000000100000000 
#define RA 0b0000000010000000 
#define Z 0b0000000001110000 
#define RCODE 0b0000000000001111 

#pragma pack(push, 1)

struct DNSHeader{
	uint16_t id;
	uint16_t flags;
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;
};


struct DNSQuestionTail{
	uint16_t qtype;
	uint16_t qclass;
};

struct DNSResourceRecordTail {
    uint16_t type;
    uint16_t dnsclass;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t* rdata;  
} ;

#pragma pack(pop)

void parse_answer(char*, struct DNSResourceRecordTail*);
void parse_question(char*, struct DNSQuestionTail*);
void parse_header(char*, struct DNSHeader*);
void form_answer(char*, struct DNSHeader*);

#include <stdint.h>
#include <stdio.h>

const uint8_t* get_domain_name(const uint8_t* buffer, const uint8_t* ptr, char* out, size_t out_size);