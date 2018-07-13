#include "header.h"
#include "vector.h"
#include <stdio.h>

int main()
{
	/*
	    return 2 + (1 ? 100 + 72 : 17);
	*/

	int label1 = 2;
	int label2 = 3;

	print_prologue(0);

	push_int(2);

	push_int(1);
	printf("  cmpl $0, (%%rsp)\n"
	       "  je .L%d\n",
	       label1);

	push_int(100);
	push_int(72);
	op_ints("addl");

	printf("  movl (%%rsp), %%eax\n"
	       "  addq $4, %%rsp\n"
	       "  jmp .L%d\n"
	       ".L%d:\n",
	       label2, label1);

	push_int(17);

	printf("  movl (%%rsp), %%eax\n"
	       "  addq $4, %%rsp\n"
	       ".L%d:\n"
	       "  movl %%eax, (%%rsp)\n",
	       label2);

	op_ints("addl");

	return_with_label(123);
	print_epilogue(123, 0);
	return 0;
}
