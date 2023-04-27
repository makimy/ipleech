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
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#include "ipleech.h"

int g_timeout = 5;

static bool has_stdin();
static void usage(const char * const appname);
static void print_host(const char * const host);

static void usage(const char * const appname) {
    printf("Usage: %s <filename.txt>\n"
           "  -g Grab and print hosts: <default>\n"
           "  -t Scan timeout: <default: 5s>\n"
           "  -s Also [s]can hosts\n"
           "  -h This help message\n"
           , appname);
}

static bool has_stdin() {
    fd_set set;
    struct timeval timeout = { .tv_sec = 0, .tv_usec = 100 };

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    const int ret = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);

    if (ret == -1 || ret == 0) {
        return false;
    }

    return true;
}

static void print_host(const char * const host) {
    puts(host);
}

void print_scanned(const struct host_leech * const event) {
    if (event->status == true) {
        fprintf(stdout, "\033[32m%s:%d\033[0m\n", inet_ntoa(event->saddr.sin_addr),
                    ntohs(event->saddr.sin_port));
    } else {
        fprintf(stderr, "\033[31m%s:%d\033[0m\n", inet_ntoa(event->saddr.sin_addr),
                    ntohs(event->saddr.sin_port));
    }
}

int main(int ac, char **av) {
    const char * const appname = av[0];
    FILE *in_stream = NULL;
    int opt = 0;
    bool g_flag = false;
    bool s_flag = false;

    while ((opt = getopt(ac, av, "t:sgh")) != -1) {
        switch (opt) {
            case 't':
                g_timeout = atoi(optarg);
                if (g_timeout < 0) {
                    g_timeout = 1;
                }
                break;
            case 's':
                s_flag = true;
                break;
            case 'g':
                g_flag = true;
                break;
            case 'h':
                usage(appname);
                return 0;
                break;
            default:
                break;
        }

    }

    if (ac != optind) {
        const char * const filename = av[ac - 1];

        in_stream = fopen(filename, "r");

        if (in_stream == NULL) {
            perror(filename);
            return ENOENT;
        }

    } else if (has_stdin()) {
        in_stream = stdin;
    }

    if (in_stream) {
        const char * host = NULL;

        while ((host = next_host(in_stream)) != NULL) {
            if (s_flag && !g_flag) {
                scan_host(host);
            } else {
                print_host(host);
            }
        }

        if (s_flag && !g_flag) {
            proceed_host_leech_events();
        }

    } else {
        usage(appname);
        return ENOENT;
    }

    fclose(in_stream);

    return 0;
}
