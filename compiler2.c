#include "header.h"
#if 1
#include "compiler.h"
#endif
#include "print_x86_64.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_new_label_name(struct ParserState *ptr_ps);
void print_argument_expression(struct ParserState *ptr_ps,
                               struct Expression expr, int counter);

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

struct ExprInfo remove_leftiness(struct ExprInfo info)
{
	if (info.info == LOCAL_VAR || info.info == GLOBAL_VAR) {
		info.info = NOT_ASSIGNABLE;
	}
	return info;
}

void read_all_tokens_debug(const char *str)
{
	struct Token tok;
	tok.kind = BEGINNING;
	tok.int_value = GARBAGE_INT;
	tok.ident_str = 0;

	print_token(tok);
	fprintf(stderr, "\n");

	do {
		tok = get_token(&str);
		print_token(tok);
		fprintf(stderr, "\n");
		if (tok.kind == END) {
			break;
		}
	} while (1);
}

void print_expression(struct ParserState *ptr_ps, struct Expression expr);

void print_expression_as_lvalue(struct ParserState *ptr_ps,
                                struct Expression expr)
{
	switch (expr.category) {
		case LOCAL_VAR_AS_LVALUE: {
			gen_push_int(0);

			switch (size_of(expr.details.type)) {
				case 4:
					gen_push_from_local(expr.details.offset);
					break;
				case 8:
					gen_push_from_local_8byte(expr.details.offset);
					break;
				default:
					unimplemented("Unsupported width in the "
					              "assignment operation");
			}
			return;
		}
		case GLOBAL_VAR_AS_LVALUE: {
			gen_push_int(0);
			struct Type type = expr.details.type;
			const char *name = expr.global_var_name;

			printf("//load from global `%s`\n", name);
			switch (size_of(type)) {
				case 4:
					gen_push_from_global_4byte(name);
					break;
				case 8:
					gen_push_from_global_8byte(name);
					break;
				default:
					unimplemented("Unsupported width in the "
					              "assignment operation");
			}
			return;
		}
		case UNARY_OP_EXPR:
			switch (expr.unary_operator) {
				case UNARY_OP_ASTERISK: {
					print_expression(ptr_ps, *expr.ptr1);
					struct Type type = expr.details.type;
					switch (size_of(type)) {
						case 4:
							gen_peek_deref_push_4byte();
							break;
						case 8:
							gen_peek_deref_push_8byte();
							break;
						default:
							unimplemented("Unsupported width");
					}
					return;
				}
				default:
					fprintf(stderr, "the only unary operator that can create "
					                "lvalue is `*`\n");
					exit(EXIT_FAILURE);
			}
			return;
		default:
			fprintf(stderr, "doesn't seem like an lvalue\n");
			return;
	}
}

