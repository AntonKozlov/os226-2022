#pragma once
#include "util.h"


struct pool
{
    void* mem_begin;
    int freeblocks;
    int numblk;
    int memblksz;
    void* ptr;
    int initialized_blocks;
};

#define POOL_INITIALIZER(_mem, _nmemb, _membsz) \
{\
    _mem,                                            \
    _nmemb,\
    _nmemb,\
    _membsz,\
    _mem,\
    0,\
}

#define POOL_INITIALIZER_ARRAY(_array) \
    POOL_INITIALIZER(_array, ARRAY_SIZE(_array), sizeof((_array)[0]));

void pool_init(struct pool* p, void* mem, unsigned long nmemb, unsigned long membsz);

void* pool_alloc(struct pool* p);

void pool_free(struct pool* p, void* ptr);
