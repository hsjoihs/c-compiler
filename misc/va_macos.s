	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 13
	.globl	_debug_write            ## -- Begin function debug_write
	.p2align	4, 0x90
_debug_write:                           ## @debug_write
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$216, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rdi, %rbx
	testb	%al, %al
	je	LBB0_2
## %bb.1:
	movaps	%xmm0, -208(%rbp)
	movaps	%xmm1, -192(%rbp)
	movaps	%xmm2, -176(%rbp)
	movaps	%xmm3, -160(%rbp)
	movaps	%xmm4, -144(%rbp)
	movaps	%xmm5, -128(%rbp)
	movaps	%xmm6, -112(%rbp)
	movaps	%xmm7, -96(%rbp)
LBB0_2:
	movq	%r9, -216(%rbp)
	movq	%r8, -224(%rbp)
	movq	%rcx, -232(%rbp)
	movq	%rdx, -240(%rbp)
	movq	%rsi, -248(%rbp)
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -48(%rbp)
	leaq	-256(%rbp), %r15
	movq	%r15, -64(%rbp)
	leaq	16(%rbp), %r12
	movq	%r12, -72(%rbp)
	movabsq	$206158430216, %r13     ## imm = 0x3000000008
	movq	%r13, -80(%rbp)
	movq	___stderrp@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	leaq	-80(%rbp), %r14
	movq	%rbx, %rsi
	movq	%r14, %rdx
	callq	_vfprintf
	movq	%r15, -64(%rbp)
	movq	%r12, -72(%rbp)
	movq	%r13, -80(%rbp)
	movq	%rbx, %rdi
	movq	%r14, %rsi
	callq	_vprintf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	cmpq	-48(%rbp), %rax
	jne	LBB0_4
## %bb.3:
	addq	$216, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB0_4:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function

.subsections_via_symbols
