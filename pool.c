
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool* p, void* mem, unsigned long nmemb, unsigned long membsz) {
	p->mem = mem;
	p->membsz = membsz;
	p->free_start = mem;
	p->free_end = p->mem + membsz * nmemb;
	p->free = NULL;
}

void* pool_alloc(struct pool* p) {
	struct free_block* fb = p->free;
	if (fb) {
		p->free = fb->next;
		return fb;
	}
	if (p->free_start < p->free_end) {
		void* res = p->free_start;
		p->free_start += p->membsz;
		return res;
	}
	return NULL;
}

void pool_free(struct pool* p, void* ptr) {
	struct free_block* f = ptr;
	f->next = p->free;
	p->free = f;
}