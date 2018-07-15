#include "header.h"
#include "vector.h"
#include <stdio.h>
/*
int main()
{
    int a = 3;
    a += 5;
    a *= 2;
    return a + 158;
}
*/
int main()
{
	print_prologue(4, "main");
	push_int(3);
	write_to_local(-4);
	op_ints("movl"); /* ; */

	push_from_local(-4);
	push_int(5);
	op_ints("addl");
	write_to_local(-4);
	op_ints("movl"); /* ; */

	push_from_local(-4);
	push_int(2);
	mul_ints();
	write_to_local(-4);
	op_ints("movl"); /* ; */

	push_from_local(-4);
	push_int(158);
	op_ints("addl");
	return_with_label(123);
	print_epilogue(123, 4);
	
	return 0;
}
