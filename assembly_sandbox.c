#include "header.h"
#include "vector.h"
#include <stdio.h>

/*
int foo(){return 1;}

int main() {
    int a = 0;
    do {
        a = 3;
    } while(a == foo());
    return 174; }
*/
int main()
{
	gen_prologue(0, "foo");
	{
		gen_push_int(1);
	}
	gen_epilogue(100);

	gen_prologue(16, "main");
	{
		gen_push_int(0);
		gen_write_to_local(-4);
		gen_op_ints("movl");

		puts(".L5:\n");

		gen_push_int(3);
		gen_write_to_local(-4);
		gen_op_ints("movl");

		gen_push_from_local(-4);
		gen_push_ret_of("foo");
		gen_compare_ints("sete");

		puts("  addq $4, %rsp\n"
		     "  cmpl $1, -4(%rsp)\n"
		     "  je .L5\n"
		     "  addq $4, %rsp\n");
		gen_push_int(174);
	}
	gen_epilogue(143);

	return 0;
}
