#include <string.h>

#include "ctx.h"
#define ulong unsigned long
void ctx_make(struct ctx *ctx, void *entry, void *stack) {
        memset(ctx, 0, sizeof(*ctx));

		if(0 == (ulong) stack % 16) {
			stack -= 8;
		}
        ctx->rsp = (ulong) stack;
        ctx->rsp -= 8;
        *(ulong *)ctx->rsp = (ulong) entry;
}

