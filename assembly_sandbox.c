#include "header.h"
#include "vector.h"
#include <stdio.h>
/*
int always87_(int x, int y) { return 87; }

int main()
{
    int b = 3;
    b = always87_(13 + 6, 30 + 5) + 87;
    return b;
}
*/
int main()
{

	puts("_foo:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n"
	     "  movl %edi, -4(%rbp)\n"
	     "  movl %esi, -8(%rbp)\n"
	     "  movl -4(%rbp), %eax\n"
	     "  addl $103, %eax\n"
	     "  subl -8(%rbp), %eax\n"
	     "  popq %rbp\n"
	     "  ret\n");

	print_prologue(16, "main");

	push_int(6);

	push_int(30);
	push_int(5);
	op_ints("addl");
	pop_to_reg("esi");

	push_int(13);
	op_ints("addl");
	pop_to_reg("edi");

	push_ret_of("foo");

	push_int(87);
	op_ints("addl");
	return_with_label(12);
	print_epilogue(12, 16);
}
