#include "header.h"
#include "vector.h"
#include <stdio.h>

/*

int always87()
{
    return 87;
}
    return (3, always87() + always87());
*/

int main()
{
	puts("_always87:\n"
	     "  movl $87, %eax\n"
	     "  ret");
	print_prologue(24);
	push_int(3);
	push_ret_of("always87");
	push_ret_of("always87");
	op_ints("addl");
	op_ints("movl");
	return_with_label(2);
	print_epilogue(2, 24);
	return 0;
}
