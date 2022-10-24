#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

static int time = 0;

struct task* tasks = NULL;
struct task* last_task = NULL;
struct task* current_task = NULL;


void sched_new(void (*entrypoint)(void *aspace),
		void *aspace,
		int priority,
		int deadline)
{
	struct task* new_task = (struct task *)malloc(sizeof(struct task));
	*new_task = (struct task){
		.next_task = NULL,
		.entrypoint = entrypoint,
		.ctx = aspace,
		.priority = priority,
		.deadline = deadline,
		.start_time = -1
	};

	if (!tasks)
	{
		tasks = new_task;
		last_task = tasks;
	}
	else
	{
		last_task->next_task = new_task;
		last_task = new_task;
	}

}

void sched_cont(void (*entrypoint)(void *aspace),
		void *aspace,
		int timeout)
{
	sched_new(entrypoint, aspace, current_task->priority, current_task->deadline);
	
	last_task->start_time = time + timeout;
}

void sched_time_elapsed(unsigned amount) {
	time += amount;

}

void delete_task(struct task* to_delete)
{

	if (tasks == last_task)
	{
		free(tasks);
		tasks = NULL;
		return;
	}

	if (tasks == to_delete)
	{
		tasks = to_delete->next_task;
		free(to_delete);
		to_delete = NULL;
		return;
	}

	struct task* prev_task = tasks;
	while (prev_task->next_task != to_delete)
	{
		prev_task = prev_task->next_task;
	}

	if (last_task == to_delete)
	{
		free(to_delete);
		to_delete = NULL;
		last_task = prev_task;
		return;
	}

	prev_task->next_task = to_delete->next_task;
	free(to_delete);
	to_delete = NULL;
}

struct task* get_max_prio_task()
{
	if (!tasks)
	{
		return NULL;
	}
	struct task* max_prio_task = NULL;
	int max_prio = -1;
	struct task* curr_task = tasks;
	while (curr_task)
	{
		if (curr_task->start_time <= time && curr_task->priority > max_prio)
		{
			max_prio = curr_task->priority;
			max_prio_task = curr_task;
		}
		if (curr_task == last_task)
		{
			break;
		}
		curr_task = curr_task->next_task;
	}

	return max_prio_task;

}

struct task* get_min_deadline_task()
{
	if (!tasks)
	{
		return NULL;
	}

	struct task* min_deadline_task = NULL;
	int min_deadline = INT_MAX;
	struct task* curr_task = tasks;
	while (curr_task)
	{
		if (curr_task->start_time <= time && curr_task->deadline < min_deadline && curr_task->deadline > 0)
		{
			min_deadline_task = curr_task;
			min_deadline = curr_task->deadline;
		}
		if (curr_task == last_task)
		{
			break;
		}

		curr_task = curr_task->next_task;
	}

	if (!min_deadline_task)
	{
		return get_max_prio_task();
	}

	return min_deadline_task;
}

void run_fifo_policy()
{
	current_task = tasks;
	while (tasks)
	{
		if (current_task->start_time <= time)
		{
			current_task->entrypoint(current_task->ctx);
			struct task* prev_current_task = current_task;
			current_task = current_task->next_task;
			delete_task(prev_current_task);
		}
		else
		{
			current_task = current_task->next_task;
		}
	}
}

void run_prio_policy()
{
	current_task = get_max_prio_task();
	while (current_task)
	{
		current_task->entrypoint(current_task->ctx);
		delete_task(current_task);
		current_task = get_max_prio_task();
	}
}

void run_deadline_policy()
{
	current_task = get_min_deadline_task();
	while (current_task)
	{
		current_task->entrypoint(current_task->ctx);
		delete_task(current_task);
		current_task = get_min_deadline_task();
	}
}

void sched_run(enum policy policy) 
{
	if (!tasks)
	{
		return;
	}

	if (policy == POLICY_FIFO)
	{
		run_fifo_policy();
	}
	else if (policy == POLICY_DEADLINE)
	{
		run_deadline_policy();
	}
	else if (policy == POLICY_PRIO)
	{
		run_prio_policy();
	}
}
