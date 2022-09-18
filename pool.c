
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
	p->mem = mem;
	p->membsz = membsz;
	p->free_start = mem;
	p->free_end = p->mem + membsz * nmemb;
	p->free_block = NULL;
}

void *pool_alloc(struct pool *p) {
	struct free_pool_block *fb = p->free_block;
	if (fb) {
		p->free_block = fb->next;
		return fb;
	}
	if (p->free_start < p->free_end) {
		void* ptr = p->free_start;
		p->free_start += p->membsz;
		return ptr;
	}
	return NULL;
}

void pool_free(struct pool *p, void *ptr) {
	if (ptr == NULL)
		return;
	struct free_pool_block *fb = ptr;
	fb->next = p->free_block;
	p->free_block = fb;
}
