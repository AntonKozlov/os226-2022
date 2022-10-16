#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "sched.h"

struct sched_task
{
	void (*entrypoint)(void *aspace);
	void *aspace;
	int priority;
	int deadline;
	unsigned int ready_time;
};

struct sched_node
{
	struct sched_task task;
	struct sched_node *next;
};

static struct sched_task_list
{
	struct sched_node *first;
} sched_task_list = {NULL};

static unsigned int time = 0;

static struct sched_task *cur_task = NULL;
void add_task(struct sched_task task)
{
	struct sched_node *node = malloc(sizeof(struct sched_node));

	if (node == NULL)
	{
		fprintf(stderr, "Task addition failed: cannot allocate memory for it\n");
		return;
	}

	node->task = task;
	node->next = sched_task_list.first;
	sched_task_list.first = node;
}

void run_tasks(int (*is_preferable)(struct sched_task old_task, struct sched_task new_task))
{
	while (sched_task_list.first != NULL)
	{
		struct sched_node *prev = NULL, *node = NULL;
		for (struct sched_node *cur_prev = NULL, *cur_node = sched_task_list.first;
			 cur_node != NULL;
			 cur_node = cur_node->next)
		{
			if ((node == NULL && cur_node->task.ready_time <= time) ||
				(node != NULL && is_preferable(node->task, cur_node->task)))
			{
				prev = cur_prev;
				node = cur_node;
			}
			cur_prev = cur_node;
		}
		if (node != NULL)
		{
			if (prev != NULL)
				prev->next = node->next;
			else
				sched_task_list.first = node->next;
			cur_task = &(node->task);
			cur_task->entrypoint(cur_task->aspace);
			cur_task = NULL;
			free(node);
		}
	}
}

int is_preferable_by_fifo(struct sched_task old_task, struct sched_task new_task)
{
	return new_task.ready_time <= time;
}

int is_preferable_by_prio(struct sched_task old_task, struct sched_task new_task)
{
	return new_task.ready_time <= time && new_task.priority >= old_task.priority;
}

int is_preferable_by_deadline(struct sched_task old_task, struct sched_task new_task)
{
	return new_task.ready_time <= time &&
		   ((0 < new_task.deadline && (new_task.deadline < old_task.deadline || old_task.deadline <= 0)) ||
			(new_task.deadline == old_task.deadline && new_task.priority >= old_task.priority));
}

void sched_new(void (*entrypoint)(void *aspace),
			   void *aspace,
			   int priority,
			   int deadline)
{
	struct sched_task task = {entrypoint, aspace, priority, deadline, 0};
	add_task(task);
}

void sched_cont(void (*entrypoint)(void *aspace),
				void *aspace,
				int timeout)
{
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
		run_tasks(&is_preferable_by_fifo);
		break;
	case POLICY_PRIO:
		run_tasks(&is_preferable_by_prio);
		break;
	case POLICY_DEADLINE:
		run_tasks(&is_preferable_by_deadline);
		break;
	default:
		fprintf(stderr, "Unknown policy provided\n");
		break;
	}
}