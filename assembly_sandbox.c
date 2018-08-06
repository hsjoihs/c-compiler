#include "print_x86_64.h"
#include <stdio.h>

/*
int foo(char a, char b){
    int d = 3;
    char c = a+d;
    return c*b;
}


*/

int main()
{
	puts(

	    "" PREFIX "foo:\n"
	    "  pushq %rbp\n"
	    "  movq %rsp, %rbp\n"
	    "  movl %edi, %eax\n"
	    "  movl %esi, %edx\n"
	    "  movb %al, -20(%rbp)\n"
	    "  movl %edx, %eax\n"
	    "  movb %al, -24(%rbp)\n"
	    "  movl $3, -4(%rbp)\n"
	    "  movl -4(%rbp), %eax\n"
	    "  movl %eax, %edx\n"
	    "  movzbl -20(%rbp), %eax\n"
	    "  addl %edx, %eax\n"
	    "  movb %al, -5(%rbp)\n"
	    "  movsbl -5(%rbp), %edx\n"
	    "  movsbl -24(%rbp), %eax\n"
	    "  imull %edx, %eax\n"
	    "  popq %rbp\n"
	    "  ret\n");
	/*
	int main()
	{
	    char f=3;
	    return foo(3,4)+150;
	}
	*/
	gen_prologue(16, "main");

	gen_push_int(3);
	gen_write_to_local_1byte(-1);
	gen_discard();

	gen_push_int(4);
	gen_pop_to_reg_4byte("esi");

	gen_push_int(3);
	gen_pop_to_reg_4byte("edi");

	gen_push_ret_of_4byte("foo");
	gen_push_int(150);
	gen_op_ints("addl");
	gen_epilogue(1234);
}
