#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "pool.h"

static int time = 0;
static task *tasks = NULL;
static task *current_task = NULL;

void delete_task(task *task_) {
    task *curr = tasks;
    if (curr == task_) {
        tasks = curr->next;
        free(curr);
        return;
    }
    task *prev = NULL;
    while (curr->next && curr != task_) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == task_) {
        prev->next = curr->next;
        free(curr);
    }
}

void policy_fifo() {
    while (tasks) {
        current_task = tasks;
        while (current_task) {
            if (current_task->timeout <= time) {
                current_task->entrypoint(current_task->context);
                task *done = current_task;
                current_task = current_task->next;
                delete_task(done);
            }
        }
    }
}

task *max_priority() {
    if (!tasks) {
        return NULL;
    }
    task *curr = tasks;
    task *best = NULL;
    while (curr) {
        if ((!best || curr->priority > best->priority) && curr->timeout <= time) {
            best = curr;
        }
        curr = curr->next;
    }
    return best;
}

void policy_prio() {
    current_task = max_priority();
    while (current_task) {
        current_task->entrypoint(current_task->context);
        delete_task(current_task);
        current_task = max_priority();
    }
}

task *min_deadline() {
    task *curr, *best;
    int min_deadline = INT_MAX;
    curr = tasks;
    best = NULL;
    while (curr) {
        if (curr->deadline >= 0 && curr->deadline < min_deadline && curr->timeout <= time) {
            best = curr;
            min_deadline = best->deadline;
        }
        curr = curr->next;
    }
    if (best) {
        return best;
    } else {
        return max_priority();
    }
}

void policy_deadline() {
    current_task = min_deadline();
    while (current_task) {
        current_task->entrypoint(current_task->context);
        delete_task(current_task);
        current_task = min_deadline();
    }
    if (tasks) {
        policy_prio();
    }
}

void sched_new(void (*entrypoint)(void *aspace),
               void *aspace,
               int priority,
               int deadline) {
    task *new_task = (task *) malloc(sizeof(task));
    new_task->next = NULL;
    new_task->entrypoint = entrypoint;
    new_task->context = aspace;
    new_task->priority = priority;
    new_task->deadline = deadline;
    new_task->timeout = -1;
    if (tasks) {
        task *curr = tasks;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = new_task;
    } else {
        tasks = new_task;
    }
}

void sched_cont(void (*entrypoint)(void *aspace),
                void *aspace,
                int timeout) {
    sched_new(entrypoint, aspace, current_task->priority, current_task->deadline);
    task *curr = tasks;
    while (curr->next) {
        curr = curr->next;
    }
    curr->timeout = timeout + time;
}

void sched_time_elapsed(unsigned amount) {
    time += amount;
}

void sched_run(enum policy policy) {
    if (!tasks) {
        return;
    }
    if (policy == POLICY_FIFO) {
        policy_fifo();
    } else if (policy == POLICY_PRIO) {
        policy_prio();
    } else if (policy == POLICY_DEADLINE) {
        policy_deadline();
    }

}

