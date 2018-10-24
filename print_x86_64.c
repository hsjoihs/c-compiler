#include "print_x86_64.h"
#include "print_x86_64_unofficial.h"
#include "std.h"
#include "std_io.h"

/*************************
 * prologue and epilogue *
 *************************/
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

void return_garbage(void)
{
	printf("  movl $123, %%eax\n"
	       "leave\n"
	       "ret\n");
}

/********************************
 *             push             *
 * decreases stack pointer by 8 *
 ********************************/

void gen_push_int(int num)
{
	printf("//gen_push_int(%d)\n", num);
	printf("  subq $8, %%rsp\n"
	       "  movl $%d, (%%rsp)\n",
	       num);
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

void gen_push_address_of_str(int strnum)
{
	printf("//gen_push_address_of_str(%d)\n", strnum);
	printf("  subq $8, %%rsp\n"
	       "  leaq L_str%d(%%rip), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       strnum);
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

void gen_push_nullptr(void)
{
	printf("//gen_push_nullptr()\n");
	printf("  subq $8, %%rsp\n"
	       "  movq $0, (%%rsp)\n");
}

void gen_call_reg_and_push_ret_of_nbyte(int n, const char *reg)
{
	switch (n) {
	case 1:
		gen_call_reg_and_push_ret_of_1byte(reg);
		break;
	case 4:
		gen_call_reg_and_push_ret_of_4byte(reg);
		break;
	case 8:
		gen_call_reg_and_push_ret_of_8byte(reg);
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

/*************************************
 *             control               *
 * contains jump.                    *
 * does not tweak the stack pointer. *
 *************************************/

void gen_jump(int label, const char *comment)
{
	printf("//comment: %s\n", comment);
	printf("  jmp .L%d\n", label);
}

static const char *cmp(int n)
{
	switch (n) {
	case 1:
		return "cmpb";
	case 4:
		return "cmpl";
	case 8:
		return "cmpq";
	default:
		assert0("Unsupported width; cannot happen" && 0);
	}
}

void gen_if_zero_jmp_nbyte(int n, int label1, int offset)
{
	printf("//gen_if_zero_jmp_nbyte(%d, %d, %d)\n", n, label1, offset);
	printf("  %s $0, %d(%%rsp)\n", cmp(n), offset);
	printf("  je .L%d\n", label1);
}

void gen_if_nonzero_jmp_nbyte(int n, int label1, int offset)
{
	printf("//gen_if_nonzero_jmp_1byte(%d, %d, %d)\n", n, label1, offset);
	printf("  %s $0, %d(%%rsp)\n", cmp(n), offset);
	printf("  jne .L%d\n", label1);
}

void gen_if_2nd_matches_int_jmp_4byte(int constant1, int label1)
{
	printf("//gen_if_2nd_matches_int_jmp_4byte(%d, %d)\n", constant1, label1);
	printf("  cmpl $%d, -8(%%rsp)\n"
	       "  je .L%d\n",
	       constant1, label1);
}

void gen_label(int label1)
{
	printf("//gen_label(%d)\n", label1);
	printf(".L%d:\n", label1);
}

/*************************************
 *         binary operation          *
 * pops twice, does an operation and *
 * pushes the result                 *
 *************************************/

void gen_op_ints(const char *str)
{
	printf("//gen_op_ints(\"%s\")\n", str);
	printf("  movl (%%rsp), %%eax\n"
	       "  %s %%eax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
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

void gen_mul_ints(void)
{
	printf("//gen_mul_ints()\n");
	printf("  movl 8(%%rsp), %%eax\n"
	       "  imull (%%rsp), %%eax\n"
	       "  movl %%eax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
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

void gen_discard(void)
{
	printf("//gen_discard()\n");
	printf("  addq $8, %%rsp\n");
}

void gen_discard2nd(void)
{
	printf("//gen_discard2nd()\n");
	printf("  movq (%%rsp), %%rax\n"
	       "  movq %%rax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
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

/*
    value = pop();
    addr = pop();
    *addr = value;
    push(value);
*/
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

void gen_op_8byte(const char *str)
{
	printf("//gen_op_8byte(\"%s\")\n", str);
	printf("  movq (%%rsp), %%rax\n"
	       "  %s %%rax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

/*************************************
 *          unary operation          *
 * pops once, does an operation and  *
 * pushes the result                 *
 *************************************/

void gen_cltq(void)
{
	printf("//gen_cltq()\n");
	printf("  movl (%%rsp), %%eax\n"
	       "  cltq\n"
	       "  movq %%rax, (%%rsp)\n");
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

void gen_extend_to_4byte(void)
{
	printf("//gen_extend_to_4byte()\n");
	printf("  movsbl (%%rsp), %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
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

void gen_logical_not_of_pointer(void)
{
	printf("//gen_logical_not_of_pointer()\n");
	puts("  cmpq $0, (%rsp)\n"
	     "  sete %al\n"
	     "  movzbl %al, %eax\n"
	     "  movl %eax, (%rsp)");
}

/*
 dereference what's at the top of the stack.
 */
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

/*************************************
 *          declaration              *
 * declares something                *
 *************************************/

void gen_string_literal(int strnum, const char *str)
{
	printf("//gen_string_literal(%d, \"%s\")\n", strnum, str);
	printf("L_str%d:\n"
	       ".asciz \"%s\"\n",
	       strnum, str);
}

void gen_global_declaration(const char *ident, int size)
{
	printf("//gen_global_declaration(\"%s\", %d)\n", ident, size);
	printf(".comm " PREFIX "%s,%d\n", ident, size);
}

/*************************************
 *            pop to reg             *
 * pops to a register                *
 *************************************/

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

/*************************************
 *          write to local           *
 * does not modify the stack         *
 *************************************/

/* write to local mem what's in the register */
void gen_write_register_to_local_1byte(const char *str, int offset)
{
	assert(offset < 0);
	printf("//gen_write_register_to_local_1byte(%s, %d)\n", str, offset);
	printf("  movl %%%s, %%eax\n"
	       "  movb %%al, %d(%%rbp)\n",
	       str, offset);
}
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

void gen_call_and_assign_small_struct_to_local(const char *fname, int offset,
                                               int size)
{
	printf("//gen_call_and_assign_small_struct_to_local(%s, %d, %d)\n", fname,
	       offset, size);
	gen_raw_call(PREFIX, fname);
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

/*************************************
 *            memcpy                 *
 * pops two pointers and does memcpy *
 *************************************/
static void gen_memcpy(const char *dst, const char *src, int size);

/*
struct Foo *p = pop();
struct Foo *q = pop();

*p = *q;
*/
void gen_copy_2nd_struct_to_1st_and_discard(int size)
{
	printf("//gen_copy_2nd_struct_to_1st_and_discard(%d)\n", size);
	gen_memcpy("(%rsp)", "8(%rsp)", size);
	printf("  addq $16, %%rsp\n");
}

/*
struct Foo *q = pop();
struct Foo *p = pop();

*p = *q;
*/
void gen_copy_1st_struct_to_2nd_and_discard(int size)
{
	printf("//gen_copy_1st_struct_to_2nd_and_discard(%d)\n", size);
	gen_memcpy("8(%rsp)", "(%rsp)", size);
	printf("  addq $16, %%rsp\n");
}

/*
 */
static void gen_memcpy(const char *dst, const char *src, int size)
{
	int next_copy_begins_at = 0;
	while (1) {
		if (size - next_copy_begins_at >= 8) {
			printf("  movq %s, %%rdx\n"
			       "  movq %d(%%rdx), %%rax\n"
			       "  movq %s, %%rdx\n"
			       "  movq %%rax, %d(%%rdx)\n",
			       src, next_copy_begins_at, dst, next_copy_begins_at);
			next_copy_begins_at += 8;
		} else if (size - next_copy_begins_at >= 4) {
			printf("  movq %s, %%rdx\n"
			       "  movl %d(%%rdx), %%eax\n"
			       "  movq %s, %%rdx\n"
			       "  movl %%eax, %d(%%rdx)\n",
			       src, next_copy_begins_at, dst, next_copy_begins_at);
			next_copy_begins_at += 4;
		} else if (size - next_copy_begins_at >= 1) {
			printf("  movq %s, %%rdx\n"
			       "  movzbl %d(%%rdx), %%eax\n"
			       "  movq %s, %%rdx\n"
			       "  movb %%al, %d(%%rdx)\n",
			       src, next_copy_begins_at, dst, next_copy_begins_at);
			next_copy_begins_at += 1;
		} else {
			break;
		}
	}
}

/***********
 * regname *
 ***********/

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
