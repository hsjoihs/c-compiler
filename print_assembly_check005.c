#include <stdio.h>
#include "header.h"

int main()
{
	/* 
		35,(41*3) + (7*8) - (10/(8%(!!1+2))) 
	*/
	print_prologue(0);
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
	push_int(1);
	unary_not();
	unary_not();
	push_int(2);
	op_ints("addl");
	rem_ints();
	div_ints();
	op_ints("subl");
	op_ints("movl");
	print_epilogue(0);
	return 0;
}
