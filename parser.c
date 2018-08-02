#include "header.h"
#if 1
#include "compiler.h"
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Expression parse_postfix_expression(struct ParserState *ptr_ps,
                                           const struct Token **ptr_tokvec);
struct Expression parse_conditional_expression(struct ParserState *ptr_ps,
                                               const struct Token **ptr_tokvec);
struct Expression parse_argument_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec,
                                            int counter);
struct Expression parse_primary_expression(struct ParserState *ptr_ps,
                                           const struct Token **ptr_tokvec);
struct Expression parse_assignment_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec);
struct Expression parse_cast_expression(struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec);

struct Expression unary_op_(struct Expression expr, enum TokenKind kind,
                            struct ExprInfo exprinfo)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;

	struct Expression new_expr;
	new_expr.details = exprinfo;
	new_expr.category = UNARY_OP_EXPR;
	new_expr.unary_operator = kind;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = 0;
	new_expr.ptr3 = 0;

	return new_expr;
}

struct Expression parse_unary_expression(struct ParserState *ptr_ps,
                                         const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	/* unary-operator cast-expression */
	if (tokvec[0].kind == OP_NOT || tokvec[0].kind == OP_TILDA ||
	    tokvec[0].kind == OP_PLUS || tokvec[0].kind == OP_MINUS) {
		enum TokenKind kind = tokvec[0].kind;
		++tokvec;
		struct Expression expr =
		    remove_leftiness_(parse_cast_expression(ptr_ps, &tokvec));
		expect_type(expr.details, INT_TYPE, 2);

		struct Expression new_expr = unary_op_(expr, kind, expr.details);

		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_PLUS_PLUS ||
	           tokvec[0].kind == OP_MINUS_MINUS) {
		enum TokenKind opkind = tokvec[0].kind;
		++tokvec;
		const char *name;
		if (tokvec[0].kind == IDENT_OR_RESERVED) {
			name = tokvec[0].ident_str;
			++tokvec;
			*ptr_tokvec = tokvec;

		} else {
			unimplemented("++ followed by non-identifier");
		}

		struct Expression expr = ident_as_lvalue(*ptr_ps, name);
		struct Expression new_expr =
		    unary_op_(expr, opkind, UNASSIGNABLE(INT_TYPE));
		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_AND) {
		if (tokvec[1].kind == IDENT_OR_RESERVED) {
			const char *name = tokvec[1].ident_str;

			tokvec += 2;

			struct Expression expr = ident_as_lvalue(*ptr_ps, name);

			struct Type *ptr_type = calloc(1, sizeof(struct Type));
			*ptr_type = expr.details.type;

			struct Expression new_expr =
			    unary_op_(expr, OP_AND,
			              UNASSIGNABLE(ptr_of_type_to_ptr_to_type(ptr_type)));

			*ptr_tokvec = tokvec;
			return new_expr;
		} else {
			unimplemented("& followed by non-identifier");
		}

	} else if (tokvec[0].kind == OP_ASTERISK) {
		++tokvec;
		struct Expression expr =
		    remove_leftiness_(parse_cast_expression(ptr_ps, &tokvec));
		struct Type type = deref_type(expr.details.type);

		struct ExprInfo new_expr_info;
		new_expr_info.info = DEREFERENCED_ADDRESS;
		new_expr_info.type = type;
		new_expr_info.offset = GARBAGE_INT;

		*ptr_tokvec = tokvec;

		struct Expression new_expr =
		    unary_op_(expr, OP_ASTERISK, new_expr_info);

		return new_expr;
	} else {
		struct Expression expr = parse_postfix_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr;
	}
}

struct Expression parse_cast_expression(struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec)
{
	return parse_unary_expression(ptr_ps, ptr_tokvec);
}

