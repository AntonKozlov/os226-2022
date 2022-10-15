#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "sched.h"
#include "pool.h"

static int time = 0;

struct task
{
	void (*entrypoint)(void *);

	void *aspace;
	int priority;
	int deadline;
	int timeout;
	struct task *next;
} task_array[16];

struct tsklst
{
	struct task *first;
} tsklst = {NULL};

void exec_task(struct task *(*)(struct task *, struct task *));

struct task *current = NULL;

struct task *policy_fifo(struct task *chosen_task, struct task *next_task)
{
	return (next_task->timeout > time) ? chosen_task : next_task;
}

struct task *policy_prio(struct task *chosen_task, struct task *next_task)
{
	if (next_task->timeout > time)
		return chosen_task;
	return (chosen_task->timeout <= time && chosen_task->priority > next_task->priority) ? chosen_task : next_task;
}

struct task *policy_deadline(struct task *chosen_task, struct task *next_task)
{
	if (next_task->timeout <= time && ((0 < next_task->deadline && (next_task->deadline < chosen_task->deadline || chosen_task->deadline <= 0)) || (next_task->deadline == chosen_task->deadline && next_task->priority >= chosen_task->priority)))
	{
		return next_task;
	}
	else
	{
		return chosen_task;
	}
}

struct pool task_pool = POOL_INITIALIZER_ARRAY(task_array)

	void
	add_task(void (*entrypoint)(void *),
			 void *aspace,
			 int priority,
			 int deadline,
			 int timeout)
{
	struct task *task = pool_alloc(&task_pool);
	task->entrypoint = entrypoint;
	task->aspace = aspace;
	task->priority = priority;
	task->deadline = deadline;
	task->timeout = timeout;
	task->next = tsklst.first;
	tsklst.first = task;
}

void sched_new(void (*entrypoint)(void *),
			   void *aspace,
			   int priority,
			   int deadline)
{
	add_task(entrypoint, aspace, priority, deadline, time);
}

void sched_cont(void (*entrypoint)(void *),
				void *aspace,
				int timeout)
{
	if (current == NULL)
		return;
	add_task(entrypoint, aspace, current->priority, current->deadline, time + timeout);
}

void sched_time_elapsed(unsigned amount)
{
	time += amount;
}

void sched_run(enum policy policy)
{
	switch (policy)
	{
	case POLICY_FIFO:
		exec_task(policy_fifo);
		break;
	case POLICY_PRIO:
		exec_task(policy_prio);
		break;
	case POLICY_DEADLINE:
		exec_task(policy_prio);
		break;
	default:
		exec_task(policy_deadline);
	}
}

void exec_task(struct task *(*policy)(struct task *, struct task *))
{
	while (tsklst.first != NULL)
	{
		struct task *chosen_task = tsklst.first, *next_task = chosen_task->next;
		while (next_task != NULL)
		{
			chosen_task = policy(chosen_task, next_task);
			next_task = next_task->next;
		}
		if (chosen_task->timeout > time)
		{
			sched_time_elapsed(1);
			continue;
		}
		current = chosen_task;
		current->entrypoint(current->aspace);

		if (current == tsklst.first)
		{
			tsklst.first = current->next;
		}
		else
		{
			struct task *prev;
			prev = tsklst.first;
			while (prev->next != current)
				prev = prev->next;
			prev->next = current->next;
		}

		pool_free(&task_pool, current);
		current = NULL;
	}
}
