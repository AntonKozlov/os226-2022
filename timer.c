#define _GNU_SOURCE

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "timer.h"

int timer_cnt(void) {
    struct itimerval t;

    if (getitimer(ITIMER_REAL, &t)) {
        return -1;
    }

    return (t.it_interval.tv_sec - t.it_value.tv_sec) * 1000000 + t.it_interval.tv_usec - t.it_value.tv_usec;
}

void timer_init(int ms, void (*hnd)(void)) {
    struct timeval i;
    i.tv_sec = ms / 1000;
    i.tv_usec = ms * 1000;

    struct itimerval t;
    t.it_interval = i;
    t.it_value = i;

    setitimer(ITIMER_REAL, &t, NULL);
    signal(SIGALRM, (__sighandler_t)hnd);
}
