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
	print_always87();
	print_prologue(0);
	push_int(3);
	push_ret_of("always87");
	push_ret_of("always87");
	op_ints("addl");
	op_ints("movl");
	return_with_label(2);
	print_epilogue(2, 0);
	return 0;
}
