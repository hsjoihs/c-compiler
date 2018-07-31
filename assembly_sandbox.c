#include "print_x86_64.h"
#include <stdio.h>

/*
int x;
int *y;

int *alloc4(int a, int b, int c, int d);
int main() { y = alloc4(1,2,3,174); return x + *(y+3); }

*/

int main()
{
	puts(".comm _y,8\n");
	puts(".comm _x,4\n");
	gen_prologue(16, "main");

	gen_push_int(174);
	gen_pop_to_reg("ecx");

	gen_push_int(3);
	gen_pop_to_reg("edx");

	gen_push_int(2);
	gen_pop_to_reg("esi");

	gen_push_int(1);
	gen_pop_to_reg("edi");

	gen_push_ret_of_8byte("alloc4");

	gen_write_to_global_8byte("y");

	gen_discard();

	gen_push_from_global_8byte("y");

	gen_push_int(12);
	gen_op_8byte("addq");

	gen_peek_and_dereference();

	gen_push_from_global_4byte("x");

	gen_op_ints("addl");

	gen_epilogue(1234);
	return 0;
}
