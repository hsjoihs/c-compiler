#include "print_x86_64.h"
#include "print_x86_64_unofficial.h"
#include "std.h"
#include "std_io.h"

struct __FILE *global_ir;

static void memo2(const char *funcname, const char *fmt, ...)
{
	printf("//%s(", funcname);

#ifdef __STDC__
	va_list ap;
#endif
#ifndef __STDC__
	struct va_list_tag ap[1];
#endif

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);

	printf(");\n");

	if (global_ir) {
		fprintf(global_ir, "%s(", funcname);
		va_start(ap, fmt);
		vfprintf(global_ir, fmt, ap);
		va_end(ap);

		fprintf(global_ir, ");\n");
	}
}

_Noreturn void poison_and_die(const char *msg)
{
	printf(" %%%%%%error detected (%s). Poisoning the assembly:\n", msg);
	exit(EXIT_FAILURE);
}

/*************************
 * prologue and epilogue *
 *************************/
void gen_prologue(int alloc_size, const char *fname)
{
	memo2(__func__, "%d, \"%s\"", alloc_size, fname);
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
	memo2(__func__, "%d, \"%s\"", alloc_size, fname);
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
	memo2(__func__, "%d, %d", n, label_name);
	switch (n) {
	case 1:
	case 4:
		gen_epilogue(label_name);
		break;
	case 8:
		gen_epilogue_8byte(label_name);
		break;
	default:
		poison_and_die("Unsupported width; cannot happen");
	}
}

static void copy_to_8byte_reg(int size, const char *reg_4byte,
                              const char *reg_8byte)
{
	if (size == 8) {
		printf("  movq (%%rcx), %%%s\n", reg_8byte);
		return;
	}
	if (size >= 4) {
		printf("  movl (%%rcx), %%%s\n", reg_4byte);
		if (size == 4) {
			return;
		}

		if (size == 7) {
			puts("  movzwl 4(%rcx), %edi\n"
			     "  movzbl 6(%rcx), %esi\n"
			     "  shll $16, %esi\n"
			     "  orl %esi,  %edi\n"
			     "  shlq $32, %rdi");
		} else if (size == 6) {
			puts("  movzwl 4(%rcx), %edi\n"
			     "  shlq $32, %rdi");
		} else if (size == 5) {
			puts("  movzbl 4(%rcx), %edi\n"
			     "  shlq $32, %rdi");
		}
		printf("  orq %%rdi, %%%s\n", reg_8byte);
	} else if (size == 3) {
		printf("  movzwl (%%rcx), %%%s\n"
		       "  movzbl 2(%%rcx), %%edi\n"
		       "  shll $16, %%edi\n"
		       "  orl %%edi, %%%s\n",
		       reg_4byte, reg_4byte);
	} else if (size == 2) {
		printf("  movzwl (%%rcx), %%%s\n", reg_4byte);
	} else if (size == 1) {
		printf("  movzbl (%%rcx), %%%s\n", reg_4byte);
	}
}

/*
struct Foo *p = pop();
return *p;
*/
void gen_epilogue_returning_integerclass_struct_or_union(int size, int label)
{
	memo2(__func__, "%d, %d", size, label);
	printf(".L%d:", label);

	puts("  movq (%rsp), %rcx");

	if (size > 8) {
		printf("  movq (%%rcx), %%rax\n"
		       "  addq $8, %%rcx\n");
		copy_to_8byte_reg(size - 8, "edx", "rdx");
	} else {
		copy_to_8byte_reg(size, "eax", "rax");
	}
	printf("  leave\n"
	       "  ret\n");
}

void gen_return_garbage(void)
{
	memo2(__func__, "");
	printf("  movl $123, %%eax\n"
	       "  leave\n"
	       "  ret\n");
}

/********************************
 *             push             *
 * decreases stack pointer by 8 *
 ********************************/

void gen_push_int(int num)
{
	memo2(__func__, "%d", num);
	printf("  subq $8, %%rsp\n"
	       "  movl $%d, (%%rsp)\n",
	       num);
}

void gen_push_address_of_local(int offset)
{
	memo2(__func__, "%d", offset);
	assert(offset < 0);
	printf("  subq $8, %%rsp\n"
	       "  leaq %d(%%rbp), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       offset);
}

void gen_push_address_of_str(int strnum)
{
	memo2(__func__, "%d", strnum);
	printf("  subq $8, %%rsp\n"
	       "  leaq L_str%d(%%rip), %%rax\n"
	       "  movq %%rax, (%%rsp)\n",
	       strnum);
}

void gen_push_from_local_nbyte(int n, int offset)
{
	memo2(__func__, "%d", n, offset);
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
		poison_and_die("Unsupported width; cannot happen");
	}
}

