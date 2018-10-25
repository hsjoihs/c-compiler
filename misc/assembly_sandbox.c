#include "../print_x86_64.h"
#include "../print_x86_64_unofficial.h"
#include <stdio.h>

void gen_write_to_local_8byte(int offset);

int main()
{
	gen_prologue_static(0, "divide_addr_diff_by_173");
	gen_write_register_to_local_8byte("rdi", -8);
	gen_write_register_to_local_8byte("rsi", -16);

	gen_push_from_local_nbyte(8, -8);
	gen_push_from_local_nbyte(8, -16);
	gen_op_8byte("subq");

	gen_div_by_const(173);
	gen_epilogue(32);

	/*
	int main()
	{
	    char a[1000];
	    return divide_addr_diff_by_173(a + 173 * 3, a);
	}
	*/
	puts("\n//gen_prologue(1000, \"main\")"
	     "\n.global " PREFIX "main"
	     "\n" PREFIX "main:"
	     "\n  pushq %rbp"
	     "\n  movq %rsp, %rbp"
	     "\n  subq $1000, %rsp"
	     "\n//gen_push_address_of_local(-1000);"
	     "\n  subq $8, %rsp"
	     "\n  leaq -1000(%rbp), %rax"
	     "\n  movq %rax, (%rsp)"
	     "\n//gen_push_address_of_local(-1000);"
	     "\n  subq $8, %rsp"
	     "\n  leaq -1000(%rbp), %rax"
	     "\n  movq %rax, (%rsp)"
	     "\n//gen_push_int(173)"
	     "\n  subq $8, %rsp"
	     "\n  movl $173, (%rsp)"
	     "\n//gen_push_int(3)"
	     "\n  subq $8, %rsp"
	     "\n  movl $3, (%rsp)"
	     "\n//gen_mul_ints()"
	     "\n  movl 8(%rsp), %eax"
	     "\n  imull (%rsp), %eax"
	     "\n  movl %eax, 8(%rsp)"
	     "\n  addq $8, %rsp"
	     "\n//gen_cltq()"
	     "\n  movl (%rsp), %eax"
	     "\n  cltq"
	     "\n  movq %rax, (%rsp)"
	     "\n//gen_mul_by_const(1)"
	     "\n  movq (%rsp), %rax"
	     "\n  leaq 0(,%rax,1), %rdx"
	     "\n  movq %rdx, (%rsp)"
	     "\n//gen_op_8byte(\"addq\")"
	     "\n  movq (%rsp), %rax"
	     "\n  addq %rax, 8(%rsp)"
	     "\n  addq $8, %rsp"
	     "\n//gen_pop_to_reg_8byte(rdi)"
	     "\n  movq (%rsp), %rdi"
	     "\n  addq $8, %rsp"
	     "\n//gen_pop_to_reg_8byte(rsi)"
	     "\n  movq (%rsp), %rsi"
	     "\n  addq $8, %rsp"
	     "\n//gen_push_ret_of_4byte(\"divide_addr_diff_by_173\")"
	     "\n  subq $8, %rsp"
	     "\n  movq %rsp, %rax"
	     "\n  andq $15, %rax"
	     "\n  subq %rax, %rsp"
	     "\n  movq %rax, (%rsp)"
	     "\n  movb $0, %al"
	     "\n  call " PREFIX "divide_addr_diff_by_173"
	     "\n  addq (%rsp), %rsp"
	     "\n  movl %eax, (%rsp)"
	     "\n//comment: return"
	     "\n  jmp .L4"
	     "\n//gen_epilogue(4)"
	     "\n.L4:  movl (%rsp), %eax"
	     "\n addl $171, %eax"
	     "\n  leave"
	     "\n  ret");

	return 0;
}
