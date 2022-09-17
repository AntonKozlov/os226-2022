#include <stddef.h>
#include "pool.h"
#define ull unsigned long long

void pool_init(struct pool* p, void* mem, unsigned long nmemb, unsigned long membsz)
{

}

void* pool_alloc(struct pool* p)
{
    if (p->freeblocks == 0)
    {
        return NULL;
    }

    // TODO уменьшать freeblocks
    if (p->initialized_blocks < p->numblk)
    {
        ull* var = ((ull*)((ull)p->mem_begin + p->initialized_blocks * p->memblksz + p->memblksz));
        *((ull*)((ull)p->mem_begin + p->initialized_blocks * p->memblksz)) = var;
        p->initialized_blocks++;
    }

    if (p->freeblocks == 0)
    {
        p->ptr = NULL;
        return NULL;
    }
    else
    {
        void* tmpptr = p->ptr;
        p->ptr = (void*)(*(ull*)p->ptr);
        p->freeblocks--;
        return  tmpptr;
    }
}

void pool_free(struct pool* p, void* ptr)
{
    *(unsigned long long*)ptr = (unsigned long long)p->ptr;
    p->ptr = ptr;
    p->freeblocks += 1;
}