#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_local_var(struct ScopeChain t, const char *str)
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

static struct LocalVarInfo resolve_name_locally(struct ScopeChain t,
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

static int isAssign(enum TokenKind opkind)
{
	return (opkind == OP_EQ || opkind == OP_PLUS_EQ || opkind == OP_MINUS_EQ ||
	        opkind == OP_ASTERISK_EQ || opkind == OP_SLASH_EQ ||
	        opkind == OP_PERCENT_EQ || opkind == OP_LSHIFT_EQ ||
	        opkind == OP_RSHIFT_EQ || opkind == OP_AND_EQ ||
	        opkind == OP_HAT_EQ || opkind == OP_OR_EQ);
}

static struct Type resolve_name_globally(struct Map m, const char *str)
{
	if (isElem(m, str)) {
		struct Type *ptr_type = lookup(m, str);
		return *ptr_type;
	} else {
		fprintf(stderr, "%s is not declared globally\n", str);
		exit(EXIT_FAILURE);
	}
}

enum UnaryOp to_unaryop(enum TokenKind t)
{
	switch (t) {
		case OP_NOT:
			return UNARY_OP_NOT;
		case OP_TILDA:
			return UNARY_OP_TILDA;
		case OP_PLUS:
			return UNARY_OP_PLUS;
		case OP_MINUS:
			return UNARY_OP_MINUS;

		case OP_PLUS_PLUS:
			return UNARY_OP_PLUS_PLUS;
		case OP_MINUS_MINUS:
			return UNARY_OP_MINUS_MINUS;

		case OP_AND:
			return UNARY_OP_AND;

		case OP_ASTERISK:
			return UNARY_OP_ASTERISK;

		default:
			assert("cannot happen" && 0);
	}
}

static struct Expression
parse_postfix_expression(struct ParserState *ptr_ps,
                         const struct Token **ptr_tokvec);
static struct Expression
parse_conditional_expression(struct ParserState *ptr_ps,
                             const struct Token **ptr_tokvec);
static struct Expression
parse_primary_expression(struct ParserState *ptr_ps,
                         const struct Token **ptr_tokvec);
static struct Expression
parse_assignment_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);

static struct Expression deref_expr(struct Expression expr);

static struct Expression unary_op_(struct Expression expr, enum TokenKind kind,
                                   struct Type type)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;

	struct Expression new_expr;
	new_expr.details.type = type;
	new_expr.category = UNARY_OP_EXPR;
	new_expr.unary_operator = to_unaryop(kind);
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
		struct Expression expr = parse_cast_expression(ptr_ps, &tokvec);
		expect_type(
		    expr.details.type, INT_TYPE,
		    "operand of logical not, bitnot, unary plus or unary minus");

		struct Expression new_expr = unary_op_(expr, kind, expr.details.type);
		new_expr.details.true_type = expr.details.true_type;

		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_PLUS_PLUS ||
	           tokvec[0].kind == OP_MINUS_MINUS) {
		enum TokenKind opkind = tokvec[0].kind;
		++tokvec;

		struct Expression expr = parse_unary_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "operand of unary increment/decrement");

		struct Expression new_expr = unary_op_(expr, opkind, INT_TYPE);
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
			    unary_op_(expr, OP_AND, ptr_of_type_to_ptr_to_type(ptr_type));

			*ptr_tokvec = tokvec;
			return new_expr;
		} else {
			unsupported("& followed by non-identifier");
		}

	} else if (tokvec[0].kind == OP_ASTERISK) {
		++tokvec;
		struct Expression expr = parse_cast_expression(ptr_ps, &tokvec);

		*ptr_tokvec = tokvec;
		return deref_expr(expr);
	} else {
		struct Expression expr = parse_postfix_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr;
	}
}

static struct Expression deref_expr(struct Expression expr)
{
	struct Type type = deref_type(expr.details.type);

	struct Expression new_expr =
	    unary_op_(expr, OP_ASTERISK, if_array_convert_to_ptr(type));
	new_expr.details.true_type = type;

	return new_expr;
}

