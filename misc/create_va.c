#include "../print_x86_64.h"
#include "../print_x86_64_unofficial.h"
#include <stdio.h>

void gen_store_regs_to_stack(int offset, const char *label_name)
{
	puts("  testb %al, %al");
	printf("  movq %%rsi, %d(%%rbp)\n", offset);
	printf("  movq %%rdx, %d(%%rbp)\n", offset + 8);
	printf("  movq %%rcx, %d(%%rbp)\n", offset + 16);
	printf("  movq %%r8, %d(%%rbp)\n", offset + 24);
	printf("  movq %%r9, %d(%%rbp)\n", offset + 32);
	printf("  je %s\n", label_name);
	for (int i = 0; i < 8; i++) {
		printf("  movaps %%xmm%d, %d(%%rbp)\n", i, offset + 40 + 16 * i);
	}
	printf("%s:\n", label_name);
}

int main()
{
	gen_prologue(0, "debug_write");
#ifdef OSX
	puts("	pushq	%rbx\n"
	     "	subq	$248, %rsp\n"
	     "	movq	%rdi, %rbx");
	gen_store_regs_to_stack(-248, "LBB0_2");
	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_write_to_local_8byte(-48);
	gen_discard();
	puts("	subq	$24, %rsp\n"
	     "	leaq	-256(%rbp), %rdx\n"
	     "	movq	%rdx, -64(%rbp)\n"
	     "	movq	%rdx, (%rsp)\n"
	     "	leaq	16(%rbp), %rdx\n"
	     "	movq	%rdx, -72(%rbp)\n"
	     "	movq	%rdx, 8(%rsp)\n"
	     "	movabsq	$206158430216, %rdx     ## imm = 0x3000000008\n"
	     "	movq	%rdx, -80(%rbp)\n"
	     "	movq	%rdx, 16(%rsp)\n");
	gen_push_address_of_global("__stderrp");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rdi");

	puts("	subq	$8, %rsp\n"
	     "	leaq	-80(%rbp), %rax\n"
	     "	movq	%rax, (%rsp)\n"
	     "	movq	(%rsp), %rdx\n"
	     "	movq	%rbx, %rsi\n"
	     "	callq	_vfprintf\n"
	     "	movq	8(%rsp), %rdx\n"
	     "	movq	%rdx, -64(%rbp)\n"
	     "	movq	16(%rsp), %rdx\n"
	     "	movq	%rdx, -72(%rbp)\n"
	     "	movq	24(%rsp), %rdx\n"
	     "	movq	%rdx, -80(%rbp)\n"
	     "	movq	%rbx, %rdi\n"
	     "	movq	(%rsp), %rsi\n"
	     "	addq	$32, %rsp\n"
	     "	callq	_vprintf");
	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rax");
	puts("	cmpq	-48(%rbp), %rax\n"
	     "	jne	LBB0_4\n"
	     "	addq	$248, %rsp\n"
	     "	popq	%rbx\n"
	     "	popq	%rbp\n"
	     "	retq\n"
	     "LBB0_4:\n"
	     "	callq	___stack_chk_fail");
#endif

#ifdef LINUX
	puts("	pushq	%rbx\n"
	     "	movq	%rdi, %rbx\n"
	     "	subq	$216, %rsp\n");
	gen_store_regs_to_stack(-184, "LBB0_2");
	puts("	movq	%fs:40, %rax\n"
	     "	movq	%rax, 24(%rsp)\n"
	     "	xorl	%eax, %eax\n"
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
