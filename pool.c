
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
	p->mem = mem;
	p->membsz = membsz;
	p->free_start = (char *)mem;
	p->free_end = (char *)mem + nmemb * membsz;
	p->free_pool_block = NULL;

}

void *pool_alloc(struct pool *p) {
	struct free_block* f_block = p->free_pool_block;
	if (f_block)
	{
		p->free_pool_block = f_block->next_block;
		return f_block;
	}
	if (p->free_start + p->membsz <= p->free_end)
	{
		void* pointer = p->free_start;
		p->free_start += p->membsz;
		return pointer;
	}
	return NULL;
}

void pool_free(struct pool *p, void *ptr) {
	if (ptr)
	{
		struct free_block* f_block = ptr;
		f_block->next_block = p->free_pool_block;
		p->free_pool_block = f_block;
	}
	return;
}
