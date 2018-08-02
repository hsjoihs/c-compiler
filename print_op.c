#include "header.h"
#include "print_x86_64.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_simple_binary_op(enum SimpleBinOp kind)
{
	switch (kind) {
		case SIMPLE_BIN_OP_PLUS:
			gen_op_ints("addl");
			return;
		case SIMPLE_BIN_OP_MINUS:
			gen_op_ints("subl");
			return;
		case SIMPLE_BIN_OP_ASTERISK:
			gen_mul_ints();
			return;
		case SIMPLE_BIN_OP_SLASH:
			gen_div_ints();
			return;
		case SIMPLE_BIN_OP_PERCENT:
			gen_rem_ints();
			return;
		case SIMPLE_BIN_OP_COMMA:
			gen_discard2nd_8byte();
			return;
		case SIMPLE_BIN_OP_LT:
			gen_compare_ints("setl");
			return;
		case SIMPLE_BIN_OP_LT_EQ:
			gen_compare_ints("setle");
			return;
		case SIMPLE_BIN_OP_LSHIFT:
			gen_shift_ints("sall");
			return;
		case SIMPLE_BIN_OP_GT:
			gen_compare_ints("setg");
			return;
		case SIMPLE_BIN_OP_GT_EQ:
			gen_compare_ints("setge");
			return;
		case SIMPLE_BIN_OP_RSHIFT:
			gen_shift_ints("sarl");
			return;
		case SIMPLE_BIN_OP_AND:
			gen_op_ints("andl");
			return;
		case SIMPLE_BIN_OP_OR:
			gen_op_ints("orl");
			return;
		case SIMPLE_BIN_OP_EQ_EQ:
			gen_compare_ints("sete");
			return;
		case SIMPLE_BIN_OP_NOT_EQ:
			gen_compare_ints("setne");
			return;
		case SIMPLE_BIN_OP_HAT:
			gen_op_ints("xorl");
			return;
	}
}