struct Expression parse_postfix_expression(struct ParserState *ptr_ps,
                                           const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;

		struct Type ret_type;
		if (!isElem(ptr_ps->func_info_map, ident_str)) {
			fprintf(stderr, "Undeclared function `%s()` detected.\n",
			        ident_str);
			fprintf(stderr, "Assumes that `%s()` returns `int`\n", ident_str);
			ret_type = INT_TYPE;
		} else {
			struct FuncInfo *ptr_func_info =
			    lookup(ptr_ps->func_info_map, ident_str);
			ret_type = ptr_func_info->ret_type;
		}

		tokvec += 2;

		struct Expression *args = calloc(10, sizeof(struct Expression));
		int counter = 0;
		if (tokvec[0].kind == RIGHT_PAREN) {
			tokvec++;
		} else {

			args[counter] = parse_argument_expression(ptr_ps, &tokvec, counter);
			++counter;

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;
				args[counter] =
				    parse_argument_expression(ptr_ps, &tokvec, counter);
				++counter;
			}

			*ptr_tokvec = tokvec;

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing parenthesis of function call");

			*ptr_tokvec = tokvec;
		}
		*ptr_tokvec = tokvec;

		struct Expression expr;
		expr.category = FUNCCALL_EXPR;
		expr.arg_expr_vec = args;
		expr.arg_length = counter;
		expr.details = UNASSIGNABLE(ret_type);
		expr.global_var_name = ident_str;
		return expr;

	} else if (tokvec[0].kind == IDENT_OR_RESERVED &&
	           (tokvec[1].kind == OP_PLUS_PLUS ||
	            tokvec[1].kind == OP_MINUS_MINUS)) {
		const char *name = tokvec[0].ident_str;
		enum TokenKind opkind = tokvec[1].kind;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		struct Expression expr = ident_as_lvalue(*ptr_ps, name);
		struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
		*ptr_expr1 = expr;

		struct Expression new_expr;
		new_expr.details = UNASSIGNABLE(INT_TYPE);
		new_expr.category =
		    opkind == OP_PLUS_PLUS ? POSTFIX_INCREMENT : POSTFIX_DECREMENT;
		new_expr.ptr1 = ptr_expr1;
		new_expr.ptr2 = 0;
		new_expr.ptr3 = 0;

		*ptr_tokvec = tokvec;
		return new_expr;
	} else {
		struct Expression expr = parse_primary_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr;
	}
}

struct Expression parse_primary_expression(struct ParserState *ptr_ps,
                                           const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_tokvec;

		struct Expression expr;
		expr.details = UNASSIGNABLE(INT_TYPE);
		expr.int_value = tokvec[0].int_value;
		expr.category = INT_VALUE;

		return expr;

	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_tokvec;

		if (!is_local_var(ptr_ps->scope_chain, tokvec[0].ident_str)) {
			struct Type type = resolve_name_globally(
			    ptr_ps->global_vars_type_map, tokvec[0].ident_str);

			struct ExprInfo expr_info;
			expr_info.info = GLOBAL_VAR;
			expr_info.type = type;

			struct Expression expr;
			expr.details = expr_info;
			expr.category = GLOBAL_VAR_AS_RVALUE;
			expr.global_var_name = tokvec[0].ident_str;
			return expr;
		} else {
			struct LocalVarInfo info =
			    resolve_name_locally(ptr_ps->scope_chain, tokvec[0].ident_str);

			struct ExprInfo expr_info;
			expr_info.info = LOCAL_VAR;
			expr_info.type = info.type;
			expr_info.offset = info.offset;

			struct Expression expr;
			expr.details = expr_info;
			expr.category = LOCAL_VAR_AS_RVALUE;
			return expr;
		}
	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		struct Expression expr = parse_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right paren");

		*ptr_tokvec = tokvec;
		return expr;
	}

	error_unexpected_token(tokvec,
	                       "the beginning of parseprint_primary_expression");
}

