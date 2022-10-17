#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "sched.h"
#include "pool.h"

static int time;
static task* head_task = NULL;
static task* curr_task = NULL;
static task* tail_task = NULL;

void remove_task(task* task) {

	if (!head_task || !task) {
		return;
	}
	if (head_task == tail_task) {
		free(head_task);
		head_task = NULL;
		return;
	}

	if (head_task == task) {
		head_task = task->next;
	} else if (tail_task == task) {
		tail_task = task->prev;
	} else {
		task->prev->next = task->next;
		task->next->prev = task->prev;
	}

	free(task);
}

bool priority_cmp(task* first, task* second) {
	return (first->priority > second->priority && first->time <= time);
}

task* get_top_priority_task() {
	if (!head_task) return NULL;

	task* result = head_task;
	task* cur = head_task->next;
	while (cur != head_task) {
		if (priority_cmp(cur, result)) {
			result = cur;
		}

		cur = cur->next;
	}

	return result;
}

bool deadline_cmp(task* first, task* second) {
	return (first->deadline < second->deadline && first->deadline > 0 && first->time <= time);
}

task* get_closest_deadline_task() {
	if (!head_task) return NULL;

	task* result = head_task;
	task* cur = head_task->next;
	while (cur != head_task) {
		if (deadline_cmp(cur, result)) {
			result = cur;
		}

		cur = cur->next;
	}

	if (result->time > time || result->deadline == 0) {
		return get_top_priority_task();
	} else {
		return result;
	}
}

void exec_fifo() {
	if (!head_task) return;

	curr_task = head_task;
	while (head_task) {
		if (curr_task->time > time) {
			curr_task = curr_task->next;
			continue;
		}

		curr_task->entrypoint(curr_task->ctx);
		task* remove = curr_task;
		curr_task = curr_task->next;
		remove_task(remove);
	}
}

void exec_prio() {
	if (!head_task) return;

	curr_task = get_top_priority_task();
	while (curr_task) {
		curr_task->entrypoint(curr_task->ctx);
		remove_task(curr_task);
		curr_task = get_top_priority_task();
	}
}

void exec_deadline() {
	if (!head_task) return;

	curr_task = get_closest_deadline_task();
	while (curr_task) {
		curr_task->entrypoint(curr_task->ctx);
		remove_task(curr_task);
		curr_task = get_closest_deadline_task();
	}
}

void sched_new(void (*entrypoint)(void *aspace),
		void *aspace,
		int priority,
		int deadline) {

	task* new = (task*)malloc(sizeof(task));
	new->next = head_task;
	new->prev = tail_task;
	new->entrypoint = entrypoint;
	new->ctx = aspace;
	new->priority = priority;
	new->deadline = deadline;
	new->time = -1;

	if (!head_task) {
		head_task = new;
		tail_task = head_task;
		head_task->next = head_task;
		head_task->prev = head_task;
		tail_task->next = head_task;
		tail_task->prev = head_task;
	} else {
		tail_task->next = new;
		tail_task = new;
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
	if (policy == POLICY_DEADLINE) {
		exec_deadline();
	} else if (policy == POLICY_FIFO) {
		exec_fifo();
	} else if (policy == POLICY_PRIO){
		exec_prio();
	}
}
