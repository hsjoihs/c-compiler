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

	gen_global_declaration("A", 20);

	gen_prologue(32, "foo2");

	gen_push_address_of_global("A");
	gen_write_to_local_8byte(-16);
	gen_discard();

	gen_push_from_local_8byte(-16);
	gen_peek_and_dereference();
	gen_push_int(2);
	gen_assign_to_backed_up_address();

	gen_push_int(74);
	gen_epilogue(153);

	/*


	*/
	gen_prologue(0, "main");

	gen_push_ret_of("foo2");
	gen_discard();

	gen_push_ret_of("foo");
	gen_push_ret_of("bar");
	gen_op_ints("addl");

	gen_epilogue(193);
	return 0;
}
