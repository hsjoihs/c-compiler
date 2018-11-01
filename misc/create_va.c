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

int main()
{
#ifdef OSX
	gen_prologue(304, "debug_write");
	gen_write_register_to_local_8byte("rdi", -264);
	gen_push_from_local_nbyte(8, -264);

	gen_store_regs_to_local(-256, 1, "LBB0_2");

	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_write_to_local_8byte(-48);
	gen_discard();

	gen_push_address_of_local(-256);
	gen_write_to_local_8byte(-64);
	gen_write_to_local_8byte(-272);
	gen_discard();

	gen_push_address_of_local(-8);
	puts("	addq	$24, (%rsp)\n");
	gen_write_to_local_8byte(-72);
	gen_write_to_local_8byte(-280);
	gen_discard();

	gen_push_8byte("0x3000000008");
	gen_write_to_local_8byte(-80);
	gen_write_to_local_8byte(-288);
	gen_discard();

	gen_push_address_of_global("__stderrp");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rdi");

	gen_push_address_of_local(-80);
	gen_write_to_reg_8byte("rdx");
	gen_write_local_to_register_8byte(-264, "rsi");
	puts("	callq	_vfprintf");

	puts("	movq	-272(%rbp), %rdx\n"
	     "	movq	%rdx, -64(%rbp)\n"
	     "	movq	-280(%rbp), %rdx\n"
	     "	movq	%rdx, -72(%rbp)\n"
	     "	movq	-288(%rbp), %rdx\n"
	     "	movq	%rdx, -80(%rbp)\n");

	gen_write_local_to_register_8byte(-264, "rdi");
	gen_push_address_of_local(-80);
	gen_pop_to_reg_8byte("rsi");
	puts("	addq	$16, %rsp\n"
	     "	callq	_vprintf");
	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rax");
	puts("	cmpq	-48(%rbp), %rax\n"
	     "	jne	LBB0_4\n"
	     "	addq	$248, %rsp\n"
	     "	addq	$16, %rsp\n"
	     "	addq	$32, %rsp\n"
	     "	addq	$8, %rsp\n"
	     "	popq	%rbp\n"
	     "	retq\n"
	     "LBB0_4:\n"
	     "	callq	___stack_chk_fail");
#endif

#ifdef LINUX
	gen_prologue(0, "debug_write");
	puts("	pushq	%rbx\n"
	     "	movq	%rdi, %rbx\n"
	     "	subq	$216, %rsp\n");
	gen_store_regs_to_local(-192, 1, "LBB0_2");
	puts("	movq	%fs:40, %rax\n"
	     "	movq	%rax, 24(%rsp)\n"
	     "	movl	$0, %eax\n"
	     "	leaq	240(%rsp), %rax\n"
	     "	movq	%rsp, %rbp\n"
	     "	movq	%rbx, %rdx\n"
	     "	movq	%rbp, %rcx\n"
	     "	movl	$1, %esi\n"
	     "	movq	%rax, 8(%rsp)\n");
	gen_push_address_of_global("stderr");
	puts("  movq  (%rsp), %rax\n"
	     "  addq    $8, %rsp\n"
	     "	movq	(%rax), %rdi\n"
	     "	leaq	32(%rsp), %rax\n"
	     "	movl	$8, (%rsp)\n"
	     "	movl	$48, 4(%rsp)\n"
	     "	movq	%rax, 16(%rsp)\n"
	     "	call	__vfprintf_chk@PLT\n"
	     "	leaq	240(%rsp), %rax\n"
	     "	movq	%rax, 8(%rsp)\n"
	     "	leaq	stdout(%rip), %rax\n"
	     "  movq	(%rax), %rdi\n"
	     "	movq	%rbp, %rcx\n"
	     "	movq	%rbx, %rdx\n"
	     "	movl	$1, %esi\n"
	     "	movl	$8, (%rsp)\n"
	     "	leaq	32(%rsp), %rax\n"
	     "	movl	$48, 4(%rsp)\n"
	     "	movq	%rax, 16(%rsp)\n"
	     "	call	__vfprintf_chk@PLT\n"
	     "	movq	24(%rsp), %rax\n"
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
