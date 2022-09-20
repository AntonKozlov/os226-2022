
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
<<<<<<< HEAD
	p->mem = mem;
	p->membsz = membsz;
	p->freestart = mem;
	p->freeend = p->freestart + nmemb * membsz;
	p->freehead = NULL;
}

void *pool_alloc(struct pool *p) {
	if (p->freestart < p->freeend) {
		void *r = p->freestart;
		p->freestart += p->membsz;
		return r;
	}

	struct pool_free_block *fb = p->freehead;
	if (fb) {
		p->freehead = fb->next;
		return fb;
	}
=======
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
>>>>>>> a3d0a72 (Implement pool allocator)

	return NULL;
}

void pool_free(struct pool *p, void *ptr) {
<<<<<<< HEAD
	struct pool_free_block *fb = ptr;
	fb->next = p->freehead;
	p->freehead = fb;
=======
    if (ptr == NULL) {
        return;
    }

    void *tmp_next = p->free_block;
    p->free_block = ptr;
    p->free_block->next = tmp_next;
>>>>>>> a3d0a72 (Implement pool allocator)
}
