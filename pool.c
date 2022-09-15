
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool* p, void* mem, unsigned long nmemb, unsigned long membsz) 
{
	p->memory = mem;
	p->size_of_memory = nmemb * membsz;
	p->bit_scale = 0;
	p->memory_end = (char*)mem + nmemb * membsz;
	p->size_of_block = membsz;
}

void* pool_alloc(struct pool* p) 
{
	if (p->bit_scale >= (1 << p->count_of_blocks) - 1)
	{
		return NULL;
	}

	int free_bit = 32;
	unsigned int bit_scale = ~p->bit_scale;
	for (int i = 16; i >= 1; i >>= 1)
	{
		if (bit_scale << i)
		{
			free_bit -= i;
			bit_scale <<= i;
		}
	}

	p->bit_scale += 1 << (free_bit - 1);

	return p->memory + p->size_of_block * (free_bit - 1);
}

void pool_free(struct pool* p, void* ptr)
{
	if ((char*)ptr < p->memory || (char*)ptr > p->memory_end)
	{
		return;
	}

	p->bit_scale &= ~(1 << ((char*)ptr - p->memory) / p->size_of_block);
}
