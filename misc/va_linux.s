	.globl	debug_write
debug_write:
	pushq	%rbp
	pushq	%rbx
	movq	%rdi, %rbx
	subq	$216, %rsp
	testb	%al, %al
	movq	%rsi, 40(%rsp)
	movq	%rdx, 48(%rsp)
	movq	%rcx, 56(%rsp)
	movq	%r8, 64(%rsp)
	movq	%r9, 72(%rsp)
	je	.L2
	movaps	%xmm0, 80(%rsp)
	movaps	%xmm1, 96(%rsp)
	movaps	%xmm2, 112(%rsp)
	movaps	%xmm3, 128(%rsp)
	movaps	%xmm4, 144(%rsp)
	movaps	%xmm5, 160(%rsp)
	movaps	%xmm6, 176(%rsp)
	movaps	%xmm7, 192(%rsp)
.L2:
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	leaq	240(%rsp), %rax
	movq	%rsp, %rbp
	movq	%rbx, %rdx
	movq	%rbp, %rcx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	movl	$8, (%rsp)
	movl	$48, 4(%rsp)
	movq	%rax, 16(%rsp)
	call	__vfprintf_chk@PLT
	leaq	240(%rsp), %rax
	movq	stdout(%rip), %rdi
	movq	%rbp, %rcx
	movq	%rbx, %rdx
	movl	$1, %esi
	movl	$8, (%rsp)
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	movl	$48, 4(%rsp)
	movq	%rax, 16(%rsp)
	call	__vfprintf_chk@PLT
	movq	24(%rsp), %rax
	xorq	%fs:40, %rax
	jne	.L6
	addq	$216, %rsp
	popq	%rbx
	popq	%rbp
	ret
.L6:
	call	__stack_chk_fail@PLT
