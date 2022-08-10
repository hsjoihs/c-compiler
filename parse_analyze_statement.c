#include "analyzer.h"
#include "std.h"
#include "std_io.h"

static struct Expr integer_1(void)
{
	struct Expr expr;
	expr.details.type = INT_TYPE();
	expr.int_value = 1;
	expr.category = INT_VALUE;

	return expr;
}

/*
 * Adjusts the newest_offset and add a local variable to the scope.
 * Returns the offset of the newly added variable.
 */
int add_local_var_to_scope(struct AnalyzerState *ptr_ps,
                           const struct Type *ref_vartype, const char *str)
{
	ptr_ps->newest_offset -= size_of(ptr_ps, ref_vartype);

	struct LocalVarInfo *ptr_varinfo = calloc(1, sizeof(struct LocalVarInfo));
	ptr_varinfo->offset = ptr_ps->newest_offset;
	ptr_varinfo->type = *ref_vartype;
	insert(ptr_ps->scope_chain.var_table, str, ptr_varinfo);

	return ptr_varinfo->offset;
}

struct Statement parse_labeled_statement(struct AnalyzerState *ptr_ps,
                                         const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	enum TokenKind kind = tokvec[0].kind;

	struct SourceLabel l;
	if (kind == RES_DEFAULT) {
		++tokvec;
		l.category = DEFAULT_LABEL;
	} else if (kind == RES_CASE) {
		++tokvec;
		const struct UntypedExpr u = parse_constant_expression(&tokvec);
		int case_int =
		    typecheck_constant_integral_expression(ptr_ps, &u, "case");
		l.category = CASE_LABEL;
		l.case_int = case_int;
	} else {
		l.ident_str = tokvec[0].ident_str;
		++tokvec;
		l.category = IDENT_LABEL;
	}
	expect_and_consume(&tokvec, COLON,
	                   "colon of `default:`, `case VALUE:`, or `ident:`");

	struct Statement s = parse_statement(ptr_ps, &tokvec);

	struct SourceLabel *ptr_l = calloc(1, sizeof(struct SourceLabel));
	*ptr_l = l;
	push_vector(&s.labels, ptr_l);
	*ptr_tokvec = tokvec;
	return s;
}

static struct Expr *declare_var_and_return_initializer(
    struct AnalyzerState *ptr_ps, const struct Type *ref_vartype,
    const char *str, const struct UntypedExpr *ptr_uexpr,
    struct Statement *ptr_statement);