void print_expression(struct ParserState *ptr_ps, struct Expression expr)
{
	switch (expr.category) {
		case POINTER_MINUS_POINTER: {
			print_expression(ptr_ps, *expr.ptr1);
			print_expression(ptr_ps, *expr.ptr2);
			int size = size_of(deref_type(expr.ptr1->details.type));
			gen_op_8byte("subq");
			gen_div_by_const(size);
			return;
		}
		case POINTER_PLUSORMINUS_INT: {
			print_expression(ptr_ps, *expr.ptr1);
			print_expression(ptr_ps, *expr.ptr2);
			int size = size_of(deref_type(expr.ptr1->details.type));
			gen_cltq();
			gen_mul_by_const(size);
			gen_op_8byte(expr.binary_operator == OP_PLUS ? "addq" : "subq");

			return;
		}
		case POSTFIX_INCREMENT:
		case POSTFIX_DECREMENT: {
			enum SimpleBinOp opkind2 = expr.category == POSTFIX_INCREMENT
			                               ? SIMPLE_BIN_OP_PLUS
			                               : SIMPLE_BIN_OP_MINUS;

			if (expr.ptr1->category != LOCAL_VAR_AS_LVALUE) {
				unimplemented("increment of non-(local variable)");
			}
			struct LocalVarInfo info;
			info.type = expr.ptr1->details.type;
			info.offset = expr.ptr1->details.offset;

			gen_push_from_local(info.offset);
			gen_push_int(1);

			print_simple_binary_op(opkind2);

			gen_write_to_local(info.offset);

			gen_push_int(-1);
			print_simple_binary_op(opkind2);

			return;
		}

		case LOCAL_VAR_AS_RVALUE: {
			struct LocalVarInfo info;
			info.type = expr.details.type;
			info.offset = expr.details.offset;

			if (is_array(expr.details.true_type)) {
				gen_push_address_of_local(info.offset);
				return;
			}
			switch (size_of(info.type)) {
				case 4:
					gen_push_from_local(info.offset);
					break;
				case 8:
					gen_push_from_local_8byte(info.offset);
					break;
				default:
					unimplemented("Unsupported width");
			}
			return;
		}

		case GLOBAL_VAR_AS_RVALUE: {
			struct Type type = expr.details.type;
			printf("//global `%s` as rvalue\n", expr.global_var_name);

			if (is_array(expr.details.true_type)) {
				gen_push_address_of_global(expr.global_var_name);
				return;
			}
			switch (size_of(type)) {
				case 4:
					gen_push_from_global_4byte(expr.global_var_name);
					break;
				case 8:
					gen_push_from_global_8byte(expr.global_var_name);
					break;
				default:
					unimplemented("Unsupported width");
			}
			return;
		}

		case LOCAL_VAR_AS_LVALUE:
		case GLOBAL_VAR_AS_LVALUE:
			return;
		case SIMPLE_BINARY_EXPR: {
			print_expression(ptr_ps, *expr.ptr1);
			print_expression(ptr_ps, *expr.ptr2);
			print_simple_binary_op(expr.simple_binary_operator);
			return;
		}
		case BINARY_EXPR:
			switch (expr.binary_operator) {
				case OP_OR_OR: {
					int label1 = get_new_label_name(ptr_ps);
					int label2 = get_new_label_name(ptr_ps);
					print_expression(ptr_ps, *expr.ptr1);

					gen_logical_OR_set(0, label1);
					print_expression(ptr_ps, *expr.ptr2);
					gen_logical_OR_set(1, label1);
					gen_logical_OR_final(1, label1, label2);
					return;
				}
				case OP_AND_AND: {
					int label1 = get_new_label_name(ptr_ps);
					int label2 = get_new_label_name(ptr_ps);
					print_expression(ptr_ps, *expr.ptr1);

					gen_logical_AND_set(0, label1);
					print_expression(ptr_ps, *expr.ptr2);
					gen_logical_AND_set(1, label1);
					gen_logical_AND_final(1, label1, label2);
					return;
				}
			}
		case ASSIGNMENT_EXPR: {
			{
				{
					print_expression_as_lvalue(ptr_ps, *expr.ptr1);
					print_expression(ptr_ps, *expr.ptr2);

					print_simple_binary_op(expr.simple_binary_operator);

					struct Type type = expr.ptr1->details.type;
					if (expr.ptr1->category == GLOBAL_VAR_AS_LVALUE) {
						const char *name = expr.ptr1->global_var_name;

						gen_discard2nd_8byte();

						printf("//assign to global `%s`\n", name);
						switch (size_of(type)) {
							case 4:
								gen_write_to_global_4byte(name);
								break;
							case 8:
								gen_write_to_global_8byte(name);
								break;
							default:
								unimplemented("Unsupported width in the "
								              "assignment operation");
						}
						return;
					} else if (expr.ptr1->category == LOCAL_VAR_AS_LVALUE) {

						gen_discard2nd_8byte();

						switch (size_of(type)) {
							case 4:
								gen_write_to_local(expr.ptr1->details.offset);
								break;
							case 8:
								gen_write_to_local_8byte(
								    expr.ptr1->details.offset);
								break;
							default:
								unimplemented("Unsupported width in the "
								              "assignment operation");
						}
						return;
					} else {

						switch (size_of(type)) {
							case 4:
								gen_assign_4byte();
								break;
							case 8:
								gen_assign_8byte();
								break;
						}
					}

					return;
				}
			}
		}
		case INT_VALUE:
			gen_push_int(expr.int_value);
			return;
		case UNARY_OP_EXPR:
			switch (expr.unary_operator) {
				case UNARY_OP_NOT:
					print_expression(ptr_ps, *expr.ptr1);
					gen_unary_not();
					return;
				case UNARY_OP_TILDA:
					print_expression(ptr_ps, *expr.ptr1);
					gen_unary("notl");
					return;
				case UNARY_OP_PLUS:
					print_expression(ptr_ps, *expr.ptr1);
					/* do nothing */
					return;
				case UNARY_OP_MINUS:
					print_expression(ptr_ps, *expr.ptr1);
					gen_unary("negl");
					return;

				case UNARY_OP_PLUS_PLUS:
				case UNARY_OP_MINUS_MINUS: {
					if (expr.ptr1->category != LOCAL_VAR_AS_LVALUE) {
						unimplemented("increment of non-(local variable)");
					}

					struct LocalVarInfo info;
					info.type = expr.ptr1->details.type;
					info.offset = expr.ptr1->details.offset;

					gen_push_from_local(info.offset);
					gen_push_int(1);
					print_simple_binary_op(expr.unary_operator ==
					                               UNARY_OP_PLUS_PLUS
					                           ? SIMPLE_BIN_OP_PLUS
					                           : SIMPLE_BIN_OP_MINUS);

					gen_write_to_local(info.offset);
					return;
				}

				case UNARY_OP_AND: {
					if (expr.ptr1->category == LOCAL_VAR_AS_LVALUE) {
						gen_push_address_of_local(expr.ptr1->details.offset);
						return;
					} else if (expr.ptr1->category == GLOBAL_VAR_AS_LVALUE) {
						gen_push_address_of_global(expr.ptr1->global_var_name);
						return;
					}
				}

				case UNARY_OP_ASTERISK: {
					print_expression(ptr_ps, *expr.ptr1);
					struct Type type = expr.details.type;
					switch (size_of(type)) {
						case 4:
							gen_peek_and_dereference();
							break;
						case 8:
							gen_peek_and_dereference_8byte();
							break;
						default:
							unimplemented("Unsupported width");
					}
					return;
				}
			}
			return;
		case CONDITIONAL_EXPR: {
			print_expression(ptr_ps, *expr.ptr1);
			int label1 = get_new_label_name(ptr_ps);
			int label2 = get_new_label_name(ptr_ps);

			gen_ternary_part1(label1, label2);
			print_expression(ptr_ps, *expr.ptr2);
			gen_ternary_part2(label1, label2);
			print_expression(ptr_ps, *expr.ptr3);

			gen_ternary_part3(label1, label2);
			return;
		}
		case FUNCCALL_EXPR: {
			const char *ident_str = expr.global_var_name;
			struct Type ret_type;
			if (!isElem(ptr_ps->func_info_map, ident_str)) {
				fprintf(stderr, "Undeclared function `%s()` detected.\n",
				        ident_str);
				fprintf(stderr, "Assumes that `%s()` returns `int`\n",
				        ident_str);
				ret_type = INT_TYPE;
			} else {
				struct FuncInfo *ptr_func_info =
				    lookup(ptr_ps->func_info_map, ident_str);
				ret_type = ptr_func_info->ret_type;
			}

			for (int counter = 0; counter < expr.arg_length; counter++) {
				print_argument_expression(ptr_ps, expr.arg_expr_vec[counter],
				                          counter);
			}
			switch (size_of(ret_type)) {
				case 4:
					gen_push_ret_of(ident_str);
					break;
				case 8:
					gen_push_ret_of_8byte(ident_str);
					break;
				default:
					unimplemented("Unsupported width");
			}
		}
	}
}

