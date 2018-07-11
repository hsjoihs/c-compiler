#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	/* 
		a = 123, 51 + a 
	*/
	print_header(4);

	push_int(123);
	write_to_local(-4);

	push_int(51);
	push_from_local(-4);
	op_ints("addl");

	op_ints("movl");
	print_footer(4);
	return 0;
}
