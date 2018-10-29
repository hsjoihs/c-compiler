	.file	"va.c"
	.text
	.p2align 4,,15
	.globl	debug_write
	.type	debug_write, @function
debug_write:
.LFB23:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	subq	$216, %rsp
	.cfi_def_cfa_offset 240
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
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L6:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE23:
	.size	debug_write, .-debug_write
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