int is_local_var(struct LocalVarTableList t, const char *str)
{
	if (isElem(t.var_table, str)) {
		return 1;
	} else if (t.outer == 0) {
		/* most outer, but cannot be found */
		return 0;
	} else {
		return is_local_var(*(t.outer), str);
	}
}

struct Type resolve_name_globally(struct map m, const char *str)
{
	if (isElem(m, str)) {
		struct Type *ptr_type = lookup(m, str);
		return *ptr_type;
	} else {
		fprintf(stderr, "%s is not declared globally\n", str);
		exit(EXIT_FAILURE);
	}
}

struct LocalVarInfo resolve_name_locally(struct LocalVarTableList t,
                                         const char *str)
{
	if (isElem(t.var_table, str)) {
		struct LocalVarInfo *ptr_varinfo = lookup(t.var_table, str);
		return *ptr_varinfo;
	} else if (t.outer == 0) {
		/* most outer, but cannot be found */
		fprintf(stderr, "%s is not declared locally\n", str);
		exit(EXIT_FAILURE);
	} else {
		return resolve_name_locally(*(t.outer), str);
	}
}

int isAssign(enum TokenKind opkind)
{
	return (opkind == OP_EQ || opkind == OP_PLUS_EQ || opkind == OP_MINUS_EQ ||
	        opkind == OP_ASTERISK_EQ || opkind == OP_SLASH_EQ ||
	        opkind == OP_PERCENT_EQ || opkind == OP_LSHIFT_EQ ||
	        opkind == OP_RSHIFT_EQ || opkind == OP_AND_EQ ||
	        opkind == OP_HAT_EQ || opkind == OP_OR_EQ);
}

