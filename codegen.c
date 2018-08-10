#include "codegen.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

const char *get_reg_name_from_arg_pos_4byte(int counter)
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

int get_new_label_name(struct PrinterState *ptr_prs)
{
	return ++(ptr_prs->final_label_name);
}

struct Expression integer_1(void)
{
	struct Expression expr;
	expr.details.type = INT_TYPE;
	expr.int_value = 1;
	expr.category = INT_VALUE;

	return expr;
}

enum StatementCategory {
	UNKNOWN = 1,
	COMPOUND_STATEMENT,
	IF_STATEMENT,
	IF_ELSE_STATEMENT,
	FOR_STATEMENT,
	WHILE_STATEMENT,
	DO_WHILE_STATEMENT,
	RETURN_STATEMENT,
	BREAK_STATEMENT,
	CONTINUE_STATEMENT,
	TOPLEVEL_VAR_DEFINITION,
	TOPLEVEL_FUNCTION_DEFINITION,
	EXPRESSION_STATEMENT,
	DECLARATION_STATEMENT,
};

struct Statement {
	enum StatementCategory category;
	struct Vector statement_vector;
	struct Expression expr1;
	struct Expression expr2;
	struct Expression expr3;
	struct Statement *inner_statement;
	struct {
		struct Type type;
		const char *ident_str;
	} declaration;
};

static struct Statement
parse_compound_statement(struct ParserState *ptr_ps,
                         const struct Token **ptr_tokvec);

static struct Statement parse_statement(struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LEFT_BRACE) {
		struct Statement s = parse_compound_statement(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return s;
	}

	if (tokvec[0].kind == RES_IF) { /* or SWITCH */
		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN,
		                   "left parenthesis immediately after `if`");
		struct Expression expr = parse_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `if`");

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;

		if (tokvec[0].kind == RES_ELSE) { /* must bind to the most inner one */
			++tokvec;
			struct Statement inner_s2 = parse_statement(ptr_ps, &tokvec);
			struct Statement *ptr_inner_s2 =
			    calloc(1, sizeof(struct Statement));
			*ptr_inner_s2 = inner_s2;

			*ptr_tokvec = tokvec;

			struct Statement s;
			s.category = IF_ELSE_STATEMENT;
			s.expr1 = expr;
			s.statement_vector = init_vector();
			push_vector(&s.statement_vector, ptr_inner_s);
			push_vector(&s.statement_vector, ptr_inner_s2);
			return s;
		} else {

			*ptr_tokvec = tokvec;

			struct Statement s;
			s.category = IF_STATEMENT;
			s.expr1 = expr;

			s.inner_statement = ptr_inner_s;
			return s;
		}
	}

	if (tokvec[0].kind == RES_RETURN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		if (tokvec[0].kind == SEMICOLON) {
			unsupported("`return;`");
		} else {
			struct Expression expr = parse_expression(ptr_ps, &tokvec);
			expect_type(expr.details.type, ptr_ps->func_ret_type,
			            "mismatched type in the return value");
			expect_and_consume(
			    &tokvec, SEMICOLON,
			    "semicolon after `return` followed by an expression");
			*ptr_tokvec = tokvec;

			struct Statement s;
			s.category = RETURN_STATEMENT;
			s.expr1 = expr;
			return s;
		}
	}

	if (tokvec[0].kind == RES_DO) {
		++tokvec;

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RES_WHILE, "`while` of do-while");
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of do-while");

		struct Expression expr = parse_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "right parenthesis of do-while");
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after do-while");
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = DO_WHILE_STATEMENT;
		s.expr1 = expr;

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	if (tokvec[0].kind == RES_WHILE) {
		++tokvec;

		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of while");

		struct Expression expr = parse_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RIGHT_PAREN, "left parenthesis of while");

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = WHILE_STATEMENT;
		s.expr1 = expr;

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	if (tokvec[0].kind == RES_BREAK) {
		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `break`");
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = BREAK_STATEMENT;
		return s;
	}

	if (tokvec[0].kind == RES_CONTINUE) {
		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `continue`");
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = CONTINUE_STATEMENT;
		return s;
	}

	if (tokvec[0].kind == RES_FOR) {

		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of `for`");

		struct Expression expr1;
		struct Expression expr2;
		struct Expression expr3;

		if (tokvec[0].kind == SEMICOLON) { /* expression1 is missing */
			expr1 = integer_1();
		} else {
			expr1 = parse_expression(ptr_ps, &tokvec);
		}
		expect_and_consume(&tokvec, SEMICOLON, "first semicolon of `for`");

		if (tokvec[0].kind == SEMICOLON) { /* expression2 is missing */
			expr2 = integer_1();
		} else {
			expr2 = parse_expression(ptr_ps, &tokvec);
		}
		expect_and_consume(&tokvec, SEMICOLON, "second semicolon of `for`");

		if (tokvec[0].kind == RIGHT_PAREN) { /* expression3 is missing */
			expr3 = integer_1();
		} else {
			expr3 = parse_expression(ptr_ps, &tokvec);
		}
		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `for`");

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;

		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = FOR_STATEMENT;
		s.expr1 = expr1;
		s.expr2 = expr2;
		s.expr3 = expr3;
		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	struct Expression expr = parse_expression(ptr_ps, &tokvec);

	expect_and_consume(&tokvec, SEMICOLON, "semicolon after an expression");

	*ptr_tokvec = tokvec;

	struct Statement s;
	s.category = EXPRESSION_STATEMENT;
	s.expr1 = expr;
	return s;
}

