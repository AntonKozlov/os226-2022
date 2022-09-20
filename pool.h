#pragma once

#include "util.h"

<<<<<<< HEAD
struct pool_free_block {
	struct pool_free_block *next;
};

struct pool {
	char *mem;
	unsigned long membsz;
	char *freestart;
	char *freeend;
	struct pool_free_block *freehead;
};

#define POOL_INITIALIZER(_mem, _nmemb, _membsz) { \
	.mem = (char*)(_mem), \
	.membsz	= (_membsz), \
	.freehead = NULL, \
	.freestart = (char*)(_mem), \
	.freeend = (char*)(_mem) + (_nmemb) * (_membsz), \
=======
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
>>>>>>> a3d0a72 (Implement pool allocator)
}

#define POOL_INITIALIZER_ARRAY(_array) \
	POOL_INITIALIZER(_array, ARRAY_SIZE(_array), sizeof((_array)[0]));

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz);

void *pool_alloc(struct pool *p);

void pool_free(struct pool *p, void *ptr);
