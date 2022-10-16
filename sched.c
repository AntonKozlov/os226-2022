#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

static int time;
static int time = 0;
struct task *tasks = NULL;
struct task *currenttask = NULL;
struct task *lasttask = NULL;

void delete_task(struct task *deletetarget)
{
	if (tasks == lasttask) {
		free(tasks);
		tasks = NULL;
		return;
	}

	deletetarget->previoustask->nexttask = deletetarget->nexttask;
	deletetarget->nexttask->previoustask = deletetarget->previoustask;

	if (tasks == deletetarget)
		tasks = deletetarget->nexttask;

	if (lasttask == deletetarget)
		lasttask = deletetarget->previoustask;

	free(deletetarget);
}

struct task *get_max_priority_task()
{
	if (!tasks)
		return NULL;

	struct task *returnvalue = NULL;
	int max_priority = -1;
	if (tasks->start <= time) {
		max_priority = tasks->priority;
		returnvalue = tasks;
	}

	struct task *current = tasks->nexttask;
	while (current != tasks) {
		if (current->priority > max_priority && current->start <= time) {
			max_priority = current->priority;
			returnvalue = current;
		}

		current = current->nexttask;
	}

	return returnvalue;
}

struct task *get_min_deadline_task() {
	if (!tasks)
		return NULL;

	struct task *returnvalue = NULL;
	int min_deadline = INT_MAX;
	if (tasks->start <= time && tasks->deadline > 0) {
		min_deadline = tasks->deadline;
		returnvalue = tasks;
	}

	struct task *current = tasks->nexttask;

	while (current != tasks) {
		if (min_deadline > current->deadline && current->deadline > 0 && current->start <= time) {
			min_deadline = current->deadline;
			returnvalue = current;
		}

		current = current->nexttask;
	}

	if (!returnvalue)
		returnvalue = get_max_priority_task();

	return returnvalue;
}

void run_fifo_policy()
{
	currenttask = tasks;

	while (tasks) {
		if (currenttask->start <= time) {
			currenttask->entrypoint(currenttask->ctx);
			struct task *deletetarget = currenttask;
			currenttask = currenttask->nexttask;
			delete_task(deletetarget);
		} else
			currenttask = currenttask->nexttask;
	}
}

void run_prio_policy()
{
	currenttask = get_max_priority_task();
	while (currenttask) {
		currenttask->entrypoint(currenttask->ctx);
		delete_task(currenttask);
		currenttask = get_max_priority_task();
	}
}

void run_deadline_policy()
{
	currenttask = get_min_deadline_task();
	while (currenttask) {
		currenttask->entrypoint(currenttask->ctx);
		delete_task(currenttask);
		currenttask = get_min_deadline_task();
	}
}

void sched_new(void (*entrypoint)(void *aspace),
		void *aspace,
		int priority,
		int deadline) {
	struct task *newtask = (struct task *)malloc(sizeof(struct task));
	*newtask = (struct task) {
		.nexttask = tasks,
		.previoustask = lasttask,
		.entrypoint = entrypoint,
		.ctx = aspace,
		.priority = priority,
		.deadline = deadline,
		.start = -1
	};

	if (!tasks) {
		tasks = newtask;
		lasttask = tasks;
		tasks->nexttask = tasks;
		tasks->previoustask = tasks;
		lasttask->nexttask = tasks;
		lasttask->previoustask = tasks;
	} else {
		lasttask->nexttask = newtask;
		lasttask = newtask;
		tasks->previoustask = lasttask;
	}
}

void sched_cont(void (*entrypoint)(void *aspace),
		void *aspace,
		int timeout) {
	sched_new(entrypoint, aspace, currenttask->priority, currenttask->deadline);
	lasttask->start = time + timeout;
}

void sched_time_elapsed(unsigned amount) {
	time += amount;

}

void sched_run(enum policy policy) {
	if (!tasks)
		return;
	switch (policy) {
		case POLICY_FIFO:
			run_fifo_policy();
			break;
		case POLICY_PRIO:
			run_prio_policy();
			break;
		case POLICY_DEADLINE:
			run_deadline_policy();
			break;
	}
}
