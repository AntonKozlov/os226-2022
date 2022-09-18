
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
	p->blocks_per_pool = nmemb;
	p->block_size = membsz;
	p->mem = (struct block *) (mem);

	struct block *current_block = (struct block *)(mem);
	for (int i = 1; i < nmemb; ++i)
	{
		current_block->next = (struct block *)((char *)current_block + membsz);
		current_block = current_block->next;
	}
	current_block->next = NULL;
}

void *pool_alloc(struct pool *p) {
	if (p->mem == NULL) return NULL;

	void *free_block = p->mem;
	p->mem = p->mem->next;

	return free_block;
}

void pool_free(struct pool *p, void *ptr) {
	if (ptr) {
		((struct block *)ptr)->next = p->mem;
		p->mem = (struct block *)ptr;
	}
}
