#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

static int time;
static task *task_list = NULL;
static task *last_task = NULL;
static task *current_task = NULL;

void sched_new(void (*entrypoint)(void *aspace),
		void *aspace,
		int priority,
		int deadline) {
	task *new_task = (task*) malloc(sizeof(task));
	*new_task = (task) {
		.next = task_list,
		.prev = last_task,
		.entrypoint = entrypoint,
		.ctx = aspace,
		.priority = priority,
		.deadline = deadline,
		.timeout = 0
	};

	if (task_list) {
		task_list->prev = new_task;
		last_task->next = new_task;
		last_task = new_task;
	} else {
		task_list = new_task;
		task_list->next = task_list->prev = new_task;

		last_task = new_task;
		last_task->next = last_task->prev = new_task;
	}
}

void sched_cont(void (*entrypoint)(void *aspace),
		void *aspace,
		int timeout) {
	sched_new(entrypoint, aspace, current_task->priority, current_task->deadline);
	last_task->timeout = time + timeout;
}

void sched_time_elapsed(unsigned amount) {
	time += amount;
}

void delete_task(task *task) {
	if (task_list == last_task) {
		free(task_list);
		task_list = NULL;
		last_task = NULL;

		return;
	}

	task->prev->next = task->next;
	task->next->prev = task->prev;

	if (task_list == task) {
		task_list = task->next;
	}

	if (last_task == task) {
		last_task = task->prev;
	}

	free(task);
	task = NULL;
}

void run_fifo_policy(void) {
	for (current_task = task_list; task_list; current_task = current_task->next) {
		if (current_task->timeout <= time) {
			current_task->entrypoint(current_task->ctx);

			current_task = current_task->prev;
			delete_task(current_task->next);
		}
	}
}

task *get_prioritized_task(void) {
	if (!task_list)
		return NULL;

	task *prioritized_task = NULL;
	for (current_task = task_list;; current_task = current_task->next) {
		if ((!prioritized_task || current_task->priority > prioritized_task->priority) && current_task->timeout <= time) {
			prioritized_task = current_task;
		}

		if (current_task == last_task)
			break;
	}

	return prioritized_task;
}

void run_priority_policy(void) {
	for (current_task = get_prioritized_task(); task_list; current_task = get_prioritized_task()) {
		current_task->entrypoint(current_task->ctx);
		delete_task(current_task);
	}
}

task *get_earliest_deadline_task(void) {
	if (!task_list)
		return NULL;

	task *best_task = NULL;
	for (current_task = task_list;; current_task = current_task->next) {
		if (current_task->timeout <= time && current_task->deadline > 0) {
			if (!best_task) {
				best_task = current_task;
				continue;
			}

			if (current_task->deadline < best_task->deadline)
				best_task = current_task;
		}

		if (current_task == last_task)
			break;
	}

	if (!best_task) {
		return get_prioritized_task();
	}

	for (current_task = task_list;; current_task = current_task->next) {
		if (current_task->timeout <= time && current_task->deadline == best_task->deadline &&
											 current_task->priority > best_task->priority) {
			best_task = current_task;
		}

		if (current_task == last_task)
			break;
	}

	return best_task;
}

void run_deadline_policy(void) {
	for (current_task = get_earliest_deadline_task(); task_list; current_task = get_earliest_deadline_task()) {
		current_task->entrypoint(current_task->ctx);
		delete_task(current_task);
	}
}

void sched_run(enum policy policy) {
	struct policy_runner {
		void (*run_policy)(void);
	};
	void (*run_policy[3])(void) = {
			run_fifo_policy,
			run_priority_policy,
			run_deadline_policy
	};

	run_policy[policy]();
}
