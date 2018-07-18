//print_prologue(12, main)
.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $12, %rsp
//push_int(0)
  subq $4, %rsp
  movl $0, (%rsp)
//assign to `a`
//write_to_local(-8)
  movl (%rsp), %eax
  movl %eax, -8(%rbp)
//gen_discard()
  addq $4, %rsp
//push_int(0)
  subq $4, %rsp
  movl $0, (%rsp)
//assign to `b`
//write_to_local(-12)
  movl (%rsp), %eax
  movl %eax, -12(%rbp)
//gen_discard()
  addq $4, %rsp
//gen_label(10)
.L10:
//load from `a`
//push_from_local(-8)
  subq $4, %rsp
  movl -8(%rbp), %eax
  movl %eax, (%rsp)
//push_int(1)
  subq $4, %rsp
  movl $1, (%rsp)
//before assigning to `a`:
//op_ints("subl")
  movl (%rsp), %eax
  subl %eax, +4(%rsp)
  addq $4, %rsp
//assign to `a`
//write_to_local(-8)
  movl (%rsp), %eax
  movl %eax, -8(%rbp)
//gen_discard()
  addq $4, %rsp
//load from `b`
//push_from_local(-12)
  subq $4, %rsp
  movl -12(%rbp), %eax
  movl %eax, (%rsp)
//`a` as rvalue
//push_from_local(-8)
  subq $4, %rsp
  movl -8(%rbp), %eax
  movl %eax, (%rsp)
//before assigning to `b`:
//op_ints("addl")
  movl (%rsp), %eax
  addl %eax, +4(%rsp)
  addq $4, %rsp
//assign to `b`
//write_to_local(-12)
  movl (%rsp), %eax
  movl %eax, -12(%rbp)
//gen_discard()
  addq $4, %rsp
//gen_label(12)
.L12:
//`a` as rvalue
//push_from_local(-8)
  subq $4, %rsp
  movl -8(%rbp), %eax
  movl %eax, (%rsp)
//push_int(3)
  subq $4, %rsp
  movl $3, (%rsp)
//op_ints("addl")
  movl (%rsp), %eax
  addl %eax, +4(%rsp)
  addq $4, %rsp
//gen_do_while_final(10, 11)
  addq $4, %rsp
  cmpl $0, -4(%rsp)
  jne .L10
//  addq $4, %rsp
.L11:
//`a` as rvalue
//push_from_local(-8)
  subq $4, %rsp
  movl -8(%rbp), %eax
  movl %eax, (%rsp)
//`b` as rvalue
//push_from_local(-12)
  subq $4, %rsp
  movl -12(%rbp), %eax
  movl %eax, (%rsp)
//mul_ints()
  movl +4(%rsp), %eax
  imull (%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
//return 
  jmp .L29
//print_epilogue(29)
.L29:  movl (%rsp), %eax
  movq %rbp, %rsp
  addq $8, %rsp
  movq (%rbp), %rbp
  ret
