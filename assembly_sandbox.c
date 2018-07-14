#include "header.h"
#include "vector.h"
#include <stdio.h>
/*
int main() {
    int b = 3;
    int c = 2;
    b = b && c;
    return b + 173; }
*/
int main()
{
	int label1 = 2;
	int label2 = 3;
	print_prologue(0,"main");
	push_int(3);
	logical_AND_part1(label1, label2);
	push_int(2);
	logical_AND_part2(label1, label2);
	push_int(173);
	op_ints("addl");
	return_with_label(12);
	print_epilogue(12, 0);
	return 0;
}
