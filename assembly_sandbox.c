#include "header.h"
#include <stdio.h>

/*

int main()
{
    int x;
    x = 86;
    int *y;
    y = &x;
    int **z;
    z = &y;
    return (*y) + (**z) + 2;
 }
*/
int main()
{
	gen_prologue(24, "main");

	/* x = 86; */
	gen_push_int(86);
	gen_write_to_local(-16);
	gen_discard();

	/* y = &x; */
	gen_push_address_of_local(-16);
	gen_write_to_local_8byte(-8);
	gen_discard();

	/* z = &y; */
	gen_push_address_of_local(-8);
	gen_write_to_local_8byte(-24);
	gen_discard();

	/* return (*y) + (**z) + 2; */
	{
		/* *y */
		gen_push_from_local_8byte(-8);
		gen_peek_and_dereference();

		/* **z */
		gen_push_from_local_8byte(-24);
		gen_peek_and_dereference_8byte();
		gen_peek_and_dereference();

		gen_op_ints("addl");
		gen_push_int(2);
		gen_op_ints("addl");

		gen_return_with_label(123);
	}
	gen_epilogue(123);
	return 0;
}
