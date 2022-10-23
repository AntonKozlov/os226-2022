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

    if (getitimer(ITIMER_REAL, &timer)) {
		perror("gettimer failed");
	    return -1;
    }

    return (timer.it_interval.tv_sec - timer.it_value.tv_sec) * 1000000 + timer.it_interval.tv_usec - timer.it_value.tv_usec;
}

void timer_init(int ms, void (*hnd)(void)) {
	struct itimerval timer;
	struct timeval timeval;

	timeval.tv_sec = ms / 1000;
	timeval.tv_usec = ms * 1000;
	timer.it_interval = timeval;
	timer.it_value = timeval;

	if (setitimer(ITIMER_REAL, &timer, NULL))
	{
		perror("settimer failed");
	    return;
	}

	signal(SIGALRM, (__sighandler_t)hnd);
	return;
}
