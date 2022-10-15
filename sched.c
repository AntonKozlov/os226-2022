#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

static int time = 0;
struct task *tasks = NULL;
struct task *current_task = NULL;
struct task *last_task = NULL;

void delete_task(struct task *to_delete)
{
	if (tasks == last_task)
	{
		free(tasks);
		tasks = NULL;
		return;
	}

	to_delete->previous_task->next_task = to_delete->next_task;
	to_delete->next_task->previous_task = to_delete->previous_task;

	if (tasks == to_delete)
	{
		tasks = to_delete->next_task;
	}

	if (last_task == to_delete)
	{
		last_task = to_delete->previous_task;
	}

	free(to_delete);
}

struct task *find_max_priority_task()
{
	if (!tasks)
	{
		return NULL;
	}

	int max_priority = -1;
	struct task *answer = NULL;
	if (tasks->start_time <= time)
	{
		max_priority = tasks->priority;
		answer = tasks;
	}

	struct task *current = tasks->next_task;

	while (current != tasks)
	{
		if (current->priority > max_priority && current->start_time <= time)
		{
			max_priority = current->priority;
			answer = current;
		}

		current = current->next_task;
	}

	return answer;
}

struct task *find_min_deadline_task()
{
	if (!tasks)
	{
		return NULL;
	}

	int min_deadline = INT_MAX;
	struct task *answer = NULL;
	if (tasks->start_time <= time && tasks->deadline > 0)
	{
		min_deadline = tasks->deadline;
		answer = tasks;
	}

	struct task *current = tasks->next_task;

	while (current != tasks)
	{
		if (min_deadline > current->deadline && current->deadline > 0 && current->start_time <= time)
		{
			min_deadline = current->deadline;
			answer = current;
		}

		current = current->next_task;
	}

	if (!answer)
	{
		answer = find_max_priority_task();
	}

	return answer;
}

void run_fifo_policy()
{
	current_task = tasks;

	while (tasks)
	{
		if (current_task->start_time <= time)
		{
			current_task->entrypoint(current_task->ctx);
			struct task *to_delete = current_task;
			current_task = current_task->next_task;
			delete_task(to_delete);
		}
		else
		{
			current_task = current_task->next_task;
		}
	}
}

void run_prio_policy()
{
	current_task = find_max_priority_task();
	while (current_task)
	{
		current_task->entrypoint(current_task->ctx);
		delete_task(current_task);
		current_task = find_max_priority_task();
	}
}

void run_deadline_policy()
{
	current_task = find_min_deadline_task();
	while (current_task)
	{
		current_task->entrypoint(current_task->ctx);
		delete_task(current_task);
		current_task = find_min_deadline_task();
	}
}

void sched_new(void (*entrypoint)(void *aspace),
			   void *aspace,
			   int priority,
			   int deadline)
{
	struct task *new_task = (struct task *)malloc(sizeof(struct task));
	*new_task = (struct task){
		.next_task = tasks,
		.previous_task = last_task,
		.entrypoint = entrypoint,
		.ctx = aspace,
		.priority = priority,
		.deadline = deadline,
		.start_time = -1};

	if (!tasks)
	{
		tasks = new_task;
		last_task = tasks;
		tasks->next_task = tasks;
		tasks->previous_task = tasks;
		last_task->next_task = tasks;
		last_task->previous_task = tasks;
	}
	else
	{
		last_task->next_task = new_task;
		last_task = new_task;
		tasks->previous_task = last_task;
	}
}

void sched_cont(void (*entrypoint)(void *aspace),
				void *aspace,
				int timeout)
{
	sched_new(entrypoint, aspace, current_task->priority, current_task->deadline);
	last_task->start_time = time + timeout;
}

void sched_time_elapsed(unsigned amount)
{
	time += amount;
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
	else if (policy == POLICY_PRIO)
	{
		run_prio_policy();
	}
	else if (policy == POLICY_DEADLINE)
	{
		run_deadline_policy();
	}
}
