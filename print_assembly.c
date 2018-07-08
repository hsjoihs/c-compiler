#include <stdio.h>
#include "print_assembly.h"

#define MACOS

#ifdef MACOS
#define PREFIX "_"
#else
#define PREFIX ""
#endif


void print_header()
{
	printf(
		".global " PREFIX "main\n"
		PREFIX "main:\n"
		"  pushq %%rbp\n"
		"  movq %%rsp, %%rbp\n");
}

void push_int(int num)
{
	printf(
		"  subq $4, %%rbp\n"
		"  movl $%d, -0(%%rbp)\n"
	, num);
}

void print_footer()
{
	printf(
		"  movl -0(%%rbp), %%eax\n"
		"  addq $4, %%rbp\n"
		"  popq %%rbp\n"
		"  ret\n"
	);
}
