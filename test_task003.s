.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $4, %rsp
  movl $41, -0(%rsp)
  subq $4, %rsp
  movl $3, -0(%rsp)
  movl +4(%rsp), %eax
  imull -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $7, -0(%rsp)
  subq $4, %rsp
  movl $8, -0(%rsp)
  movl +4(%rsp), %eax
  imull -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  addl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $5, -0(%rsp)
  subq $4, %rsp
  movl $1, -0(%rsp)
  movl +4(%rsp), %eax
  imull -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  subl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  addq $12, %rsp
  movq (%rbp), %rbp
  ret
