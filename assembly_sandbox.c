#include "header.h"
#include "vector.h"
#include <stdio.h>
/*
int always87_(int x, int y) { return 87; }

int foo_(int x, int y)
{
    return 103+x-y;
}

int main()
{
    int b = 3;
    b = always87_(13 + 6, 30 + 5) + 87;
    return b;
}
*/
int main()
{

	print_prologue(8, "foo");
	write_register_to_local("edi",-4);
	write_register_to_local("esi",-8);
	push_from_local(-4);
	push_int(103);
	op_ints("addl");
	push_from_local(-8);
	op_ints("subl");
	return_with_label(2);
	print_epilogue(2,8);

	print_prologue(16, "main");

	push_int(6);

	push_int(30);
	push_int(5);
	op_ints("addl");
	pop_to_reg("esi");

	push_int(13);
	op_ints("addl");
	pop_to_reg("edi");

	push_ret_of("foo");

	push_int(87);
	op_ints("addl");
	return_with_label(12);
	print_epilogue(12, 16);
}
