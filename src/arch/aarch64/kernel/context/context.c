/**
 * @file context.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */
#include <stdint.h>
#include <string.h>

#include <hal/context.h>
#include <hal/reg.h>

/* Space context_switch uses to spill q0-q31 below the saved SP. */
#define FPSIMD_FRAME 512

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	uintptr_t top;

	memset(ctx, 0, sizeof(*ctx));

	ctx->lr = (uint64_t)routine_fn;

	/* Reserve the FPSIMD frame the first context_switch to this context
	 * will restore from. Its contents do not matter - a thread that has
	 * never run has no live FPSIMD state - but the area has to exist and
	 * to be 16-byte aligned, since `stp q` faults otherwise, and thread
	 * stacks are only guaranteed 8-byte alignment.
	 *
	 * The frame is deliberately not zeroed: context_init() is also used
	 * for the bootstrap context, whose stack pointer is the one currently
	 * in use, and memset()ing memory below it would clobber the live
	 * frame of the caller. */
	top = (uintptr_t)sp & ~(uintptr_t)(16 - 1);
	ctx->sp = (uint64_t)(top - FPSIMD_FRAME);

	if (!(flags & CONTEXT_IRQDISABLE)) {
		ctx->daif |= DAIF_I | DAIF_F;
	}
}
