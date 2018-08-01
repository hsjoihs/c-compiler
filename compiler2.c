#include "header.h"
#include "print_x86_64.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct LocalVarInfo {
	struct Type type;
	int offset;
};

struct LocalVarTableList {
	struct map var_table;
	struct LocalVarTableList *outer;
};

struct ParserState {
	struct LocalVarTableList scope_chain;
	struct map global_vars_type_map;
	struct map func_ret_type_map;
	int newest_offset;
	int final_label_name;
	int return_label_name; /* the label at the end of the function */
	struct Type func_ret_type;
	int break_label_name;    /* the label at the end of the current loop */
	int continue_label_name; /* the label at the beginning of the current loop
	                          */
};

int get_new_label_name(struct ParserState *ptr_ps);

struct ExprInfo remove_leftiness(struct ExprInfo info)
{
	if (info.info == LOCAL_VAR) {
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

struct ExprInfo parseprint_expression(struct ParserState *ptr_ps,
                                      const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo info = parseprint_assignment_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_COMMA) {
			break;
		}
		++tokvec;
		info =
		    remove_leftiness(parseprint_assignment_expression(ptr_ps, &tokvec));
		print_binary_op(OP_COMMA);
	}
	*ptr_tokvec = tokvec;
	return info;
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

void print_before_assign(enum TokenKind kind)
{
	switch (kind) {
		case OP_PLUS_EQ:
		case OP_PLUS_PLUS:
			print_binary_op(OP_PLUS);
			return;
		case OP_MINUS_EQ:
		case OP_MINUS_MINUS:
			print_binary_op(OP_MINUS);
			return;
		case OP_ASTERISK_EQ:
			print_binary_op(OP_ASTERISK);
			return;
		case OP_SLASH_EQ:
			print_binary_op(OP_SLASH);
			return;
		case OP_PERCENT_EQ:
			print_binary_op(OP_PERCENT);
			return;
		case OP_LSHIFT_EQ:
			print_binary_op(OP_LSHIFT);
			return;
		case OP_RSHIFT_EQ:
			print_binary_op(OP_RSHIFT);
			return;
		case OP_AND_EQ:
			print_binary_op(OP_AND);
			return;
		case OP_HAT_EQ:
			print_binary_op(OP_HAT);
			return;
		case OP_OR_EQ:
			print_binary_op(OP_OR);
			return;
		default:
			assert("cannot happen" && 0);
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

struct ExprInfo
parseprint_assignment_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	enum TokenKind opkind = tokvec[1].kind;

	if (tokvec[0].kind == IDENT_OR_RESERVED && isAssign(opkind)) {
		const char *name = tokvec[0].ident_str;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		if (!is_local_var(ptr_ps->scope_chain, name)) {
			unimplemented("assignment to a global variable");
		}
		struct LocalVarInfo info =
		    resolve_name_locally(ptr_ps->scope_chain, name);

		if (opkind != OP_EQ) {
			printf("//load from `%s`\n", name);
			switch (size_of(info.type)) {
				case 4:
					gen_push_from_local(info.offset);
					break;
				case 8:
					gen_push_from_local_8byte(info.offset);
					break;
				default:
					unimplemented(
					    "Unsupported width in the assignment operation");
			}
		}

		struct ExprInfo expr_info =
		    parseprint_assignment_expression(ptr_ps, &tokvec);
		expect_type(expr_info, info.type, 0);

		if (opkind != OP_EQ) {
			printf("//before assigning to `%s`:\n", name);
			print_before_assign(opkind);
		}

		printf("//assign to `%s`\n", name);
		switch (size_of(info.type)) {
			case 4:
				gen_write_to_local(info.offset);
				break;
			case 8:
				gen_write_to_local_8byte(info.offset);
				break;
			default:
				unimplemented("Unsupported width in the assignment operation");
		}
		*ptr_tokvec = tokvec;
		return UNASSIGNABLE(info.type);
	}

	int label = get_new_label_name(ptr_ps);

	const struct Token *tokvec2 = tokvec;
	struct ParserState *ptr_ps2 = ptr_ps;
	gen_jump(label, "commenting out");
	parseprint_unary_expression(ptr_ps2, &tokvec2);
	printf("// comment finishes\n");
	printf("  .L%d:\n", label);

	/* parse failed */
	if (!isAssign(tokvec2[0].kind)) {
		struct ExprInfo expr_info =
		    parseprint_conditional_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr_info;
	}

	struct ExprInfo expr_info = parseprint_unary_expression(ptr_ps, &tokvec);

	assert(isAssign(tokvec[0].kind));
	switch (expr_info.info) {
		case LOCAL_VAR:
			assert("supposed to be handled separately, at least for now" && 0);
			exit(EXIT_FAILURE);
			break;
		case NOT_ASSIGNABLE:
			fprintf(stderr, "Expected an lvalue, but did not get one.\n");
			exit(EXIT_FAILURE);
			break;
		case DEREFERENCED_ADDRESS: {
			enum TokenKind opkind = tokvec[0].kind;
			++tokvec;

			/* push the backup */
			gen_push_from_local_8byte(-8);

			struct ExprInfo expr_info2 =
			    parseprint_assignment_expression(ptr_ps, &tokvec);
			expect_type(expr_info, expr_info2.type, 19);

			gen_pop2nd_to_local_8byte(-8);
			if (opkind != OP_EQ) {
				print_before_assign(opkind);
			} else {
				gen_discard2nd_8byte();
			}
			gen_assign_to_backed_up_address();
			*ptr_tokvec = tokvec;
			return remove_leftiness(expr_info);
		};
	}
}

struct ExprInfo
parseprint_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info =
	    parseprint_logical_OR_expression(ptr_ps, &tokvec);
	if (tokvec[0].kind == QUESTION) {
		int label1 = get_new_label_name(ptr_ps);
		int label2 = get_new_label_name(ptr_ps);

		gen_ternary_part1(label1, label2);
		++tokvec;
		*ptr_tokvec = tokvec;
		struct ExprInfo true_branch_info =
		    parseprint_expression(ptr_ps, &tokvec);

		gen_ternary_part2(label1, label2);

		expect_and_consume(&tokvec, COLON, "colon of the conditional operator");

		*ptr_tokvec = tokvec;
		struct ExprInfo false_branch_info =
		    parseprint_conditional_expression(ptr_ps, &tokvec);

		gen_ternary_part3(label1, label2);

		*ptr_tokvec = tokvec;

		expect_type(false_branch_info, true_branch_info.type, 1);
		return remove_leftiness(true_branch_info);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct ExprInfo
parseprint_logical_OR_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	int label1 = get_new_label_name(ptr_ps);
	int label2 = get_new_label_name(ptr_ps);

	int counter = 0;
	struct ExprInfo first_expr_info =
	    parseprint_logical_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR_OR) {
			break;
		}

		if (counter == 0) {
			gen_logical_OR_set(0, label1, label2);
		}

		++tokvec;
		parseprint_logical_AND_expression(ptr_ps, &tokvec);
		++counter;
		gen_logical_OR_set(counter, label1, label2);
	}

