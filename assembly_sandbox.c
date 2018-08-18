#include "print_x86_64.h"
#include <stdio.h>

int main()
{
	/*

int foo(){return 3;}
int main()
{
	int a = 24;
	int b = 2;
	switch(foo()){
	    case 13:
	    a=14;
	default:
	    b = 150;
	}
	return a+b;
}


	*/
	gen_prologue(0, "foo");

	puts(

	    "  movl $3, %eax\n"
	    "  popq %rbp\n"
	    "  ret\n");

	gen_prologue(16, "main");

	puts("  movl $24, -4(%rbp)\n"
	     "  movl $2, -8(%rbp)\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "foo\n"
	     "  cmpl $13, %eax\n"
	     "  jne .L4\n"
	     "  movl $14, -4(%rbp)\n"
	     ".L4:\n"
	     "  movl $150, -8(%rbp)\n"
	     "  movl -4(%rbp), %edx\n"
	     "  movl -8(%rbp), %eax\n"
	     "  addl %edx, %eax\n"
	     "  leave\n"
	     "  ret\n");
	return 0;
}
