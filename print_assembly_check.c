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
	add_ints();
	push_int(5);
	sub_ints();
	print_footer();
	return 0;
}
