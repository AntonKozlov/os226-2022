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

		if (!head) {
			head = (task*)malloc(sizeof(task));
			head->next = NULL;
			head->entrypoint = entrypoint;
			head->ctx = aspace;
			head->priority = priority;
			head->deadline = deadline;
			head->time_when_can_start = -1;
		}
		else {
			task* new = (task*)malloc(sizeof(task));
			new->next = NULL;
			new->entrypoint = entrypoint;
			new->ctx = aspace;
			new->priority = priority;
			new->deadline = deadline;
			new->time_when_can_start = -1;
			task* last = last_task();
			last->next = new;
			}
}

void sched_cont(void (*entrypoint)(void *aspace),
		void *aspace,
		int timeout) {
s			ched_new(entrypoint, aspace, current->priority, current->deadline);
			task* last = last_task();
			last->time_when_can_start = time + timeout;

}

void sched_time_elapsed(unsigned amount) {
	time += amount;

}

void sched_run(enum policy policy) {
	if (!tasks) {
		return;
	}
	
	current = head;
	
	if (policy == POLICY_FIFO) 
		{while (head) {
			current = head;
			while (current) {
				if (current->time_when_can_start <= time) {
					current->entrypoint(current->ctx);
					task* to_del = current;
					current = current->next;
					del_task(to_del);
				}
				else {
					current = current->next;
				}
			}
		}
	}

	if (policy == POLICY_PRIO) {
		current = max_prio_task();
		while (current) {
			current->entrypoint(current->ctx);
			del_task(current);
			current = max_prio_task();
		}
	}

	if (policy == POLICY_DEADLINE) {

	}
}