struct Statement parse_statement(struct AnalyzerState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == RES_CASE || tokvec[0].kind == RES_DEFAULT ||
	    (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == COLON)) {
		return parse_labeled_statement(ptr_ps, ptr_tokvec);
	}

	switch (tokvec[0].kind) {
	case LEFT_BRACE: {
		return parse_compound_statement(ptr_ps, ptr_tokvec);
	}

	case RES_IF: {
		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN,
		                   "left parenthesis immediately after `if`");

		const struct UntypedExpr u = parse_expression(&tokvec);
		struct Expr expr = typecheck_expression(ptr_ps, &u);
		expect_scalar(&expr.details.type, "`if` statement");

		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `if`");

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = parse_statement(ptr_ps, &tokvec);

		struct Statement s;
		s.labels = init_vector();
		s.expr1 = expr;
		if (tokvec[0].kind == RES_ELSE) { /* must bind to the most inner one */
			++tokvec;
			struct Statement *ptr_inner_s2 =
			    calloc(1, sizeof(struct Statement));
			*ptr_inner_s2 = parse_statement(ptr_ps, &tokvec);

			*ptr_tokvec = tokvec;

			s.category = IF_ELSE_STATEMENT;
			s.statement_vector = init_vector();
			push_vector(&s.statement_vector, ptr_inner_s);
			push_vector(&s.statement_vector, ptr_inner_s2);
			return s;
		}

		*ptr_tokvec = tokvec;
		s.category = IF_STATEMENT;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	case RES_SWITCH: {
		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN,
		                   "left parenthesis immediately after `switch`");

		const struct UntypedExpr u = parse_expression(&tokvec);

		struct Expr expr = typecheck_expression(ptr_ps, &u);
		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "right parenthesis of `switch`");

		expect_integral(
		    &expr.details.type,
		    "controlling expression of `switch` must be integer type");

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = parse_statement(ptr_ps, &tokvec);

		struct Statement s;
		s.labels = init_vector();
		s.expr1 = expr;
		*ptr_tokvec = tokvec;
		s.category = SWITCH_STATEMENT;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	case RES_RETURN: {
		++tokvec;
		*ptr_tokvec = tokvec;
		struct Statement s;
		s.labels = init_vector();
		s.category = RETURN_STATEMENT;

		if (tokvec[0].kind == SEMICOLON) {
			++tokvec;
			s.expr1.category = VOID_EXPR;
			s.expr1.details.type.type_category = VOID_;
			s.expr1.details.true_type.type_category = VOID_;
		} else {
			const struct UntypedExpr u = parse_expression(&tokvec);
			struct Expr expr = typecheck_expression(ptr_ps, &u);

			if (ptr_ps->func_ret_type.type_category == PTR_) {
				struct TypePair details;
				details.type = ptr_ps->func_ret_type;
				details.true_type = ptr_ps->func_ret_type;
				cast_to_null_pointer_if_possible(&expr, &details);
				if_function_cast_to_pointer(&expr);
			}

			expect_type(ptr_ps, &expr.details.type, &ptr_ps->func_ret_type,
			            "mismatched type in the return value");
			expect_and_consume(&tokvec, SEMICOLON,
			                   "semicolon (after `return expr`)");
			s.expr1 = expr;
		}

		*ptr_tokvec = tokvec;
		return s;
	}

	case RES_DO: {
		++tokvec;

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RES_WHILE, "`while` of do-while");
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of do-while");

		const struct UntypedExpr u = parse_expression(&tokvec);

		struct Expr expr = typecheck_expression(ptr_ps, &u);
		expect_scalar(&expr.details.type, "`do-while` statement");

		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "right parenthesis of do-while");
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after do-while");
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.labels = init_vector();
		s.category = DO_WHILE_STATEMENT;
		s.expr1 = expr;

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	case RES_WHILE: {
		++tokvec;

		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of while");

		const struct UntypedExpr u = parse_expression(&tokvec);

		struct Expr expr = typecheck_expression(ptr_ps, &u);
		expect_scalar(&expr.details.type, "`while` statement");

		expect_and_consume(&tokvec, RIGHT_PAREN, "left parenthesis of while");

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);

		*ptr_tokvec = tokvec;

		struct Statement s;
		s.labels = init_vector();
		s.category = WHILE_STATEMENT;
		s.expr1 = expr;

		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;
		return s;
	}

	case RES_BREAK: {
		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `break`");
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.labels = init_vector();
		s.category = BREAK_STATEMENT;
		return s;
	}

	case RES_CONTINUE: {
		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `continue`");
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.labels = init_vector();
		s.category = CONTINUE_STATEMENT;
		return s;
	}

	case RES_GOTO: {
		++tokvec;
		expect_and_consume(&tokvec, IDENT_OR_RESERVED,
		                   "identifier after `goto`");
		const char *ident = tokvec[-1].ident_str;
		expect_and_consume(&tokvec, SEMICOLON,
		                   "semicolon after `goto identifier`");
		*ptr_tokvec = tokvec;

		struct Statement s;
		s.labels = init_vector();
		s.category = GOTO_STATEMENT;
		s.destination = ident;
		return s;
	}

	case RES_FOR: {

		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of `for`");

		/* make the whole thing into a block*/
		struct Statement statement;
		statement.category = COMPOUND_STATEMENT;
		statement.labels = init_vector();
		statement.statement_vector = init_vector();

		/* block scope begins */
		struct ScopeChain current_table = ptr_ps->scope_chain;
		struct ScopeChain new_table;
		new_table.var_table = init_map();
		new_table.outer = &current_table;
		ptr_ps->scope_chain = new_table;

		struct Expr expr1;
		struct Expr expr2;
		struct Expr expr3;

		if (tokvec[0].kind == SEMICOLON) { /* expression1 is missing */
			expr1 = integer_1();
			++tokvec;
		} else if (can_start_a_type(tokvec)) {
			const char *str;
			struct UntypedExpr *ptr_uexpr;
			const struct Type vartype =
			    parse_declaration(&tokvec, &str, &ptr_uexpr);

			struct Expr *ptr_expr = declare_var_and_return_initializer(
			    ptr_ps, &vartype, str, ptr_uexpr, &statement);

			expr1 = ptr_expr ? *ptr_expr : integer_1();
		} else {
			const struct UntypedExpr u = parse_expression(&tokvec);
			expr1 = typecheck_expression(ptr_ps, &u);
			expect_and_consume(&tokvec, SEMICOLON, "first semicolon of `for`");
		}

		if (tokvec[0].kind == SEMICOLON) { /* expression2 is missing */
			expr2 = integer_1();
		} else {
			const struct UntypedExpr u = parse_expression(&tokvec);
			expr2 = typecheck_expression(ptr_ps, &u);
		}
		expect_scalar(&expr2.details.type, "`for` statement");
		expect_and_consume(&tokvec, SEMICOLON, "second semicolon of `for`");

		if (tokvec[0].kind == RIGHT_PAREN) { /* expression3 is missing */
			expr3 = integer_1();
		} else {
			const struct UntypedExpr u = parse_expression(&tokvec);
			expr3 = typecheck_expression(ptr_ps, &u);
		}
		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `for`");
		struct Statement s;
		s.labels = init_vector();
		s.category = FOR_STATEMENT;
		s.expr1 = expr1;
		s.expr2 = expr2;
		s.expr3 = expr3;

		struct Statement inner_s = parse_statement(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		struct Statement *ptr_inner_s = calloc(1, sizeof(struct Statement));
		*ptr_inner_s = inner_s;
		s.inner_statement = ptr_inner_s;

		struct Statement *ptr_s = calloc(1, sizeof(struct Statement));
		*ptr_s = s;
		push_vector(&statement.statement_vector, ptr_s);

		/* block scope ends */
		ptr_ps->scope_chain = current_table;
		return statement;
	}

	default: {
		const struct UntypedExpr uexpr = parse_expression(&tokvec);
		struct Expr expr = typecheck_expression(ptr_ps, &uexpr);

		expect_and_consume(&tokvec, SEMICOLON, "semicolon after an expression");

		*ptr_tokvec = tokvec;

		struct Statement s;
		s.labels = init_vector();
		s.category = EXPRESSION_STATEMENT;
		s.expr1 = expr;
		return s;
	}
	}
}

