//gen_prologue(240, "debug_write")
.global debug_write
debug_write:
  pushq %rbp
  movq %rsp, %rbp
  subq $240, %rsp
//gen_write_register_to_local_8byte("rdi", -232)
  movq %rdi, -232(%rbp)
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

  movl $8,  -224(%rbp)
  movl $48,  -220(%rbp)
  leaq 16(%rbp), %rax
  movq %rax, -216(%rbp)
  leaq -192(%rbp), %rax
  movq %rax, -208(%rbp)
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
  movq -232(%rbp), %rsi
//gen_push_address_of_local(-224);
  subq $8, %rsp
  leaq -224(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rdx")
  movq (%rsp), %rdx
  addq $8, %rsp
	call	vfprintf

//gen_push_address_of_global("stdout");
  subq $8, %rsp
  leaq stdout(%rip), %rax
  movq %rax, (%rsp)
//gen_peek_and_dereference_8byte()
  movq (%rsp), %rax 
  movq (%rax), %rax
  movq  %rax, (%rsp)
//gen_pop_to_reg_8byte("rdi")
  movq (%rsp), %rdi
  addq $8, %rsp
//gen_push_address_of_local(-224);
  subq $8, %rsp
  leaq -224(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rdx")
  movq (%rsp), %rdx
  addq $8, %rsp
  movq -232(%rbp), %rsi
  movl $8,  -224(%rbp)
  movl $48,  -220(%rbp)
  leaq 16(%rbp), %rax
  movq %rax, -216(%rbp)
  leaq -192(%rbp), %rax
  movq %rax, -208(%rbp)
	call	vfprintf

	movq	-200(%rbp), %rax
	cmpq	%fs:40, %rax
	jne	.L6
	leave
	ret
.L6:
	call	__stack_chk_fail