struct Expression parse_cast_expression(struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec)
{
	return parse_unary_expression(ptr_ps, ptr_tokvec);
}

static struct Expression
parse_postfix_expression(struct ParserState *ptr_ps,
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
			args[counter] = parse_assignment_expression(ptr_ps, &tokvec);
			++counter;

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;
				if (counter > 5) {
					unsupported("calling with 7 or more arguments");
				}

				args[counter] = parse_assignment_expression(ptr_ps, &tokvec);
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
		expr.details.type = ret_type;
		expr.global_var_name = ident_str;
		return expr;
	}

	if (tokvec[0].kind == IDENT_OR_RESERVED &&
	    (tokvec[1].kind == OP_PLUS_PLUS || tokvec[1].kind == OP_MINUS_MINUS)) {
		const char *name = tokvec[0].ident_str;
		enum TokenKind opkind = tokvec[1].kind;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		struct Expression expr = ident_as_lvalue(*ptr_ps, name);
		struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
		*ptr_expr1 = expr;

		struct Expression new_expr;
		new_expr.details.type = INT_TYPE;
		new_expr.category =
		    opkind == OP_PLUS_PLUS ? POSTFIX_INCREMENT : POSTFIX_DECREMENT;
		new_expr.ptr1 = ptr_expr1;
		new_expr.ptr2 = 0;
		new_expr.ptr3 = 0;

		*ptr_tokvec = tokvec;
		return new_expr;
	} else {
		struct Expression expr = parse_primary_expression(ptr_ps, &tokvec);

		while (1) {
			if (tokvec[0].kind == LEFT_BRACKET) {
				++tokvec;
				struct Expression expr2 = parse_expression(ptr_ps, &tokvec);
				expect_and_consume(&tokvec, RIGHT_BRACKET, "right bracket ]");

				expr = deref_expr(combine_by_add(expr, expr2));
			} else {
				break;
			}
		}
		*ptr_tokvec = tokvec;
		return expr;
	}
}

static struct Expression
parse_primary_expression(struct ParserState *ptr_ps,
                         const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_tokvec;

		struct Expression expr;
		expr.details.type = INT_TYPE;
		expr.int_value = tokvec[0].int_value;
		expr.category = INT_VALUE;

		return expr;

	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_tokvec;

		if (!is_local_var(ptr_ps->scope_chain, tokvec[0].ident_str)) {
			struct Type type = resolve_name_globally(
			    ptr_ps->global_vars_type_map, tokvec[0].ident_str);

			struct Expression expr;
			expr.details.type = if_array_convert_to_ptr(type);
			;
			expr.details.true_type = type;
			expr.category = GLOBAL_VAR_;
			expr.global_var_name = tokvec[0].ident_str;
			return expr;
		} else {
			struct LocalVarInfo info =
			    resolve_name_locally(ptr_ps->scope_chain, tokvec[0].ident_str);

			struct Expression expr;
			expr.details.type = if_array_convert_to_ptr(info.type);
			expr.details.true_type = info.type;
			expr.category = LOCAL_VAR_;
			expr.local_var_offset = info.offset;
			return expr;
		}
	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		struct Expression expr = parse_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right paren");

		*ptr_tokvec = tokvec;
		return expr;
	} else if (tokvec[0].kind == LIT_STRING) {
		struct Type *ptr_char = calloc(1, sizeof(struct Type));
		*ptr_char = CHAR_TYPE;

		struct Expression expr;
		expr.details.type = ptr_of_type_to_ptr_to_type(ptr_char);
		expr.details.true_type = ptr_of_type_to_arr_of_type(
		    ptr_char, strlen(tokvec[0].literal_str) + 1);
		expr.category = STRING_LITERAL;
		expr.literal_string = tokvec[0].literal_str;

		++*ptr_tokvec;
		return expr;
	}

	error_unexpected_token(tokvec, "the beginning of parse_primary_expression");
}

