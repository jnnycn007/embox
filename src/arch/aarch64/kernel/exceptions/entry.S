/**
 * @file
 * @brief
 *
 * @date 17.07.2019
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

/* Exception frame:
 *
 *   [sp                              ] x0-x29, lr, sp, elr_el1, spsr_el1
 *   [sp + EXCPT_GPRS                 ] q0-q31
 *   [sp + EXCPT_GPRS + EXCPT_Q       ] fpsr, fpcr
 *
 * The GPR part is struct excpt_context and keeps its layout. */
#define EXCPT_GPRS  (8 * 34)
#define EXCPT_Q     (16 * 32)
#define EXCPT_SYS   16
#define EXCPT_FRAME (EXCPT_GPRS + EXCPT_Q + EXCPT_SYS)

/* All vectors are 128 byte aligned and limited to 32 instructions, which
 * is not enough for the FPSIMD part, so a vector only stores what it needs
 * to pass on (the EL it was taken from and the handler) and branches to the
 * common entry code. */
.macro excpt_vector el:req fn:req
.align 7
	sub     sp, sp, #EXCPT_FRAME
	stp     x0, x1, [sp, #(8 * 0)]
	stp     x2, x3, [sp, #(8 * 2)]
	mov     x0, #\el
	ldr     x1, =\fn
	b       excpt_entry_common
.endm

.section .text.kernel_vectors
.global aarch64_kernel_vectors

/* Vector tables must be placed at a 2KB-aligned address */
.align 11
aarch64_kernel_vectors:
	/* EL1t */
	excpt_vector 1, aarch64_sync_handler
	excpt_vector 1, aarch64_irq_handler
	excpt_vector 1, aarch64_fiq_handler
	excpt_vector 1, aarch64_serror_handler
	/* EL1h */
	excpt_vector 1, aarch64_sync_handler
	excpt_vector 1, aarch64_irq_handler
	excpt_vector 1, aarch64_fiq_handler
	excpt_vector 1, aarch64_serror_handler
	/* EL0 64-bit */
	excpt_vector 0, aarch64_sync_handler
	excpt_vector 0, aarch64_irq_handler
	excpt_vector 0, aarch64_fiq_handler
	excpt_vector 0, aarch64_serror_handler
	/* EL0 32-bit */
	excpt_vector 0, aarch64_sync_handler
	excpt_vector 0, aarch64_irq_handler
	excpt_vector 0, aarch64_fiq_handler
	excpt_vector 0, aarch64_serror_handler

/* x0 = source EL, x1 = handler, x0-x3 already stored */
excpt_entry_common:
	/* Store x4-x29 regs */
	stp     x4, x5, [sp, #(8 * 4)]
	stp     x6, x7, [sp, #(8 * 6)]
	stp     x8, x9, [sp, #(8 * 8)]
	stp     x10, x11, [sp, #(8 * 10)]
	stp     x12, x13, [sp, #(8 * 12)]
	stp     x14, x15, [sp, #(8 * 14)]
	stp     x16, x17, [sp, #(8 * 16)]
	stp     x18, x19, [sp, #(8 * 18)]
	stp     x20, x21, [sp, #(8 * 20)]
	stp     x22, x23, [sp, #(8 * 22)]
	stp     x24, x25, [sp, #(8 * 24)]
	stp     x26, x27, [sp, #(8 * 26)]
	stp     x28, x29, [sp, #(8 * 28)]
#ifdef __ARM_NEON
	/* Store q0-q31 */
	add     x2, sp, #EXCPT_GPRS
	stp     q0, q1, [x2, #0]
	stp     q2, q3, [x2, #32]
	stp     q4, q5, [x2, #64]
	stp     q6, q7, [x2, #96]
	stp     q8, q9, [x2, #128]
	stp     q10, q11, [x2, #160]
	stp     q12, q13, [x2, #192]
	stp     q14, q15, [x2, #224]
	stp     q16, q17, [x2, #256]
	stp     q18, q19, [x2, #288]
	stp     q20, q21, [x2, #320]
	stp     q22, q23, [x2, #352]
	stp     q24, q25, [x2, #384]
	stp     q26, q27, [x2, #416]
	stp     q28, q29, [x2, #448]
	stp     q30, q31, [x2, #480]
	/* Store fpsr, fpcr */
	mrs     x3, fpsr
	mrs     x4, fpcr
	add     x5, sp, #(EXCPT_GPRS + EXCPT_Q)
	stp     x3, x4, [x5]
#endif
	/* Store lr, sp, pc, psr */
	cbz     x0, 1f
	add     x10, sp, #EXCPT_FRAME
	b       2f
1:
	mrs     x10, sp_el0
2:
	mrs     x11, elr_el1
	mrs     x12, spsr_el1
	stp     x30, x10, [sp, #(8 * 30)]
	stp     x11, x12, [sp, #(8 * 32)]
	/* Call the handler */
	mov     x2, x1
	mov     x0, sp
	ldr     x30, =excpt_exit
	br      x2

excpt_exit:
#ifdef __ARM_NEON
	/* Load q0-q31 */
	add     x2, sp, #EXCPT_GPRS
	ldp     q0, q1, [x2, #0]
	ldp     q2, q3, [x2, #32]
	ldp     q4, q5, [x2, #64]
	ldp     q6, q7, [x2, #96]
	ldp     q8, q9, [x2, #128]
	ldp     q10, q11, [x2, #160]
	ldp     q12, q13, [x2, #192]
	ldp     q14, q15, [x2, #224]
	ldp     q16, q17, [x2, #256]
	ldp     q18, q19, [x2, #288]
	ldp     q20, q21, [x2, #320]
	ldp     q22, q23, [x2, #352]
	ldp     q24, q25, [x2, #384]
	ldp     q26, q27, [x2, #416]
	ldp     q28, q29, [x2, #448]
	ldp     q30, q31, [x2, #480]
	/* Load fpsr, fpcr */
	add     x5, sp, #(EXCPT_GPRS + EXCPT_Q)
	ldp     x3, x4, [x5]
	msr     fpsr, x3
	msr     fpcr, x4
#endif
	/* Load lr, pc, psr */
	ldp     x30, x10, [sp, #(8 * 30)]
	ldp     x11, x12, [sp, #(8 * 32)]
	msr     elr_el1, x11
	msr     spsr_el1, x12
	/* Load x0-x29 regs */
	ldp     x0, x1, [sp, #(8 * 0)]
	ldp     x2, x3, [sp, #(8 * 2)]
	ldp     x4, x5, [sp, #(8 * 4)]
	ldp     x6, x7, [sp, #(8 * 6)]
	ldp     x8, x9, [sp, #(8 * 8)]
	ldp     x10, x11, [sp, #(8 * 10)]
	ldp     x12, x13, [sp, #(8 * 12)]
	ldp     x14, x15, [sp, #(8 * 14)]
	ldp     x16, x17, [sp, #(8 * 16)]
	ldp     x18, x19, [sp, #(8 * 18)]
	ldp     x20, x21, [sp, #(8 * 20)]
	ldp     x22, x23, [sp, #(8 * 22)]
	ldp     x24, x25, [sp, #(8 * 24)]
	ldp     x26, x27, [sp, #(8 * 26)]
	ldp     x28, x29, [sp, #(8 * 28)]
	/* Return from exception */
	add     sp, sp, #EXCPT_FRAME
	eret
