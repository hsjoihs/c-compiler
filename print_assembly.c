#include <stdio.h>
#include <assert.h>
#include "print_assembly.h"

#define MACOS

#ifdef MACOS
#define PREFIX "_"
#else
#define PREFIX ""
#endif


void print_header(int alloc_size)
{
	printf(
		".global " PREFIX "main\n"
		PREFIX "main:\n"
		"  pushq %%rbp\n"
		"  movq %%rsp, %%rbp\n"
	);
	if (alloc_size) { 
		printf("  subq $%d, %%rsp\n", alloc_size);
	}
}

/* write to local mem what's at the top of the stack. does not consume stack. */
void write_to_local(int offset)
{
	assert(offset < 0);
	printf("  movl -0(%%rsp), %%eax\n");
	printf("  movl %%eax, %d(%%rbp)\n", offset);
}

/* push what's on local mem */
void push_from_local(int offset)
{
	assert(offset < 0);
	printf(
		"  subq $4, %%rsp\n"
		"  movl %d(%%rbp), %%eax\n"
		"  movl %%eax, -0(%%rsp)\n"
	, offset);
}

void push_int(int num)
{
	printf(
		"  subq $4, %%rsp\n"
		"  movl $%d, -0(%%rsp)\n"
	, num);
}

void op_ints(const char* str)
{
	printf(
		"  movl -0(%%rsp), %%eax\n"
		"  %s %%eax, +4(%%rsp)\n"
		"  addq $4, %%rsp\n"
	, str);
}

void mul_ints(void)
{
	printf(
		"  movl +4(%%rsp), %%eax\n"
		"  imull -0(%%rsp), %%eax\n"
		"  movl %%eax, +4(%%rsp)\n"
		"  addq $4, %%rsp\n"
	);
}

void div_ints(void)
{
	printf(
		"  movl +4(%%rsp), %%eax\n"
  		"  cltd\n"
  		"  idivl -0(%%rsp)\n"
  		"  movl %%eax, +4(%%rsp)\n"
  		"  addq $4, %%rsp\n"
	);
}

void rem_ints(void)
{
	printf(
		"  movl +4(%%rsp), %%eax\n"
  		"  cltd\n"
  		"  idivl -0(%%rsp)\n"
  		"  movl %%edx, +4(%%rsp)\n"
  		"  addq $4, %%rsp\n"
	);
}

/*
setl: less than
setle: less than or eq
setg: greater than
setge: greater than or eq
*/
void compare_ints(const char* str)
{
	printf(
		"  movl +4(%%rsp), %%eax\n"
		"  cmpl -0(%%rsp), %%eax\n"
		"  %s %%al\n"
		"  movzbl %%al, %%eax\n"
		"  movl %%eax, +4(%%rsp)\n"
		"  addq $4, %%rsp\n"
	,str);
}

void unary_not(void)
{
	printf(
		"  cmpl $0, -0(%%rsp)\n"
		"  sete %%al\n"
		"  movzbl %%al, %%eax\n"
		"  movl %%eax, -0(%%rsp)\n"
	);
}

void unary_bitnot(void)
{
	printf("  notl -0(%%rsp)\n");
}

/*
sall: left shift
sarl: right shift
*/
void shift_ints(const char* str)
{
	printf(
		"  movl -0(%%rsp), %%eax\n"
		"  movl %%eax, %%ecx\n"
		"  %s %%cl, +4(%%rsp)\n"
		"  addq $4, %%rsp\n"
	,str);
}

void print_footer(int alloc_size)
{
	printf(
		"  movl -0(%%rsp), %%eax\n"
		"  addq $%d, %%rsp\n"
		"  movq (%%rbp), %%rbp\n"
		"  ret\n"
	, 12+alloc_size);
}
