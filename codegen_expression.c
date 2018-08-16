#include "header.h"
#include "print_x86_64.h"
#include "printerstate.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void print_simple_binary_op(enum SimpleBinOp kind)
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

void print_expression(struct PrinterState *ptr_prs, struct Expression expr);
static void print_expression_as_lvalue(struct PrinterState *ptr_prs,
                                       struct Expression expr);

static void print_expression_as_lvalue(struct PrinterState *ptr_prs,
                                       struct Expression expr)
{
	switch (expr.category) {
		case LOCAL_VAR_: {
			gen_push_address_of_local(expr.local_var_offset);
			gen_push_from_local_nbyte(size_of_basic(expr.details.type),
			                          expr.local_var_offset);

			return;
		}
		case GLOBAL_VAR_: {
			const char *name = expr.global_var_name;
			gen_push_address_of_global(name);
			struct Type type = expr.details.type;

			printf("//load from global `%s`\n", name);
			gen_push_from_global_nbyte(size_of_basic(type), name);

			return;
		}
		case UNARY_OP_EXPR:
			switch (expr.unary_operator) {
				case UNARY_OP_ASTERISK: {
					print_expression(ptr_prs, *expr.ptr1);
					struct Type type = expr.details.type;
					gen_peek_deref_push_nbyte(size_of_basic(type));
					return;
				}
				default:
					fprintf(stderr, "the only unary operator that can create "
					                "lvalue is `*`\n");
					exit(EXIT_FAILURE);
			}
		default:
			fprintf(stderr, "doesn't seem like an lvalue\n");
			exit(EXIT_FAILURE);
	}
}

static void print_op_pointer_plusminus_int(struct Type type_of_pointer,
                                           int is_plus)
{
	int size = size_of(deref_type(type_of_pointer));
	gen_cltq();
	gen_mul_by_const(size);
	gen_op_8byte(is_plus ? "addq" : "subq");
}