const char *get_reg_name_from_arg_pos(int counter)
{
	switch (counter) {
		case 0:
			return "edi";
		case 1:
			return "esi";
		case 2:
			return "edx";
		case 3:
			return "ecx";
		case 4:
			return "r8d";
		case 5:
			return "r9d";
		default:
			assert("cannot happen" && 0);
	}
}

const char *get_reg_name_from_arg_pos_8byte(int counter)
{
	switch (counter) {
		case 0:
			return "rdi";
		case 1:
			return "rsi";
		case 2:
			return "rdx";
		case 3:
			return "rcx";
		case 4:
			return "r8";
		case 5:
			return "r9";
		default:
			assert("cannot happen" && 0);
	}
}

void print_argument_expression(struct ParserState *ptr_ps,
                               struct Expression expr, int counter)
{
	print_expression(ptr_ps, expr);
	if (counter > 5) {
		unimplemented("calling with 7 or more arguments");
	}

	switch (size_of(expr.details.type)) {
		case 4:
			gen_pop_to_reg(get_reg_name_from_arg_pos(counter));
			break;
		case 8:
			gen_pop_to_reg_8byte(get_reg_name_from_arg_pos_8byte(counter));
			break;
		default:
			unimplemented("Unsupported width");
	}
}

int get_new_label_name(struct ParserState *ptr_ps)
{
	return ++(ptr_ps->final_label_name);
}

