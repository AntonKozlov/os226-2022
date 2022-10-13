#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

static int time = 0;
task* head = NULL;
task* current = NULL;

task* last_task() {
	if (!head) {
		return NULL;
	}

	task* curr = head;
	while (curr->next) {
		curr = curr->next;
	}

	return curr;
}

void del_task(task* target) {
	task* curr = head;
	if (!curr) {
		return;
	}

	if (curr == target) {
		head = curr->next;
		free(curr);
		return;
	}

	task* prev = NULL;

	while (curr != target && curr->next) {
		prev = curr;
		curr = curr->next;
	}

	if (curr == target) {
		prev->next = curr->next;
		free(curr);
	}
	
}

task* max_prio_task() {
	if (!head) {
		return NULL;
	}

	int max_p = -1;
	task* curr = head;
	task* rez;

	while (curr) {
		if (max_p < curr->priority && curr->time_when_can_start <= time) {
			max_p = curr->priority;
			rez = curr;
		}
		curr = curr->next;
	}

	return rez;
}

task* min_deadline_task() {
	if (!head) {
		return NULL;
	}

	int min_dl = INT_MAX;
	task* curr = head;
	task* rez = NULL;

	while(curr) {
		if (curr->deadline > -1 && min_dl >= curr->deadline && curr->time_when_can_start <= time) {
			if (!rez || curr->priority > rez->priority) {
				min_dl = curr->deadline;
				rez = curr;
			}
		}
		
		curr = curr->next;
	}

	if (!rez) {
		rez = max_prio_task();
	}

	return rez;
}

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

		sched_new(entrypoint, aspace, 0, -1);
		((task_link*)tasks)->prev->time_when_can_start = time + timeout;

}

void sched_time_elapsed(unsigned amount) {
	time += amount;

}

void sched_run(enum policy policy) {
	if (!tasks) {
		return;
	}

	if (policy == POLICY_FIFO) {
			do {
				if (tasks->time_when_can_start <= time) {
					tasks->entrypoint(tasks->ctx);
					task* prev = ((task_link*)tasks)->prev;
					task* next = ((task_link*)tasks)->next;
					((task_link*)prev)->next = next;
					((task_link*)next)->prev = prev;
				}

				tasks = ((task_link*)tasks)->next;
			} while (tasks != ((task_link*)tasks)->prev);
	}

	if (policy == POLICY_PRIO) {
		int max_prio = -1;

		do {
			max_prio = -1;
			task* curr = tasks;
			while (tasks != ((task_link*)curr)->next) {
			max_prio = max_prio < curr->priority ? curr->priority : max_prio;
			curr = ((task_link*)curr)->next;
		}

		curr = tasks;
		do {
			if (curr->time_when_can_start <= time && curr->priority == max_prio) {
				curr->entrypoint(curr->ctx);
			}

			curr = ((task_link*)curr)->next;
			} while (tasks != ((task_link*)curr)->next);

		} while (max_prio != -1);
	}

	if (policy == POLICY_DEADLINE) {

	}
}
