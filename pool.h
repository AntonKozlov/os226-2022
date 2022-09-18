#pragma once

#include "util.h"

struct pool {
	char *mem;
	unsigned long membsz;
	char *freestart;
	char *freeend;
	struct pool_free_block 
	{
			struct pool_free_block *next;
	} *free;
};

#define POOL_INITIALIZER(_mem, _nmemb, _membsz) { \
	_mem, \
	_membsz, \
	_mem, \
	_mem + _nmemb * _membsz, \
	NULL \
}

#define POOL_INITIALIZER_ARRAY(_array) \
	POOL_INITIALIZER(_array, ARRAY_SIZE(_array), sizeof((_array)[0]));

void pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz);

void *pool_alloc(struct pool *p);

void pool_free(struct pool *p, void *ptr);