static void update_ptr_ps(struct ParserState *ptr_ps, const struct Type vartype,
                          const char *str)
{
	ptr_ps->newest_offset -= size_of(vartype) < 4 ? 4 : size_of(vartype);

	struct Map map_ = ptr_ps->scope_chain.var_table;

	struct LocalVarInfo *ptr_varinfo = calloc(1, sizeof(struct LocalVarInfo));
	ptr_varinfo->offset = ptr_ps->newest_offset;
	ptr_varinfo->type = vartype;
	insert(&map_, str, ptr_varinfo);

	ptr_ps->scope_chain.var_table = map_;
}

static struct Statement
parse_compound_statement(struct ParserState *ptr_ps,
                         const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Statement statement;
	statement.category = COMPOUND_STATEMENT;
	statement.statement_vector = init_vector();
	if (tokvec[0].kind == LEFT_BRACE) {

		struct LocalVarTableList current_table = ptr_ps->scope_chain;

		struct LocalVarTableList new_table;
		new_table.var_table = init_map();

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

				return statement;
			} else if (can_start_a_type(tokvec)) {
				struct Type vartype;

				const char *str;
				vartype = parse_declarator(&tokvec, &str);

				/* while function prototypes are also allowed here in C, I will
				 * not implement it here */
				if (vartype.type_category == FN) {
					fprintf(stderr, "cannot declare function here\n");
					exit(EXIT_FAILURE);
				}
				expect_and_consume(
				    &tokvec, SEMICOLON,
				    "semicolon at the end of variable definition");

				update_ptr_ps(ptr_ps, vartype, str);

				struct Statement s;
				s.category = DECLARATION_STATEMENT;
				s.declaration.type = vartype;
				s.declaration.ident_str = str;
				struct Statement *ptr_s = calloc(1, sizeof(struct Statement));
				*ptr_s = s;
				push_vector(&statement.statement_vector, ptr_s);
			} else {
				struct Statement s = parse_statement(ptr_ps, &tokvec);
				struct Statement *ptr_s = calloc(1, sizeof(struct Statement));
				*ptr_s = s;
				push_vector(&statement.statement_vector, ptr_s);
			}
		}
	}
	exit(EXIT_FAILURE);
}

static void parseprint_compound_statement(struct ParserState *ptr_ps,
                                          struct PrinterState *ptr_prs,
                                          const struct Token **ptr_tokvec,
                                          struct Statement sta);

static void print_statement(struct PrinterState *ptr_prs, struct Statement sta)
{
	switch (sta.category) {
		case BREAK_STATEMENT: {
			if (ptr_prs->break_label_name == GARBAGE_INT) {
				fprintf(stderr, "invalid `break`; no loop, no switch\n");
				exit(EXIT_FAILURE);
			} else {
				gen_jump(ptr_prs->break_label_name, "break");
			}
			return;
		}
		case EXPRESSION_STATEMENT: {
			print_expression(ptr_prs, sta.expr1);
			gen_discard();
			return;
		}
		case RETURN_STATEMENT: {

			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);
			/* the first occurrence of return within a function */
			if (ptr_prs->return_label_name == GARBAGE_INT) {
				int ret_label = get_new_label_name(ptr_prs);
				ptr_prs->return_label_name = ret_label;
				gen_jump(ret_label, "return");
			} else {
				gen_jump(ptr_prs->return_label_name, "return");
			}

			struct Statement s;
			s.category = RETURN_STATEMENT;
			s.expr1 = expr;
			return;
		}
		case CONTINUE_STATEMENT: {
			if (ptr_prs->continue_label_name == GARBAGE_INT) {
				fprintf(stderr, "invalid `continue`; no loop\n");
				exit(EXIT_FAILURE);
			} else {
				gen_jump(ptr_prs->continue_label_name, "continue");
			}
			return;
		}
	}
}

