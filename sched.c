#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "sched.h"
#include "pool.h"

static int time;
task *all_tasks_list;
task *current;

void exec_task(task *t) {
    t->entrypoint(t->ctx);
    task *c = all_tasks_list;
    while (c != t) c = (task *) (c->next);
    if (c->prev == NULL) {
        all_tasks_list = c->next;
        if (all_tasks_list != NULL)
            all_tasks_list->prev = NULL;
    } else {
        c->prev->next = c->next;
        if (c->next != NULL)
            c->next->prev = c->prev;
    }
    free(c);
}

void exec_fifo() {
    task *tmp = NULL;
    while (all_tasks_list != NULL) {
        current = all_tasks_list;
        while (current != NULL) {
            tmp = current->next;
            if (current->timeout_till <= time)
                exec_task(current);
            current = tmp;
        }
    }
}

int convert_dd(int dd) {
    if (dd > 0)
        return dd;
    return INT_MAX;
}

task *find_suitable_deadline_task() {
    // Find task with the best deadline
    task *best_ddln = NULL;
    task *c = all_tasks_list;
    while (c != NULL) {
        if ((c->timeout_till <= time) &&
            (best_ddln == NULL || convert_dd(c->deadline) < convert_dd(best_ddln->deadline)))
            best_ddln = c;
        c = c->next;
    }
    return best_ddln;
}

task *find_suitable_prio_task() {
    // Find task with the best priority
    task *best_prio = NULL;
    task *c = all_tasks_list;
    while (c != NULL) {
        if ((c->timeout_till <= time) && (best_prio == NULL || c->priority > best_prio->priority)) best_prio = c;
        c = c->next;
    }
    return best_prio;
}

void exec_prio() {
    while ((current = find_suitable_prio_task()) != NULL)
        exec_task(current);
}

void exec_deadline() {
    while ((current = find_suitable_deadline_task()) != NULL) exec_task(current);
}

void sched_new_w_timeout(void (*entrypoint)(void *aspace),
                         void *aspace,
                         int priority,
                         int deadline,
                         int timeout) {
    if (all_tasks_list != NULL) {
        task *latest = all_tasks_list;
        while (latest->next != NULL)
            latest = latest->next;
        latest->next = (task *) malloc(sizeof(task));
        latest->next->priority = priority;
        latest->next->deadline = deadline;
        latest->next->entrypoint = entrypoint;
        latest->next->ctx = aspace;
        latest->next->timeout_till = timeout;
        latest->next->prev = latest;
        latest->next->next = NULL;
    } else {
        all_tasks_list = (task *) malloc(sizeof(task));
        all_tasks_list->priority = priority;
        all_tasks_list->deadline = deadline;
        all_tasks_list->entrypoint = entrypoint;
        all_tasks_list->ctx = aspace;
        all_tasks_list->prev = NULL;
        all_tasks_list->next = NULL;
        all_tasks_list->timeout_till = timeout;
    }
}

void sched_new(void (*entrypoint)(void *aspace), void *aspace, int priority, int deadline) {
    sched_new_w_timeout(entrypoint, aspace, priority, deadline, 0);
}

void sched_cont(void (*entrypoint)(void *aspace),
                void *aspace,
                int timeout) {
    sched_new_w_timeout(entrypoint, aspace, current->priority, current->deadline, timeout + time);
}

void sched_time_elapsed(unsigned amount) {
    time += amount;
}

void sched_run(enum policy policy) {
    time = 0;
    switch (policy) {
        case POLICY_FIFO:
            exec_fifo();
            return;
        case POLICY_PRIO:
            exec_prio();
            return;
        case POLICY_DEADLINE:
            exec_deadline();
            return;
    }
}
