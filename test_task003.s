.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
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
  movl $5, -0(%rbp)
  subq $4, %rbp
  movl $1, -0(%rbp)
  movl +4(%rbp), %eax
  imull -0(%rbp), %eax
  movl %eax, +4(%rbp)
  addq $4, %rbp
  movl -0(%rbp), %eax
  subl %eax, +4(%rbp)
  addq $4, %rbp
  movl -0(%rbp), %eax
  addq $4, %rbp
  popq %rbp
  ret