static void parseprint_statement(struct ParserState *ptr_ps,
                                 struct PrinterState *ptr_prs,
                                 const struct Token **ptr_tokvec,
                                 struct Statement sta)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (sta.category == BREAK_STATEMENT ||
	    sta.category == EXPRESSION_STATEMENT ||
	    sta.category == RETURN_STATEMENT ||
	    sta.category == CONTINUE_STATEMENT) {
		parse_statement(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
	}

	if (sta.category == COMPOUND_STATEMENT) {
		const struct Token *tokvec2 = tokvec;
		struct Statement s = parse_compound_statement(ptr_ps, &tokvec2);
		parseprint_compound_statement(ptr_ps, ptr_prs, &tokvec, s);
		assert(tokvec2 == tokvec);

		*ptr_tokvec = tokvec;
		return;
	}

	if (sta.category == IF_STATEMENT || sta.category == IF_ELSE_STATEMENT) {
		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN,
		                   "left parenthesis immediately after `if`");
		struct Expression expr = parse_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `if`");

		const struct Token *tokvec2 = tokvec;
		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;

		if (tokvec[0].kind == RES_ELSE) { /* must bind to the most inner one */
			++tokvec;
			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			print_expression(ptr_prs, expr);

			gen_if_else_part1(label1, label2);

			parseprint_statement(ptr_ps, ptr_prs, &tokvec2, inner_s);
			assert(tokvec2 == tokvec - 1);

			gen_if_else_part2(label1, label2);

			const struct Token *tokvec3 = tokvec;
			struct Statement inner_s2 = parse_statement(ptr_ps, &tokvec3);
			parseprint_statement(ptr_ps, ptr_prs, &tokvec, inner_s2);
			assert(tokvec3 == tokvec);
			gen_if_else_part3(label1, label2);
			struct Statement *ptr_inner_s2 =
			    calloc(1, sizeof(struct Statement));
			*ptr_inner_s2 = inner_s2;

			*ptr_tokvec = tokvec;

			struct Statement s;
			s.category = IF_ELSE_STATEMENT;
			s.expr1 = expr;
			s.statement_vector = init_vector();
			push_vector(&s.statement_vector, ptr_inner_s);
			push_vector(&s.statement_vector, ptr_inner_s2);
			return;
		} else {
			*ptr_tokvec = tokvec;

			struct Statement s;
			s.category = IF_STATEMENT;
			s.expr1 = expr;

			s.inner_statement = ptr_inner_s;
			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			print_expression(ptr_prs, expr);

			gen_if_else_part1(label1, label2);

			parseprint_statement(ptr_ps, ptr_prs, &tokvec2, inner_s);
			assert(tokvec2 == tokvec);

			gen_if_else_part2(label1, label2);
			gen_if_else_part3(label1, label2);

			return;
		}
	}

	if (sta.category == RETURN_STATEMENT) {
		print_statement(ptr_prs, sta);
		return;
	}

	if (sta.category == DO_WHILE_STATEMENT) {
		++tokvec;

		const struct Token *tokvec2 = tokvec;
		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RES_WHILE, "`while` of do-while");
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of do-while");

		int stashed_break_label = ptr_prs->break_label_name;
		int stashed_continue_label = ptr_prs->continue_label_name;

		int label1 = get_new_label_name(ptr_prs);
		int break_label = get_new_label_name(ptr_prs);
		int cont_label = get_new_label_name(ptr_prs);

		ptr_prs->break_label_name = break_label;
		ptr_prs->continue_label_name = cont_label;

		gen_label(label1);
		parseprint_statement(ptr_ps, ptr_prs, &tokvec2, inner_s);
		assert(tokvec2 == tokvec - 2);
		gen_label(cont_label);

		struct Expression expr = parse_expression(ptr_ps, &tokvec);

		print_expression(ptr_prs, expr);

		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "right parenthesis of do-while");
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after do-while");
		*ptr_tokvec = tokvec;

		gen_do_while_final(label1, break_label);

		ptr_prs->break_label_name = stashed_break_label;
		ptr_prs->continue_label_name = stashed_continue_label;
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = DO_WHILE_STATEMENT;
		s.expr1 = expr;

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return;
	}

	if (sta.category == WHILE_STATEMENT) {
		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of while");
		struct Expression expr = parse_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "left parenthesis of while");
		const struct Token *tokvec2 = tokvec;
		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		int stashed_break_label = ptr_prs->break_label_name;
		int stashed_continue_label = ptr_prs->continue_label_name;

		int label1 = get_new_label_name(ptr_prs);
		int break_label = get_new_label_name(ptr_prs);
		int cont_label = get_new_label_name(ptr_prs);
		ptr_prs->break_label_name = break_label;
		ptr_prs->continue_label_name = cont_label;

		gen_label(label1);

		print_expression(ptr_prs, expr);

		gen_while_part2(label1, break_label);

		parseprint_statement(ptr_ps, ptr_prs, &tokvec2, inner_s);
		assert(tokvec2 == tokvec);

		gen_label(cont_label);
		gen_for_part4(label1, break_label);

		*ptr_tokvec = tokvec;

		ptr_prs->break_label_name = stashed_break_label;
		ptr_prs->continue_label_name = stashed_continue_label;

		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = WHILE_STATEMENT;
		s.expr1 = expr;

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return;
	}

	if (sta.category == BREAK_STATEMENT) {
		print_statement(ptr_prs, sta);
		return;
	}

	if (sta.category == CONTINUE_STATEMENT) {
		print_statement(ptr_prs, sta);
		return;
	}

	if (sta.category == FOR_STATEMENT) {
		int stashed_break_label = ptr_prs->break_label_name;
		int stashed_continue_label = ptr_prs->continue_label_name;
		int label1 = get_new_label_name(ptr_prs);
		int break_label = get_new_label_name(ptr_prs);
		int cont_label = get_new_label_name(ptr_prs);
		ptr_prs->break_label_name = break_label;
		ptr_prs->continue_label_name = cont_label;

		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of `for`");

		struct Expression expr1;
		struct Expression expr2;
		struct Expression expr3;

		if (tokvec[0].kind == SEMICOLON) { /* expression1 is missing */
			expr1 = integer_1();
		} else {
			expr1 = parse_expression(ptr_ps, &tokvec);
		}
		expect_and_consume(&tokvec, SEMICOLON, "first semicolon of `for`");

		if (tokvec[0].kind == SEMICOLON) { /* expression2 is missing */
			expr2 = integer_1();
		} else {
			expr2 = parse_expression(ptr_ps, &tokvec);
		}
		expect_and_consume(&tokvec, SEMICOLON, "second semicolon of `for`");

		if (tokvec[0].kind == RIGHT_PAREN) { /* expression3 is missing */
			expr3 = integer_1();
		} else {
			expr3 = parse_expression(ptr_ps, &tokvec);
		}
		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `for`");

		print_expression(ptr_prs, expr1); /* expression1 */
		gen_discard();
		gen_label(label1);
		print_expression(ptr_prs, expr2); /* expression2 */
		gen_while_part2(label1, break_label);
		const struct Token *tokvec2 = tokvec;
		struct Statement inner_s = parse_statement(ptr_ps, &tokvec2);
		parseprint_statement(ptr_ps, ptr_prs, &tokvec, inner_s);
		assert(tokvec2 == tokvec);
		gen_label(cont_label);
		print_expression(ptr_prs, expr3);
		gen_discard();
		gen_for_part4(label1, break_label);
		*ptr_tokvec = tokvec;

		ptr_prs->break_label_name = stashed_break_label;
		ptr_prs->continue_label_name = stashed_continue_label;

		*ptr_tokvec = tokvec;

		struct Statement s;
		s.category = FOR_STATEMENT;
		s.expr1 = expr1;
		s.expr2 = expr2;
		s.expr3 = expr3;
		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return;
	}

	print_statement(ptr_prs, sta);
	return;
}

