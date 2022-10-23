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
	return ((long)(timer.it_interval.tv_sec * 1000 + timer.it_interval.tv_usec / 1000) -
	(long)(timer.it_value.tv_sec * 1000 + timer.it_value.tv_usec / 1000));
}

void timer_init(int ms, void (*hnd)(void)) {
	struct timeval interval;
	interval.tv_sec = ms / 1000;
	interval.tv_usec = ms * 1000;

	struct itimerval timer;
	timer.it_interval = interval;
	timer.it_value = interval;

	setitimer(ITIMER_REAL, &timer, NULL);
	signal(SIGALRM, (__sighandler_t)hnd);
}
