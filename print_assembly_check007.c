#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	/* 
		7*5,(12,(41)*(4-(9>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/((!0<<3)%3)
	*/
	print_header(0);
	push_int(7);
	push_int(5);
	mul_ints();
	push_int(12);
	push_int(41);
	push_int(4);
	push_int(9);
	push_int(8);
	compare_ints("setg");
	op_ints("subl");
	mul_ints();
	op_ints("movl");
	push_int(7);
	push_int(1);
	push_int(1);
	unary_bitnot();
	op_ints("subl");
	push_int(3);
	compare_ints("setge");
	push_int(4);
	shift_ints("sall");
	mul_ints();
	push_int(9);
	push_int(4);
	push_int(10);
	push_int(10);
	compare_ints("setle");
	shift_ints("sarl");
	push_int(3);
	push_int(3);
	compare_ints("setl");
	op_ints("addl");
	op_ints("movl");
	div_ints();
	op_ints("addl");
	push_int(10);

	push_int(0);
	unary_not();
	push_int(3);
	shift_ints("sall");
	push_int(3);
	rem_ints();
	div_ints();
	op_ints("subl");
	op_ints("movl");
	print_footer(0);
	return 0;
}
