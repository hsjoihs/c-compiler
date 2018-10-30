//gen_prologue(0, "debug_write")
.global _debug_write
_debug_write:
  pushq %rbp
  movq %rsp, %rbp
	pushq	%rbx
	subq	$248, %rsp
	movq	%rdi, %rbx
  testb %al, %al
  movq %rsi, -248(%rbp)
  movq %rdx, -240(%rbp)
  movq %rcx, -232(%rbp)
  movq %r8, -224(%rbp)
  movq %r9, -216(%rbp)
  je LBB0_2
  movaps %xmm0, -208(%rbp)
  movaps %xmm1, -192(%rbp)
  movaps %xmm2, -176(%rbp)
  movaps %xmm3, -160(%rbp)
  movaps %xmm4, -144(%rbp)
  movaps %xmm5, -128(%rbp)
  movaps %xmm6, -112(%rbp)
  movaps %xmm7, -96(%rbp)
LBB0_2:
//gen_push_address_of_global("__stack_chk_guard");
  subq $8, %rsp
  movq ___stack_chk_guard@GOTPCREL(%rip), %rax
  movq %rax, (%rsp)
//gen_peek_and_dereference_8byte()
  movq (%rsp), %rax 
  movq (%rax), %rax
  movq  %rax, (%rsp)
//gen_write_to_local_8byte(-48)
  movq (%rsp), %rax
  movq %rax, -48(%rbp)
//gen_discard()
  addq $8, %rsp
	subq	$24, %rsp
	leaq	-256(%rbp), %rdx
	movq	%rdx, -64(%rbp)
	movq	%rdx, (%rsp)
	leaq	16(%rbp), %rdx
	movq	%rdx, -72(%rbp)
	movq	%rdx, 8(%rsp)
	movabsq	$206158430216, %rdx     ## imm = 0x3000000008
	movq	%rdx, -80(%rbp)
	movq	%rdx, 16(%rsp)

//gen_push_address_of_global("__stderrp");
  subq $8, %rsp
  movq ___stderrp@GOTPCREL(%rip), %rax
  movq %rax, (%rsp)
//gen_peek_and_dereference_8byte()
  movq (%rsp), %rax 
  movq (%rax), %rax
  movq  %rax, (%rsp)
//gen_pop_to_reg_8byte("rdi")
  movq (%rsp), %rdi
  addq $8, %rsp
	subq	$8, %rsp
	leaq	-80(%rbp), %rax
	movq	%rax, (%rsp)
	movq	(%rsp), %rdx
	movq	%rbx, %rsi
	callq	_vfprintf
	movq	8(%rsp), %rdx
	movq	%rdx, -64(%rbp)
	movq	16(%rsp), %rdx
	movq	%rdx, -72(%rbp)
	movq	24(%rsp), %rdx
	movq	%rdx, -80(%rbp)
	movq	%rbx, %rdi
	movq	(%rsp), %rsi
	addq	$32, %rsp
	callq	_vprintf
//gen_push_address_of_global("__stack_chk_guard");
  subq $8, %rsp
  movq ___stack_chk_guard@GOTPCREL(%rip), %rax
  movq %rax, (%rsp)
//gen_peek_and_dereference_8byte()
  movq (%rsp), %rax 
  movq (%rax), %rax
  movq  %rax, (%rsp)
//gen_pop_to_reg_8byte("rax")
  movq (%rsp), %rax
  addq $8, %rsp
	cmpq	-48(%rbp), %rax
	jne	LBB0_4
	addq	$248, %rsp
	popq	%rbx
	popq	%rbp
	retq
LBB0_4:
	callq	___stack_chk_fail
