//gen_prologue(0, "debug_write")
.global debug_write
debug_write:
  pushq %rbp
  movq %rsp, %rbp
	pushq	%rbx
	movq	%rdi, %rbx
	subq	$216, %rsp

  testb %al, %al
  movq %rsi, -184(%rbp)
  movq %rdx, -176(%rbp)
  movq %rcx, -168(%rbp)
  movq %r8, -160(%rbp)
  movq %r9, -152(%rbp)
  je LBB0_2
  movaps %xmm0, -144(%rbp)
  movaps %xmm1, -128(%rbp)
  movaps %xmm2, -112(%rbp)
  movaps %xmm3, -96(%rbp)
  movaps %xmm4, -80(%rbp)
  movaps %xmm5, -64(%rbp)
  movaps %xmm6, -48(%rbp)
  movaps %xmm7, -32(%rbp)
LBB0_2:
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	leaq	240(%rsp), %rax
	movq	%rsp, %rbp
	movq	%rbx, %rdx
	movq	%rbp, %rcx
	movl	$1, %esi
	movq	%rax, 8(%rsp)

//gen_push_address_of_global("stderr");
  subq $8, %rsp
  leaq stderr(%rip), %rax
  movq %rax, (%rsp)
  movq  (%rsp), %rax
  addq    $8, %rsp
	movq	(%rax), %rdi
	leaq	32(%rsp), %rax
	movl	$8, (%rsp)
	movl	$48, 4(%rsp)
	movq	%rax, 16(%rsp)
	call	__vfprintf_chk@PLT
	leaq	240(%rsp), %rax
	movq	%rax, 8(%rsp)
	leaq	stdout(%rip), %rax
  movq	(%rax), %rdi
	movq	%rbp, %rcx
	movq	%rbx, %rdx
	movl	$1, %esi
	movl	$8, (%rsp)
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
