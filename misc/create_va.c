#include "../print_x86_64.h"
#include "../print_x86_64_unofficial.h"
#include <stdio.h>

void gen_store_regs_to_stack(int offset, const char *label_name)
{
	puts("	testb	%al, %al");
	printf("	movq	%%rsi, %d(%%rbp)\n", offset);
	printf("	movq	%%rdx, %d(%%rbp)\n", offset + 8);
	printf("	movq	%%rcx, %d(%%rbp)\n", offset + 16);
	printf("	movq	%%r8, %d(%%rbp)\n", offset + 24);
	printf("	movq	%%r9, %d(%%rbp)\n", offset + 32);
	printf("	je	%s\n", label_name);
	for (int i = 0; i < 8; i++) {
		printf("	movaps	%%xmm%d, %d(%%rbp)\n", i, offset + 40 + 16 * i);
	}
	printf("%s:\n", label_name);
}

int main()
{
	gen_prologue(0, "debug_write");
#ifdef OSX
	puts("	pushq	%r15\n"
	     "	pushq	%r14\n"
	     "	pushq	%r13\n"
	     "	pushq	%r12\n"
	     "	pushq	%rbx\n"
	     "	subq	$216, %rsp\n"
	     "	movq	%rdi, %rbx");
	gen_store_regs_to_stack(-248, "LBB0_2");
	puts("	movq	___stack_chk_guard@GOTPCREL(%rip), %rax\n"
	     "	movq	(%rax), %rax\n"
	     "	movq	%rax, -48(%rbp)\n"
	     "	leaq	-256(%rbp), %r15\n"
	     "	movq	%r15, -64(%rbp)\n"
	     "	leaq	16(%rbp), %r12\n"
	     "	movq	%r12, -72(%rbp)\n"
	     "	movabsq	$206158430216, %r13     ## imm = 0x3000000008\n"
	     "	movq	%r13, -80(%rbp)\n"
	     "	movq	___stderrp@GOTPCREL(%rip), %rax\n"
	     "	movq	(%rax), %rdi\n"
	     "	leaq	-80(%rbp), %r14\n"
	     "	movq	%rbx, %rsi\n"
	     "	movq	%r14, %rdx\n"
	     "	callq	_vfprintf\n"
	     "	movq	%r15, -64(%rbp)\n"
	     "	movq	%r12, -72(%rbp)\n"
	     "	movq	%r13, -80(%rbp)\n"
	     "	movq	%rbx, %rdi\n"
	     "	movq	%r14, %rsi\n"
	     "	callq	_vprintf\n"
	     "	movq	___stack_chk_guard@GOTPCREL(%rip), %rax\n"
	     "	movq	(%rax), %rax\n"
	     "	cmpq	-48(%rbp), %rax\n"
	     "	jne	LBB0_4\n"
	     "	addq	$216, %rsp\n"
	     "	popq	%rbx\n"
	     "	popq	%r12\n"
	     "	popq	%r13\n"
	     "	popq	%r14\n"
	     "	popq	%r15\n"
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
	     "	movq	stderr(%rip), %rdi\n"
	     "	movq	%rax, 8(%rsp)\n"
	     "	leaq	32(%rsp), %rax\n"
	     "	movl	$8, (%rsp)\n"
	     "	movl	$48, 4(%rsp)\n"
	     "	movq	%rax, 16(%rsp)\n"
	     "	call	__vfprintf_chk@PLT\n"
	     "	leaq	240(%rsp), %rax\n"
	     "	movq	stdout(%rip), %rdi\n"
	     "	movq	%rbp, %rcx\n"
	     "	movq	%rbx, %rdx\n"
	     "	movl	$1, %esi\n"
	     "	movl	$8, (%rsp)\n"
	     "	movq	%rax, 8(%rsp)\n"
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
