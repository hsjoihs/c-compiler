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
	movq	%rax, -200(%rbp)
	leaq	16(%rbp), %rax
	movq	%rsp, %rcx
	movq	%rbx, %rdx
	movl	$1, %esi
	movq	%rax, -216(%rbp)

//gen_push_address_of_global("stderr");
  subq $8, %rsp
  leaq stderr(%rip), %rax
  movq %rax, (%rsp)
//gen_peek_and_dereference_8byte()
  movq (%rsp), %rax 
  movq (%rax), %rax
  movq  %rax, (%rsp)
//gen_pop_to_reg_8byte("rdi")
  movq (%rsp), %rdi
  addq $8, %rsp
	leaq	-192(%rbp), %rax
	movq	%rax, -208(%rbp)
	movl	$8, -224(%rbp)
	movl	$48, -220(%rbp)
	call	__vfprintf_chk@PLT
	leaq	16(%rbp), %rax
	movq	%rax, -216(%rbp)
	leaq	stdout(%rip), %rax
  movq	(%rax), %rdi
	movq	%rsp, %rcx
	movq	%rbx, %rdx
	movl	$1, %esi
	movl	$8, -224(%rbp)
	leaq	-192(%rbp), %rax
	movl	$48, -220(%rbp)
	movq	%rax, -208(%rbp)
	call	__vfprintf_chk@PLT
	movq	-200(%rbp), %rax
	xorq	%fs:40, %rax
	jne	.L6
	addq	$216, %rsp
	popq	%rbx
	popq	%rbp
	ret
.L6:
	call	__stack_chk_fail@PLT
