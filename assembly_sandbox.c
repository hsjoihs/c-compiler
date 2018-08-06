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
	gen_prologue(0, "foo");
	puts("  movl %edi, %eax\n"
	     "  movb %al, -20(%rbp)\n"
	     "  movl %esi, %eax\n"
	     "  movb %al, -24(%rbp)\n");

	gen_push_int(3);
	gen_write_to_local(-4);
	gen_discard();

	gen_push_from_local_4byte(-4);
	gen_push_from_local_1byte(-20);
	gen_op_ints("addl");
	gen_write_to_local_1byte(-5);
	gen_discard();

	gen_push_from_local_1byte(-5);
	gen_push_from_local_1byte(-24);
	gen_mul_ints();
	gen_epilogue(1222);
	/*
	int main()
	{
	    char f=3;
	    return foo(f,4)+150;
	}
	*/
	gen_prologue(16, "main");

	gen_push_int(3);
	gen_write_to_local_1byte(-1);
	gen_discard();

	gen_push_int(4);
	gen_pop_to_reg_4byte("esi");

	puts("movsbl -1(%rbp), %eax\n"
	     "movl %eax, %edi\n");

	gen_push_ret_of_4byte("foo");
	gen_push_int(150);
	gen_op_ints("addl");
	gen_epilogue(1234);
}
