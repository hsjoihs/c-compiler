#include "header.h"
#include <assert.h>
#include <stdio.h>

#define MACOS

#ifdef MACOS
#define PREFIX "_"
#else
#define PREFIX ""
#endif

void gen_prologue(int alloc_size, const char *fname)
{
	printf("//gen_prologue(%d, %s)\n", alloc_size, fname);
	printf(".global " PREFIX "%s\n" PREFIX "%s:\n"
	       "  pushq %%rbp\n"
	       "  movq %%rsp, %%rbp\n",
	       fname, fname);
	if (alloc_size) {
		printf("  subq $%d, %%rsp\n", alloc_size);
	}
}

/* write to local mem what's at the top of the stack. does not consume stack. */
void gen_write_to_local(int offset)
{
	assert(offset < 0);
	printf("//gen_write_to_local(%d)\n", offset);
	printf("  movl (%%rsp), %%eax\n");
	printf("  movl %%eax, %d(%%rbp)\n", offset);
}

/*
    value = pop();
    addr = pop();
    *addr = value;
    push(value);
*/
void gen_deref_and_write(void)
{
	puts("  movq 8(%rsp), %rbx\n"
	     "  movl (%rsp), %eax\n"
	     "  movl %eax, (%rbx)\n"
	     "  addq $8, %rsp\n"
	     "  movl %eax, (%rsp)\n");
}

/* write to local mem what's at the top of the stack. does not consume stack. */
void gen_write_to_local_8byte(int offset)
{
	assert(offset < 0);
	printf("//gen_write_to_local_8byte(%d)\n", offset);
	printf("  movq (%%rsp), %%rax\n");
	printf("  movq %%rax, %d(%%rbp)\n", offset);
}

/* write to local mem what's in the register */
void gen_write_register_to_local(const char *str, int offset)
{
	assert(offset < 0);
	printf("//gen_write_register_to_local(%s, %d)\n", str, offset);
	printf("  movl %%%s, %d(%%rbp)\n", str, offset);
}

void gen_label(int label1)
{
	printf("//gen_label(%d)\n", label1);
	printf(".L%d:\n", label1);
}

void gen_do_while_final(int label1, int label2)
{
	printf("//gen_do_while_final(%d, %d)\n", label1, label2);
	printf("  addq $8, %%rsp\n"
	       "  cmpl $0, -8(%%rsp)\n"
	       "  jne .L%d\n"
	       ".L%d:\n",
	       label1, label2);
}

void gen_discard(void)
{
	printf("//gen_discard()\n");
	printf("  addq $8, %%rsp\n");
}

void gen_while_part2(int label1, int label2)
{
	printf("//gen_while_part2(%d, %d)\n", label1, label2);
	printf("  addq $8, %%rsp\n"
	       "  cmpl $0, -8(%%rsp)\n");
	printf("  je .L%d\n", label2);
}

void gen_for_part4(int label1, int label2)
{
	printf("//gen_for_part4(%d, %d)\n", label1, label2);
	printf("  jmp .L%d\n", label1);
	printf(".L%d:\n", label2);
}

/* push what's on local mem */
void gen_push_from_local(int offset)
{
	assert(offset < 0);
	printf("//gen_push_from_local(%d)\n", offset);
	printf("  subq $8, %%rsp\n"
	       "  movl %d(%%rbp), %%eax\n"
	       "  movl %%eax, (%%rsp)\n",
	       offset);
}

/* dereference what's at the top of the stack */
void gen_peek_and_dereference(void)
{
	printf("//gen_peek_and_dereference()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movq %rax, -8(%rbp) //backup\n"
	     "  movl (%rax), %eax\n"
	     "  movl  %eax, (%rsp)\n");
}

/* dereference what's at the top of the stack */
void gen_peek_and_dereference_8byte(void)
{
	printf("//gen_peek_and_dereference()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movq %rax, -8(%rbp) //backup\n"
	     "  movq (%rax), %rax\n"
	     "  movq  %rax, (%rsp)\n");
}

