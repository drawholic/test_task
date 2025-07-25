#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>  

#include "parser.h"

void fill_header(struct DNSHeader* h, char* buffer)
{
    if (!h || !buffer) return;
	
	memcpy(h, buffer, 12);

	h->id      = ntohs(h->id);
    h->flags   = ntohs(h->flags);
    h->qdcount = ntohs(h->qdcount);
    h->ancount = ntohs(h->ancount);
    h->nscount = ntohs(h->nscount);
    h->arcount = ntohs(h->arcount);
};

void get_domain_name(char* buffer, char* domain_name)
{
    if (!buffer || !domain_name) return;

    char* ptr = buffer + 12;
    int pos = 0;

    while (*ptr != 0) {
        uint8_t len = (uint8_t)*ptr;
        ptr++;

        for (int i = 0; i < len; ++i) {
            domain_name[pos++] = *ptr++;
        }

        domain_name[pos++] = '.';
    }

    if (pos > 0) pos--; 
    domain_name[pos] = 0;
}
