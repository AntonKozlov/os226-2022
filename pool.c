
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
	p->untraced_memory = (char *)mem;
	p->block_size = membsz;
	p->untraced_block_number = nmemb;
	p->free = NULL;
}

void *pool_alloc(struct pool *p) {
	if (p->untraced_block_number > 0) {
		p->untraced_block_number--;
		void *ref_to_user_mem = p->untraced_memory;
		p->untraced_memory = p->untraced_memory + p->block_size;
		return ref_to_user_mem;
	} else if (p->free) {
		void *ref_to_user_mem = p->free;
		p->free = ((struct pool_free_block *)p->free)->next;
		return ref_to_user_mem;
	} else {
		return NULL;
	}
}

void pool_free(struct pool *p, void *ptr) {
	((struct pool_free_block *)ptr)->next = p->free;
	p->free = ptr;
}
