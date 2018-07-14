#include "header.h"
#include <assert.h>
#include <stdio.h>

#define MACOS

#ifdef MACOS
#define PREFIX "_"
#else
#define PREFIX ""
#endif

void print_prologue(int alloc_size, const char *fname)
{
	printf("//print_prologue(%d, %s)\n", alloc_size, fname);
	printf(".global " PREFIX "%s\n" PREFIX "%s:\n"
	       "  pushq %%rbp\n"
	       "  movq %%rsp, %%rbp\n",
	       fname, fname);
	if (alloc_size) {
		printf("  subq $%d, %%rsp\n", alloc_size);
	}
}

/* write to local mem what's at the top of the stack. does not consume stack. */
void write_to_local(int offset)
{
	assert(offset < 0);
	printf("//write_to_local(%d)\n", offset);
	printf("  movl (%%rsp), %%eax\n");
	printf("  movl %%eax, %d(%%rbp)\n", offset);
}

/* write to local mem what's in the register */
void write_register_to_local(const char* str, int offset)
{
	assert(offset < 0);
	printf("//write_register_to_local(%s, %d)\n",str, offset);
	printf("  movl %%%s, %d(%%rbp)\n", str, offset);
}

/* push what's on local mem */
void push_from_local(int offset)
{
	assert(offset < 0);
	printf("//push_from_local(%d)\n", offset);
	printf("  subq $4, %%rsp\n"
	       "  movl %d(%%rbp), %%eax\n"
	       "  movl %%eax, (%%rsp)\n",
	       offset);
}

void push_int(int num)
{
	printf("//push_int(%d)\n", num);
	printf("  subq $4, %%rsp\n"
	       "  movl $%d, (%%rsp)\n",
	       num);
}

void push_ret_of(const char *fname)
{
	printf("//push_ret_of(\"%s\")\n", fname);
	printf("  movl $0, %%eax\n"
	       "  call " PREFIX "%s\n",
	       fname);
	printf("  subq $4, %%rsp\n"
	       "  movl %%eax, (%%rsp)\n");
}

void push_eax(void)
{
	printf("//push_eax()\n");
	printf("  subq $4, %%rsp\n"
	       "  movl %%eax, (%%rsp)\n");
}

void pop_to_reg(const char *str)
{
	printf("//pop_to_reg(%s)\n", str);
	printf("  movl (%%rsp), %%%s\n", str);
	printf("  addq $4, %%rsp\n");
}

void op_ints(const char *str)
{
	printf("//op_ints(\"%s\")\n", str);
	printf("  movl (%%rsp), %%eax\n"
	       "  %s %%eax, +4(%%rsp)\n"
	       "  addq $4, %%rsp\n",
	       str);
}

void mul_ints(void)
{
	printf("//mul_ints()\n");
	printf("  movl +4(%%rsp), %%eax\n"
	       "  imull (%%rsp), %%eax\n"
	       "  movl %%eax, +4(%%rsp)\n"
	       "  addq $4, %%rsp\n");
}

void return_with_label(int label)
{
	printf("//return \n"
	       "  jmp .L%d\n",
	       label);
}

void div_ints(void)
{
	printf("//div_ints()\n");
	printf("  movl +4(%%rsp), %%eax\n"
	       "  cltd\n"
	       "  idivl (%%rsp)\n"
	       "  movl %%eax, +4(%%rsp)\n"
	       "  addq $4, %%rsp\n");
}

void rem_ints(void)
{
	printf("//rem_ints()\n");
	printf("  movl +4(%%rsp), %%eax\n"
	       "  cltd\n"
	       "  idivl (%%rsp)\n"
	       "  movl %%edx, +4(%%rsp)\n"
	       "  addq $4, %%rsp\n");
}

/*
setl: less than
setle: less than or eq
setg: greater than
setge: greater than or eq
*/
void compare_ints(const char *str)
{
	printf("//compare_ints(\"%s\")\n", str);
	printf("  movl +4(%%rsp), %%eax\n"
	       "  cmpl (%%rsp), %%eax\n"
	       "  %s %%al\n"
	       "  movzbl %%al, %%eax\n"
	       "  movl %%eax, +4(%%rsp)\n"
	       "  addq $4, %%rsp\n",
	       str);
}

void unary_not(void)
{
	printf("//unary_not()\n");
	printf("  cmpl $0, (%%rsp)\n"
	       "  sete %%al\n"
	       "  movzbl %%al, %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

/*
notl: bitnot
negl: integer negation
*/
void unary(const char *str)
{
	printf("//unary(\"%s\")\n", str);
	printf("  %s (%%rsp)\n", str);
}

/*
sall: left shift
sarl: right shift
*/
void shift_ints(const char *str)
{
	printf("//shift_ints(\"%s\")\n", str);
	printf("  movl (%%rsp), %%eax\n"
	       "  movl %%eax, %%ecx\n"
	       "  %s %%cl, +4(%%rsp)\n"
	       "  addq $4, %%rsp\n",
	       str);
}

void print_epilogue(int label, int alloc_size)
{
	printf("//print_epilogue(%d)\n", alloc_size);
	printf(".L%d:"
	       "  movl (%%rsp), %%eax\n"
	       "  addq $%d, %%rsp\n"
	       "  movq (%%rbp), %%rbp\n"
	       "  ret\n",
	       label, 12 + alloc_size);
}
