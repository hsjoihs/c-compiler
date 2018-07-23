#include "header.h"
#include <stdio.h>

/*

int main() {
    int x;
    int *y;
    x = 3;
    y = &x;
    *y = 171;
    *y += 3;
    return x;
}
*/
int main()
{
	gen_prologue(24, "main");

	/* x = 3; */
	gen_push_int(3);
	gen_write_to_local(-12);
	gen_discard();

	/* y = &x; */
	gen_push_address_of_local(-12);
	gen_write_to_local_8byte(-8);
	gen_discard();

	/* *y = 171; */
	gen_push_from_local_8byte(-8);
	gen_push_int(171);
	gen_deref_and_write();
	gen_discard();

	gen_push_from_local_8byte(-8);
	gen_peek_and_dereference();
	/* *y; y is backed up */
	/* 3 */
	gen_push_int(3);
	/* + */
	gen_op_ints("addl");

	gen_read_from_backup_and_prepare();

	gen_deref_and_write();

	gen_push_from_local(-12);

	gen_epilogue(12345);
	return 0;
}
