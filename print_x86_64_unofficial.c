#include "print_x86_64_unofficial.h"
#include "print_x86_64.h"
#include "std.h"
#include "std_io.h"

/*******************
 * unofficial APIs *
 *******************/

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

void gen_peek_deref_push_4byte(void)
{
	printf("//gen_peek_deref_push_4byte()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movl (%rax), %edx\n"
	     "  subq $8, %rsp\n"
	     "  movq %rdx, (%rsp)\n");
}

void gen_peek_and_dereference_4byte(void)
{
	printf("//gen_peek_and_dereference_4byte()\n");
	puts("  movq (%rsp), %rax \n"
	     "  movl (%rax), %eax\n"
	     "  movl  %eax, (%rsp)");
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

void gen_swap(void)
{
	printf("//gen_swap()\n");
	printf("movq (%%rsp), %%rax\n"
	       "movq 8(%%rsp), %%rdx\n"
	       "movq %%rdx, (%%rsp)\n"
	       "movq %%rax, 8(%%rsp)\n");
}

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

void gen_call_reg_and_push_ret_of_1byte(const char *reg)
{
	printf("//gen_call_reg_and_push_ret_of_1byte(\"%s\")\n", reg);
	gen_call("*%", reg);
	printf("  movsbl %%al, %%eax\n"
	       "  movl %%eax, (%%rsp)\n");
}

void gen_call_reg_and_push_ret_of_4byte(const char *reg)
{
	printf("//gen_call_reg_and_push_ret_of_4byte(\"%s\")\n", reg);
	gen_call("*%", reg);
	printf("  movl %%eax, (%%rsp)\n");
}

void gen_call_reg_and_push_ret_of_8byte(const char *reg)
{
	printf("//gen_call_reg_and_push_ret_of_8byte(\"%s\")\n", reg);
	gen_call("*%", reg);
	printf("  movq %%rax, (%%rsp)\n");
}

void gen_call(const char *s1, const char *s2)
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

void gen_discard3rd(void)
{
	printf("//gen_discard3rd()\n");
	printf("  movq 8(%%rsp), %%rax\n"
	       "  movq %%rax, 16(%%rsp)\n"
	       "  movq (%%rsp), %%rax\n"
	       "  movq %%rax, 8(%%rsp)\n"
	       "  addq $8, %%rsp\n");
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
