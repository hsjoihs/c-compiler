#include "print_x86_64.h"
#include <stdio.h>

/*
int foo()
{
    int a[1];
    int *p;
    p = a;
    *p = 2;
    return 74;
}
int bar()
{
    int a[1][2];
    int (*p)[2];
    p = a;
    int *q;
    q = *p;
    return 100;
}

int main() { foo2(); return foo() + bar(); }

int A[5];
int foo2()
{
    int *p;
    p = A;
    *p = 2;
    return 3;
}
*/

int main()
{
	gen_prologue(32, "foo");

	gen_push_address_of_local(-12);
	gen_write_to_local_8byte(-20);
	gen_discard();

	gen_push_from_local_8byte(-20);
	gen_peek_and_dereference();
	gen_discard();

	gen_push_int(2);
	gen_assign_to_backed_up_address();

	gen_push_int(74);
	gen_epilogue(123);

	/*



	*/
	gen_prologue(32, "bar");
	gen_push_address_of_local(-24);
	gen_write_to_local_8byte(-8);
	gen_discard();

	gen_push_from_local_8byte(-8);
	gen_write_to_local_8byte(-16);
	gen_discard();

	gen_push_int(100);
	gen_epilogue(234);

	/*



	*/

	puts(".global " PREFIX "main\n"
	     ".comm	" PREFIX "A,20,4\n"
	     "" PREFIX "foo2:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n");
	gen_push_address_of_global("A");
	puts("  movq (%rsp), %rcx\n"
	     "  movq %rcx, -8(%rbp)\n"
	     "  addq $8, %rsp\n"
	     "  movq -8(%rbp), %rax\n"
	     "  movl $2, (%rax)\n"
	     "  movl $74, %eax\n"
	     "  popq %rbp\n"
	     "  ret\n"
	     "" PREFIX "main:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n"
	     "  pushq %rbx\n"
	     "  subq $8, %rsp\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "foo2\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "foo\n"
	     "  movl %eax, %ebx\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "bar\n"
	     "  addl %ebx, %eax\n"
	     "  addq $8, %rsp\n"
	     "  popq %rbx\n"
	     "  popq %rbp\n"
	     "  ret\n");
	return 0;
}
