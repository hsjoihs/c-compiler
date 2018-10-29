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
	subq	$432, %rsp              ## imm = 0x1B0
	testb	%al, %al
	movaps	%xmm7, -240(%rbp)       ## 16-byte Spill
	movaps	%xmm6, -256(%rbp)       ## 16-byte Spill
	movaps	%xmm5, -272(%rbp)       ## 16-byte Spill
	movaps	%xmm4, -288(%rbp)       ## 16-byte Spill
	movaps	%xmm3, -304(%rbp)       ## 16-byte Spill
	movaps	%xmm2, -320(%rbp)       ## 16-byte Spill
	movaps	%xmm1, -336(%rbp)       ## 16-byte Spill
	movaps	%xmm0, -352(%rbp)       ## 16-byte Spill
	movq	%rdi, -360(%rbp)        ## 8-byte Spill
	movq	%r9, -368(%rbp)         ## 8-byte Spill
	movq	%r8, -376(%rbp)         ## 8-byte Spill
	movq	%rcx, -384(%rbp)        ## 8-byte Spill
	movq	%rdx, -392(%rbp)        ## 8-byte Spill
	movq	%rsi, -400(%rbp)        ## 8-byte Spill
	je	LBB0_4
## %bb.3:
	movaps	-352(%rbp), %xmm0       ## 16-byte Reload
	movaps	%xmm0, -176(%rbp)
	movaps	-336(%rbp), %xmm1       ## 16-byte Reload
	movaps	%xmm1, -160(%rbp)
	movaps	-320(%rbp), %xmm2       ## 16-byte Reload
	movaps	%xmm2, -144(%rbp)
	movaps	-304(%rbp), %xmm3       ## 16-byte Reload
	movaps	%xmm3, -128(%rbp)
	movaps	-288(%rbp), %xmm4       ## 16-byte Reload
	movaps	%xmm4, -112(%rbp)
	movaps	-272(%rbp), %xmm5       ## 16-byte Reload
	movaps	%xmm5, -96(%rbp)
	movaps	-256(%rbp), %xmm6       ## 16-byte Reload
	movaps	%xmm6, -80(%rbp)
	movaps	-240(%rbp), %xmm7       ## 16-byte Reload
	movaps	%xmm7, -64(%rbp)
LBB0_4:
	movq	-368(%rbp), %rax        ## 8-byte Reload
	movq	%rax, -184(%rbp)
	movq	-376(%rbp), %rcx        ## 8-byte Reload
	movq	%rcx, -192(%rbp)
	movq	-384(%rbp), %rdx        ## 8-byte Reload
	movq	%rdx, -200(%rbp)
	movq	-392(%rbp), %rsi        ## 8-byte Reload
	movq	%rsi, -208(%rbp)
	movq	-400(%rbp), %rdi        ## 8-byte Reload
	movq	%rdi, -216(%rbp)
	movq	-360(%rbp), %r8         ## 8-byte Reload
	leaq	-32(%rbp), %r9
	movq	___stderrp@GOTPCREL(%rip), %r10
	movq	___stack_chk_guard@GOTPCREL(%rip), %r11
	movq	(%r11), %r11
	movq	%r11, -8(%rbp)
	movq	%r8, -40(%rbp)
	movq	%r9, %r8
	leaq	-224(%rbp), %r11
	movq	%r11, 16(%r8)
	leaq	16(%rbp), %r11
	movq	%r11, 8(%r8)
	movl	$48, 4(%r8)
	movl	$8, (%r8)
	movq	(%r10), %rdi
	movq	-40(%rbp), %rsi
	movq	%r9, %rdx
	callq	_vfprintf
	leaq	-32(%rbp), %rcx
	movq	%rcx, %rdx
	movq	%rcx, %rsi
	leaq	-224(%rbp), %rdi
	movq	%rdi, 16(%rsi)
	leaq	16(%rbp), %rdi
	movq	%rdi, 8(%rsi)
	movl	$48, 4(%rsi)
	movl	$8, (%rsi)
	movq	-40(%rbp), %rdi
	movq	%rcx, %rsi
	movl	%eax, -404(%rbp)        ## 4-byte Spill
	movq	%rdx, -416(%rbp)        ## 8-byte Spill
	callq	_vprintf
	leaq	-32(%rbp), %rcx
	movq	___stack_chk_guard@GOTPCREL(%rip), %rdx
	movq	(%rdx), %rdx
	movq	-8(%rbp), %rsi
	cmpq	%rsi, %rdx
	movl	%eax, -420(%rbp)        ## 4-byte Spill
	movq	%rcx, -432(%rbp)        ## 8-byte Spill
	jne	LBB0_2
## %bb.1:
	addq	$432, %rsp              ## imm = 0x1B0
	popq	%rbp
	retq
LBB0_2:
	callq	___stack_chk_fail
	ud2
	.cfi_endproc
                                        ## -- End function

.subsections_via_symbols
