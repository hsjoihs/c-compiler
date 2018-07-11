.global _main
_main:
  pushq %rbp
  movq %rsp, %rbp
  subq $4, %rsp
  movl $35, -0(%rsp)
  subq $4, %rsp
  movl $41, -0(%rsp)
  subq $4, %rsp
  movl $8, -0(%rsp)
  subq $4, %rsp
  movl $15, -0(%rsp)
  movl +4(%rsp), %eax
  cmpl -0(%rsp), %eax
  setne %al
  movzbl %al, %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  orl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $3, -0(%rsp)
  subq $4, %rsp
  movl $3, -0(%rsp)
  movl +4(%rsp), %eax
  cmpl -0(%rsp), %eax
  sete %al
  movzbl %al, %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $2, -0(%rsp)
  movl -0(%rsp), %eax
  addl %eax, +4(%rsp)
  addq $4, %rsp
  movl +4(%rsp), %eax
  imull -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $5, -0(%rsp)
  subq $4, %rsp
  movl $2, -0(%rsp)
  movl -0(%rsp), %eax
  orl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $9, -0(%rsp)
  subq $4, %rsp
  movl $10, -0(%rsp)
  movl -0(%rsp), %eax
  andl %eax, +4(%rsp)
  addq $4, %rsp
  movl +4(%rsp), %eax
  imull -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  addl %eax, +4(%rsp)
  addq $4, %rsp
  subq $4, %rsp
  movl $10, -0(%rsp)
  subq $4, %rsp
  movl $8, -0(%rsp)
  subq $4, %rsp
  movl $3, -0(%rsp)
  movl +4(%rsp), %eax
  cltd
  idivl -0(%rsp)
  movl %edx, +4(%rsp)
  addq $4, %rsp
  movl +4(%rsp), %eax
  cltd
  idivl -0(%rsp)
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  subl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  movl %eax, +4(%rsp)
  addq $4, %rsp
  movl -0(%rsp), %eax
  addq $4, %rsp
  popq %rbp
  ret