static struct Expr *declare_var_and_return_initializer(
    struct AnalyzerState *ptr_ps, const struct Type *ref_vartype,
    const char *str, const struct UntypedExpr *ptr_uexpr,
    struct Statement *ptr_statement)
{
	const struct Type vartype = *ref_vartype;
	add_local_var_to_scope(ptr_ps, ref_vartype, str);

	struct Statement s;
	s.category = DECLARATION_STATEMENT;
	s.declaration.type = vartype;
	s.declaration.ident_str = str;
	s.labels = init_vector();
	struct Statement *ptr_s = calloc(1, sizeof(struct Statement));
	*ptr_s = s;
	push_vector(&ptr_statement->statement_vector, ptr_s);

	struct Expr *ptr_expr = 0;
	if (ptr_uexpr) { /* initializer exists */
		struct UntypedExpr left_uexpr;
		left_uexpr.category = VAR;
		left_uexpr.var_name = str;

		struct UntypedExpr uexpr =
		    binary_op_untyped(&left_uexpr, ptr_uexpr, OP_EQ);
		struct Expr expr = typecheck_expression(ptr_ps, &uexpr);
		ptr_expr = calloc(1, sizeof(struct Expr));
		*ptr_expr = expr;
	}
	return ptr_expr;
}

struct Statement parse_compound_statement(struct AnalyzerState *ptr_ps,
                                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Statement statement;
	statement.category = COMPOUND_STATEMENT;
	statement.labels = init_vector();
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
				struct Type *optional_ptr_type =
				    try_parse_type_specifier_and_semicolon(&tokvec);
				if (optional_ptr_type) {
					struct Statement s;
					s.category = DECLARATION_STATEMENT;
					s.declaration.type = *optional_ptr_type;
					s.declaration.ident_str = 0;
					s.labels = init_vector(); /* must initialize this, since it
					                             is used in goto traversal */
					struct Statement *ptr_s =
					    calloc(1, sizeof(struct Statement));
					*ptr_s = s;
					push_vector(&statement.statement_vector, ptr_s);
					continue;
				}

				const char *str;
				struct UntypedExpr *ptr_uexpr;
				const struct Type vartype =
				    parse_declaration(&tokvec, &str, &ptr_uexpr);

				/* while function prototypes are also allowed here in C, I will
				 * not implement it here */
				if (vartype.type_category == FN) {
					unsupported(
					    "declaring a function inside a compound statement\n");
					exit(EXIT_FAILURE);
				}

				struct Expr *ptr_expr = declare_var_and_return_initializer(
				    ptr_ps, &vartype, str, ptr_uexpr, &statement);

				if (ptr_expr) {
					struct Statement assignment;
					assignment.category = EXPRESSION_STATEMENT;
					assignment.expr1 = *ptr_expr;
					assignment.labels = init_vector();
					struct Statement *ptr_s =
					    calloc(1, sizeof(struct Statement));
					*ptr_s = assignment;
					push_vector(&statement.statement_vector, ptr_s);
				}

			} else {
				struct Statement s = parse_statement(ptr_ps, &tokvec);
				struct Statement *ptr_s = calloc(1, sizeof(struct Statement));
				*ptr_s = s;
				push_vector(&statement.statement_vector, ptr_s);
			}
		}
	}
	assert0("should not reach here" && 0);
}
