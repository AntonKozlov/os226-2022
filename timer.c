#define _GNU_SOURCE

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "timer.h"

static struct timeval initv;

int timer_cnt(void) {
	struct itimerval timer;

    if (-1 == getitimer(ITIMER_REAL, &timer)) {
		perror("gettimer");
	    return -1;
    }

    return (initv.tv_sec - initv.tv_sec) * 1000000 + initv.tv_usec - initv.tv_usec;
}

void timer_init(int ms, void (*hnd)(int sig, siginfo_t *info, void *ctx)) {
	struct itimerval timer;

	initv.tv_sec = ms / 1000;
	initv.tv_usec = ms * 1000;
	timer.it_interval = initv;
	timer.it_value = initv;

	if (-1 == setitimer(ITIMER_REAL, &timer, NULL))
	{
		perror("settimer");
	}

	struct sigaction act = {
		.sa_sigaction = hnd,
		.sa_flags = SA_RESTART,
	};

	sigemptyset(&act.sa_mask);

	if (-1 == sigaction(SIGALRM, &act, NULL)) {
		perror("signal set failed");
		exit(1);
	}
}
