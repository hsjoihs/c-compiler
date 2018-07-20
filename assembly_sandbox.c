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

	gen_push_from_local_8byte(-8);

	puts("  movq (%rsp), %rax \n"
	     "  movl (%rax), %eax\n"
	     "  movl  %eax, (%rsp)\n");
	gen_epilogue(123);
	return 0;
}
