#pragma once

#include "util.h"

struct pool 
{
	char* memory;
	char* memory_end;
	int count_of_blocks;
	unsigned long size_of_block;
	unsigned long size_of_memory;
	unsigned int bit_scale;
};

#define POOL_INITIALIZER(_mem, _nmemb, _membsz) { \
    _mem, \
    (char*)(_mem) + (_nmemb) * (_membsz), \
	_nmemb, \
	_membsz, \
	_membsz, \
    0 \
}


#define POOL_INITIALIZER_ARRAY(_array) \
	POOL_INITIALIZER(_array, ARRAY_SIZE(_array), sizeof((_array)[0]))

void pool_init(struct pool* p, void* mem, unsigned long nmemb, unsigned long membsz);

void* pool_alloc(struct pool* p);

void pool_free(struct pool* p, void* ptr);