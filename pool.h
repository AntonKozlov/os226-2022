#pragma once
#include "util.h"
#include <stdint.h>

struct pool {
    void *memStart;
    void* nextFreeBlock;
    uint32_t blockSize;
    uint32_t blockAmount;
    uint32_t freeBlockAmount;
    uint32_t initBlockAmount;
};

#define POOL_INITIALIZER(_mem, _nmemb, _membsz) { \
    _mem,                                         \
    _mem,                                         \
    _membsz,                                      \
    _nmemb,                                       \
    _nmemb,                                       \
    0,                                            \
}

#define POOL_INITIALIZER_ARRAY(_array) \
	POOL_INITIALIZER(_array, ARRAY_SIZE(_array), sizeof((_array)[0]));

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz);

void *pool_alloc(struct pool *p);

void pool_free(struct pool *p, void *ptr);