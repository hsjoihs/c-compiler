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

void add_ints()
{
	printf(
		"  movl -0(%%rbp), %%eax\n"
		"  addl %%eax, +4(%%rbp)\n"
		"  addq $4, %%rbp\n"
	);
}

void sub_ints()
{
	printf(
		"  movl -0(%%rbp), %%eax\n"
		"  subl %%eax, +4(%%rbp)\n"
		"  addq $4, %%rbp\n"
	);
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
