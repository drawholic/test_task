#include "parser.h"
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void parse_header(char* buffer, struct DNSHeader* header)
{
    uint16_t* ptr = (uint16_t*) buffer;

    header->id      = ntohs(ptr[0]);
    header->flags   = ntohs(ptr[1]);
    header->qdcount = ntohs(ptr[2]);
    header->ancount = ntohs(ptr[3]);
    header->nscount = ntohs(ptr[4]);
    header->arcount = ntohs(ptr[5]);
}

void parse_answer(char* buffer, struct DNSResourceRecordTail* answer)
{
    char* p = buffer;

    if ((uint8_t)p[0] == 0xC0) {
        p += 2;
    } else {
        while (*p != 0) {
            p += (*p) + 1;
        }
        p += 1;
    }

    answer->type = ntohs(*(uint16_t*)p); p += 2;
    answer->dnsclass = ntohs(*(uint16_t*)p); p += 2;
    answer->ttl = ntohl(*(uint32_t*)p); p += 4;
    answer->rdlength = ntohs(*(uint16_t*)p); p += 2;

    answer->rdata = malloc(answer->rdlength);
    memcpy(answer->rdata, p, answer->rdlength);
}

void parse_question(char* buffer, struct DNSQuestionTail* question)
{
    char* p = buffer;
    while (*p != 0) {
        p += (*p) + 1;
    }
    p += 1;

    uint16_t* ptr = (uint16_t*) p;
    question->qtype = ntohs(ptr[0]);
    question->qclass = ntohs(ptr[1]);
}

const char* parse_name(const uint8_t* buffer, const uint8_t* ptr, char* out) {
    int i = 0, jumped = 0, offset;
    const uint8_t* start = ptr;
    while (*ptr) {
        if ((*ptr & 0xC0) == 0xC0) {
            offset = ((*ptr & 0x3F) << 8) | *(ptr + 1);
            ptr = buffer + offset;
            if (!jumped) start += 2;
            jumped = 1;
            continue;
        }
        uint8_t len = *ptr++;
        for (int j = 0; j < len; j++) {
            out[i++] = *ptr++;
        }
        out[i++] = '.';
    }
    out[i - 1] = '\0';
    return jumped ? (const char*)start : (const char*)(ptr + 1);
}

// void print_question(const uint8_t* buffer, const uint8_t* ptr) {
//     char name[256];
//     const uint8_t* after_name = (const uint8_t*)parse_name(buffer, ptr, name);

//     uint16_t qtype = ntohs(*(uint16_t*)after_name);
//     uint16_t qclass = ntohs(*(uint16_t*)(after_name + 2));

//     printf(";; QUESTION SECTION:\n");
//     printf("%s\t\tIN\t%s\n", name, (qtype == 1) ? "A" : "OTHER");
// }

// void print_answer(const uint8_t* buffer, const uint8_t* ptr) {
//     char name[256];
//     const uint8_t* after_name = (const uint8_t*)parse_name(buffer, ptr, name);

//     uint16_t type = ntohs(*(uint16_t*)after_name);
//     uint16_t class = ntohs(*(uint16_t*)(after_name + 2));
//     uint32_t ttl = ntohl(*(uint32_t*)(after_name + 4));
//     uint16_t rdlength = ntohs(*(uint16_t*)(after_name + 8));

//     const uint8_t* rdata = after_name + 10;

//     printf(";; ANSWER SECTION:\n");
//     printf("%s\t%d\tIN\t", name, ttl);
//     if (type == 1 && rdlength == 4) {
//         printf("A\t%d.%d.%d.%d\n", rdata[0], rdata[1], rdata[2], rdata[3]);
//     } else {
//         printf("TYPE%d\t<rdata>\n", type);
//     }
// }


// Parses the domain name from DNS message buffer starting at 'ptr'.
// 'buffer' is the start of the DNS message for compression handling.
// 'out' is where the resulting domain name string will be stored.
// Returns a pointer to the byte just after the domain name in the buffer.
const uint8_t* get_domain_name(const uint8_t* buffer, const uint8_t* ptr, char* out, size_t out_size) {
    size_t pos = 0;
    int jumped = 0;        // flag if we followed a pointer
    const uint8_t* original_ptr = ptr; // remember where we started
    const uint8_t* jump_ptr = NULL;

    while (*ptr != 0) {
        // Check for compression pointer: two highest bits set (11)
        if ((*ptr & 0xC0) == 0xC0) {
            if (!jumped) {
                jump_ptr = ptr + 2; // next byte after pointer
            }
            uint16_t offset = ((*ptr & 0x3F) << 8) | *(ptr + 1);
            ptr = buffer + offset;
            jumped = 1;
            continue;
        } else {
            uint8_t len = *ptr;
            ptr++;
            if (pos + len + 1 >= out_size) {
                // Output buffer too small
                break;
            }
            for (int i = 0; i < len; i++) {
                out[pos++] = *ptr++;
            }
            out[pos++] = '.';
        }
    }
    if (pos > 0) out[pos - 1] = '\0'; // Replace last dot with null terminator
    else out[pos] = '\0';

    if (jumped) {
        return jump_ptr;
    } else {
        return ptr + 1; // +1 to skip the zero length byte
    }
}


void form_answer(char* b, struct DNSHeader* h)
{
	
};