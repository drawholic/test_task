#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void trim(char* s) {
    while (*s && isspace((unsigned char)*s)) s++;

    char* start = s;
    memmove(s - (s - start), start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }
}

void read_config(struct Config* c) {
    FILE* f = fopen(c->pathname, "r");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    char* buffer = NULL;
    size_t buf_size = 0;
    ssize_t line_size;

    c->blacklist_len = 0;

    if ((line_size = getline(&buffer, &buf_size, f)) == -1) {
        fprintf(stderr, "Error: missing upstream IP\n");
        exit(1);
    }
    if (line_size > 0 && buffer[line_size - 1] == '\n') buffer[line_size - 1] = '\0';
    trim(buffer);
    strncpy(c->upstream_ip, buffer, sizeof(c->upstream_ip) - 1);
    c->upstream_ip[sizeof(c->upstream_ip) - 1] = '\0';

    if ((line_size = getline(&buffer, &buf_size, f)) == -1) {
        fprintf(stderr, "Error: missing response type\n");
        exit(1);
    }
    if (line_size > 0 && buffer[line_size - 1] == '\n') buffer[line_size - 1] = '\0';
    trim(buffer);

    if (strcasecmp(buffer, "REFUSED") == 0) {
        c->response_type = RESPONSE_REFUSED;
    } else if (strcasecmp(buffer, "NXDOMAIN") == 0) {
        c->response_type = RESPONSE_NXDOMAIN;
    } else if (strcasecmp(buffer, "NOERROR") == 0) {
        c->response_type = RESPONSE_NOERROR;
    } else {
        fprintf(stderr, "Error: unknown response type '%s'\n", buffer);
        exit(1);
    }

    while ((line_size = getline(&buffer, &buf_size, f)) != -1) {
        if (line_size > 0 && buffer[line_size - 1] == '\n') buffer[line_size - 1] = '\0';
        trim(buffer);

        if (strlen(buffer) == 0) continue;

        if (c->blacklist_len < MAX_BLACKLIST) {
            c->blacklist[c->blacklist_len++] = strdup(buffer);
        }
    }

    free(buffer);
    fclose(f);
}


int is_in_blacklist(struct Config* c, char *domain_name)
{
    if (!c || !domain_name) return 0;

    for (int i = 0; i < c->blacklist_len; ++i) {
        if (strcmp(c->blacklist[i], domain_name) == 0) {
            return 1;
        }
    }

    return 0;
}
