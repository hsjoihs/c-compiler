#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static struct Expression integer_1(void)
{
	struct Expression expr;
	expr.details.type = INT_TYPE;
	expr.int_value = 1;
	expr.category = INT_VALUE;

	return expr;
}

/*
 * Adjusts the newest_offset and add a local variable to the scope.
 * Returns the offset of the newly added variable.
 */
int add_local_var_to_scope(struct ParserState *ptr_ps,
                           const struct Type vartype, const char *str)
{
	ptr_ps->newest_offset -= size_of(vartype) < 4 ? 4 : size_of(vartype);

	struct Map map_ = ptr_ps->scope_chain.var_table;

	struct LocalVarInfo *ptr_varinfo = calloc(1, sizeof(struct LocalVarInfo));
	ptr_varinfo->offset = ptr_ps->newest_offset;
	ptr_varinfo->type = vartype;
	insert(&map_, str, ptr_varinfo);

	ptr_ps->scope_chain.var_table = map_;
	return ptr_varinfo->offset;
}

struct Statement parse_statement(struct ParserState *ptr_ps,
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
		struct Expression expr =
		    typecheck_expression(ptr_ps, parse_expression(&tokvec));
		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `if`");

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;

		struct Statement s;
		s.expr1 = expr;
		if (tokvec[0].kind == RES_ELSE) { /* must bind to the most inner one */
			++tokvec;
			struct Statement inner_s2 = parse_statement(ptr_ps, &tokvec);
			struct Statement *ptr_inner_s2 =
			    calloc(1, sizeof(struct Statement));
			*ptr_inner_s2 = inner_s2;

			*ptr_tokvec = tokvec;

			s.category = IF_ELSE_STATEMENT;
			s.statement_vector = init_vector();
			push_vector(&s.statement_vector, ptr_inner_s);
			push_vector(&s.statement_vector, ptr_inner_s2);
			return s;
		} else {

			*ptr_tokvec = tokvec;
			s.category = IF_STATEMENT;
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
			struct Expression expr =
			    typecheck_expression(ptr_ps, parse_expression(&tokvec));
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

		struct Expression expr =
		    typecheck_expression(ptr_ps, parse_expression(&tokvec));

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

		struct Expression expr =
		    typecheck_expression(ptr_ps, parse_expression(&tokvec));

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
			expr1 = typecheck_expression(ptr_ps, parse_expression(&tokvec));
		}
		expect_and_consume(&tokvec, SEMICOLON, "first semicolon of `for`");

		if (tokvec[0].kind == SEMICOLON) { /* expression2 is missing */
			expr2 = integer_1();
		} else {
			expr2 = typecheck_expression(ptr_ps, parse_expression(&tokvec));
		}
		expect_and_consume(&tokvec, SEMICOLON, "second semicolon of `for`");

		if (tokvec[0].kind == RIGHT_PAREN) { /* expression3 is missing */
			expr3 = integer_1();
		} else {
			expr3 = typecheck_expression(ptr_ps, parse_expression(&tokvec));
		}
		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `for`");
		struct Statement s;
		s.category = FOR_STATEMENT;
		s.expr1 = expr1;
		s.expr2 = expr2;
		s.expr3 = expr3;

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	struct UntypedExpression uexpr = parse_expression(&tokvec);
	struct Expression expr = typecheck_expression(ptr_ps, uexpr);

	expect_and_consume(&tokvec, SEMICOLON, "semicolon after an expression");

	*ptr_tokvec = tokvec;

	struct Statement s;
	s.category = EXPRESSION_STATEMENT;
	s.expr1 = expr;
	return s;
}

struct Statement parse_compound_statement(struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Statement statement;
	statement.category = COMPOUND_STATEMENT;
	statement.statement_vector = init_vector();
	if (tokvec[0].kind == LEFT_BRACE) {

		struct ScopeChain current_table = ptr_ps->scope_chain;

		struct ScopeChain new_table;
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

				add_local_var_to_scope(ptr_ps, vartype, str);

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