void print_expression(struct PrinterState *ptr_prs, struct Expression expr)
{
	switch (expr.category) {
		case POINTER_MINUS_POINTER: {
			print_expression(ptr_prs, *expr.ptr1);
			print_expression(ptr_prs, *expr.ptr2);
			int size = size_of(deref_type(expr.ptr1->details.type));
			gen_op_8byte("subq");
			gen_div_by_const(size);
			return;
		}
		case POINTER_PLUS_INT:
		case POINTER_MINUS_INT: {
			print_expression(ptr_prs, *expr.ptr1);
			print_expression(ptr_prs, *expr.ptr2);
			print_op_pointer_plusminus_int(expr.ptr1->details.type,
			                               expr.category == POINTER_PLUS_INT);
			return;
		}
		case POSTFIX_INCREMENT:
		case POSTFIX_DECREMENT: {
			enum SimpleBinOp opkind2 = expr.category == POSTFIX_INCREMENT
			                               ? SIMPLE_BIN_OP_PLUS
			                               : SIMPLE_BIN_OP_MINUS;

			print_expression_as_lvalue(ptr_prs, *expr.ptr1);
			gen_push_int(1);

			print_simple_binary_op(opkind2);

			struct Type type = expr.ptr1->details.type;

			gen_assign_nbyte(size_of_basic(type));

			gen_push_int(-1);
			print_simple_binary_op(opkind2);

			return;
		}

		case LOCAL_VAR_: {
			if (is_array(expr.details.true_type)) {
				gen_push_address_of_local(expr.local_var_offset);
				return;
			}
			gen_push_from_local_nbyte(size_of_basic(expr.details.type),
			                          expr.local_var_offset);

			return;
		}

		case GLOBAL_VAR_: {
			printf("//global `%s` as rvalue\n", expr.global_var_name);

			if (is_array(expr.details.true_type)) {
				gen_push_address_of_global(expr.global_var_name);
				return;
			}

			gen_push_from_global_nbyte(size_of_basic(expr.details.type),
			                           expr.global_var_name);

			return;
		}

		case SIMPLE_BINARY_EXPR: {
			print_expression(ptr_prs, *expr.ptr1);
			print_expression(ptr_prs, *expr.ptr2);
			print_simple_binary_op(expr.simple_binary_operator);
			return;
		}

		case LOGICAL_OR_EXPR: {

			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			print_expression(ptr_prs, *expr.ptr1);

			gen_logical_OR_part1(label1);
			print_expression(ptr_prs, *expr.ptr2);
			gen_logical_OR_part2(label1, label2);
			return;
		}
		case LOGICAL_AND_EXPR: {

			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			print_expression(ptr_prs, *expr.ptr1);

			gen_logical_AND_part1(label1);
			print_expression(ptr_prs, *expr.ptr2);
			gen_logical_AND_part2(label1, label2);
			return;
		}

		case ASSIGNMENT_EXPR: {

			print_expression_as_lvalue(ptr_prs, *expr.ptr1);
			print_expression(ptr_prs, *expr.ptr2);

			print_simple_binary_op(expr.simple_binary_operator);

			struct Type type = expr.ptr1->details.type;

			gen_assign_nbyte(size_of_basic(type));

			return;
		}
		case INT_VALUE:
			gen_push_int(expr.int_value);
			return;
		case UNARY_OP_EXPR:
			switch (expr.unary_operator) {
				case UNARY_OP_NOT:
					print_expression(ptr_prs, *expr.ptr1);
					gen_unary_not();
					return;
				case UNARY_OP_TILDA:
					print_expression(ptr_prs, *expr.ptr1);
					gen_unary("notl");
					return;
				case UNARY_OP_PLUS:
					print_expression(ptr_prs, *expr.ptr1);
					/* do nothing */
					return;
				case UNARY_OP_MINUS:
					print_expression(ptr_prs, *expr.ptr1);
					gen_unary("negl");
					return;

				case UNARY_OP_PLUS_PLUS:
				case UNARY_OP_MINUS_MINUS: {
					print_expression_as_lvalue(ptr_prs, *expr.ptr1);

					gen_push_int(1);
					print_simple_binary_op(expr.unary_operator ==
					                               UNARY_OP_PLUS_PLUS
					                           ? SIMPLE_BIN_OP_PLUS
					                           : SIMPLE_BIN_OP_MINUS);

					struct Type type = expr.ptr1->details.type;

					gen_assign_nbyte(size_of_basic(type));
					return;
				}

				case UNARY_OP_AND: {
					/* pushes address, then pushes value */
					print_expression_as_lvalue(ptr_prs, *expr.ptr1);
					gen_discard();
					return;
				}

				case UNARY_OP_ASTERISK: {
					print_expression(ptr_prs, *expr.ptr1);
					struct Type type = expr.details.type;
					struct Type true_type = expr.details.true_type;

					if (is_pointer(type) && is_array(true_type)) {
						/* do not dereference, if it is an array */
						return;
					}
					gen_peek_and_dereference_nbyte(size_of_basic(type));

					return;
				}
			}
			return;
		case CONDITIONAL_EXPR: {
			print_expression(ptr_prs, *expr.ptr1);
			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);

			gen_ternary_part1(label1, label2);
			print_expression(ptr_prs, *expr.ptr2);
			gen_ternary_part2(label1, label2);
			print_expression(ptr_prs, *expr.ptr3);

			gen_ternary_part3(label1, label2);
			return;
		}
		case FUNCCALL_EXPR: {
			const char *ident_str = expr.global_var_name;
			struct Type ret_type = expr.details.type;

			for (int counter = expr.arg_length - 1; counter >= 0; counter--) {
				struct Expression expr_ = expr.arg_expr_vec[counter];

				print_expression(ptr_prs, expr_);
			}

			for (int counter = 0; counter < expr.arg_length; counter++) {
				struct Expression expr_ = expr.arg_expr_vec[counter];

				switch (size_of_basic(expr_.details.type)) {
					case 1:
					case 4:
						gen_pop_to_reg_4byte(
						    get_reg_name_from_arg_pos_4byte(counter));
						break;
					case 8:
						gen_pop_to_reg_8byte(
						    get_reg_name_from_arg_pos_8byte(counter));
						break;
					default:
						unsupported("Unsupported width");
				}
			}
			gen_push_ret_of_nbyte(size_of_basic(ret_type), ident_str);

			return;
		}

		case STRING_LITERAL: {
			const char *str = expr.literal_string;
			push_vector(&ptr_prs->string_constant_pool, str);
			gen_push_address_of_str(ptr_prs->string_constant_pool.length - 1);
		}
	}
}
