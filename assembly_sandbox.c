#include "header.h"
#include "vector.h"
#include <stdio.h>

/*
int foo(){return 3;}

int main() {
    int a = 0;

    while(a == foo()) {
        a = 3;
    }
    return 174;
}
*/
int main()
{
	gen_prologue(0, "foo");
	{
		gen_push_int(3);
	}
	gen_epilogue(100);

	gen_prologue(16, "main");
	{
		/* int a = 0; */
		gen_push_int(0);
		gen_write_to_local(-4);
		gen_op_ints("movl");

		int label1 = 7;
		int label2 = 6;
		int label3 = 8;
		/* a == foo() */
		printf(".L%d:\n", label1);
		gen_push_from_local(-4);
		gen_push_ret_of("foo");
		gen_compare_ints("sete");

		gen_while_part2(label1, label2);

		gen_push_int(3);
		gen_write_to_local(-4);
		gen_op_ints("movl");

		gen_while_part3(label1, label2, label3);

		gen_push_int(174);
	}
	gen_epilogue(143);

	return 0;
}
