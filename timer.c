#define _GNU_SOURCE

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include "timer.h"

int timer_cnt(void)
{
	struct itimerval timer;
	getitimer(ITIMER_REAL, &timer);

	long interval = timer.it_interval.tv_sec * 1000 + timer.it_interval.tv_usec / 1000;
	long time_left = timer.it_value.tv_sec * 1000 + timer.it_value.tv_usec / 1000;

	return (interval - time_left);
}

void timer_init(int ms, void (*hnd)(void))
{
	struct timeval interval = {ms / 1000, ms * 1000};
	struct itimerval timer = {interval, interval};

	setitimer(ITIMER_REAL, &timer, NULL);
	signal(SIGALRM, (__sighandler_t)hnd);
}
