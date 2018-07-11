#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	/* 
		7*5,(12,41*3)+7*16/(9,2+(3<3))-10/((1<<3)%3)
	*/
	print_header();
	push_int(7);
	push_int(5);
	mul_ints();
	push_int(12);
	push_int(41);
	push_int(3);
	mul_ints();
	op_ints("movl");
	push_int(7);
	push_int(16);
	mul_ints();
	push_int(9);
	push_int(2);
	push_int(3);
	push_int(3);
	compare_ints("setl");
	op_ints("addl");
	op_ints("movl");
	div_ints();
	op_ints("addl");
	push_int(10);

	push_int(1);
	push_int(3);
	shift_ints("sall");
	push_int(3);
	rem_ints();
	div_ints();
	op_ints("subl");
	op_ints("movl");
	print_footer();
	return 0;
}
