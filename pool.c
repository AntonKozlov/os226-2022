
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
    p->mem = mem;
    p->memb_sz = membsz;
    p->free_st = mem;
    p->free_end = p->mem + nmemb * membsz;
    p->free = NULL;
}

void *pool_alloc(struct pool *p) {
    struct pool_free_block *fb = p->free;
    if (fb) {
        p->free = fb->next;
        return fb;
    }
    if (p->free_st < p->free_end) {
        void *r = p->free_st;
        p->free_st += p->memb_sz;
        return r;
    }
    return NULL;
}

void pool_free(struct pool *p, void *ptr) {
    if (ptr == NULL)
        return;

    struct pool_free_block* head = p->free;
    p->free = ptr;
    p->free->next = head;
}
