#include "header.h"
#include "vector.h"
#include <stdio.h>
/*
int foo(){ return 2;}
int bar(){ return 7;}

int main() {
    int a = 3;
    int b = 5;
    int c = 2;
    if(a) {
        b = foo();
    } else {
        c = bar();
    }
    return 174;
    }
*/
int main()
{
	gen_prologue(0, "foo");
	gen_push_int(2);
	gen_epilogue(1234);
	gen_prologue(0, "bar");
	gen_push_int(2);
	gen_epilogue(134);

	gen_prologue(16, "main");

	gen_push_int(3);
	gen_push_from_local(-4);
	gen_op_ints("movl"); /* ; */

	gen_push_int(5);
	gen_push_from_local(-8);
	gen_op_ints("movl"); /* ; */

	gen_push_int(2);
	gen_push_from_local(-12);
	gen_op_ints("movl"); /* ; */

	int label1 = 6;
	int label2 = 7;

	gen_push_from_local(-4);
	gen_if_else_part1(label1, label2);

	gen_push_ret_of("foo");
	gen_write_to_local(-8);
	puts("  addq $4, %rsp\n"); /* ; */

	gen_if_else_part2(label1, label2);

	gen_push_ret_of("bar");
	gen_write_to_local(-12);
	puts("  addq $4, %rsp\n"); /* ; */

	gen_if_else_part3(label1, label2);

	gen_push_int(174);
	gen_return_with_label(109);
	gen_epilogue(109);

	return 0;
}
