#include "print_x86_64.h"
#include <stdio.h>

int main()
{
	/*

int main()
{
	int a[5];
	int *p = a;
	int *q = a+3;
	int r = p < q;
	if (r) {
	    return 174;
	} else {
	    return 1;
	}
}

<  : setb
<= : setbe
>  : seta
>= : setnb
== : sete
!= : setne


	*/
	gen_prologue(60, "main");

	gen_push_address_of_local(-48);
	gen_write_to_local_8byte(-8);
	gen_discard();

	gen_push_address_of_local(-48);
	gen_push_int(12);
	gen_cltq();
	gen_op_8byte("addq");
	gen_write_to_local_8byte(-16);
	gen_discard();

	gen_push_from_local_8byte(-8);
	gen_push_from_local_8byte(-16);
	puts("  movq 8(%rsp), %rax\n"
	     "  cmpq (%rsp), %rax\n"
	     "  setb %al\n"
	     "  movzbl %al, %eax\n"
	     "  movl %eax, 8(%rsp)\n"
	     "  addq $8, %rsp\n");
	gen_write_to_local_8byte(-20);
	gen_discard();

	gen_if_zero_jmp_nbyte(4, 2, -20);
	gen_push_int(174);

	gen_jump(4, "");
	gen_label(2);
	gen_push_int(1);

	gen_epilogue(4);
	return 0;
}
