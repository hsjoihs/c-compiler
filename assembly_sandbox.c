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
	print_prologue(0, "foo");
	push_int(2);
	print_epilogue(1234);
	print_prologue(0, "bar");
	push_int(2);
	print_epilogue(134);

	print_prologue(16, "main");

	push_int(3);
	push_from_local(-4);
	op_ints("movl"); /* ; */

	push_int(5);
	push_from_local(-8);
	op_ints("movl"); /* ; */

	push_int(2);
	push_from_local(-12);
	op_ints("movl"); /* ; */

	int label1 = 6;
	int label2 = 7;

	push_from_local(-4);
	if_else_part1(label1, label2);

	push_ret_of("foo");
	write_to_local(-8);
	puts("  addq $4, %rsp\n"); /* ; */

	if_else_part2(label1, label2);

	push_ret_of("bar");
	write_to_local(-12);
	puts("  addq $4, %rsp\n"); /* ; */

	if_else_part3(label1, label2);

	push_int(174);
	return_with_label(109);
	print_epilogue(109);

	return 0;
}
