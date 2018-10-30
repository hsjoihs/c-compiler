//gen_prologue(0, "debug_write")
.global _debug_write
_debug_write:
  pushq %rbp
  movq %rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$216, %rsp
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
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -48(%rbp)
	leaq	-256(%rbp), %r15
	movq	%r15, -64(%rbp)
	leaq	16(%rbp), %r12
	movq	%r12, -72(%rbp)
	movabsq	$206158430216, %r13     ## imm = 0x3000000008
	movq	%r13, -80(%rbp)
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
