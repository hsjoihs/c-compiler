#include <stdio.h>
#include "header.h"

int main()
{
	/* 
		35	,   (  (41|(8!=15))  * ((3==3)+2)) + ((5|2)*(9&10)) - (10/(8%3)) 
	*/
	print_prologue(0);
	push_int(35);
	push_int(41);
	push_int(8);
	push_int(15);
	compare_ints("setne");
	op_ints("orl");
	push_int(3);
	push_int(3);
	compare_ints("sete");
	push_int(2);
	op_ints("addl");
	mul_ints();
	push_int(5);
	push_int(2);
	op_ints("orl");
	push_int(9);
	push_int(10);
	op_ints("andl");
	mul_ints();
	op_ints("addl");
	push_int(10);
	push_int(8);
	push_int(3);
	rem_ints();
	div_ints();
	op_ints("subl");
	op_ints("movl");
	print_epilogue(0);
	return 0;
}
