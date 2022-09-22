
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
	p->mem = mem;
	p->membsz = membsz;
	p->head = mem;
	p->tail = (char *) mem + nmemb * membsz;
	p->free = NULL;
}

void *pool_alloc(struct pool *p) {
	void *allocated_chunk = NULL;
	if (p->free) {
		allocated_chunk = p->free;
		p->free = (p->free)->next;
	} else if (p->head < p->tail) {
		allocated_chunk = p->head;
		p->head += p->membsz;
	}

	return allocated_chunk;
}

void pool_free(struct pool *p, void *ptr) {
	if (ptr) {
		void *prev_head = p->free;
		p->free = ptr;
		(p->free)->next = prev_head;
	}
}
