.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp

  subq $4, %rbp
  movl $123, -0(%rbp)


  movl -0(%rbp), %eax
  addq $4, %rbp
  popq %rbp
  ret