	if (counter != 0) {
		gen_logical_OR_final(counter, label1, label2);
		*ptr_tokvec = tokvec;
		return UNASSIGNABLE(INT_TYPE);
	}

	*ptr_tokvec = tokvec;
	return first_expr_info;
}

struct ExprInfo
parseprint_logical_AND_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	int label1 = get_new_label_name(ptr_ps);
	int label2 = get_new_label_name(ptr_ps);

	int counter = 0;
	struct ExprInfo first_expr_info =
	    parseprint_inclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND_AND) {
			break;
		}

		if (counter == 0) {
			gen_logical_AND_set(0, label1, label2);
		}

		++tokvec;
		parseprint_inclusive_OR_expression(ptr_ps, &tokvec);
		++counter;
		gen_logical_AND_set(counter, label1, label2);
	}

	if (counter != 0) {
		gen_logical_AND_final(counter, label1, label2);
		*ptr_tokvec = tokvec;
		return UNASSIGNABLE(INT_TYPE);
	}

	*ptr_tokvec = tokvec;
	return first_expr_info;
}

struct ExprInfo parseprint_cast_expression(struct ParserState *ptr_ps,
                                           const struct Token **ptr_tokvec)
{
	return parseprint_unary_expression(ptr_ps, ptr_tokvec);
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

void parseprint_argument_expression(struct ParserState *ptr_ps,
                                    const struct Token **ptr_tokvec,
                                    int counter)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct ExprInfo expr_info =
	    parseprint_assignment_expression(ptr_ps, &tokvec);
	if (counter > 5) {
		unimplemented("calling with 7 or more arguments");
	}

	switch (size_of(expr_info.type)) {
		case 4:
			gen_pop_to_reg(get_reg_name_from_arg_pos(counter));
			break;
		case 8:
			gen_pop_to_reg_8byte(get_reg_name_from_arg_pos_8byte(counter));
			break;
		default:
			unimplemented("Unsupported width");
	}

	*ptr_tokvec = tokvec;
}

