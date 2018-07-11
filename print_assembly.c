#include <stdio.h>
#include "print_assembly.h"

#define MACOS

#ifdef MACOS
#define PREFIX "_"
#else
#define PREFIX ""
#endif


void print_header(void)
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

void op_ints(const char* str)
{
	printf(
		"  movl -0(%%rbp), %%eax\n"
		"  %s %%eax, +4(%%rbp)\n"
		"  addq $4, %%rbp\n"
	, str);
}

void mul_ints(void)
{
	printf(
		"  movl +4(%%rbp), %%eax\n"
		"  imull -0(%%rbp), %%eax\n"
		"  movl %%eax, +4(%%rbp)\n"
		"  addq $4, %%rbp\n"
	);
}

void div_ints(void)
{
	printf(
		"  movl +4(%%rbp), %%eax\n"
  		"  cltd\n"
  		"  idivl -0(%%rbp)\n"
  		"  movl %%eax, +4(%%rbp)\n"
  		"  addq $4, %%rbp\n"
	);
}

void rem_ints(void)
{
	printf(
		"  movl +4(%%rbp), %%eax\n"
  		"  cltd\n"
  		"  idivl -0(%%rbp)\n"
  		"  movl %%edx, +4(%%rbp)\n"
  		"  addq $4, %%rbp\n"
	);
}

void comma_ints(void)
{
	printf(
		"  movl -0(%%rbp), %%eax\n"
		"  movl %%eax, +4(%%rbp)\n"
	);
}

void print_footer(void)
{
	printf(
		"  movl -0(%%rbp), %%eax\n"
		"  addq $4, %%rbp\n"
		"  popq %%rbp\n"
		"  ret\n"
	);
}
