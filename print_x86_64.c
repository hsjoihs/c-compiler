#include "print_x86_64.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

void gen_after_prologue(int label1, int label2)
{
	printf("//gen_after_prologue(%d, %d)\n", label1, label2);
	printf("  jmp .L%d\n", label1);
	printf(".L%d:\n", label2);
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

void gen_write_to_local_1byte(int offset)
{
	assert(offset < 0);
	printf("//gen_write_to_local_1byte(%d)\n", offset);
	printf("  movb (%%rsp), %%al\n");
	printf("  movb %%al, %d(%%rbp)\n", offset);
}

/* write to local mem what's in the register */
void gen_write_register_to_local_4byte(const char *str, int offset)
{
	assert(offset < 0);
	printf("//gen_write_register_to_local_4byte(%s, %d)\n", str, offset);
	printf("  movl %%%s, %d(%%rbp)\n", str, offset);
}

void gen_write_register_to_local_8byte(const char *str, int offset)
{
	assert(offset < 0);
	printf("//gen_write_register_to_local_8byte(%s, %d)\n", str, offset);
	printf("  movq %%%s, %d(%%rbp)\n", str, offset);
}

void gen_write_register_to_local_1byte(const char *str, int offset)
{
	assert(offset < 0);
	printf("//gen_write_register_to_local_1byte(%s, %d)\n", str, offset);
	printf("  movl %%%s, %%eax\n"
	       "  movb %%al, %d(%%rbp)\n",
	       str, offset);
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
void gen_push_from_local_4byte(int offset)
{
	assert(offset < 0);
	printf("//gen_push_from_local_4byte(%d)\n", offset);
	printf("  subq $8, %%rsp\n"
	       "  movl %d(%%rbp), %%eax\n"
	       "  movl %%eax, (%%rsp)\n",
	       offset);
}

/*
 dereference what's at the top of the stack.
 */
void gen_peek_and_dereference_4byte(void)
{
	printf("//gen_peek_and_dereference_4byte()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movl (%rax), %eax\n"
	     "  movl  %eax, (%rsp)");
}

void gen_peek_deref_push_4byte(void)
{
	printf("//gen_peek_deref_push_4byte()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movl (%rax), %edx\n"
	     "  subq $8, %rsp\n"
	     "  movq %rdx, (%rsp)\n");
}

/*
 dereference what's at the top of the stack.
  */
void gen_peek_and_dereference_8byte(void)
{
	printf("//gen_peek_and_dereference()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movq (%rax), %rax\n"
	     "  movq  %rax, (%rsp)");
}

void gen_peek_deref_push_8byte(void)
{
	printf("//gen_peek_deref_push_8byte()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movq (%rax), %rdx\n"
	     "  subq $8, %rsp\n"
	     "  movq %rdx, (%rsp)\n");
}

void gen_peek_and_dereference_1byte(void)
{
	printf("//gen_peek_and_dereference_1byte()\n");
	printf("  movq (%%rsp), %%rax\n"
	       "  movsbl (%%rax), %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

void gen_peek_deref_push_1byte(void)
{
	printf("//gen_peek_deref_push_1byte()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movb (%rax), %dl\n"
	     "  subq $8, %rsp\n"
	     "  movb %dl, (%rsp)\n");
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

void gen_push_from_local_1byte(int offset)
{
	assert(offset < 0);
	printf("//gen_push_from_local_1byte(%d)\n", offset);
	printf("  subq $8, %%rsp\n"
	       "  movsbl %d(%%rbp), %%eax\n"
	       "  movl %%eax, (%%rsp)\n",
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

void gen_push_ret_of_1byte(const char *fname)
{
	printf("//gen_push_ret_of_1byte(\"%s\")\n", fname);
	gen_push_ret_of_(fname, 1);
}

void gen_push_ret_of_4byte(const char *fname)
{
	printf("//gen_push_ret_of_4byte(\"%s\")\n", fname);
	gen_push_ret_of_(fname, 4);
}

void gen_push_ret_of_8byte(const char *fname)
{
	printf("//gen_push_ret_of_8byte(\"%s\")\n", fname);
	gen_push_ret_of_(fname, 8);
}

void gen_push_ret_of_(const char *fname, int byte)
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
	       "  movq %%rax, (%%rsp)\n"
	       "  movb $0, %%al\n" /* printf */
	);
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
	if (byte == 8) {
		printf("  movq %%rax, (%%rsp)\n");
	} else if (byte == 4) {
		printf("  movl %%eax, (%%rsp)\n");
	} else if (byte == 1) {
		printf("  movsbl %%al, %%eax\n"
		       "  movl %%eax, (%%rsp)\n");
	}
}

void gen_pop_to_reg_4byte(const char *str)
{
	printf("//gen_pop_to_reg_4byte(%s)\n", str);
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
	       "  %s %%eax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_op_8byte(const char *str)
{
	printf("//gen_op_8byte(\"%s\")\n", str);
	printf("  movq (%%rsp), %%rax\n"
	       "  %s %%rax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_discard2nd_8byte(void)
{
	printf("//gen_discard2nd_8byte()\n");
	printf("  movq (%%rsp), %%rax\n"
	       "  movq %%rax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_discard3rd_8byte(void)
{
	printf("//gen_discard3rd_8byte()\n");
	printf("  movq 8(%%rsp), %%rax\n"
	       "  movq %%rax, 16(%%rsp)\n"
	       "  movq (%%rsp), %%rax\n"
	       "  movq %%rax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_mul_ints(void)
{
	printf("//gen_mul_ints()\n");
	printf("  movl 8(%%rsp), %%eax\n"
	       "  imull (%%rsp), %%eax\n"
	       "  movl %%eax, 8(%%rsp)\n"
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
	printf("  movl 8(%%rsp), %%eax\n"
	       "  cltd\n"
	       "  idivl (%%rsp)\n"
	       "  movl %%eax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_rem_ints(void)
{
	printf("//gen_rem_ints()\n");
	printf("  movl 8(%%rsp), %%eax\n"
	       "  cltd\n"
	       "  idivl (%%rsp)\n"
	       "  movl %%edx, 8(%%rsp)\n"
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
	printf("  movl 8(%%rsp), %%eax\n"
	       "  cmpl (%%rsp), %%eax\n"
	       "  %s %%al\n"
	       "  movzbl %%al, %%eax\n"
	       "  movl %%eax, 8(%%rsp)\n"
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

void gen_logical_AND_set(int depth, int label1)
{
	printf("//gen_logical_AND_set(%d, %d);\n", depth, label1);
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
	       "  %s %%cl, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_before_epilogue(int label1, int label2, int capacity)
{
	printf("//gen_before_epilogue(%d, %d, %d)\n", label1, label2, capacity);
	printf(".L%d:\n", label1);
	printf("  subq $%d, %%rsp\n", capacity);
	printf("  jmp .L%d\n", label2);
}

/* for both 4byte and 1byte */
void gen_epilogue(int label)
{
	printf("//gen_epilogue(%d)\n", label);
	printf(".L%d:"
	       "  movl (%%rsp), %%eax\n"
	       "  leave\n"
	       "  ret\n",
	       label);
}

void gen_epilogue_8byte(int label)
{
	printf("//gen_epilogue_8byte(%d)\n", label);
	printf(".L%d:"
	       "  movq (%%rsp), %%rax\n"
	       "  leave\n"
	       "  ret\n",
	       label);
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

void gen_write_to_global_8byte(const char *ident)
{
	printf("//gen_write_to_global_8byte(\"%s\")\n", ident);
	printf("  movq (%%rsp), %%rax\n"
	       "  movq %%rax, " PREFIX "%s(%%rip)\n",
	       ident);
}

void gen_write_to_global_4byte(const char *ident)
{
	printf("//gen_write_to_global_4byte(\"%s\")\n", ident);
	printf("  movl (%%rsp), %%eax\n"
	       "  movl %%eax, " PREFIX "%s(%%rip)\n",
	       ident);
}

void gen_push_from_global_8byte(const char *ident)
{
	printf("//gen_push_from_global_8byte(\"%s\")\n", ident);
	printf("  subq $8, %%rsp\n"
	       "  movq " PREFIX "%s(%%rip), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       ident);
}

void gen_push_from_global_4byte(const char *ident)
{
	printf("//gen_push_from_global_4byte(\"%s\")\n", ident);
	printf("  subq $8, %%rsp\n"
	       "  movl " PREFIX "%s(%%rip), %%eax\n"
	       "  movl %%eax, (%%rsp)\n",
	       ident);
}

void gen_push_from_global_1byte(const char *ident)
{
	printf("//gen_push_from_global_4byte(\"%s\")\n", ident);
	printf("  subq $8, %%rsp\n"
	       "  movsbl " PREFIX "%s(%%rip), %%eax\n"
	       "  movb %%al, (%%rsp)\n",
	       ident);
}

void gen_global_declaration(const char *ident, int size)
{
	printf("//gen_global_declaration(\"%s\", %d)\n", ident, size);
	printf(".comm " PREFIX "%s,%d\n", ident, size);
}

/* confirmed in both environments */
void gen_push_address_of_global(const char *ident)
{
	printf("//gen_push_address_of_global(\"%s\");\n", ident);
	printf("  subq $8, %%rsp\n");
#ifdef OSX
	printf("  movq " PREFIX "%s@GOTPCREL(%%rip), %%rax\n", ident);
#else
	printf("  leaq " PREFIX "%s(%%rip), %%rax\n", ident);
#endif
	printf("  movq %%rax, (%%rsp)\n");
}

/*
    value = pop();
    addr = pop();
    *addr = value;
    push(value);
*/
void gen_assign_8byte(void)
{
	printf("//gen_assign_8byte()\n");
	printf("  movq (%%rsp), %%rax\n");
	printf("  movq 8(%%rsp), %%rdx\n");

	printf("  movq %%rax, (%%rdx)\n"
	       "  addq $8, %%rsp\n"
	       "  movq %%rax, (%%rsp)\n");
}

/*
    value = pop();
    addr = pop();
    *addr = value;
    push(value);
*/
void gen_assign_4byte(void)
{
	printf("//gen_assign_4byte()\n");
	printf("  movl (%%rsp), %%eax\n");
	printf("  movq 8(%%rsp), %%rdx\n");

	printf("  movl %%eax, (%%rdx)\n"
	       "  addq $8, %%rsp\n"
	       "  movl %%eax, (%%rsp)\n");
}

/*
    value = pop();
    addr = pop();
    *addr = value;
    push(value);
*/
void gen_assign_1byte(void)
{
	printf("//gen_assign_1byte()\n");
	printf("  movb (%%rsp), %%al\n");
	printf("  movq 8(%%rsp), %%rdx\n");

	printf("  movb %%al, (%%rdx)\n");
	printf("  addq $8, %%rsp\n");
	printf("  movb %%al, (%%rsp)\n");
}

void gen_str(int strnum, const char *str)
{
	printf("//gen_str(%d, \"%s\")\n", strnum, str);
	printf("L_str%d:\n"
	       ".asciz \"%s\"\n",
	       strnum, str);
}

void gen_push_address_of_str(int strnum)
{
	printf("//gen_push_address_of_str(%d)\n", strnum);
	printf("  subq $8, %%rsp\n"
	       "  leaq L_str%d(%%rip), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       strnum);
}

void gen_logical_OR_part1(int label1)
{
	printf("//gen_logical_OR_part1(%d)\n", label1);
	printf("  cmpl $0, (%%rsp)\n");
	printf("  jne .L%d\n", label1);
}

void gen_logical_OR_part2(int label1, int label2)
{
	printf("//gen_logical_OR_part2(%d, %d)\n", label1, label2);
	printf("  addq $8, %%rsp\n"
	       "  cmpl $0, -8(%%rsp)\n"
	       "  jne .L%d\n",
	       label1);
	printf("  movl $0, %%eax\n"
	       "  jmp .L%d\n",
	       label2);
	printf(".L%d:\n", label1);
	printf("  movl $1, %%eax\n");
	printf(".L%d:\n", label2);
	printf("  movl %%eax, (%%rsp)\n");
}
