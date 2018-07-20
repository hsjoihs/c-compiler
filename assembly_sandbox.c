#include "header.h"
#include <stdio.h>

/*

int main()
{
    int x;
    x = 174;
    int *y;
    y = &x;
    return *y;
 }
*/
int main()
{
	gen_prologue(0, "main");

	/* x = 174; */
	gen_push_int(174);
	gen_write_to_local(-12);
	gen_discard();

	/* y = &x; */
	gen_push_address_of_local(-12);
	gen_write_to_local_8byte(-8);
	gen_discard();

	/* return *y; */
	gen_push_from_local_8byte(-8);
	gen_peek_and_dereference();
	gen_return_with_label(123);

	gen_epilogue(123);
	return 0;
}
