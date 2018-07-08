#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	int num;

	/* 
		123 + 56 - 5 
	*/
	print_header();
	push_int(123);
	push_int(56);
	op_ints("addl");
	push_int(5);
	op_ints("subl");
	print_footer();
	return 0;
}
