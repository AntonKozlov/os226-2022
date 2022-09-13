#pragma once

#include "util.h"

struct pool {
    char *memory;
    unsigned long member_size;
    char *free_start;
    char *free_end;
    struct pool_block {
        struct pool_block *next;
    } *free_block;
};

#define POOL_INITIALIZER(_mem, _nmemb, _membsz) { \
    _mem,                                         \
    _membsz,                                      \
    _mem,                                         \
    (char*)(_mem) + (_nmemb) * (_membsz),         \
    NULL                                          \
}

#define POOL_INITIALIZER_ARRAY(_array) \
	POOL_INITIALIZER(_array, ARRAY_SIZE(_array), sizeof((_array)[0]));

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz);

void *pool_alloc(struct pool *p);

void pool_free(struct pool *p, void *ptr);
