
#include <stddef.h>
#include "pool.h"

// 1 - free
// 0 - occupied

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {
    p->memStart = mem;
    p->nextFreeBlock = mem;
    p->blockAmount = nmemb;
    p->blockSize = membsz;
    p->freeBlockAmount = nmemb;
}

void *pool_alloc(struct pool *p) {
    if(p->initBlockAmount < p->blockAmount){
        uint32_t *ptr = (uint32_t*)((uint64_t)p->memStart + p->initBlockAmount * p->blockSize);
        *ptr = p->initBlockAmount + 1;
        p->initBlockAmount++;
    }

    void* res = NULL;
    if(p->freeBlockAmount > 0){
        res = p->nextFreeBlock;
        --p->freeBlockAmount;
        if(p->freeBlockAmount != 0)
            p->nextFreeBlock = (void*)((uint64_t)p->memStart + (uint64_t)(*(uint32_t*)(p->nextFreeBlock)) * p->blockSize);
        else
            p->nextFreeBlock = NULL;
    }
    return res;
}

void pool_free(struct pool *p, void *ptr) {
    if (p->nextFreeBlock != NULL) {
        (*(uint32_t*)ptr) = (((uint32_t)((uint64_t)p->nextFreeBlock - (uint64_t)p->memStart)) / p->blockSize);
        p->nextFreeBlock = (void*)ptr;
    }
    else{
        (*(uint32_t*)ptr) = p->blockAmount;
        p->nextFreeBlock = (void*)ptr;
    }
    ++p->freeBlockAmount;
}
