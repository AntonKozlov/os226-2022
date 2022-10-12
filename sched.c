#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

static int time = 0;
task* tasks = NULL;

void sched_new(void (*entrypoint)(void *aspace),
		void *aspace,
		int priority,
		int deadline) {

		if (!tasks) {
			tasks = (task*)malloc(sizeof(task));
			tasks->task_link.next = tasks;
			tasks->task_link.prev = tasks;
			tasks->entrypoint = NULL;
			tasks->ctx = NULL;
			tasks->priority = 0;
			tasks->deadline = 0;
			tasks->time_when_can_start = -1;
		}

		task* last = ((task_link*)tasks)->prev;
		task* new = (task*)malloc(sizeof(task));
		((task_link*)new)->next = tasks;
		((task_link*)new)->prev = last;
		new->entrypoint = entrypoint;
		new->ctx = aspace;
		new->priority = priority;
		new->deadline = deadline;
		new->time_when_can_start = -1;
}

void sched_cont(void (*entrypoint)(void *aspace),
		void *aspace,
		int timeout) {

}

void sched_time_elapsed(unsigned amount) {
	time += amount;

}

void sched_run(enum policy policy) {
}