void parseprint_statement(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LEFT_BRACE) {
		parseprint_compound_statement(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
	} else if (tokvec[0].kind == RES_IF) { /* or SWITCH */
		int label1 = get_new_label_name(ptr_ps);
		int label2 = get_new_label_name(ptr_ps);
		++tokvec;

		expect_and_consume(&tokvec, LEFT_PAREN,
		                   "left parenthesis immediately after `if`");

		struct Expression expr = parse_expression(ptr_ps, &tokvec);

		print_expression(ptr_ps, expr);

		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `if`");

		gen_if_else_part1(label1, label2);

		parseprint_statement(ptr_ps, &tokvec);

		gen_if_else_part2(label1, label2);

		if (tokvec[0].kind == RES_ELSE) { /* must bind to the most inner one */
			++tokvec;
			parseprint_statement(ptr_ps, &tokvec);
		} else {
			/* do nothing */
		}

		gen_if_else_part3(label1, label2);

		*ptr_tokvec = tokvec;
	} else if (tokvec[0].kind == RES_RETURN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		if (tokvec[0].kind == SEMICOLON) {
			unimplemented("`return;`");
		} else {
			struct Expression expr = parse_expression(ptr_ps, &tokvec);

			print_expression(ptr_ps, expr);
			struct ExprInfo expr_info = expr.details;
			expect_type(expr_info, ptr_ps->func_ret_type, 20);

			expect_and_consume(
			    &tokvec, SEMICOLON,
			    "semicolon after `return` followed by an expression");

			*ptr_tokvec = tokvec;

			/* the first occurrence of return within a function */
			if (ptr_ps->return_label_name == GARBAGE_INT) {
				int ret_label = get_new_label_name(ptr_ps);
				ptr_ps->return_label_name = ret_label;
				gen_jump(ret_label, "return");
			} else {
				gen_jump(ptr_ps->return_label_name, "return");
			}

			return;
		}
	} else if (tokvec[0].kind == RES_DO) {

		int stashed_break_label = ptr_ps->break_label_name;
		int stashed_continue_label = ptr_ps->continue_label_name;

		++tokvec;
		*ptr_tokvec = tokvec;
		int label1 = get_new_label_name(ptr_ps);
		int break_label = get_new_label_name(ptr_ps);
		int cont_label = get_new_label_name(ptr_ps);

		ptr_ps->break_label_name = break_label;
		ptr_ps->continue_label_name = cont_label;

		gen_label(label1);
		parseprint_statement(ptr_ps, &tokvec);
		gen_label(cont_label);

		expect_and_consume(&tokvec, RES_WHILE, "`while` of do-while");
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of do-while");

		struct Expression expr = parse_expression(ptr_ps, &tokvec);

		print_expression(ptr_ps, expr);

		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "right parenthesis of do-while");
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after do-while");
		*ptr_tokvec = tokvec;

		gen_do_while_final(label1, break_label);

		ptr_ps->break_label_name = stashed_break_label;
		ptr_ps->continue_label_name = stashed_continue_label;

	} else if (tokvec[0].kind == RES_WHILE) {

		int stashed_break_label = ptr_ps->break_label_name;
		int stashed_continue_label = ptr_ps->continue_label_name;

		++tokvec;
		*ptr_tokvec = tokvec;

		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of while");

		int label1 = get_new_label_name(ptr_ps);
		int break_label = get_new_label_name(ptr_ps);
		int cont_label = get_new_label_name(ptr_ps);
		ptr_ps->break_label_name = break_label;
		ptr_ps->continue_label_name = cont_label;

		gen_label(label1);

		struct Expression expr = parse_expression(ptr_ps, &tokvec);

		print_expression(ptr_ps, expr);

		expect_and_consume(&tokvec, RIGHT_PAREN, "left parenthesis of while");

		gen_while_part2(label1, break_label);

		parseprint_statement(ptr_ps, &tokvec);

		gen_label(cont_label);
		gen_for_part4(label1, break_label);

		*ptr_tokvec = tokvec;

		ptr_ps->break_label_name = stashed_break_label;
		ptr_ps->continue_label_name = stashed_continue_label;

	} else if (tokvec[0].kind == RES_BREAK) {

		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `break`");
		*ptr_tokvec = tokvec;

		if (ptr_ps->break_label_name == GARBAGE_INT) {
			fprintf(stderr, "invalid `break`; no loop, no switch\n");
			exit(EXIT_FAILURE);
		} else {
			gen_jump(ptr_ps->break_label_name, "break");
		}

		return;
	} else if (tokvec[0].kind == RES_CONTINUE) {

		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `continue`");
		*ptr_tokvec = tokvec;

		if (ptr_ps->continue_label_name == GARBAGE_INT) {
			fprintf(stderr, "invalid `continue`; no loop\n");
			exit(EXIT_FAILURE);
		} else {
			gen_jump(ptr_ps->continue_label_name, "continue");
		}

		return;

	} else if (tokvec[0].kind == RES_FOR) {
		int stashed_break_label = ptr_ps->break_label_name;
		int stashed_continue_label = ptr_ps->continue_label_name;
		int label1 = get_new_label_name(ptr_ps);
		int break_label = get_new_label_name(ptr_ps);
		int cont_label = get_new_label_name(ptr_ps);
		ptr_ps->break_label_name = break_label;
		ptr_ps->continue_label_name = cont_label;

		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of `for`");

		if (tokvec[0].kind == SEMICOLON) { /* expression1 is missing */
			;
			/* do nothing */
		} else {
			struct Expression expr = parse_expression(ptr_ps, &tokvec);

			print_expression(ptr_ps, expr); /* expression1 */
			gen_discard();
		}

		expect_and_consume(&tokvec, SEMICOLON, "first semicolon of `for`");

		gen_label(label1);

		if (tokvec[0].kind == SEMICOLON) { /* expression2 is missing */
			gen_push_int(1);
		} else {
			struct Expression expr = parse_expression(ptr_ps, &tokvec);

			print_expression(ptr_ps, expr); /* expression2 */
		}

		expect_and_consume(&tokvec, SEMICOLON, "second semicolon of `for`");

		gen_while_part2(label1, break_label);

		if (tokvec[0].kind == RIGHT_PAREN) { /* expression3 is missing */
			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "right parenthesis of `for`");

			parseprint_statement(ptr_ps, &tokvec);

			gen_label(cont_label);
			gen_for_part4(label1, break_label);

			*ptr_tokvec = tokvec;

		} else {
			const struct Token *tokvec2 = tokvec;

			// expression3 of `for`
			struct Expression expr = parse_expression(ptr_ps, &tokvec2);

			expect_and_consume(&tokvec2, RIGHT_PAREN,
			                   "right parenthesis of `for`");

			parseprint_statement(ptr_ps, &tokvec2);

			gen_label(cont_label);

			print_expression(ptr_ps, expr);
			gen_discard();

			gen_for_part4(label1, break_label);

			tokvec = tokvec2;
		}

		ptr_ps->break_label_name = stashed_break_label;
		ptr_ps->continue_label_name = stashed_continue_label;
	} else {
		struct Expression expr = parse_expression(ptr_ps, &tokvec);

		print_expression(ptr_ps, expr);
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after an expression");

		gen_discard();
		*ptr_tokvec = tokvec;
		return;
	}
	*ptr_tokvec = tokvec;
}