struct Expression ident_as_lvalue(struct ParserState ps, const char *name)
{
	if (!is_local_var(ps.scope_chain, name)) {
		struct Type type = resolve_name_globally(ps.global_vars_type_map, name);

		struct Expression expr;
		expr.details.info = GLOBAL_VAR;
		expr.details.type = type;
		expr.details.offset = GARBAGE_INT;
		expr.category = GLOBAL_VAR_AS_LVALUE;
		expr.global_var_name = name;
		return expr;
	} else {
		struct LocalVarInfo info = resolve_name_locally(ps.scope_chain, name);

		struct Expression expr;
		expr.details.info = LOCAL_VAR;
		expr.details.type = info.type;
		expr.details.offset = info.offset;
		expr.category = LOCAL_VAR_AS_LVALUE;
		return expr;
	}
}

struct Expression parse_assignment_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	enum TokenKind opkind = tokvec[1].kind;

	if (tokvec[0].kind == IDENT_OR_RESERVED && isAssign(opkind)) {
		const char *name = tokvec[0].ident_str;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		struct Expression expr = ident_as_lvalue(*ptr_ps, name);
		struct Expression expr2 = parse_assignment_expression(ptr_ps, &tokvec);
		struct ExprInfo expr_info = expr2.details;
		expect_type(expr_info, expr.details.type, 0);

		*ptr_tokvec = tokvec;
		return binary_op_(expr, expr2, opkind, BINARY_EXPR,
		                  UNASSIGNABLE(expr.details.type));
	}

	const struct Token *tokvec2 = tokvec;
	struct ParserState *ptr_ps2 = ptr_ps;
	parse_unary_expression(ptr_ps2, &tokvec2);

	/* parse failed */
	if (!isAssign(tokvec2[0].kind)) {
		struct Expression expr = parse_conditional_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr;
	}

	struct Expression expr = parse_unary_expression(ptr_ps, &tokvec);
	struct ExprInfo expr_info = expr.details;

	assert(isAssign(tokvec[0].kind));
	switch (expr.details.info) {
		case LOCAL_VAR:
		case GLOBAL_VAR:
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

			struct Expression expr2 =
			    parse_assignment_expression(ptr_ps, &tokvec);
			struct ExprInfo expr_info2 = expr2.details;
			expect_type(expr_info, expr_info2.type, 19);

			*ptr_tokvec = tokvec;
			return binary_op_(expr, expr2, opkind, BINARY_EXPR,
			                  remove_leftiness(expr_info));
		};
	}
}

struct Expression parse_conditional_expression(struct ParserState *ptr_ps,
                                               const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_logical_OR_expression(ptr_ps, &tokvec);
	if (tokvec[0].kind == QUESTION) {

		++tokvec;
		*ptr_tokvec = tokvec;
		struct Expression true_branch = parse_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, COLON, "colon of the conditional operator");

		*ptr_tokvec = tokvec;
		struct Expression false_branch =
		    parse_conditional_expression(ptr_ps, &tokvec);

		*ptr_tokvec = tokvec;

		expect_type(false_branch.details, true_branch.details.type, 1);

		struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
		struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
		struct Expression *ptr_expr3 = calloc(1, sizeof(struct Expression));
		*ptr_expr1 = expr;
		*ptr_expr2 = true_branch;
		*ptr_expr3 = false_branch;

		struct Expression new_expr;
		new_expr.details = remove_leftiness(true_branch.details);
		new_expr.category = CONDITIONAL_EXPR;
		new_expr.ptr1 = ptr_expr1;
		new_expr.ptr2 = ptr_expr2;
		new_expr.ptr3 = ptr_expr3;

		return new_expr;
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct Expression parse_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_assignment_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_COMMA) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    remove_leftiness_(parse_assignment_expression(ptr_ps, &tokvec));

		expr = binary_op_(expr, expr2, kind, BINARY_EXPR, expr2.details);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct Expression parse_argument_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec,
                                            int counter)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Expression expr = parse_assignment_expression(ptr_ps, &tokvec);
	if (counter > 5) {
		unimplemented("calling with 7 or more arguments");
	}

	*ptr_tokvec = tokvec;

	return expr;
}
