
.text
.global tramptramp, exittramp

tramptramp:
	pushfq
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
	push %rbp
	/* FIXME vfp, xmm ...*/

	mov %rsp, %rdi
	mov 18*8(%rsp), %rsi
	mov 17*8(%rsp), %rsp
	call *16*8(%rdi)
	mov %rax, %rsp

exittramp:

	pop %rbp
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax
	popfq

	mov 3*8(%rsp), %rsp
	jmp *-(128/*red_zone*/ + 8/*rip push*/)(%rsp)
