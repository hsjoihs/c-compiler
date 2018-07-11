.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $4, %rsp
  movl $123, -0(%rsp)
  movl -0(%rsp), %eax
  addq $12, %rsp
  movq (%rbp), %rbp
  ret
