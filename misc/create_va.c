#include "../print_x86_64.h"
#include "../print_x86_64_unofficial.h"
#include <stdio.h>

void gen_store_regs_to_local(int offset, int start_from, const char *label_name)
{
	puts("  testb %al, %al");
	for (int i = start_from; i <= 5; i++) {
		printf("  movq %%%s, %d(%%rbp)\n", get_reg_name_from_arg_pos_8byte(i),
		       offset + i * 8);
	}
	printf("  je %s\n", label_name);
	for (int i = 0; i < 8; i++) {
		printf("  movaps %%xmm%d, %d(%%rbp)\n", i, 8 + offset + 40 + 16 * i);
	}
	printf("%s:\n", label_name);
}

void gen_push_8byte(const char *num_as_str)
{
	printf("  subq $8, %%rsp\n"
	       "  movq $%s, %%rdx\n"
	       "  movq %%rdx, (%%rsp)\n",
	       num_as_str);
}

void gen_write_local_to_register_8byte(int offset, const char *str)
{
	printf("  movq %d(%%rbp), %%%s\n", offset, str);
}

void gen_write_to_reg_8byte(const char *str)
{
	printf("  movq (%%rsp), %%%s\n", str);
}

void gen_initialize_va_list(int dst_struct_offset, int gp_offset, int fp_offset,
                            int reg_save_area_offset)
{
	gen_push_int(gp_offset);
	gen_write_to_local(dst_struct_offset);
	gen_discard();

	gen_push_int(fp_offset);
	gen_write_to_local(dst_struct_offset + 4);
	gen_discard();

	printf("  subq $8, %%rsp\n"
	       "  leaq 16(%%rbp), %%rax\n"
	       "  movq %%rax, (%%rsp)\n");
	gen_write_to_local_8byte(dst_struct_offset + 8);
	gen_discard();

	gen_push_address_of_local(reg_save_area_offset);
	gen_write_to_local_8byte(dst_struct_offset + 16);
	gen_discard();
}

/*

void debug_write(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

*/

int main()
{
#ifdef OSX
	gen_prologue(304, "debug_write");
	gen_write_register_to_local_8byte("rdi", -264); /* fmt */

	gen_store_regs_to_local(-256, 1, "LBB0_2"); /* va_start(ap, fmt) */

	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_write_to_local_8byte(-48);
	gen_discard();

	gen_initialize_va_list(-80, 8, 0x30, -256); /* va_list ap; va_start(ap, fmt) */

	gen_push_address_of_global("__stderrp");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rdi"); /* %rdi <- __stderrp */

	gen_push_address_of_local(-80);
	gen_pop_to_reg_8byte("rdx"); /* %rdx <- ap */

	gen_write_local_to_register_8byte(-264, "rsi"); /* %rsi <- fmt */

	puts("	callq	" PREFIX "vfprintf");

	gen_initialize_va_list(-80, 8, 0x30, -256);

	gen_write_local_to_register_8byte(-264, "rdi"); /* %rdi <- fmt */

	gen_push_address_of_local(-80);
	gen_pop_to_reg_8byte("rsi"); /* %rsi <- ap */

	puts("	callq	" PREFIX "vprintf");

	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rax");
	puts("	cmpq	-48(%rbp), %rax\n"
	     "	jne	LBB0_4\n");

	gen_push_int(3);
	gen_epilogue(2314);

	puts("LBB0_4:\n"
	     "	callq	" PREFIX "__stack_chk_fail");
#endif

#ifdef LINUX
	gen_prologue(0, "debug_write");
	puts("	pushq	%rbx\n"
	     "	movq	%rdi, %rbx\n"
	     "	subq	$216, %rsp\n");
	gen_store_regs_to_local(-192, 1, "LBB0_2");
	puts("	movq	%fs:40, %rax\n"
	     "	movq	%rax, -200(%rbp)\n"
	     "	leaq	16(%rbp), %rax\n"
	     "	movq	%rsp, %rcx\n"
	     "	movq	%rbx, %rdx\n"
	     "	movl	$1, %esi\n"
	     "	movq	%rax, -216(%rbp)\n");

	gen_push_address_of_global("stderr");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rdi");

	puts("	leaq	-192(%rbp), %rax\n"
	     "	movq	%rax, -208(%rbp)\n"

	     "	movl	$8, -224(%rbp)\n"
	     "	movl	$48, -220(%rbp)\n"

	     "	call	__vfprintf_chk@PLT\n"
	     "	leaq	16(%rbp), %rax\n"
	     "	movq	%rax, -216(%rbp)\n"
	     "	leaq	stdout(%rip), %rax\n"
	     "  movq	(%rax), %rdi\n"
	     "	movq	%rsp, %rcx\n"
	     "	movq	%rbx, %rdx\n"
	     "	movl	$1, %esi\n"
	     "	movl	$8, -224(%rbp)\n"
	     "	leaq	-192(%rbp), %rax\n"
	     "	movl	$48, -220(%rbp)\n"
	     "	movq	%rax, -208(%rbp)\n"
	     "	call	__vfprintf_chk@PLT\n"
	     "	movq	-200(%rbp), %rax\n"
	     "	xorq	%fs:40, %rax\n"
	     "	jne	.L6\n"
	     "	addq	$216, %rsp\n"
	     "	popq	%rbx\n"
	     "	popq	%rbp\n"
	     "	ret\n"
	     ".L6:\n"
	     "	call	__stack_chk_fail@PLT");
#endif
}