/* confirmed in both environments */
void gen_push_address_of_global(const char *ident)
{
	memo2(__func__, "\"%s\"", ident);
	printf("  subq $8, %%rsp\n");
#ifdef OSX
	printf("  movq " PREFIX "%s@GOTPCREL(%%rip), %%rax\n", ident);
#endif
#ifndef OSX
	printf("  leaq " PREFIX "%s(%%rip), %%rax\n", ident);
#endif
	printf("  movq %%rax, (%%rsp)\n");
}

void gen_push_ret_of_nbyte(int n, const char *ident_str, int arg_stacksize)
{
	memo2(__func__, "%d, \"%s\"", n, ident_str);

	switch (n) {
	case 1:
		gen_push_ret_of_1byte(ident_str, arg_stacksize);
		break;
	case 4:
		gen_push_ret_of_4byte(ident_str, arg_stacksize);
		break;
	case 8:
		gen_push_ret_of_8byte(ident_str, arg_stacksize);
		break;
	default:
		poison_and_die("Unsupported width; cannot happen");
	}
}

void gen_push_nullptr(void)
{
	memo2(__func__, "");
	printf("  subq $8, %%rsp\n"
	       "  movq $0, (%%rsp)\n");
}

void gen_call_reg_and_push_ret_of_nbyte(int n, const char *reg)
{
	memo2(__func__, "%d, \"%s\"", n, reg);

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
		poison_and_die("Unsupported width; cannot happen");
	}
}

void gen_peek_deref_push_nbyte(int n)
{
	memo2(__func__, "%d", n);
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
		poison_and_die("Unsupported width; cannot happen");
	}
}

/*************************************
 *             control               *
 * contains jump.                    *
 * does not tweak the stack pointer. *
 *************************************/

void gen_jump(int label, const char *comment)
{
	memo2(__func__, "%d, \"%s\"", label, comment);
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
		poison_and_die("Unsupported width; cannot happen");
	}
}

void gen_if_zero_jmp_nbyte(int n, int label1, int offset)
{
	memo2(__func__, "%d, %d, %d", n, label1, offset);
	printf("  %s $0, %d(%%rsp)\n", cmp(n), offset);
	printf("  je .L%d\n", label1);
}

void gen_if_nonzero_jmp_nbyte(int n, int label1, int offset)
{
	memo2(__func__, "%d, %d, %d", n, label1, offset);
	printf("  %s $0, %d(%%rsp)\n", cmp(n), offset);
	printf("  jne .L%d\n", label1);
}

void gen_if_2nd_matches_int_jmp_4byte(int constant1, int label1)
{
	memo2(__func__, "%d, %d", constant1, label1);
	printf("  cmpl $%d, -8(%%rsp)\n"
	       "  je .L%d\n",
	       constant1, label1);
}

void gen_label(int label1)
{
	memo2(__func__, "%d", label1);
	printf(".L%d:\n", label1);
}

/*************************************
 *         binary operation          *
 * pops twice, does an operation and *
 * pushes the result                 *
 *************************************/

