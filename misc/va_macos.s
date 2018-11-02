//gen_prologue(304, "debug_write")
.global _debug_write
_debug_write:
  pushq %rbp
  movq %rsp, %rbp
  subq $304, %rsp
//gen_write_register_to_local_8byte("rdi", -264)
  movq %rdi, -264(%rbp)
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
//gen_push_int(8)
  subq $8, %rsp
  movl $8, (%rsp)
//gen_write_to_local(-80)
  movl (%rsp), %eax
  movl %eax, -80(%rbp)
//gen_discard()
  addq $8, %rsp
//gen_push_int(48)
  subq $8, %rsp
  movl $48, (%rsp)
//gen_write_to_local(-76)
  movl (%rsp), %eax
  movl %eax, -76(%rbp)
//gen_discard()
  addq $8, %rsp
  subq $8, %rsp
  leaq 16(%rbp), %rax
  movq %rax, (%rsp)
//gen_write_to_local_8byte(-72)
  movq (%rsp), %rax
  movq %rax, -72(%rbp)
//gen_discard()
  addq $8, %rsp
//gen_push_address_of_local(-256);
  subq $8, %rsp
  leaq -256(%rbp), %rax
  movq %rax, (%rsp)
//gen_write_to_local_8byte(-64)
  movq (%rsp), %rax
  movq %rax, -64(%rbp)
//gen_discard()
  addq $8, %rsp
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
//gen_push_address_of_local(-80);
  subq $8, %rsp
  leaq -80(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rdx")
  movq (%rsp), %rdx
  addq $8, %rsp
  movq -264(%rbp), %rsi
	callq	_vfprintf
//gen_push_int(8)
  subq $8, %rsp
  movl $8, (%rsp)
//gen_write_to_local(-80)
  movl (%rsp), %eax
  movl %eax, -80(%rbp)
//gen_discard()
  addq $8, %rsp
//gen_push_int(48)
  subq $8, %rsp
  movl $48, (%rsp)
//gen_write_to_local(-76)
  movl (%rsp), %eax
  movl %eax, -76(%rbp)
//gen_discard()
  addq $8, %rsp
  subq $8, %rsp
  leaq 16(%rbp), %rax
  movq %rax, (%rsp)
//gen_write_to_local_8byte(-72)
  movq (%rsp), %rax
  movq %rax, -72(%rbp)
//gen_discard()
  addq $8, %rsp
//gen_push_address_of_local(-256);
  subq $8, %rsp
  leaq -256(%rbp), %rax
  movq %rax, (%rsp)
//gen_write_to_local_8byte(-64)
  movq (%rsp), %rax
  movq %rax, -64(%rbp)
//gen_discard()
  addq $8, %rsp
  movq -264(%rbp), %rdi
//gen_push_address_of_local(-80);
  subq $8, %rsp
  leaq -80(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rsi")
  movq (%rsp), %rsi
  addq $8, %rsp
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

//gen_push_int(3)
  subq $8, %rsp
  movl $3, (%rsp)
//gen_epilogue(2314)
.L2314:  movl (%rsp), %eax
  leave
  ret
LBB0_4:
	callq	___stack_chk_fail
