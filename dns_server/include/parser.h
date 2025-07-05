
#include <stdint.h>
#include <stdlib.h>

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
 
struct DNSRecourseRecordTail{
	uint16_t type;
	uint16_t dnsclass;
	uint32_t ttl;
	uint16_t rdlength;
};

#pragma pack(pop)


