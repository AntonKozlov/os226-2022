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

	if (getitimer(ITIMER_REAL, &timer))
		return -1;
	return (timer.it_interval.tv_sec - timer.it_value.tv_sec) * 1000000 + timer.it_interval.tv_usec - timer.it_value.tv_usec;
}

void timer_init(int ms, void (*hnd)(void)) {
	struct timeval initial_timeval = (struct timeval) {
		.tv_usec = ms * 1000,
		.tv_sec = ms / 1000
	};

	struct itimerval initial_timer = (struct itimerval) {
		.it_value = initial_timeval,
		.it_interval = initial_timeval
	};

	setitimer(ITIMER_REAL, &initial_timer, NULL);
	signal(SIGALRM, (__sighandler_t) hnd);
}