/* push what's on local mem */
void gen_push_from_local_8byte(int offset)
{
	assert(offset < 0);
	printf("//gen_push_from_local_8byte(%d)\n", offset);
	printf("  subq $8, %%rsp\n"
	       "  movq %d(%%rbp), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       offset);
}

void gen_push_int(int num)
{
	printf("//gen_push_int(%d)\n", num);
	printf("  subq $8, %%rsp\n"
	       "  movl $%d, (%%rsp)\n",
	       num);
}

void gen_push_ret_of(const char *fname)
{
	printf("//gen_push_ret_of(\"%s\")\n", fname);
	printf("  movl $0, %%eax\n"
	       "  call " PREFIX "%s\n",
	       fname);
	printf("  subq $8, %%rsp\n"
	       "  movl %%eax, (%%rsp)\n");
}

void gen_pop_to_reg(const char *str)
{
	printf("//gen_pop_to_reg(%s)\n", str);
	printf("  movl (%%rsp), %%%s\n", str);
	printf("  addq $8, %%rsp\n");
}

void gen_op_ints(const char *str)
{
	printf("//gen_op_ints(\"%s\")\n", str);
	printf("  movl (%%rsp), %%eax\n"
	       "  %s %%eax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_mul_ints(void)
{
	printf("//gen_mul_ints()\n");
	printf("  movl +8(%%rsp), %%eax\n"
	       "  imull (%%rsp), %%eax\n"
	       "  movl %%eax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_return_with_label(int label)
{
	printf("//return \n"
	       "  jmp .L%d\n",
	       label);
}

void gen_div_ints(void)
{
	printf("//gen_div_ints()\n");
	printf("  movl +8(%%rsp), %%eax\n"
	       "  cltd\n"
	       "  idivl (%%rsp)\n"
	       "  movl %%eax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_rem_ints(void)
{
	printf("//gen_rem_ints()\n");
	printf("  movl +8(%%rsp), %%eax\n"
	       "  cltd\n"
	       "  idivl (%%rsp)\n"
	       "  movl %%edx, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

/*
setl: less than
setle: less than or eq
setg: greater than
setge: greater than or eq
*/
void gen_compare_ints(const char *str)
{
	printf("//gen_compare_ints(\"%s\")\n", str);
	printf("  movl +8(%%rsp), %%eax\n"
	       "  cmpl (%%rsp), %%eax\n"
	       "  %s %%al\n"
	       "  movzbl %%al, %%eax\n"
	       "  movl %%eax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_unary_not(void)
{
	printf("//gen_unary_not()\n");
	printf("  cmpl $0, (%%rsp)\n"
	       "  sete %%al\n"
	       "  movzbl %%al, %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

/*
notl: bitnot
negl: integer negation
*/
void gen_unary(const char *str)
{
	printf("//gen_unary(\"%s\")\n", str);
	printf("  %s (%%rsp)\n", str);
}

void gen_ternary_part1(int label1, int label2)
{
	printf("//gen_ternary_part1(%d, %d)\n", label1, label2);
	printf("  cmpl $0, (%%rsp)\n"
	       "  je .L%d\n",
	       label1);
}

void gen_ternary_part2(int label1, int label2)
{
	printf("//gen_ternary_part2(%d, %d)\n", label1, label2);
	printf("  movl (%%rsp), %%eax\n"
	       "  addq $8, %%rsp\n"
	       "  jmp .L%d\n"
	       ".L%d:\n",
	       label2, label1);
}

void gen_ternary_part3(int label1, int label2)
{
	printf("//gen_ternary_part3(%d, %d)\n", label1, label2);
	printf("  movl (%%rsp), %%eax\n"
	       "  addq $8, %%rsp\n"
	       ".L%d:\n"
	       "  movl %%eax, (%%rsp)\n",
	       label2);
}

void gen_logical_OR_set(int depth, int label1, int label2)
{
	printf("//gen_logical_OR_set(%d, %d, %d);\n", depth, label1, label2);
	printf("  addq $%d, %%rsp\n", depth * 8);
	printf("  cmpl $0, %d(%%rsp)\n", -depth * 8);
	printf("  jne .L%d\n", label1);
	printf("  subq $%d, %%rsp\n", depth * 8);
}

void gen_logical_OR_final(int final_depth, int label1, int label2)
{
	printf("//gen_logical_OR_final(%d, %d, %d);\n", final_depth, label1,
	       label2);
	printf("  addq $%d, %%rsp\n", final_depth * 8);
	printf("  movl $0, %%eax\n"
	       "  jmp .L%d\n"
	       ".L%d:\n"
	       "  movl $1, %%eax\n"
	       ".L%d:\n"
	       "  movl %%eax, (%%rsp)\n",
	       label2, label1, label2);
}

void gen_logical_AND_set(int depth, int label1, int label2)
{
	printf("//gen_logical_AND_set(%d, %d, %d);\n", depth, label1, label2);
	printf("  addq $%d, %%rsp\n", depth * 8);
	printf("  cmpl $0, %d(%%rsp)\n", -depth * 8);
	printf("  je .L%d\n", label1);
	printf("  subq $%d, %%rsp\n", depth * 8);
}

void gen_logical_AND_final(int final_depth, int label1, int label2)
{
	printf("//gen_logical_AND_final(%d, %d, %d);\n", final_depth, label1,
	       label2);
	printf("  addq $%d, %%rsp\n", final_depth * 8);
	printf("  movl $1, %%eax\n"
	       "  jmp .L%d\n"
	       ".L%d:\n"
	       "  movl $0, %%eax\n"
	       ".L%d:\n"
	       "  movl %%eax, (%%rsp)\n",
	       label2, label1, label2);
}

/* consumes the top of the stack and branch */
void gen_if_else_part1(int label1, int label2)
{
	printf("//gen_if_else_part1(%d, %d);\n", label1, label2);
	printf("  cmpl $0, (%%rsp)\n"
	       "  je .L%d\n"
	       "  addq $8, %%rsp\n",
	       label1);
}

void gen_if_else_part2(int label1, int label2)
{
	printf("//gen_if_else_part2(%d, %d);\n", label1, label2);
	printf("  jmp .L%d\n"
	       ".L%d:\n"
	       "  addq $8, %%rsp\n",
	       label2, label1);
}

void gen_if_else_part3(int label1, int label2)
{
	printf("//gen_if_else_part3(%d, %d);\n", label1, label2);
	printf(".L%d:\n", label2);
}

void gen_push_address_of_local(int offset)
{
	assert(offset < 0);
	printf("//gen_push_address_of_local(%d);\n", offset);
	printf("  subq $8, %%rsp\n"
	       "  leaq %d(%%rbp), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       offset);
}

/*
sall: left shift
sarl: right shift
*/
void gen_shift_ints(const char *str)
{
	printf("//gen_shift_ints(\"%s\")\n", str);
	printf("  movl (%%rsp), %%eax\n"
	       "  movl %%eax, %%ecx\n"
	       "  %s %%cl, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_epilogue(int label)
{
	printf("//gen_epilogue(%d)\n", label);
	printf(".L%d:"
	       "  movl (%%rsp), %%eax\n"
	       "  movq %%rbp, %%rsp\n"
	       "  addq $8, %%rsp\n"
	       "  movq (%%rbp), %%rbp\n"
	       "  ret\n",
	       label);
}
