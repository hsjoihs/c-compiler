#include "header.h"
#include "vector.h"
#include <stdio.h>

int main()
{
	/*
	    return 2 + (1 ? 100 + 72 : 17);
	*/

	print_prologue(0);

	push_int(2);
	push_int(1);
	printf("  cmpl $0, (%%rsp)\n"
	       "  je .L2\n");
	push_int(100);
	push_int(72);
	op_ints("addl");
	printf("  movl (%%rsp), %%eax\n"
	       "  addq $4, %%rsp\n"
	       "  jmp .L3\n"
	       ".L2:\n");
	push_int(17);
	printf("  movl (%%rsp), %%eax\n"
	       "  addq $4, %%rsp\n"
	       ".L3:\n"
	       "  movl %%eax, (%%rsp)\n");

	op_ints("addl");

	return_with_label("FIXME");
	print_epilogue("FIXME", 0);
	return 0;
}
