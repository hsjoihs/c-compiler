#include "header.h"
#include "print_x86_64.h"
#include "printerstate.h"
#include "std.h"
#include "std_io.h"

static void print_op_pointer_plusminus_int(int is_plus, int size)
{
	gen_cltq();
	gen_mul_by_const(size);
	gen_op_8byte(is_plus ? "addq" : "subq");
}

/* size is garbage unless left_type is a pointer */
static void print_simple_binary_op(enum SimpleBinOp kind,
                                   const struct Type *ref_left_type, int size)
{
	const struct Type left_type = *ref_left_type;
	if (left_type.type_category == PTR_) {
		switch (kind) {
			case SIMPLE_BIN_OP_COMMA:
				gen_discard2nd_8byte();
				return;
			case SIMPLE_BIN_OP_PLUS:
				print_op_pointer_plusminus_int(1, size);
				return;
			case SIMPLE_BIN_OP_MINUS:
				print_op_pointer_plusminus_int(0, size);
				return;

			case SIMPLE_BIN_OP_EQ_EQ:
				gen_compare_ptrs("sete");
				return;
			case SIMPLE_BIN_OP_NOT_EQ:
				gen_compare_ptrs("setne");
				return;
			case SIMPLE_BIN_OP_LT_EQ:
				gen_compare_ptrs("setbe");
				return;
			case SIMPLE_BIN_OP_GT_EQ:
				gen_compare_ptrs("setnb");
				return;
			case SIMPLE_BIN_OP_LT:
				gen_compare_ptrs("setb");
				return;
			case SIMPLE_BIN_OP_GT:
				gen_compare_ptrs("seta");
				return;
			default:
				simple_error("unexpected pointer in binary operation\n");
		}
		return;
	}
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

void print_address_of_lvalue(struct PrinterState *ptr_prs,
                             const struct Expr *ref_expr)
{
	const struct Expr expr = *ref_expr;
	switch (expr.category) {
		case STRUCT_AND_OFFSET: {
			print_address_of_lvalue(ptr_prs, expr.ptr1);
			int offset = expr.struct_offset;
			gen_push_int(offset);
			gen_cltq();
			gen_op_8byte("addq");
			return;
		}
		case LOCAL_VAR_: {
			gen_push_address_of_local(expr.local_var_offset);

			return;
		}
		case GLOBAL_VAR_: {
			gen_push_address_of_global(expr.global_var_name);

			return;
		}
		case UNARY_OP_EXPR:
			switch (expr.unary_operator) {
				case UNARY_OP_ASTERISK: {
					print_expression(ptr_prs, expr.ptr1);
					return;
				}
				default:
					simple_error("the only unary operator that can create "
					             "lvalue is `*`\n");
			}
		default:
			simple_error("doesn't seem like an lvalue\n");
	}
}

void print_expression(struct PrinterState *ptr_prs,
                      const struct Expr *ref_expr);
static void print_expression_as_lvalue(struct PrinterState *ptr_prs,
                                       const struct Expr *ref_expr);

static void print_expression_as_lvalue(struct PrinterState *ptr_prs,
                                       const struct Expr *ref_expr)
{
	const struct Expr expr = *ref_expr;
	print_address_of_lvalue(ptr_prs, &expr);
	switch (expr.category) {
		case STRUCT_AND_OFFSET: {
			struct Type type = expr.details.type;
			gen_peek_deref_push_nbyte(
			    size_of_basic(&type, "foo.bar as lvalue"));
			return;
		}
		case LOCAL_VAR_: {
			gen_push_from_local_nbyte(
			    size_of_basic(&expr.details.type, "local var as lvalue"),
			    expr.local_var_offset);

			return;
		}
		case GLOBAL_VAR_: {
			const char *name = expr.global_var_name;
			struct Type type = expr.details.type;

			printf("//load from global `%s`\n", name);
			gen_peek_deref_push_nbyte(
			    size_of_basic(&type, "global var as lvalue"));

			return;
		}
		case UNARY_OP_EXPR:
			switch (expr.unary_operator) {
				case UNARY_OP_ASTERISK: {
					struct Type type = expr.details.type;
					gen_peek_deref_push_nbyte(
					    size_of_basic(&type, "*expr as lvalue"));
					return;
				}
				default:
					simple_error("the only unary operator that can create "
					             "lvalue is `*`\n");
			}
		default:
			simple_error("doesn't seem like an lvalue\n");
	}
}

void print_expression(struct PrinterState *ptr_prs, const struct Expr *ref_expr)
{
	const struct Expr expr = *ref_expr;
	switch (expr.category) {
		case STRUCT_ASSIGNMENT_EXPR: {
			print_address_of_lvalue(ptr_prs, expr.ptr1);
			print_address_of_lvalue(ptr_prs, expr.ptr2);
			int size = expr.size_info_for_struct_assign;
			gen_copy_struct_and_discard(size);
			gen_push_int(143253); /* random garbage */
		}
		case VOID_EXPR: {
			gen_push_int(123); /* random garbage */
			return;
		}
		case NULLPTR: {
			gen_push_nullptr();
			return;
		}
		case STRUCT_AND_OFFSET: {
			print_expression_as_lvalue(ptr_prs, &expr);
			gen_discard2nd_8byte();
			return;
		}
		case POINTER_MINUS_POINTER: {
			print_expression(ptr_prs, expr.ptr1);
			print_expression(ptr_prs, expr.ptr2);
			int size = expr.size_info_for_pointer_arith;
			gen_op_8byte("subq");
			gen_div_by_const(size);
			return;
		}
		case POINTER_PLUS_INT:
		case POINTER_MINUS_INT: {
			int size = expr.size_info_for_pointer_arith;
			print_expression(ptr_prs, expr.ptr1);
			print_expression(ptr_prs, expr.ptr2);
			print_op_pointer_plusminus_int(expr.category == POINTER_PLUS_INT,
			                               size);
			return;
		}
		case POSTFIX_INCREMENT:
		case POSTFIX_DECREMENT: {
			enum SimpleBinOp opkind2 = expr.category == POSTFIX_INCREMENT
			                               ? SIMPLE_BIN_OP_PLUS
			                               : SIMPLE_BIN_OP_MINUS;

			print_expression_as_lvalue(ptr_prs, expr.ptr1);
			gen_push_int(1);

			print_simple_binary_op(opkind2, &expr.ptr1->details.type,
			                       expr.size_info_for_pointer_arith);

			gen_assign_nbyte(
			    size_of_basic(&expr.ptr1->details.type, "postfix inc/dec"));

			gen_push_int(-1);

			print_simple_binary_op(opkind2, &expr.ptr1->details.type,
			                       expr.size_info_for_pointer_arith);

			return;
		}

		case LOCAL_VAR_: {
			if (expr.details.true_type.type_category == ARRAY) {
				gen_push_address_of_local(expr.local_var_offset);
				return;
			}
			gen_push_from_local_nbyte(
			    size_of_basic(&expr.details.type, "local var as rvalue"),
			    expr.local_var_offset);

			return;
		}

		case GLOBAL_VAR_: {
			printf("//global `%s` as rvalue\n", expr.global_var_name);

			if (expr.details.true_type.type_category == ARRAY) {
				gen_push_address_of_global(expr.global_var_name);
				return;
			}

			gen_push_from_global_nbyte(
			    size_of_basic(&expr.details.type, "global var as rvalue"),
			    expr.global_var_name);

			return;
		}

		case SIMPLE_BINARY_EXPR: {
			print_expression(ptr_prs, expr.ptr1);
			print_expression(ptr_prs, expr.ptr2);
			print_simple_binary_op(expr.simple_binary_operator,
			                       &expr.ptr1->details.type,
			                       expr.size_info_for_pointer_arith);
			return;
		}

		case LOGICAL_OR_EXPR: {

			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			print_expression(ptr_prs, expr.ptr1);

			gen_if_nonzero_jmp_nbyte(size_of_basic(&expr.ptr1->details.type,
			                                       "operand of logical or"),
			                         label1, 0);
			print_expression(ptr_prs, expr.ptr2);
			gen_discard();
			gen_if_nonzero_jmp_nbyte(size_of_basic(&expr.ptr2->details.type,
			                                       "operand of logical or"),
			                         label1, -8);
			gen_logical_OR_part2(label1, label2);
			return;
		}
		case LOGICAL_AND_EXPR: {

			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			print_expression(ptr_prs, expr.ptr1);

			gen_if_zero_jmp_nbyte(size_of_basic(&expr.ptr1->details.type,
			                                    "operand of logical and"),
			                      label1, 0);
			print_expression(ptr_prs, expr.ptr2);
			gen_discard();
			gen_if_zero_jmp_nbyte(size_of_basic(&expr.ptr2->details.type,
			                                    "operand of logical and"),
			                      label1, -8);
			gen_logical_AND_part2(label1, label2);
			return;
		}

		case ASSIGNMENT_EXPR: {

			print_expression_as_lvalue(ptr_prs, expr.ptr1);
			print_expression(ptr_prs, expr.ptr2);

			print_simple_binary_op(expr.simple_binary_operator,
			                       &expr.ptr1->details.type,
			                       expr.size_info_for_pointer_arith);

			struct Type type = expr.ptr1->details.type;

			gen_assign_nbyte(size_of_basic(&type, "operand of assignment"));

			return;
		}
		case INT_VALUE:
			gen_push_int(expr.int_value);
			return;
		case UNARY_OP_EXPR:
			switch (expr.unary_operator) {
				case UNARY_OP_NOT:
					print_expression(ptr_prs, expr.ptr1);
					gen_unary_not();
					return;
				case UNARY_OP_TILDA:
					print_expression(ptr_prs, expr.ptr1);
					gen_unary("notl");
					return;
				case UNARY_OP_PLUS:
					print_expression(ptr_prs, expr.ptr1);
					/* do nothing */
					return;
				case UNARY_OP_MINUS:
					print_expression(ptr_prs, expr.ptr1);
					gen_unary("negl");
					return;

				case UNARY_OP_PLUS_PLUS:
				case UNARY_OP_MINUS_MINUS: {
					print_expression_as_lvalue(ptr_prs, expr.ptr1);

					gen_push_int(1);
					print_simple_binary_op(expr.unary_operator ==
					                               UNARY_OP_PLUS_PLUS
					                           ? SIMPLE_BIN_OP_PLUS
					                           : SIMPLE_BIN_OP_MINUS,
					                       &expr.ptr1->details.type,
					                       expr.size_info_for_pointer_arith);

					struct Type type = expr.ptr1->details.type;

					gen_assign_nbyte(size_of_basic(&type, "prefix inc/dec"));
					return;
				}

				case UNARY_OP_AND: {
					struct Type type = expr.ptr1->details.type;
					struct Type true_type = expr.ptr1->details.true_type;
					if (type.type_category == PTR_ &&
					    true_type.type_category == ARRAY) {
						print_expression(ptr_prs, expr.ptr1);
					} else {
						print_address_of_lvalue(ptr_prs, expr.ptr1);
					}
					return;
				}

				case UNARY_OP_ASTERISK: {
					print_expression(ptr_prs, expr.ptr1);
					struct Type type = expr.details.type;
					struct Type true_type = expr.details.true_type;

					if (type.type_category == PTR_ &&
					    true_type.type_category == ARRAY) {
						/* do not dereference, if it is an array */
						return;
					}
					gen_peek_and_dereference_nbyte(
					    size_of_basic(&type, "*expr as rvalue"));

					return;
				}
			}
			return;
		case CONDITIONAL_EXPR: {
			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);

			print_expression(ptr_prs, expr.ptr1);
			gen_if_zero_jmp_nbyte(
			    size_of_basic(&expr.ptr1->details.type,
			                  "condition of conditional expression"),
			    label1, 0);
			print_expression(ptr_prs, expr.ptr2);
			gen_jump(label2, "ternary operator");
			gen_label(label1);
			print_expression(ptr_prs, expr.ptr3);
			gen_label(label2);
			gen_discard2nd_8byte();
			return;
		}
		case FUNCCALL_EXPR: {
			const char *ident_str = expr.global_var_name;
			struct Type ret_type = expr.details.type;

			for (int counter = expr.args.length - 1; counter >= 0; counter--) {
				const struct Expr *ptr_expr_ = expr.args.vector[counter];

				print_expression(ptr_prs, ptr_expr_);
			}

			for (int counter = 0; counter < expr.args.length; counter++) {
				const struct Expr *ptr_expr_ = expr.args.vector[counter];

				switch (size_of_basic(&ptr_expr_->details.type, "argument")) {
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

			int size;
			if (ret_type.type_category != VOID_) {
				size = size_of_basic(&ret_type, "return value");
			} else {
				size = 4; /* for convenience */
			}
			gen_push_ret_of_nbyte(size, ident_str);

			return;
		}

		case STRING_LITERAL: {
			const char *str = expr.literal_string;
			push_vector(&ptr_prs->string_constant_pool, str);
			gen_push_address_of_str(ptr_prs->string_constant_pool.length - 1);
		}
	}
}
