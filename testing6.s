.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $4, %rsp
  subq $4, %rsp
  movl $123, -0(%rsp)
  movl -0(%rsp), %eax
  movl %eax, -4(%rbp)
  subq $4, %rsp
  movl $51, -0(%rsp)
  subq $4, %rsp
  movl -4(%rbp), %eax
  movl %eax, -0(%rsp)
  movl -0(%rsp), %eax
  addl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  addq $16, %rsp
  movq (%rbp), %rbp
  ret
