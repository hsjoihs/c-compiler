.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $4, %rbp
  movl $35, -0(%rbp)
  subq $4, %rbp
  movl $41, -0(%rbp)
  subq $4, %rbp
  movl $3, -0(%rbp)
  movl +4(%rbp), %eax
  imull -0(%rbp), %eax
  movl %eax, +4(%rbp)
  addq $4, %rbp
  subq $4, %rbp
  movl $7, -0(%rbp)
  subq $4, %rbp
  movl $8, -0(%rbp)
  movl +4(%rbp), %eax
  imull -0(%rbp), %eax
  movl %eax, +4(%rbp)
  addq $4, %rbp
  movl -0(%rbp), %eax
  addl %eax, +4(%rbp)
  addq $4, %rbp
  subq $4, %rbp
  movl $10, -0(%rbp)
  subq $4, %rbp
  movl $8, -0(%rbp)
  subq $4, %rbp
  movl $3, -0(%rbp)
  movl +4(%rbp), %eax
  cltd
  idivl -0(%rbp)
  movl %edx, +4(%rbp)
  addq $4, %rbp
  movl +4(%rbp), %eax
  cltd
  idivl -0(%rbp)
  movl %eax, +4(%rbp)
  addq $4, %rbp
  movl -0(%rbp), %eax
  subl %eax, +4(%rbp)
  addq $4, %rbp
  movl -0(%rbp), %eax
  movl %eax, +4(%rbp)
  addq $4, %rbp
  movl -0(%rbp), %eax
  addq $4, %rbp
  popq %rbp
  ret
