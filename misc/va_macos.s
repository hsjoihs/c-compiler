//gen_prologue(304, "debug_write")
.global _debug_write
_debug_write:
  pushq %rbp
  movq %rsp, %rbp
  subq $304, %rsp
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
//gen_push_address_of_global("__stack_chk_guard");
  subq $8, %rsp
  movq ___stack_chk_guard@GOTPCREL(%rip), %rax
  movq %rax, (%rsp)
//gen_peek_and_dereference_8byte()
  movq (%rsp), %rax 
  movq (%rax), %rax
  movq  %rax, (%rsp)
//gen_write_to_local_8byte(-200)
  movq (%rsp), %rax
  movq %rax, -200(%rbp)
//gen_discard()
  addq $8, %rsp
//gen_push_address_of_local(-224);
  subq $8, %rsp
  leaq -224(%rbp), %rax
  movq %rax, (%rsp)
  movq (%rsp), %rdx
  movl $8,  (%rdx)
  movl $48,  4(%rdx)
  leaq 16(%rbp), %rax
  movq %rax, 8(%rdx)
  leaq -192(%rbp), %rax
  movq %rax, 16(%rdx)
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
//gen_push_address_of_local(-224);
  subq $8, %rsp
  leaq -224(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rdx")
  movq (%rsp), %rdx
  addq $8, %rsp
//gen_push_from_local_8byte(-232)
  subq $8, %rsp
  movq -232(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rsi")
  movq (%rsp), %rsi
  addq $8, %rsp
  call _vfprintf
//gen_push_address_of_local(-224);
  subq $8, %rsp
  leaq -224(%rbp), %rax
  movq %rax, (%rsp)
  movq (%rsp), %rdx
  movl $8,  (%rdx)
  movl $48,  4(%rdx)
  leaq 16(%rbp), %rax
  movq %rax, 8(%rdx)
  leaq -192(%rbp), %rax
  movq %rax, 16(%rdx)
//gen_discard()
  addq $8, %rsp
//gen_push_address_of_local(-224);
  subq $8, %rsp
  leaq -224(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rsi")
  movq (%rsp), %rsi
  addq $8, %rsp
//gen_push_from_local_8byte(-232)
  subq $8, %rsp
  movq -232(%rbp), %rax
  movq %rax, (%rsp)
//gen_pop_to_reg_8byte("rdi")
  movq (%rsp), %rdi
  addq $8, %rsp
  call _vprintf
//gen_push_int(123)
  subq $8, %rsp
  movl $123, (%rsp)
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
  cmpq -200(%rbp), %rax
  jne .L6
//gen_epilogue(5421)
.L5421:  movl (%rsp), %eax
  leave
  ret
.L6:
  call ___stack_chk_fail
