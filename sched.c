#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "sched.h"
#include "pool.h"

static int time = 0;

static task_t task_array[10];
static struct pool task_pool = POOL_INITIALIZER_ARRAY(task_array)

static task_list_t task_list = {NULL};

static task_t *current_task;

void add_task(
        entrypoint_t entrypoint,
        void *ctx,
        int priority,
        int deadline,
        int timeout
) {
    task_t *new_task = pool_alloc(&task_pool);
    new_task->entrypoint = entrypoint;
    new_task->ctx = ctx;
    new_task->priority = priority;
    new_task->deadline = deadline;
    new_task->timeout = timeout;
    new_task->created_at = time;
    new_task->next = task_list.first;
    task_list.first = new_task;
}

void sched_new(
        entrypoint_t entrypoint,
        void *ctx,
        int priority,
        int deadline) {

    add_task(entrypoint, ctx, priority, deadline, 0);
}

void sched_cont(
        entrypoint_t entrypoint,
        void *ctx,
        int timeout) {
    if (current_task != NULL)
        add_task(entrypoint, ctx, current_task->priority, current_task->deadline, timeout);
}

void sched_time_elapsed(unsigned amount) {
    time += amount;
}

task_t *choose_by_fifo(task_t *first, task_t *second) {
    return second;
}

task_t *choose_by_prio(task_t *first, task_t *second) {
    return first->priority > second->priority ? first : second;
}

task_t *choose_by_deadline(task_t *first, task_t *second) {
    if (first->deadline == second->deadline)
        return choose_by_prio(first, second);

    if (first->deadline <= 0)
        return second;
    else if (second->deadline <= 0)
        return first;

    return first->deadline < second->deadline ? first : second;
}

policy_fn_t *choose_policy_fn(enum policy policy) {
    switch (policy) {
        case POLICY_FIFO:
            return choose_by_fifo;
        case POLICY_PRIO:
            return choose_by_prio;
        case POLICY_DEADLINE:
            return choose_by_deadline;
        default:
            return NULL;
    }
}

void sched_run(enum policy policy) {
    policy_fn_t *policy_fn = choose_policy_fn(policy);

    while (task_list.first != NULL) {
        task_t *chosen = task_list.first;
        task_t *next = chosen->next;

        while (next != NULL) {
            if (chosen->created_at + chosen->timeout > time)
                chosen = next;
            else if (next->created_at + next->timeout <= time)
                chosen = policy_fn(chosen, next);

            next = next->next;
        }

        if (chosen->created_at + chosen->timeout <= time) {
            current_task = chosen;

            current_task->entrypoint(current_task->ctx);

            if (current_task == task_list.first)
                task_list.first = current_task->next;
            else {
                task_t *prev = task_list.first;

                while (current_task != prev->next)
                    prev = prev->next;

                prev->next = current_task->next;
            }

            pool_free(&task_pool, current_task);
        } else {
            sched_time_elapsed(1);
        }

    }
}
