#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

const int INF = (1 << 30);

static int time = 0;
task *head_task = NULL;
task *curr_task = NULL;
task *tail_task = NULL;

task *get_top_priority_task() {
	if (!head_task) return NULL;

	int priority = -1;
	task *result = NULL;
	if (head_task->time <= time) {
		priority = head_task->priority;
		result = head_task;
	}

	for (task *current = head_task->next; current != head_task; current = current->next) {
		if (current->priority > priority && current->time <= time) {
			priority = current->priority;
			result = current;
		}
	}

	return result;
}

void remove_task(task *task) {
	if (head_task == tail_task) {
		free(head_task);
		head_task = NULL;
		return;
	}

	task->prev->next = task->next;
	task->next->prev = task->prev;
	if (head_task == task) head_task = task->next;
	if (tail_task == task) tail_task = task->prev;

	free(task);
}

task *get_closest_deadline_task() {
	if (!head_task) return NULL;

	int mn;
	task *result;
	if (head_task->time <= time && head_task->deadline > 0) {
		mn = head_task->deadline;
		result = head_task;
	} else {
		mn = INF;
		result = NULL;
	}

	for (task *current = head_task->next; current != head_task; current = current->next) {
		if (mn > current->deadline && current->deadline > 0 && current->time <= time) {
			mn = current->deadline;
			result = current;
		}
	}

	if (!result) return get_top_priority_task();
	else return result;
}

void exec_prio() {
	if (!head_task) return;

	for (curr_task = get_top_priority_task(); curr_task != NULL; curr_task = get_top_priority_task()) {
		curr_task->entrypoint(curr_task->ctx);
		remove_task(curr_task);
		curr_task = get_top_priority_task();
	}
}

void exec_fifo() {
	if (!head_task) return;

	curr_task = head_task;
	while (head_task) {
		if (curr_task->time <= time) {
			curr_task->entrypoint(curr_task->ctx);
			task *remove = curr_task;
			curr_task = curr_task->next;
			remove_task(remove);
		} else {
			curr_task = curr_task->next;
		}
	}
}

void exec_deadline() {
	if (!head_task) return;
	for (curr_task = get_closest_deadline_task(); curr_task != NULL; curr_task = get_closest_deadline_task()) {
		curr_task->entrypoint(curr_task->ctx);
		remove_task(curr_task);
		curr_task = get_closest_deadline_task();
	}
}

void sched_new(void (*entrypoint)(void *aspace),
			   void *aspace,
			   int priority,
			   int deadline) {
	task *next_task = (task *) malloc(sizeof(task));
	*next_task = (task) {
			.next = head_task,
			.prev = tail_task,
			.entrypoint = entrypoint,
			.ctx = aspace,
			.priority = priority,
			.deadline = deadline,
			.time = -1
	};

	if (!head_task) {
		head_task = next_task;
		tail_task = head_task;
		head_task->next = head_task;
		head_task->prev = head_task;
		tail_task->next = head_task;
		tail_task->prev = head_task;
	} else {
		tail_task->next = next_task;
		tail_task = next_task;
		head_task->prev = tail_task;
	}
}

void sched_cont(void (*entrypoint)(void *aspace),
				void *aspace,
				int timeout) {
	sched_new(entrypoint, aspace, curr_task->priority, curr_task->deadline);
	tail_task->time = time + timeout;
}

void sched_time_elapsed(unsigned amount) {
	time += amount;
}

void sched_run(enum policy policy) {
	if (policy == POLICY_FIFO) {
		exec_fifo();
	} else if (policy == POLICY_PRIO) {
		exec_prio();
	} else if (policy == POLICY_DEADLINE) {
		exec_deadline();
	}
}