void parse_final(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	expect_and_consume(&tokvec, END, "the end of file");
	return;
}

void parseprint_compound_statement(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LEFT_BRACE) {

		struct LocalVarTableList current_table = ptr_ps->scope_chain;

		struct LocalVarTableList new_table;
		new_table.var_table = init_int_map();

		/* current_table disappears at the end of this function,
		   but there is no problem because new_table itself gets overwritten at
		   the end of this function.
		 */
		new_table.outer = &current_table;

		ptr_ps->scope_chain = new_table;

		++tokvec;
		*ptr_tokvec = tokvec;
		while (1) {
			if (tokvec[0].kind == RIGHT_BRACE) {
				++tokvec;
				*ptr_tokvec = tokvec;
				ptr_ps->scope_chain = current_table;

				return;
			} else if (can_start_a_type(tokvec)) {
				struct Type vartype;

				const char *str;
				vartype = parse_var_declarator(&tokvec, &str);
				/* while function prototypes are also allowed here, I will not
				 * implement it here */
				ptr_ps->newest_offset -= size_of(vartype);
				expect_and_consume(
				    &tokvec, SEMICOLON,
				    "semicolon at the end of variable definition");

				struct map map_ = ptr_ps->scope_chain.var_table;

				struct LocalVarInfo *ptr_varinfo =
				    calloc(1, sizeof(struct LocalVarInfo));
				ptr_varinfo->offset = ptr_ps->newest_offset;
				ptr_varinfo->type = vartype;
				insert(&map_, str, ptr_varinfo);

				ptr_ps->scope_chain.var_table = map_;
			} else {
				parseprint_statement(ptr_ps, &tokvec);
			}
		}
	}
}

void print_parameter_declaration(struct ParserState *ptr_ps, int counter,
                                 struct ParamInfos param_infos)
{

	struct ParamInfo param_info = *(param_infos.param_vec[counter]);
	const char *ident_str;

	struct Type type = param_info.param_type;
	ident_str = param_info.ident_str;

	if (counter > 5) {
		unimplemented("6-or-more args");
	}

	ptr_ps->newest_offset -= size_of(type);

	struct map map_ = ptr_ps->scope_chain.var_table;
	struct LocalVarInfo *ptr_varinfo = calloc(1, sizeof(struct LocalVarInfo));
	ptr_varinfo->offset = ptr_ps->newest_offset;
	ptr_varinfo->type = type;

	insert(&map_, ident_str, ptr_varinfo);

	ptr_ps->scope_chain.var_table = map_;

	switch (size_of(type)) {
		case 4:
			gen_write_register_to_local(
			    get_reg_name_from_arg_pos(counter),
			    resolve_name_locally(ptr_ps->scope_chain, ident_str).offset);
			break;
		case 8:
			gen_write_register_to_local_8byte(
			    get_reg_name_from_arg_pos_8byte(counter),
			    resolve_name_locally(ptr_ps->scope_chain, ident_str).offset);
			break;
		default:
			unimplemented("Unsupported width in function argument");
	}
}