void parse_final(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	expect_and_consume(&tokvec, END, "the end of file");
	return;
}

static void parseprint_compound_statement(struct ParserState *ptr_ps,
                                          struct PrinterState *ptr_prs,
                                          const struct Token **ptr_tokvec,
                                          struct Statement sta)
{
	struct Vector vec = sta.statement_vector;

	const struct Token *tokvec = *ptr_tokvec;
	{
		struct LocalVarTableList current_table = ptr_ps->scope_chain;

		struct LocalVarTableList new_table;
		new_table.var_table = init_map();

		/* current_table disappears at the end of this function,
		   but there is no problem because new_table itself gets overwritten at
		   the end of this function.
		 */
		new_table.outer = &current_table;

		ptr_ps->scope_chain = new_table;

		++tokvec;
		for (int counter = 0; counter != vec.length; ++counter) {
			const struct Statement *ptr_ith = vec.vector[counter];
			if (ptr_ith->category == DECLARATION_STATEMENT) {
				const char *dummy;
				parse_declarator(&tokvec, &dummy);

				struct Type vartype = ptr_ith->declaration.type;
				const char *str = ptr_ith->declaration.ident_str;
				expect_and_consume(
				    &tokvec, SEMICOLON,
				    "semicolon at the end of variable definition");

				update_ptr_ps(ptr_ps, vartype, str);

			} else {
				const struct Token *tokvec2 = tokvec;
				struct Statement s = parse_statement(ptr_ps, &tokvec2);
				parseprint_statement(ptr_ps, ptr_prs, &tokvec, s);
				assert(tokvec2 == tokvec);
			}
		}
		++tokvec;
		*ptr_tokvec = tokvec;
		ptr_ps->scope_chain = current_table;

		return;
	}
}

