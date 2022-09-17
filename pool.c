
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
}

void* pool_alloc(struct pool *p) {
	int first_free = ffsll(~p->bit_scale) - 1;
	if (first_free < p->count && first_free != -1) {
		p->bit_scale = (1 << first_free) | p->bit_scale;
		return p->mem_begin + p->size * first_free;
	}
	return NULL;
}

void pool_free(struct pool *p, void *ptr) {
	p->bit_scale = ~(1 << ((ptr - p->mem_begin) / p->size)) & p->bit_scale;
}