struct ExprInfo parseprint_postfix_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;

		struct Type ret_type;
		if (!isElem(ptr_ps->func_ret_type_map, ident_str)) {
			fprintf(stderr, "Undeclared function `%s()` detected.\n",
			        ident_str);
			fprintf(stderr, "Assumes that `%s()` returns `int`\n", ident_str);
			ret_type = INT_TYPE;
		} else {
			struct FuncInfo *ptr_func_info =
			    lookup(ptr_ps->func_ret_type_map, ident_str);
			ret_type = ptr_func_info->ret_type;
		}

		tokvec += 2;
		if (tokvec[0].kind == RIGHT_PAREN) {
			switch (size_of(ret_type)) {
				case 4:
					gen_push_ret_of(ident_str);
					break;
				case 8:
					gen_push_ret_of_8byte(ident_str);
					break;
				default:
					unimplemented("Unsupported width in the return type");
			}

			tokvec++;
		} else {
			int counter = 0;

			parseprint_argument_expression(ptr_ps, &tokvec, counter);
			++counter;

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;
				parseprint_argument_expression(ptr_ps, &tokvec, counter);
				++counter;
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
			*ptr_tokvec = tokvec;

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing parenthesis of function call");

			*ptr_tokvec = tokvec;
		}
		*ptr_tokvec = tokvec;
		return UNASSIGNABLE(ret_type);

	} else if (tokvec[0].kind == IDENT_OR_RESERVED &&
	           (tokvec[1].kind == OP_PLUS_PLUS ||
	            tokvec[1].kind == OP_MINUS_MINUS)) {
		const char *name = tokvec[0].ident_str;
		enum TokenKind opkind = tokvec[1].kind;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		print_inc_or_dec(ptr_ps, name, opkind);

		gen_push_int(-1);
		print_before_assign(opkind);
		*ptr_tokvec = tokvec;
		return UNASSIGNABLE(INT_TYPE);
	} else {
		struct ExprInfo expr_info =
		    parseprint_primary_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr_info;
	}
}

