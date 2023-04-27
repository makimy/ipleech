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

#pragma once

#include <stdbool.h>
#include <arpa/inet.h>

struct host_leech {
    int fd;
    bool status;
    struct sockaddr_in saddr;
};

char *next_host(FILE * const in);
bool hoststr_split(struct sockaddr_in * const dst, const char * host);

void scan_host(const char * const host);
void proceed_host_leech_events();

void print_scanned(const struct host_leech * const event);

