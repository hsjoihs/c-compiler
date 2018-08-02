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
    int(*p)[2];
    p = a;
    return 100;
}

int main() { return foo() + bar(); }
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
	gen_push_address_of_local(-16);
	gen_write_to_local_8byte(-8);

	gen_push_int(100);
	gen_epilogue(234);

	/*



	*/

	puts(".global _main\n"
	     "_main:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n"
	     "  pushq %rbx\n"
	     "  movl $0, %eax\n"
	     "  call _foo\n"
	     "  movl %eax, %ebx\n"
	     "  movl $0, %eax\n"
	     "  call _bar\n"
	     "  addl %ebx, %eax\n"
	     "  popq %rbx\n"
	     "  popq %rbp\n"
	     "  ret\n");
	return 0;
}