struct Expression ident_as_lvalue(struct ParserState ps, const char *name)
{
	if (!is_local_var(ps.scope_chain, name)) {
		struct Type type = resolve_name_globally(ps.global_vars_type_map, name);
		if (is_array(type)) {
			fprintf(stderr, "array is not an lvalue\n");
			exit(EXIT_FAILURE);
		}

		struct Expression expr;
		expr.details.type = type;
		expr.category = GLOBAL_VAR_;
		expr.global_var_name = name;
		return expr;
	} else {
		struct LocalVarInfo info = resolve_name_locally(ps.scope_chain, name);
		if (is_array(info.type)) {
			fprintf(stderr, "array is not an lvalue\n");
			exit(EXIT_FAILURE);
		}

		struct Expression expr;
		expr.details.type = info.type;
		expr.local_var_offset = info.offset;
		expr.category = LOCAL_VAR_;
		return expr;
	}
}

enum SimpleBinOp op_before_assign(enum TokenKind kind)
{
	switch (kind) {
		case OP_EQ:
			return SIMPLE_BIN_OP_COMMA;
		case OP_PLUS_EQ:
			return SIMPLE_BIN_OP_PLUS;
		case OP_MINUS_EQ:
			return SIMPLE_BIN_OP_MINUS;
		case OP_ASTERISK_EQ:
			return SIMPLE_BIN_OP_ASTERISK;
		case OP_SLASH_EQ:
			return SIMPLE_BIN_OP_SLASH;
		case OP_PERCENT_EQ:
			return SIMPLE_BIN_OP_PERCENT;
		case OP_LSHIFT_EQ:
			return SIMPLE_BIN_OP_LSHIFT;
		case OP_RSHIFT_EQ:
			return SIMPLE_BIN_OP_RSHIFT;
		case OP_AND_EQ:
			return SIMPLE_BIN_OP_AND;
		case OP_HAT_EQ:
			return SIMPLE_BIN_OP_HAT;
		case OP_OR_EQ:
			return SIMPLE_BIN_OP_OR;
		default:
			assert("op_before_assign called before a non-assignment operator" &&
			       0);
	}
}

struct Expression assignment_expr(struct Expression expr,
                                  struct Expression expr2,
                                  enum TokenKind opkind)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct Expression new_expr;
	new_expr.details = expr.details;
	new_expr.category = ASSIGNMENT_EXPR;
	new_expr.simple_binary_operator = op_before_assign(opkind);
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct Expression
parse_assignment_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	const struct Token *tokvec2 = tokvec;
	struct ParserState *ptr_ps2 = ptr_ps;
	struct Expression expr = parse_unary_expression(ptr_ps2, &tokvec2);

	/* parse failed */
	if (!isAssign(tokvec2[0].kind)) {
		struct Expression expr_ = parse_conditional_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr_;
	}

	tokvec = tokvec2;

	if (is_array(expr.details.type) || is_array(expr.details.true_type)) {
		fprintf(stderr, "array is not an lvalue\n");
		exit(EXIT_FAILURE);
	}

	assert(isAssign(tokvec[0].kind));
	{
		{
			enum TokenKind opkind = tokvec[0].kind;
			++tokvec;

			struct Expression expr2 =
			    parse_assignment_expression(ptr_ps, &tokvec);
			expect_type(expr.details.type, expr2.details.type,
			            "mismatch in assignment operator");

			*ptr_tokvec = tokvec;
			return assignment_expr(expr, expr2, opkind);
		};
	}
}

static struct Expression
parse_conditional_expression(struct ParserState *ptr_ps,
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

		expect_type(false_branch.details.type, true_branch.details.type,
		            "mismatch of type in the false branch and the true branch");

		struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
		struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
		struct Expression *ptr_expr3 = calloc(1, sizeof(struct Expression));
		*ptr_expr1 = expr;
		*ptr_expr2 = true_branch;
		*ptr_expr3 = false_branch;

		struct Expression new_expr;
		new_expr.details = true_branch.details;
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

		struct Expression expr2 = parse_assignment_expression(ptr_ps, &tokvec);

		expr = simple_binary_op(expr, expr2, kind, expr2.details.type);
	}
	*ptr_tokvec = tokvec;
	return expr;
}
