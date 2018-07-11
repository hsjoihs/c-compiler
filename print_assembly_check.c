#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	/* 
		(41*3) + (7*8) - (5*1) 
	*/
	print_header(0);
	push_int(41);
	push_int(3);
	mul_ints();
	push_int(7);
	push_int(8);
	mul_ints();
	op_ints("addl");
	push_int(5);
	push_int(1);
	mul_ints();
	op_ints("subl");
	print_footer(0);
	return 0;
}