static void print_parameter_declaration(struct ParserState *ptr_ps, int counter,
                                        struct ParamInfos param_infos)
{

	struct ParamInfo param_info = *(param_infos.param_vec[counter]);
	const char *ident_str;

	struct Type type = param_info.param_type;
	ident_str = param_info.ident_str;

	if (counter > 5) {
		unsupported("6-or-more parameters");
	}

	ptr_ps->newest_offset -= size_of(type) < 4 ? 4 : size_of(type);

	struct Map map_ = ptr_ps->scope_chain.var_table;
	struct LocalVarInfo *ptr_varinfo = calloc(1, sizeof(struct LocalVarInfo));
	ptr_varinfo->offset = ptr_ps->newest_offset;
	ptr_varinfo->type = type;

	insert(&map_, ident_str, ptr_varinfo);

	ptr_ps->scope_chain.var_table = map_;

	int offset = resolve_name_locally(ptr_ps->scope_chain, ident_str).offset;

	switch (size_of(type)) {
		case 1:
			gen_write_register_to_local_1byte(
			    /* yes, the register is 4byte */
			    get_reg_name_from_arg_pos_4byte(counter), offset);
		case 4:
			gen_write_register_to_local_4byte(
			    get_reg_name_from_arg_pos_4byte(counter), offset);
			break;
		case 8:
			gen_write_register_to_local_8byte(
			    get_reg_name_from_arg_pos_8byte(counter), offset);
			break;
		default:
			unsupported("Unsupported width in function parameter");
	}
}

void parseprint_toplevel_definition(struct ParserState *ptr_ps,
                                    struct PrinterState *ptr_prs,
                                    const struct Token **ptr_tokvec)
{

	const char *declarator_name;
	const struct Token *tokvec2 = *ptr_tokvec;
	struct Type declarator_type = parse_declarator(&tokvec2, &declarator_name);
	if (declarator_type.type_category != FN && tokvec2[0].kind == SEMICOLON) {
		++tokvec2; /* consume the semicolon */
		struct Map globalmap = ptr_ps->global_vars_type_map;

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
	ptr_ps->scope_chain.var_table = init_map();
	ptr_prs->return_label_name = GARBAGE_INT;   /* INITIALIZE */
	ptr_prs->break_label_name = GARBAGE_INT;    /* INITIALIZE */
	ptr_prs->continue_label_name = GARBAGE_INT; /* INITIALIZE */
	/* 8 is the space to store the address to handle deref */
	ptr_ps->newest_offset = -8;
	ptr_ps->func_ret_type = ret_type;

	struct Map retmap = ptr_ps->func_info_map;

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

		label1 = get_new_label_name(ptr_prs);
		label2 = get_new_label_name(ptr_prs);
		gen_prologue(0, declarator_name);
		gen_after_prologue(label1, label2);

	} else {
		label1 = get_new_label_name(ptr_prs);
		label2 = get_new_label_name(ptr_prs);
		gen_prologue(0, declarator_name);
		gen_after_prologue(label1, label2);

		int counter = 0;

		do {
			print_parameter_declaration(ptr_ps, counter, param_infos);
			++counter;
		} while (param_infos.param_vec[counter]);
	}

	const struct Token *tokvec = tokvec2;
	struct Statement sta = parse_compound_statement(ptr_ps, &tokvec2);
	parseprint_compound_statement(ptr_ps, ptr_prs, &tokvec, sta);

	gen_before_epilogue(label1, label2, -(ptr_ps->newest_offset));
	gen_epilogue_nbyte(size_of(ret_type), ptr_prs->return_label_name);

	*ptr_tokvec = tokvec2;
}

void print_string_pool(struct Vector pool)
{
	for (int i = 0; i < pool.length; ++i) {
		const char *str = pool.vector[i];
		gen_str(i, str);
	}
}
