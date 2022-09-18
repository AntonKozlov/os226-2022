
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
    p->mem = mem;
    p->membsz = membsz;
    p->free_start = mem;
    p->free_end = p->mem + nmemb * membsz;
    p->free = NULL;
}

void *pool_alloc(struct pool *p) {
    struct pool_free_block *free_block = p->free;
    if (free_block){
        p->free = free_block->next;
        return free_block;
    }

    if (p->free_start < p->free_end) {
        void *r = p->free_start;
        p->free_start += p->membsz;
        return r;
    }
    return NULL;
}

void pool_free(struct pool *p, void *ptr) {
    if (ptr) {
        struct pool_free_block* free_block = ptr;
        free_block->next = p->free;
        p->free = free_block;
    }

    return;
}