void parseprint_toplevel_definition(struct ParserState *ptr_ps,
                                    const struct Token **ptr_tokvec)
{

	const char *declarator_name;
	const struct Token *tokvec2 = *ptr_tokvec;
	struct Type declarator_type = parse_declarator(&tokvec2, &declarator_name);
	if (declarator_type.type_category != FN && tokvec2[0].kind == SEMICOLON) {
		++tokvec2; /* consume the semicolon */
		struct map globalmap = ptr_ps->global_vars_type_map;

		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = declarator_type;
		insert(&globalmap, declarator_name, ptr_type);
		ptr_ps->global_vars_type_map = globalmap;

		gen_global_declaration(declarator_name, size_of(declarator_type));

		*ptr_tokvec = tokvec2;
		return;
	}

	struct ParamInfos param_infos = declarator_type.param_infos;
	struct Type ret_type = *declarator_type.derived_from;

	ptr_ps->scope_chain.outer = 0; /* most outer scope */
	ptr_ps->scope_chain.var_table = init_int_map();
	ptr_ps->return_label_name = GARBAGE_INT;   /* INITIALIZE */
	ptr_ps->break_label_name = GARBAGE_INT;    /* INITIALIZE */
	ptr_ps->continue_label_name = GARBAGE_INT; /* INITIALIZE */
	/* 8 is the space to store the address to handle deref */
	ptr_ps->newest_offset = -8;
	ptr_ps->func_ret_type = ret_type;

	struct map retmap = ptr_ps->func_info_map;

	struct FuncInfo *ptr_func_info = calloc(1, sizeof(struct FuncInfo));
	ptr_func_info->ret_type = ret_type;
	insert(&retmap, declarator_name, ptr_func_info);

	ptr_ps->func_info_map = retmap;

	int label1;
	int label2;

	if (!param_infos.param_vec) {           /* empty parameter */
		if (tokvec2[0].kind == SEMICOLON) { /* function prototype */
			++tokvec2;
			/* do nothing, since the return value is already in the retmap
			 */
			*ptr_tokvec = tokvec2;
			return;
		}

		label1 = get_new_label_name(ptr_ps);
		label2 = get_new_label_name(ptr_ps);
		gen_prologue(0, declarator_name);
		gen_after_prologue(label1, label2);

	} else {
		label1 = get_new_label_name(ptr_ps);
		label2 = get_new_label_name(ptr_ps);
		gen_prologue(0, declarator_name);
		gen_after_prologue(label1, label2);

		int counter = 0;

		do {
			print_parameter_declaration(ptr_ps, counter, param_infos);
			++counter;
		} while (param_infos.param_vec[counter]);
	}

	parseprint_compound_statement(ptr_ps, &tokvec2);

	gen_before_epilogue(label1, label2, -(ptr_ps->newest_offset));
	switch (size_of(ret_type)) {
		case 4:
			gen_epilogue(ptr_ps->return_label_name);
			break;
		case 8:
			gen_epilogue_8byte(ptr_ps->return_label_name);
			break;
		default:
			unimplemented("Unsupported width");
	}

	*ptr_tokvec = tokvec2;
}

int main(int argc, char const **argv)
{
	char *str;

	str = calloc(10000, sizeof(char));

	/* const char* str = "123+456-789"; */
	scanf("%[^\n]s", str); /* VULNERABLE!!! */
	if (argc == 2) {
		if (strcmp(argv[1], "--lexer-debug") == 0) {
			read_all_tokens_debug(str);
		}
	} else {
		const struct Token *tokvec = read_all_tokens_(str);

		++tokvec; /* skip the dummy token BEGINNING */

		struct ParserState ps;
		ps.final_label_name = 1;
		ps.return_label_name = GARBAGE_INT;
		ps.func_info_map = init_int_map();
		ps.global_vars_type_map = init_int_map();

		while (1) {
			if (tokvec[0].kind == END) {
				parse_final(&tokvec);
				return 0;
			} else {
				parseprint_toplevel_definition(&ps, &tokvec);
			}
		}
	}
	return 0;
}
