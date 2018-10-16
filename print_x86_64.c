#include "print_x86_64.h"
#include "std.h"
#include "std_io.h"

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

void gen_prologue_static(int alloc_size, const char *fname)
{
	printf("//gen_prologue_static(%d, %s)\n", alloc_size, fname);
	printf(PREFIX "%s:\n"
	              "  pushq %%rbp\n"
	              "  movq %%rsp, %%rbp\n",
	       fname);
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

void gen_discard(void)
{
	printf("//gen_discard()\n");
	printf("  addq $8, %%rsp\n");
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
	printf("//gen_peek_and_dereference_8byte()\n");
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

static void gen_call(const char *s1, const char *s2);

void gen_push_ret_of_1byte(const char *fname)
{
	printf("//gen_push_ret_of_1byte(\"%s\")\n", fname);
	gen_call(PREFIX, fname);

	printf("  movsbl %%al, %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

void gen_push_ret_of_4byte(const char *fname)
{
	printf("//gen_push_ret_of_4byte(\"%s\")\n", fname);
	gen_call(PREFIX, fname);
	printf("  movl %%eax, (%%rsp)\n");
}

void gen_push_ret_of_8byte(const char *fname)
{
	printf("//gen_push_ret_of_8byte(\"%s\")\n", fname);
	gen_call(PREFIX, fname);
	printf("  movq %%rax, (%%rsp)\n");
}

void gen_call_local_fp_and_push_ret_of_1byte(int offset)
{
	printf("//gen_call_local_fp_and_push_ret_of_1byte(%d)\n", offset);
	char str[100];
	sprintf(str, "*%d", offset);
	gen_call(str, "(%rbp)");

	printf("  movsbl %%al, %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

void gen_call_local_fp_and_push_ret_of_4byte(int offset)
{
	printf("//gen_call_local_fp_and_push_ret_of_4byte(%d)\n", offset);
	char str[100];
	sprintf(str, "*%d", offset);
	gen_call(str, "(%rbp)");
	printf("  movl %%eax, (%%rsp)\n");
}

void gen_call_local_fp_and_push_ret_of_8byte(int offset)
{
	printf("//gen_call_local_fp_and_push_ret_of_8byte(%d)\n", offset);
	char str[100];
	sprintf(str, "*%d", offset);
	gen_call(str, "(%rbp)");
	printf("  movq %%rax, (%%rsp)\n");
}

static void gen_call(const char *s1, const char *s2)
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
	printf("  call %s%s\n", s1, s2);

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
}

void gen_call_and_assign_small_struct_to_local(const char *fname, int offset,
                                               int size)
{
	printf("//gen_call_and_assign_small_struct_to_local(%s, %d, %d)\n", fname,
	       offset, size);
	gen_call(PREFIX, fname);
	switch (size) {
	case 16:
		printf("  movq %%rax, %d(%%rbp)\n", offset);
		printf("  movq %%rdx, %d(%%rbp)\n", offset + 8);
		break;
	case 12:
		printf("  movq %%rax, %d(%%rbp)\n", offset);
		printf("  movl %%edx, %d(%%rbp)\n", offset + 8);
		break;
	case 8:
		printf("  movq %%rax, %d(%%rbp)\n", offset);
		break;
	case 4:
		printf("  movl %%eax, %d(%%rbp)\n", offset);
		break;
	default:
		assert0("forbidden struct size as a funccall" && 0);
	}
	gen_discard();
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

static void gen_if_zero_jmp_4byte(int label1, int offset)
{
	printf("//gen_if_zero_jmp_4byte(%d, %d)\n", label1, offset);
	printf("  cmpl $0, %d(%%rsp)\n", offset);
	printf("  je .L%d\n", label1);
}

static void gen_if_nonzero_jmp_4byte(int label1, int offset)
{
	printf("//gen_if_nonzero_jmp_4byte(%d, %d)\n", label1, offset);
	printf("  cmpl $0, %d(%%rsp)\n", offset);
	printf("  jne .L%d\n", label1);
}

static void gen_if_zero_jmp_8byte(int label1, int offset)
{
	printf("//gen_if_zero_jmp_8byte(%d, %d)\n", label1, offset);
	printf("  cmpq $0, %d(%%rsp)\n", offset);
	printf("  je .L%d\n", label1);
}

static void gen_if_nonzero_jmp_8byte(int label1, int offset)
{
	printf("//gen_if_nonzero_jmp_8byte(%d, %d)\n", label1, offset);
	printf("  cmpq $0, %d(%%rsp)\n", offset);
	printf("  jne .L%d\n", label1);
}

static void gen_if_zero_jmp_1byte(int label1, int offset)
{
	printf("//gen_if_zero_jmp_1byte(%d, %d)\n", label1, offset);
	printf("  cmpb $0, %d(%%rsp)\n", offset);
	printf("  je .L%d\n", label1);
}

static void gen_if_nonzero_jmp_1byte(int label1, int offset)
{
	printf("//gen_if_nonzero_jmp_1byte(%d, %d)\n", label1, offset);
	printf("  cmpb $0, %d(%%rsp)\n", offset);
	printf("  jne .L%d\n", label1);
}

void gen_if_zero_jmp_nbyte(int n, int label1, int offset)
{
	switch (n) {
	case 1:
		gen_if_zero_jmp_1byte(label1, offset);
		break;
	case 4:
		gen_if_zero_jmp_4byte(label1, offset);
		break;
	case 8:
		gen_if_zero_jmp_8byte(label1, offset);
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

void gen_if_nonzero_jmp_nbyte(int n, int label1, int offset)
{
	switch (n) {
	case 1:
		gen_if_nonzero_jmp_1byte(label1, offset);
		break;
	case 4:
		gen_if_nonzero_jmp_4byte(label1, offset);
		break;
	case 8:
		gen_if_nonzero_jmp_8byte(label1, offset);
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
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

/*
struct Foo *p = pop();
return *p;
*/
void gen_epilogue_returning_small_struct(int size, int label)
{
	printf("//gen_epilogue_returning_small_struct(%d, %d)\n", size, label);
	printf(".L%d:", label);

	puts("  movq (%rsp), %rcx");
	switch (size) {
	case 16:
		printf("  movq (%%rcx), %%rax\n"
		       "  movq 8(%%rcx), %%rdx\n");
		break;
	case 12:
		printf("  movq (%%rcx), %%rax\n"
		       "  movl 8(%%rcx), %%edx\n");
		break;
	case 8:
		printf("  movq (%%rcx), %%rax\n");
		break;
	case 4:
		printf("  movl (%%rcx), %%eax\n");
		break;
	default:
		assert0("hbjnklsdgf" && 0);
	}

	printf("  leave\n"
	       "  ret\n");
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
		assert0("Unsupported width; cannot happen" && 0);
	}
	printf("  movq %%rax, (%%rsp)\n");
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
#endif
#ifndef OSX
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

void gen_logical_OR_part2(int label1, int label2)
{
	printf("//gen_logical_OR_part2(%d, %d)\n", label1, label2);

	printf("  movl $0, %%eax\n"
	       "  jmp .L%d\n",
	       label2);
	printf(".L%d:\n", label1);
	printf("  movl $1, %%eax\n");
	printf(".L%d:\n", label2);
	printf("  movl %%eax, (%%rsp)\n");
}

void gen_logical_AND_part2(int label1, int label2)
{

	printf("  movl $1, %%eax\n"
	       "  jmp .L%d\n",
	       label2);
	printf(".L%d:\n", label1);
	printf("  movl $0, %%eax\n");
	printf(".L%d:\n", label2);
	printf("  movl %%eax, (%%rsp)\n");
}

void gen_push_from_local_nbyte(int n, int offset)
{
	switch (n) {
	case 1:
		gen_push_from_local_1byte(offset);
		break;
	case 4:
		gen_push_from_local_4byte(offset);
		break;
	case 8:
		gen_push_from_local_8byte(offset);
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

void gen_peek_deref_push_nbyte(int n)
{
	switch (n) {
	case 1:
		gen_peek_deref_push_1byte();
		break;
	case 4:
		gen_peek_deref_push_4byte();
		break;
	case 8:
		gen_peek_deref_push_8byte();
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

void gen_assign_nbyte(int n)
{
	switch (n) {
	case 1:
		gen_assign_1byte();
		break;
	case 4:
		gen_assign_4byte();
		break;
	case 8:
		gen_assign_8byte();
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

void gen_peek_and_dereference_nbyte(int n)
{
	switch (n) {
	case 1:
		gen_peek_and_dereference_1byte();
		break;
	case 4:
		gen_peek_and_dereference_4byte();
		break;
	case 8:
		gen_peek_and_dereference_8byte();
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

void gen_push_ret_of_nbyte(int n, const char *ident_str)
{
	switch (n) {
	case 1:
		gen_push_ret_of_1byte(ident_str);
		break;
	case 4:
		gen_push_ret_of_4byte(ident_str);
		break;
	case 8:
		gen_push_ret_of_8byte(ident_str);
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

void gen_epilogue_nbyte(int n, int label_name)
{
	switch (n) {
	case 1:
	case 4:
		gen_epilogue(label_name);
		break;
	case 8:
		gen_epilogue_8byte(label_name);
		break;
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

const char *get_reg_name_from_arg_pos_4byte(int counter)
{
	switch (counter) {
	case 0:
		return "edi";
	case 1:
		return "esi";
	case 2:
		return "edx";
	case 3:
		return "ecx";
	case 4:
		return "r8d";
	case 5:
		return "r9d";
	default:
		assert0("cannot happen" && 0);
	}
}

const char *get_reg_name_from_arg_pos_8byte(int counter)
{
	switch (counter) {
	case 0:
		return "rdi";
	case 1:
		return "rsi";
	case 2:
		return "rdx";
	case 3:
		return "rcx";
	case 4:
		return "r8";
	case 5:
		return "r9";
	default:
		assert0("cannot happen" && 0);
	}
}

void gen_push_nullptr(void)
{
	printf("//gen_push_nullptr()\n");
	printf("  subq $8, %%rsp\n"
	       "  movq $0, (%%rsp)\n");
}

void gen_if_neg8_matches_jmp_4byte(int constant1, int label1)
{
	printf("//gen_if_neg8_matches_jmp_4byte(%d, %d)\n", constant1, label1);
	printf("  cmpl $%d, -8(%%rsp)\n"
	       "  je .L%d\n",
	       constant1, label1);
}

void gen_extend_to_4byte(void)
{
	printf("//gen_extend_to_4byte()\n");
	printf("  movsbl (%%rsp), %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

/*
<  : setb
<= : setbe
>  : seta
>= : setnb
== : sete
!= : setne
*/
void gen_compare_ptrs(const char *str)
{
	printf("//gen_compare_ptrs(\"%s\")\n", str);
	puts("  movq 8(%rsp), %rax\n"
	     "  cmpq (%rsp), %rax");
	printf("  %s %%al\n", str);
	puts("  movzbl %al, %eax\n"
	     "  movl %eax, 8(%rsp)\n"
	     "  addq $8, %rsp");
}

static void copy_8bytes_of_struct(int offset)
{
	printf("  movq (%%rsp), %%rdx\n"
	       "  movq %d(%%rdx), %%rax\n"
	       "  movq 8(%%rsp), %%rdx\n"
	       "  movq %%rax, %d(%%rdx)\n",
	       offset, offset);
}

static void copy_4bytes_of_struct(int offset)
{
	printf("  movq (%%rsp), %%rdx\n"
	       "  movl %d(%%rdx), %%eax\n"
	       "  movq 8(%%rsp), %%rdx\n"
	       "  movl %%eax, %d(%%rdx)\n",
	       offset, offset);
}

static void copy_1byte_of_struct(int offset)
{
	printf("  movq (%%rsp), %%rdx\n"
	       "  movzbl %d(%%rdx), %%eax\n"
	       "  movq 8(%%rsp), %%rdx\n"
	       "  movb %%al, %d(%%rdx)\n",
	       offset, offset);
}

/*
struct Foo *q = pop();
struct Foo *p = pop();

*p = *q;
*/
void gen_copy_struct_and_discard(int size)
{
	printf("//gen_copy_struct_and_discard(%d)\n", size);
	int next_copy_begins_at = 0;
	while (1) {
		if (size - next_copy_begins_at >= 8) {
			copy_8bytes_of_struct(next_copy_begins_at);
			next_copy_begins_at += 8;
		} else if (size - next_copy_begins_at >= 4) {
			copy_4bytes_of_struct(next_copy_begins_at);
			next_copy_begins_at += 4;
		} else if (size - next_copy_begins_at >= 1) {
			copy_1byte_of_struct(next_copy_begins_at);
			next_copy_begins_at += 1;
		} else {
			break;
		}
	}
	gen_discard();
	gen_discard();
}

void gen_logical_not_of_pointer(void)
{
	printf("//gen_logical_not_of_pointer()\n");
	puts("  cmpq $0, (%rsp)\n"
	     "  sete %al\n"
	     "  movzbl %al, %eax\n"
	     "  movl %eax, (%rsp)");
}
