/*
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ipleech.h"

static char is_number(const char c) {
    if (c >= '0' && c <= '9') {
        return c;
    }

    return 0;
}

static bool is_validstr(const char c) {
    if (!is_number(c)) {
        if (c != ':' && c != '.') {
            return false;
        }
    }

    return true;
}

char *next_host(FILE *in) {
#define HOST_SIZE 32
    static char host[HOST_SIZE] = { 0 };
    int c = 0;

    while ((c = getc(in)) != EOF) {
        if (!is_number(c)) {
            continue;
        }

        int octets = 0;
        int semi = 0;
        int idx = 0;

        do {
            if (is_validstr(c) == false) {
                break;
            }

            switch (c) {
                case '.':
                    octets++;
                    break;
                case ':':
                    semi++;
                    break;
                default:
                    break;
            }

            host[idx++] = c;
        } while ((c = getc(in)) != EOF && idx < HOST_SIZE);

        if (octets == 3 && semi == 1) {
            host[idx] = '\0';
            return host;
        }
    }

    return NULL;
}

bool hoststr_split(struct sockaddr_in * const dst, const char * host) {
    char port_str[6] = { 0 };
    char ip_str[16] = { 0 };
    int idx = 0;
    unsigned int port = 0;
    const char *hp = host;

    while (*hp && *hp != ':' && idx < 16 ) {
        ip_str[idx++] = *hp++;
    }

    idx = 0;

    while (*hp++ && idx < 6) {
        port_str[idx++] = *hp;
    }

    char *endl = NULL;
    port = strtol(port_str, &endl, 10);

    if (port < 1 || *endl != '\0' || strlen(ip_str) < 7) {
        return false;
    }

    dst->sin_port = htons(port);

    if ((dst->sin_addr.s_addr = inet_addr(ip_str)) < 1) {
        return false;
    }

    return true;
}

