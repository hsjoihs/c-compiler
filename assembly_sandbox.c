#include "header.h"
#include <stdio.h>

/*
int foo(int a){return a;}

int main() {
    int a;
    int b=0;
    int c = 0;
    for(
        a = 0;
         foo(a < 10) + c;
          a++){
        b += a + 2;
    }
    return a+b; }
*/
int main()
{
	puts(

	    "_foo:\n"
	    "  pushq %rbp\n"
	    "  movq %rsp, %rbp\n"
	    "  movl %edi, -4(%rbp)\n"
	    "  movl -4(%rbp), %eax\n"
	    "  popq %rbp\n"
	    "  ret\n"
	    ".global _main\n"
	    "_main:\n"
	    "  pushq %rbp\n"
	    "  movq %rsp, %rbp\n"
	    "  subq $16, %rsp\n"
	    "  movl $0, -8(%rbp)\n"
	    "  movl $0, -12(%rbp)\n"
	    "  movl $0, -4(%rbp)\n"
	    "  jmp .L4\n"
	    ".L5:\n"
	    "  movl -4(%rbp), %eax\n"
	    "  addl $2, %eax\n"
	    "  addl %eax, -8(%rbp)\n"
	    "  addl $1, -4(%rbp)\n"
	    ".L4:\n"
	    "  cmpl $9, -4(%rbp)\n"
	    "  setle %al\n"
	    "  movzbl %al, %eax\n"
	    "  movl %eax, %edi\n"
	    "  call _foo\n"
	    "  movl %eax, %edx\n"
	    "  movl -12(%rbp), %eax\n"
	    "  addl %edx, %eax\n"
	    "  testl %eax, %eax\n"
	    "  jne .L5\n"
	    "  movl -4(%rbp), %edx\n"
	    "  movl -8(%rbp), %eax\n"
	    "  addl %edx, %eax\n"
	    "  leave\n"
	    "  ret\n");

	return 0;
}
