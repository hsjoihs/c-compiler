.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $4, %rsp
  movl $123, -0(%rsp)
  movl -0(%rsp), %eax
  addq $4, %rsp
  popq %rbp
  ret
