#include "print_x86_64.h"
#include <stdio.h>

#ifdef OSX
void gen_str(int strnum, const char *str)
{
	printf("//gen_str(%d, %s)\n", strnum, str);
	printf("L_str%d:\n"
	       ".asciz \"%s\"\n",
	       strnum, str);
}

void gen_push_address_of_str(int strnum)
{
	printf("//gen_push_address_of_str(%d)\n", strnum);
	printf("  subq $8, %%rsp\n"
	       "  leaq L_str%d(%%rip), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       strnum);
}
#endif

int main()
{
	/*
	char foo() { char *x = "1ab"; return x[0]; }
	*/
	gen_prologue(0, "foo");
	gen_push_address_of_str(0);
	gen_write_to_local_8byte(-8); /* x */
	gen_discard();

	gen_push_from_local_8byte(-8);
	gen_peek_and_dereference_1byte();
	gen_epilogue(1234);
	/*
	int main()
	{
	 char *y = "a2b";
	 int z = 12; char a = y[1];
	 return (a-foo())*z+162;
	}
	*/
	gen_prologue(32, "main");
	gen_push_address_of_str(1);
	gen_write_to_local_8byte(-16); /* y */
	gen_discard();

	gen_push_int(0);
	gen_write_to_local(-4);
	gen_discard();

	gen_push_int(12);
	gen_write_to_local(-20); /* z */
	gen_discard();

	gen_push_from_local_8byte(-16); /* y */
	gen_push_int(1);
	gen_cltq();
	gen_op_8byte("addq");             /* y+1 */
	gen_peek_and_dereference_1byte(); /* y[1] */
	gen_write_to_local_1byte(-21);    /* a */
	gen_discard();

	gen_push_from_local_1byte(-21); /* a */
	gen_write_to_local(-28);        /* (int)a */
	gen_discard();

	gen_push_from_local_4byte(-28); /* (int)a */
	gen_push_ret_of_1byte("foo");   /* foo() */
	gen_op_ints("subl");
	gen_push_from_local_4byte(-20); /* z */
	gen_mul_ints();
	gen_push_int(162);
	gen_op_ints("addl");
	gen_epilogue(234);
	gen_str(0, "1ab");
	gen_str(1, "a2b");
	return 0;
}
