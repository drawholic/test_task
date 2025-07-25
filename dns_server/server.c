#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void fill_server(struct Server* s, struct Config* c)
{
    s->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (s->sockfd < 0) {
        perror("Failure creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&s->serveraddr, 0, sizeof(s->serveraddr));
    s->serveraddr.sin_family = AF_INET;
    s->serveraddr.sin_addr.s_addr = INADDR_ANY;
    s->serveraddr.sin_port = htons(54);

    if (bind(s->sockfd, (struct sockaddr*)&s->serveraddr, sizeof(s->serveraddr)) < 0) {
        perror("Failure to bind");
        close(s->sockfd);
        exit(EXIT_FAILURE);
    }

    s->buffer = malloc(MAX_BUF);
    if (!s->buffer) {
        perror("malloc failed");
        close(s->sockfd);
        exit(EXIT_FAILURE);
    }

    s->clientaddrlen = sizeof(s->clientaddr);
}

void receive(struct Server* s, struct DNSHeader* h)
{
    ssize_t n = recvfrom(s->sockfd, s->buffer, MAX_BUF, 0,
                         (struct sockaddr*)&s->clientaddr, &s->clientaddrlen);
    if (n < 0) {
        perror("Failure receiving");
        return;
    }
    s->buffer_length = (unsigned)n;

    fill_header(h, s->buffer);
}

void server_loop(struct Server* s, struct Config* c)
{
    struct DNSHeader h;
    char* domain_name = malloc(MAX_BUF);
    if (!domain_name) {
        perror("malloc failed");
        return;
    }

    while (1) {
        receive(s, &h);
        get_domain_name(s->buffer, domain_name);
        printf("Requested domain: %s\n", domain_name);

        if (is_in_blacklist(c, domain_name)) {
            printf("Domain is blacklisted, sending response\n");
            send_response(s, &h, c);
        } else {
            printf("Domain not blacklisted, forwarding to upstream DNS\n");
            if (forward_to_upstream(s, c) == 0) {
                // Successfully received upstream response, send it back to client
                ssize_t sent = sendto(s->sockfd, s->buffer, s->buffer_length, 0,
                                      (struct sockaddr*)&s->clientaddr, s->clientaddrlen);
                if (sent < 0) {
                    perror("sendto client failed");
                }
            } else {
                // Upstream failed, optionally send SERVFAIL response or drop
                fprintf(stderr, "Failed to get response from upstream\n");
            }
        }
    }

    free(domain_name);
}


void send_response(struct Server* s, struct DNSHeader* req_header, struct Config* c)
{
    unsigned char response[MAX_BUF];
    memset(response, 0, MAX_BUF);

    struct DNSHeader* res_header = (struct DNSHeader*)response;

    res_header->id = htons(req_header->id);
    res_header->flags = htons(0x8000); 
    res_header->qdcount = htons(1);
    res_header->ancount = htons(0);
    res_header->nscount = htons(0);
    res_header->arcount = htons(0);

    uint16_t flags = ntohs(res_header->flags);
    switch (c->response_type) {
        case RESPONSE_REFUSED:
            flags |= 0x0005; 
            break;
        case RESPONSE_NXDOMAIN:
            flags |= 0x0003; 
            break;
        case RESPONSE_NOERROR:
            flags |= 0x0000; 
            break;
    }
    res_header->flags = htons(flags);

    size_t offset = sizeof(struct DNSHeader);
    size_t question_len = s->buffer_length > offset ? s->buffer_length - offset : 0;
    if (question_len + offset >= MAX_BUF) {
        fprintf(stderr, "Question section too large to respond\n");
        return;
    }
    memcpy(response + offset, s->buffer + offset, question_len);
    offset += question_len;

    ssize_t sent = sendto(s->sockfd, response, offset, 0,
                          (struct sockaddr*)&s->clientaddr, s->clientaddrlen);
    if (sent < 0) {
        perror("sendto failed");
    }
}

#include <sys/time.h>

int forward_to_upstream(struct Server* s, struct Config* c)
{
    struct sockaddr_in upstream_addr;
    memset(&upstream_addr, 0, sizeof(upstream_addr));
    upstream_addr.sin_family = AF_INET;
    upstream_addr.sin_port = htons(53);
    if (inet_pton(AF_INET, c->upstream_ip, &upstream_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        return -1;
    }

    ssize_t sent = sendto(s->sockfd, s->buffer, s->buffer_length, 0,
                          (struct sockaddr*)&upstream_addr, sizeof(upstream_addr));
    if (sent < 0) {
        perror("sendto upstream failed");
        return -1;
    }

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(s->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    socklen_t addrlen = sizeof(upstream_addr);
    ssize_t n = recvfrom(s->sockfd, s->buffer, MAX_BUF, 0,
                         (struct sockaddr*)&upstream_addr, &addrlen);
    if (n < 0) {
        perror("recvfrom upstream failed");
        return -1;
    }

    s->buffer_length = (unsigned)n;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    setsockopt(s->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    return 0;
}