struct ExprInfo parseprint_primary_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_tokvec;
		gen_push_int(tokvec[0].int_value);

		return UNASSIGNABLE(INT_TYPE);

	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_tokvec;

		if (!is_local_var(ptr_ps->scope_chain, tokvec[0].ident_str)) {
			unimplemented("global var as a primary expression");
		}
		struct LocalVarInfo info =
		    resolve_name_locally(ptr_ps->scope_chain, tokvec[0].ident_str);

		printf("//`%s` as rvalue\n", tokvec[0].ident_str);
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

		struct ExprInfo expr_info;
		expr_info.info = LOCAL_VAR;
		expr_info.type = info.type;
		expr_info.offset = info.offset;
		return expr_info;
	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		struct ExprInfo expr_info = parseprint_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right paren");

		*ptr_tokvec = tokvec;
		return expr_info;
	}

	error_unexpected_token(tokvec,
	                       "the beginning of parseprint_primary_expression");
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

		parseprint_expression(ptr_ps, &tokvec);

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
			struct ExprInfo expr_info = parseprint_expression(ptr_ps, &tokvec);
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

		parseprint_expression(ptr_ps, &tokvec);

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

		parseprint_expression(ptr_ps, &tokvec);

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
			parseprint_expression(ptr_ps, &tokvec); /* expression1 */
			gen_discard();
		}

		expect_and_consume(&tokvec, SEMICOLON, "first semicolon of `for`");

		gen_label(label1);

		if (tokvec[0].kind == SEMICOLON) { /* expression2 is missing */
			gen_push_int(1);
		} else {
			parseprint_expression(ptr_ps, &tokvec); /* expression2 */
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

			int lab = get_new_label_name(ptr_ps);

			gen_jump(lab, "commenting out, to handle expression3 of `for`");
			parseprint_expression(ptr_ps, &tokvec2);
			printf("// comment finishes\n");
			printf(".L%d:\n", lab);

			expect_and_consume(&tokvec2, RIGHT_PAREN,
			                   "right parenthesis of `for`");

			parseprint_statement(ptr_ps, &tokvec2);

			gen_label(cont_label);

			printf("// what was previously ignored\n");

			parseprint_expression(ptr_ps, &tokvec);
			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "right parenthesis of `for`");
			gen_discard();

			gen_for_part4(label1, break_label);

			tokvec = tokvec2;
		}

		ptr_ps->break_label_name = stashed_break_label;
		ptr_ps->continue_label_name = stashed_continue_label;
	} else {
		parseprint_expression(ptr_ps, &tokvec);
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

	struct map retmap = ptr_ps->func_ret_type_map;

	struct FuncInfo *ptr_func_info = calloc(1, sizeof(struct FuncInfo));
	ptr_func_info->ret_type = ret_type;
	insert(&retmap, declarator_name, ptr_func_info);

	ptr_ps->func_ret_type_map = retmap;

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

void print_inc_or_dec(struct ParserState *ptr_ps, const char *name,
                      enum TokenKind opkind)
{
	if (!is_local_var(ptr_ps->scope_chain, name)) {
		unimplemented("incrementing or decrementing a global variable");
	}
	struct LocalVarInfo info = resolve_name_locally(ptr_ps->scope_chain, name);

	printf("//load from `%s`\n", name);
	gen_push_from_local(info.offset);
	gen_push_int(1);

	printf("//before assigning to `%s`:\n", name);
	print_before_assign(opkind);

	printf("//assign to `%s`\n", name);
	gen_write_to_local(info.offset);
}

struct ExprInfo parseprint_unary_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	/* unary-operator cast-expression */
	if (tokvec[0].kind == OP_NOT || tokvec[0].kind == OP_TILDA ||
	    tokvec[0].kind == OP_PLUS || tokvec[0].kind == OP_MINUS) {
		enum TokenKind kind = tokvec[0].kind;
		++tokvec;
		struct ExprInfo expr_info =
		    remove_leftiness(parseprint_cast_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 2);
		print_unary_prefix_op(kind);

		*ptr_tokvec = tokvec;
		return remove_leftiness(expr_info);
	} else if (tokvec[0].kind == OP_PLUS_PLUS ||
	           tokvec[0].kind == OP_MINUS_MINUS) {
		enum TokenKind opkind = tokvec[0].kind;
		++tokvec;
		if (tokvec[0].kind == IDENT_OR_RESERVED) {
			const char *name = tokvec[0].ident_str;
			++tokvec;
			*ptr_tokvec = tokvec;

			print_inc_or_dec(ptr_ps, name, opkind);
		} else {
			unimplemented("++ followed by non-identifier");
		}
		*ptr_tokvec = tokvec;
		return UNASSIGNABLE(INT_TYPE);
	} else if (tokvec[0].kind == OP_AND) {
		if (tokvec[1].kind == IDENT_OR_RESERVED) {
			const char *name = tokvec[1].ident_str;

			tokvec += 2;

			if (!is_local_var(ptr_ps->scope_chain, name)) {
				unimplemented("& of a global variable");
			}
			struct LocalVarInfo info =
			    resolve_name_locally(ptr_ps->scope_chain, name);
			gen_push_address_of_local(info.offset);
			*ptr_tokvec = tokvec;

			struct Type *ptr_type = calloc(1, sizeof(struct Type));
			*ptr_type = info.type;

			return UNASSIGNABLE(ptr_of_type_to_ptr_to_type(ptr_type));
		} else {
			unimplemented("& followed by non-identifier");
		}

	} else if (tokvec[0].kind == OP_ASTERISK) {
		++tokvec;
		struct ExprInfo expr_info =
		    remove_leftiness(parseprint_cast_expression(ptr_ps, &tokvec));
		struct Type type = deref_type(expr_info.type);

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

		struct ExprInfo new_expr_info;
		new_expr_info.info = DEREFERENCED_ADDRESS;
		new_expr_info.type = type;
		new_expr_info.offset = GARBAGE_INT;

		*ptr_tokvec = tokvec;
		return new_expr_info;
	} else {
		struct ExprInfo expr_info =
		    parseprint_postfix_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr_info;
	}
}

struct ExprInfo UNASSIGNABLE(struct Type type) {
	struct ExprInfo expr_info;
	expr_info.info = NOT_ASSIGNABLE;
	expr_info.type = type;
	expr_info.offset = GARBAGE_INT;

	return expr_info;
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
		ps.func_ret_type_map = init_int_map();
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
