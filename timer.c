#define _GNU_SOURCE

#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include "timer.h"

static int get_raw_time(struct timeval *tv);

int timer_cnt(void) {
    struct itimerval itv;
    getitimer(ITIMER_REAL, &itv);
    return get_raw_time(&itv.it_interval) - get_raw_time(&itv.it_value);
}

void timer_init(int ms, void (*hnd)(void)) {
    struct timeval tv = {
        .tv_sec = ms / 1000,
        .tv_usec = (ms % 1000) * 1000
    };
    struct itimerval itv = {
        .it_value = tv,
        .it_interval = tv
    };
    setitimer(ITIMER_REAL, &itv, NULL);
    signal(SIGALRM, (__sighandler_t) hnd);
}


// Utils

static int get_raw_time(struct timeval *tv) {
    return 1000000 * tv->tv_sec + tv->tv_usec;
}