void gen_op_ints(const char *str)
{
	memo2(__func__, "\"%s\"", str);
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
	memo2(__func__, "\"%s\"", str);
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
	memo2(__func__, "");
	printf("  movl 8(%%rsp), %%eax\n"
	       "  imull (%%rsp), %%eax\n"
	       "  movl %%eax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_div_ints(void)
{
	memo2(__func__, "");
	printf("  movl 8(%%rsp), %%eax\n"
	       "  cltd\n"
	       "  idivl (%%rsp)\n"
	       "  movl %%eax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
}

void gen_rem_ints(void)
{
	memo2(__func__, "");
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
	memo2(__func__, "\"%s\"", str);
	printf("  movl (%%rsp), %%eax\n"
	       "  movl %%eax, %%ecx\n"
	       "  %s %%cl, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n",
	       str);
}

void gen_discard(void)
{
	memo2(__func__, "");
	printf("  addq $8, %%rsp\n");
}

void gen_discard2nd(void)
{
	memo2(__func__, "");
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
	memo2(__func__, "\"%s\"", str);
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
	memo2(__func__, "%d", n);
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
		poison_and_die("Unsupported width; cannot happen");
	}
}

void gen_op_8byte(const char *str)
{
	memo2(__func__, "\"%s\"", str);
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
	memo2(__func__, "");
	printf("  movl (%%rsp), %%eax\n"
	       "  cltq\n"
	       "  movq %%rax, (%%rsp)\n");
}

void gen_unary_not(void)
{
	memo2(__func__, "");
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
	memo2(__func__, "\"%s\"", str);
	printf("  %s (%%rsp)\n", str);
}

void gen_extend_to_4byte(void)
{
	memo2(__func__, "");
	printf("  movsbl (%%rsp), %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

void gen_mul_by_const(int mul)
{
	memo2(__func__, "%d", mul);
	printf("  movq (%%rsp), %%rax\n");
	printf((mul == 1 || mul == 2 || mul == 4 || mul == 8)
	           ? "  leaq 0(,%%rax,%d), %%rdx\n"
	           : "  imulq $%d, %%rax, %%rdx\n",
	       mul);

	printf("  movq %%rdx, (%%rsp)\n");
}

void gen_div_by_const(int num)
{
	memo2(__func__, "%d", num);
	if (num == 1) { /* do nothing */
		return;
	}

	printf("//gen_div_by_const(%d)\n", num);
	printf("  movq (%%rsp), %%rax\n");

	switch (num) {
	case 2:
		printf("  sarq $1, %%rax\n");
		printf("  movq %%rax, (%%rsp)\n");
		return;
	case 4:
		printf("  sarq $2, %%rax\n");
		printf("  movq %%rax, (%%rsp)\n");
		return;
	case 8:
		printf("  sarq $3, %%rax\n");
		printf("  movq %%rax, (%%rsp)\n");
		return;
	default:
		printf("  movl $%d, %%ecx\n", num);
		puts("  cqto\n"
		     "  idivq %rcx\n"
		     "  movl %eax, (%rsp)");
		return;
	}
}

void gen_logical_not_of_pointer(void)
{
	memo2(__func__, "");
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
	memo2(__func__, "%d", n);
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
		poison_and_die("Unsupported width; cannot happen");
	}
}

/*************************************
 *          declaration              *
 * declares something                *
 *************************************/

void gen_string_literal(int strnum, const char *str)
{
	memo2(__func__, "%d, escape(\"%s\")", strnum, str);
	printf("L_str%d:\n"
	       ".asciz \"%s\"\n",
	       strnum, str);
}

void gen_global_declaration(const char *ident, int size)
{
	memo2(__func__, "\"%s\", %d", ident, size);
	printf(".comm " PREFIX "%s,%d\n", ident, size);
}

/*************************************
 *            pop to reg             *
 * pops to a register                *
 *************************************/

void gen_pop_to_reg_4byte(const char *str)
{
	memo2(__func__, "\"%s\"", str);
	printf("  movl (%%rsp), %%%s\n", str);
	printf("  addq $8, %%rsp\n");
}

void gen_pop_to_reg_8byte(const char *str)
{
	memo2(__func__, "\"%s\"", str);
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
	memo2(__func__, "\"%s\", %d", str, offset);
	assert(offset < 0);
	printf("  movl %%%s, %%eax\n"
	       "  movb %%al, %d(%%rbp)\n",
	       str, offset);
}
void gen_write_register_to_local_4byte(const char *str, int offset)
{
	memo2(__func__, "\"%s\", %d", str, offset);
	assert(offset < 0);
	printf("  movl %%%s, %d(%%rbp)\n", str, offset);
}

void gen_write_register_to_local_8byte(const char *str, int offset)
{
	memo2(__func__, "\"%s\", %d", str, offset);
	assert(offset < 0);
	printf("  movq %%%s, %d(%%rbp)\n", str, offset);
}

static void gen_memcpy(const char *dst, const char *src, int size);

void gen_call_and_assign_integerclass_struct_or_union_to_local(
    const char *fname, int offset, int size)
{
	memo2(__func__, "\"%s\", %d, %d", fname, offset, size);

	gen_raw_call_partB(PREFIX, fname);
	gen_raw_call_partC();
	printf("  movq %%rdx, (%%rsp)\n"
	       "  subq $8, %%rsp\n"
	       "  movq %%rax, (%%rsp)\n"
	       "  leaq %d(%%rbp), %%rdi\n",
	       offset);
	gen_memcpy("%rdi", "%rsp", size);

	gen_discard();
	gen_discard();
}

void gen_call_reg_and_assign_integerclass_struct_or_union_or_union_to_local(
    const char *regname, int offset, int size)
{
	memo2(__func__, "\"%s\", %d, %d", regname, offset, size);

	gen_raw_call_partB("*%", regname);
	gen_raw_call_partC();
	printf("  movq %%rdx, (%%rsp)\n"
	       "  subq $8, %%rsp\n"
	       "  movq %%rax, (%%rsp)\n"
	       "  leaq %d(%%rbp), %%rdi\n",
	       offset);
	gen_memcpy("%rdi", "%rsp", size);

	gen_discard();
	gen_discard();
}

/*************************************
 *            memcpy                 *
 * pops two pointers and does memcpy *
 *************************************/

/*
struct Foo *dst = pop();
struct Foo *src = pop();

*dst = *src;
*/
void gen_copy_2nd_struct_or_union_to_1st_and_discard(int size)
{
	memo2(__func__, "%d", size);
	gen_memcpy("(%rsp)", "8(%rsp)", size);
	printf("  addq $16, %%rsp\n");
}

/*
struct Foo *src = pop();
struct Foo *dst = pop();

*dst = *src;
*/
void gen_copy_1st_struct_or_union_to_2nd_and_discard(int size)
{
	memo2(__func__, "%d", size);
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
 * va_list *
 ***********/

void gen_write_stack_chk_guard_to_local(int offset)
{
	memo2(__func__, "%d", offset);
#ifdef OSX
	puts("  movq ___stack_chk_guard@GOTPCREL(%rip), %rax\n"
	     "  movq (%rax), %rax");
	printf("  movq %%rax, %d(%%rbp)\n", offset);

#endif

#ifdef LINUX
	printf("  movq %%fs:40, %%rax\n");
	printf("  movq %%rax, %d(%%rbp)\n", offset);
#endif
}

void gen_epilogue_nbyte_with_stack_check(int n, int return_label_name,
                                         int checksum_offset,
                                         int failing_label_name)
{
	memo2(__func__, "%d, %d, %d, %d", n, return_label_name, checksum_offset,
	      failing_label_name);
#ifdef OSX
	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rax");
	printf("  cmpq %d(%%rbp), %%rax\n", checksum_offset);
#endif

#ifdef LINUX
	printf("  movq %d(%%rbp), %%rax\n", checksum_offset);
	printf("  cmpq %%fs:40, %%rax\n");
#endif

	printf("  jne .L%d\n", failing_label_name);
	gen_epilogue_nbyte(n, return_label_name);
	printf(".L%d:\n", failing_label_name);
	printf("  call " PREFIX "__stack_chk_fail\n");
}

void gen_store_regs_to_local(int offset, int start_from, int label_name)
{
	memo2(__func__, "%d, %d, %d", offset, start_from, label_name);
	puts("  testb %al, %al");
	for (int i = start_from; i <= 5; i++) {
		printf("  movq %%%s, %d(%%rbp)\n", get_reg_name_from_arg_pos_8byte(i),
		       offset + i * 8);
	}
	printf("  je .L%d\n", label_name);
	for (int i = 0; i < 8; i++) {
		printf("  movaps %%xmm%d, %d(%%rbp)\n", i, 8 + offset + 40 + 16 * i);
	}
	printf(".L%d:\n", label_name);
}

/* stack top has va_list, and this function will pour contents to there */
void gen_va_start(int gp_offset, int fp_offset, int reg_save_area_offset)
{
	memo2(__func__, "%d, %d, %d", gp_offset, fp_offset, reg_save_area_offset);
	printf("  movq (%%rsp), %%rdx\n");
	printf("  movl $%d,  (%%rdx)\n", gp_offset);
	printf("  movl $%d,  4(%%rdx)\n", fp_offset);

	printf("  leaq 16(%%rbp), %%rax\n"
	       "  movq %%rax, 8(%%rdx)\n");

	printf("  leaq %d(%%rbp), %%rax\n", reg_save_area_offset);
	printf("  movq %%rax, 16(%%rdx)\n");
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
		poison_and_die("tried to pass the 7th argument or later");
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
		poison_and_die("tried to pass the 7th argument or later");
	}
}
