
#include <stddef.h>

#include "pool.h"


struct block {
    void *next;
};


int pool_init(struct pool *p, void *mem, unsigned long nmemb, unsigned long membsz) {

	if(nmemb < sizeof(unsigned long)) {
        return -1;
    }
    p->nmemb = nmemb;
    p->ul_membsz = membsz;
    p->ptr = NULL;
    p->ul_ptr = mem;

    return 0;
}

void *pool_alloc(struct pool *p) {

    if(p->ul_membsz > 0) {
        p->ul_membsz--;
        void *ptr = p->ul_ptr;
        p->ul_ptr = (void *) (((unsigned char *) p->ul_ptr) + p->nmemb);
        return ptr;
    } else if(p->ptr) {
        void *ptr = p->ptr;
        p->ptr = ((struct block *) p->ptr)->next;
        return ptr;
    }

    return NULL;
}



void pool_free(struct pool *p, void *ptr) {
	
    ((struct block *) ptr)->next = p->ptr;
    p->ptr = ptr;
}
