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
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "ipleech.h"

static struct host_leech * pop_event();
static bool post_event(const struct host_leech * const event);
static void run_scanner(struct host_leech * const poll[], const int size);

#define MAX_EVENTS 5
static struct host_leech events[MAX_EVENTS] = { 0 };
static int event_tail = -1;

static struct host_leech * pop_event() {
    if (event_tail < 0) {
        return NULL;
    }

    return &events[event_tail--];
}

static bool post_event(const struct host_leech * const event) {
    if (event_tail == (MAX_EVENTS - 1)) {
        return false;
    }

    struct host_leech * const e = &events[++event_tail];
    memcpy(&e->saddr, &event->saddr, sizeof(struct sockaddr_in));

    return true;
}

static void run_scanner(struct host_leech * const poll[], const int size) {

    for (int i = 0; i <= size; ++i) {
        struct host_leech * const p = poll[i];

        p->status = false;
        p->saddr.sin_family = AF_INET;
        p->fd = socket(AF_INET, SOCK_STREAM, 0);

        if (p->fd == -1) {
            continue;
        }

        const int flags = fcntl(p->fd, F_GETFL, 0);

        if (flags == -1 || fcntl(p->fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            continue;
        }

        const int conn_status = connect(p->fd,
               (struct sockaddr *)&p->saddr, sizeof(p->saddr));

        if (conn_status == -1 && errno != EINPROGRESS) {
            continue;
        }
    }

    int scan_restart = 0;
    extern int g_timeout;
    const time_t start_time = time(NULL);

    while (scan_restart < MAX_EVENTS) {
        struct timeval timeout = { .tv_sec = g_timeout, .tv_usec = 0 };
        fd_set fds;
        int maxfd = 0;

        FD_ZERO(&fds);

        for (int i = 0; i <= size; ++i) {
            struct host_leech * const p = poll[i];

            if (p->status == true) {
                continue;
            }

            if (p->fd > maxfd) {
                maxfd += p->fd;
            }

            FD_SET(p->fd, &fds);
        }


        select(maxfd + 1, NULL, &fds, NULL, &timeout);

        for (int i = 0; i <= size; ++i) {
            struct host_leech * const p = poll[i];

            if (FD_ISSET(p->fd, &fds)) {
                p->status = true;
                scan_restart++;
            }
        }

        if ((time(NULL) - start_time) >= g_timeout) {
            scan_restart = MAX_EVENTS;
        }
    }

    for (int i = 0; i <= size; ++i) {
        struct host_leech * const p = poll[i];
        close(p->fd);
    }
}

void proceed_host_leech_events() {
    struct host_leech *e = NULL;
    struct host_leech *poll[MAX_EVENTS] = { NULL };
    int idx = -1;

    // collect
    while ((e = pop_event()) != NULL) {
        poll[++idx] = e;
    }

    // scan
    run_scanner(poll, idx);

    // print
    for (int i = 0; i <= idx; ++i) {
        print_scanned(poll[i]);
    }
}

void scan_host(const char * const host) {
    struct host_leech event = { 0 };

    if (hoststr_split(&event.saddr, host)) {
        if (!post_event(&event)) {
            proceed_host_leech_events();
            post_event(&event);
        }
    }
}

