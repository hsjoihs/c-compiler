#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	/* 
		35,(41*3) + (7*8) - (10/(8%3)) 
	*/
	print_header();
	push_int(35);
	push_int(41);
	push_int(3);
	mul_ints();
	push_int(7);
	push_int(8);
	mul_ints();
	op_ints("addl");
	push_int(10);
	push_int(8);
	push_int(3);
	rem_ints();
	div_ints();
	op_ints("subl");
	comma_ints();
	print_footer();
	return 0;
}
