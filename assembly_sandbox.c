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
	gen_push_int(3);
	gen_epilogue(123);

	gen_prologue(16, "main");

	gen_push_int(24);
	gen_write_to_local(-4); /* a */
	gen_discard();

	gen_push_int(2);
	gen_write_to_local(-8); /* b */
	gen_discard();

	int label1 = 9;
	int default_label = 4;
	int constant1 = 13;
	gen_push_ret_of_4byte("foo");

	gen_discard();
	gen_if_neg8_matches_jmp_4byte(constant1, label1);
	gen_jump(default_label, "switch-default");

	gen_label(label1); /* case 13: */

	gen_push_int(14);
	gen_write_to_local(-4); /* a */
	gen_discard();

	gen_label(default_label); /* default */

	gen_push_int(150);
	gen_write_to_local(-8); /* b */
	gen_discard();

	gen_push_from_local_4byte(-4);
	gen_push_from_local_4byte(-8);
	gen_op_ints("addl");
	gen_epilogue(234);
	return 0;
}
