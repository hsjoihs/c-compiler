#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

void gen_write_register_to_local_8byte(const char *str, int offset)
{
	assert(offset < 0);
	printf("//gen_write_register_to_local_8byte(%s, %d)\n", str, offset);
	printf("  movq %%%s, %d(%%rbp)\n", str, offset);
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

/*
 dereference what's at the top of the stack.
 backs up the address to -8(%rbp).
 */
void gen_peek_and_dereference(void)
{
	printf("//gen_peek_and_dereference()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movq %rax, -8(%rbp) //backup\n"
	     "  movl (%rax), %eax\n"
	     "  movl  %eax, (%rsp)");
}

/*
 dereference what's at the top of the stack.
 backs up the address to -8(%rbp).
  */
void gen_peek_and_dereference_8byte(void)
{
	printf("//gen_peek_and_dereference()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movq %rax, -8(%rbp) //backup\n"
	     "  movq (%rax), %rax\n"
	     "  movq  %rax, (%rsp)");
}

/*
    value = pop();
    addr = backup;
    *addr = value;
    push(value);
*/
void gen_assign_to_backed_up_address(void)
{
	printf("//gen_assign_to_backed_up_address()\n");
	puts("  movq (%rsp), %rax\n"
	     "  movq -8(%rbp), %rbx\n"
	     "  movq %rax, (%rbx)\n"
	     "  movq %rax, (%rsp)");
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

void gen_swap(void)
{
	printf("//gen_swap()\n");
	printf("movq (%%rsp), %%rax\n"
	       "movq 8(%%rsp), %%rdx\n"
	       "movq %%rdx, (%%rsp)\n"
	       "movq %%rax, 8(%%rsp)\n");
}

void gen_push_ret_of_(const char *fname, int is8byte);

void gen_push_ret_of(const char *fname)
{
	printf("//gen_push_ret_of(\"%s\")\n", fname);
	gen_push_ret_of_(fname, 0);
}

void gen_push_ret_of_8byte(const char *fname)
{
	printf("//gen_push_ret_of_8byte(\"%s\")\n", fname);
	gen_push_ret_of_(fname, 1);
}

void gen_push_ret_of_(const char *fname, int is8byte)
{

	/* alignment */

	/*
	if it is already aligned:
	    `subq $8, %rsp` will de-align; mod 16 gives 8.
	    Thus, `subq %rax, %rsp\n` subtracts another 8.
	    `movq %rax, (%rsp)` puts 8 on top of the stack.
	*/

	/*
	if it is not aligned:
	    `subq $8, %rsp` will align; mod 16 gives 0.
	    Thus, `subq %rax, %rsp\n` will not subtract.
	    `movq %rax, (%rsp)` puts 0 on top of the stack.
	*/
	printf("  subq $8, %%rsp\n"
	       "  movq %%rsp, %%rax\n"
	       "  andq $15, %%rax\n"
	       "  subq %%rax, %%rsp\n"
	       "  movq %%rax, (%%rsp)\n");
	printf("  call " PREFIX "%s\n", fname);

	/*
	if it was already aligned:
	    the top contains 8, and you must add 16 to the stack in order to resume.
	    Since this function is a `push`, you need to subtract 8 to push the
	returned value. Hence, you only need to add 8.
	*/
	/*
	if it was not aligned:
	    the top contains 0, and you must add 8 to the stack in order to resume.
	    Since this function is a `push`, you need to subtract 8 to push the
	returned value. Hence, you only need to add 0.
	*/
	printf("  addq (%%rsp), %%rsp\n");
	if (is8byte) {
		printf("  movq %%rax, (%%rsp)\n");
	} else {
		printf("  movl %%eax, (%%rsp)\n");
	}
}

void gen_pop_to_reg(const char *str)
{
	printf("//gen_pop_to_reg(%s)\n", str);
	printf("  movl (%%rsp), %%%s\n", str);
	printf("  addq $8, %%rsp\n");
}

void gen_pop_to_reg_8byte(const char *str)
{
	printf("//gen_pop_to_reg_8byte(%s)\n", str);
	printf("  movq (%%rsp), %%%s\n", str);
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

void gen_op_8byte(const char *str)
{
	printf("//gen_op_8byte(\"%s\")\n", str);
	printf("  movq (%%rsp), %%rax\n"
	       "  %s %%rax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_discard2nd_8byte(void)
{
	printf("//gen_discard2nd_8byte()\n");
	printf("  movq (%%rsp), %%rax\n"
	       "  movq %%rax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_mul_ints(void)
{
	printf("//gen_mul_ints()\n");
	printf("  movl +8(%%rsp), %%eax\n"
	       "  imull (%%rsp), %%eax\n"
	       "  movl %%eax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_jump(int label, const char *comment)
{
	printf("//comment: %s\n", comment);
	printf("  jmp .L%d\n", label);
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

void gen_epilogue_8byte(int label)
{
	printf("//gen_epilogue_8byte(%d)\n", label);
	printf(".L%d:"
	       "  movq (%%rsp), %%rax\n"
	       "  movq %%rbp, %%rsp\n"
	       "  addq $8, %%rsp\n"
	       "  movq (%%rbp), %%rbp\n"
	       "  ret\n",
	       label);
}

/*
    value = pop();
    b = pop();
    local[offset] = b;
    push(value);
*/
void gen_pop2nd_to_local_8byte(int offset)
{
	printf("//gen_pop2nd_to_local_8byte(%d)\n", offset);
	printf("  movq 8(%%rsp), %%rbx\n"
	       "  movq %%rbx, %d(%%rbp)\n"
	       "  movq (%%rsp), %%rax\n"
	       "  movq %%rax, +8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       offset);
}

void gen_cltq(void)
{
	printf("//gen_cltq()\n");
	printf("  movl (%%rsp), %%eax\n"
	       "  cltq\n"
	       "  movq %%rax, (%%rsp)\n");
}

void gen_mul_by_const(int mul)
{
	printf("//gen_mul_by_const(%d)\n", mul);
	printf("  movq (%%rsp), %%rax\n");
	printf((mul == 1 || mul == 2 || mul == 4 || mul == 8)
	           ? "  leaq 0(,%%rax,%d), %%rdx\n"
	           : "  imulq $%d, %%rax, %%rdx\n",
	       mul);

	printf("  movq %%rdx, (%%rsp)\n");
}

void gen_div_by_const(int num)
{
	printf("//gen_div_by_const(%d)\n", num);
	printf("  movq (%%rsp), %%rax\n");

	switch (num) {
		case 1: /* do nothing */
			break;
		case 2:
			printf("  sarq $1, %%rax\n");
			break;
		case 4:
			printf("  sarq $2, %%rax\n");
			break;
		case 8:
			printf("  sarq $3, %%rax\n");
			break;
		default:
			fprintf(stderr, "Unsupported width\n");
			exit(EXIT_FAILURE);
	}
	printf("  movq %%rax, (%%rsp)\n");
}
