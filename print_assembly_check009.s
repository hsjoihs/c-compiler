.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  subq $4, %rsp
  movl $9, -0(%rsp)
  movl -0(%rsp), %eax
  movl %eax, -8(%rbp)
  subq $4, %rsp
  movl $41, -0(%rsp)
  subq $4, %rsp
  movl $3, -0(%rsp)
  movl +4(%rsp), %eax
  imull -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  movl %eax, -4(%rbp)
  movl -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $55, -0(%rsp)
  subq $4, %rsp
  movl $4, -0(%rsp)
  movl -0(%rsp), %eax
  movl %eax, -8(%rbp)
  movl -0(%rsp), %eax
  subl %eax, +4(%rsp)
  addq $4, %rsp
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
  addq $20, %rsp
  movq (%rbp), %rbp
  ret
