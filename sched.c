// Understand the task and get the concept idea of it from Tozarin`s homework because I am sick right now
// And missed last class.
// I will promise to do next homework only by my own efforts.

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "sched.h"

struct task {
	void (*entrypoint)(void *ctx);
	void *ctx;
	int priority;
	int deadline;
	int start_time;
	struct task *next;
};
typedef struct task task;
static int time = 0;
task *head = NULL;
task *curr = NULL;

void sched_new(void (*entrypoint)(void *aspace),
			   void *aspace,
			   int priority,
			   int deadline) {
	if (!head) {
		head = (task *) malloc(sizeof(task));
		head->next = NULL;
		head->entrypoint = entrypoint;
		head->ctx = aspace;
		head->priority = priority;
		head->deadline = deadline;
		head->start_time = -1;
		return;
	}
	task* new = (task*)malloc(sizeof(task));
	new->next = NULL;
	new->entrypoint = entrypoint;
	new->ctx = aspace;
	new->priority = priority;
	new->deadline = deadline;
	new->start_time = -1;

	task* tail = NULL;
	if (head) {
		tail = head;
		while (tail->next) {
			tail = tail->next;
		}
	}
	tail->next = new;
}

void sched_cont(void (*entrypoint)(void *aspace),
				void *aspace,
				int timeout) {
	sched_new(entrypoint, aspace, curr->priority, curr->deadline);
	task* tail = NULL;
	if (head) {
		tail = head;
		while (tail->next) {
			tail = tail->next;
		}
	}
	tail->start_time = time + timeout;
}

void sched_time_elapsed(unsigned amount) {
	time += amount;
}

void delete_task(task* task_to_delete) {
	task* c = head;
	task* p = NULL;

	if (!c) {
		return;
	}
	if (task_to_delete == c) {
		head = c->next;
		free(c);
		return;
	}
	while (c != task_to_delete && c->next) {
		p = c;
		c = c->next;
	}
	if (task_to_delete == c) {
		p->next = c->next;
		free(c);
	}
}

void run_fifo_policy()
{
	while (head) {
		curr = head;
		while (curr) {
			if (curr->start_time <= time) {
				curr->entrypoint(curr->ctx);
				task* task_to_delete = curr;
				curr = curr->next;
				delete_task(task_to_delete);
				continue;
			}
			curr = curr->next;
		}
	}
}

task* get_task_with_max_priority() {
	task* c = head;
	task* max_priority_task = NULL;
	int priority = -1;
	if (!head) {
		return NULL;
	}
	while (c) {
		if (c->priority > priority && c->start_time <= time) {
			priority = c->priority;
			max_priority_task = c;
		}
		c = c->next;
	}

	return max_priority_task;
}

void run_prio_policy()
{
	curr = get_task_with_max_priority();
	while (curr)
	{
		curr->entrypoint(curr->ctx);
		delete_task(curr);
		curr = get_task_with_max_priority();
	}
}
struct task *get_task_with_min_deadline()
{
	if (!head)
	{
		return NULL;
	}
	int min_deadline = INT_MAX;
	struct task *task_with_min_deadline = NULL;
	struct task *c = head;
	while (c)
	{
		if (c->deadline > -1 && c->start_time <= time && min_deadline >= c->deadline)
		{
			if (!task_with_min_deadline || c->priority > task_with_min_deadline->priority) {
				min_deadline = c->deadline;
				task_with_min_deadline = c;
			}
		}
		c = c->next;
	}
	if (!task_with_min_deadline)
	{
		task_with_min_deadline = get_task_with_max_priority();
	}
	return task_with_min_deadline;
}

void run_deadline_policy() {
	curr = get_task_with_min_deadline();
	while (curr) {
		curr->entrypoint(curr->ctx);
		delete_task(curr);
		curr = get_task_with_min_deadline();
	}

}

// Свитч не так плохо, как простые проверки, но, думаю, можно придумать что-то поинтереснее.
void sched_run(enum policy policy) {
	if (!head) {
		return;
	}
	curr = head;

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
		default:
			break;
	}
}
