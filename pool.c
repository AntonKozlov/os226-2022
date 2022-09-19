
#include <stddef.h>

#include "pool.h"

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) 
{
	p->mem = mem;
    p->membsz = membsz;
    p->freestart = (char*)mem;
    p->freeend = (char*)mem + nmemb * membsz;
    p->free = NULL;
}

void *pool_alloc(struct pool *p) 
{
	struct PoolBlock *FreeBlock = p->free;
    if (FreeBlock) 
	{
        p->free = FreeBlock->next;
        return FreeBlock;
    }

    if (p->freestart < p->freeend) 
	{
        void *result = p->freestart;
        p->freestart += p->membsz;
        return result;
    }
	return NULL;
}

void pool_free(struct pool *p, void *ptr) 
{
	if (ptr)
	{
		struct PoolBlock *FreeBlock = ptr;
		FreeBlock->next = p->free;
		p->free = FreeBlock;
	}
}
