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
    struct itimerval timer;
    getitimer(ITIMER_REAL, &timer);
    return (timer.it_interval.tv_sec - timer.it_value.tv_sec) * 1000 +
           (timer.it_interval.tv_usec - timer.it_value.tv_usec) / 1000;
}

void timer_init(int ms, void (*hnd)(int)) {
    struct itimerval timer = {
            .it_value = {
                    .tv_sec = ms / 1000,
                    .tv_usec = ms * 1000
            },
            .it_interval = {
                    .tv_sec = ms / 1000,
                    .tv_usec = ms * 1000
            }
    };
    signal(SIGALRM, hnd);
    setitimer(ITIMER_REAL, &timer, NULL);
}
