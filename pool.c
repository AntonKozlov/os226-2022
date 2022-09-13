
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
    p->memory = mem;
    p->member_size = membsz;
    p->free_start = mem;
    p->free_end = p->memory + nmemb * membsz;
    p->free_block = NULL;
}

void *pool_alloc(struct pool *p) {
    struct pool_block *free_block = p->free_block;
    if (free_block) {
        p->free_block = free_block->next;
        return free_block;
    }

    if (p->free_start < p->free_end) {
        void *result = p->free_start;
        p->free_start += p->member_size;
        return result;
    }

	return NULL;
}

void pool_free(struct pool *p, void *ptr) {
    if (ptr == NULL) {
        return;
    }

    void *tmp_next = p->free_block;
    p->free_block = ptr;
    p->free_block->next = tmp_next;
}
