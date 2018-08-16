#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static struct UntypedExpression
parse_unary_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_conditional_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_logical_OR_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_AND_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_exclusive_OR_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_equality_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_postfix_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_assignment_expression(const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_primary_expression(const struct Token **ptr_tokvec);

static struct UntypedExpression
binary_op_untyped(struct UntypedExpression expr, struct UntypedExpression expr2,
                  enum TokenKind kind)
{
	struct UntypedExpression *ptr_expr1 =
	    calloc(1, sizeof(struct UntypedExpression));
	struct UntypedExpression *ptr_expr2 =
	    calloc(1, sizeof(struct UntypedExpression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct UntypedExpression new_expr;
	new_expr.category = BINARY_EXPR;
	new_expr.operator= kind;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct UntypedExpression
parse_inclusive_OR_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_exclusive_OR_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_exclusive_OR_expression(&tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_exclusive_OR_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_AND_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_HAT) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_AND_expression(&tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct UntypedExpression
parse_AND_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_equality_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_equality_expression(&tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct UntypedExpression
parse_cast_expression(const struct Token **ptr_tokvec)
{
	return parse_unary_expression(ptr_tokvec);
}

static struct UntypedExpression
parse_multiplicative_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_cast_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_ASTERISK && kind != OP_SLASH && kind != OP_PERCENT) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_cast_expression(&tokvec);

		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_additive_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_multiplicative_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_PLUS && kind != OP_MINUS) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_multiplicative_expression(&tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_shift_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_additive_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_LSHIFT && kind != OP_RSHIFT) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_additive_expression(&tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_relational_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_shift_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_GT && kind != OP_GT_EQ && kind != OP_LT &&
		    kind != OP_LT_EQ) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_shift_expression(&tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_equality_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_relational_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_EQ_EQ && kind != OP_NOT_EQ) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_relational_expression(&tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_logical_AND_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	int counter = 0;

	struct UntypedExpression first_expr =
	    parse_inclusive_OR_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND_AND) {
			break;
		}

		++tokvec;

		struct UntypedExpression expr2 = parse_inclusive_OR_expression(&tokvec);
		++counter;

		first_expr = binary_op_untyped(first_expr, expr2, OP_AND_AND);
	}

	*ptr_tokvec = tokvec;
	return first_expr;
}

static struct UntypedExpression
parse_logical_OR_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_logical_AND_expression(&tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR_OR) {
			break;
		}

		++tokvec;
		struct UntypedExpression expr2 = parse_logical_AND_expression(&tokvec);

		expr = binary_op_untyped(expr, expr2, OP_OR_OR);
	}

	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression unary_op_untyped(struct UntypedExpression expr,
                                                 enum TokenKind kind)
{
	struct UntypedExpression *ptr_expr1 =
	    calloc(1, sizeof(struct UntypedExpression));
	*ptr_expr1 = expr;

	struct UntypedExpression new_expr;
	new_expr.category = UNARY_EXPR;
	new_expr.operator= kind;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = 0;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct UntypedExpression
parse_unary_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	if (tokvec[0].kind == OP_NOT || tokvec[0].kind == OP_TILDA ||
	    tokvec[0].kind == OP_PLUS || tokvec[0].kind == OP_MINUS ||
	    tokvec[0].kind == OP_ASTERISK || tokvec[0].kind == OP_AND) {
		enum TokenKind kind = tokvec[0].kind;
		++tokvec;

		struct UntypedExpression expr = parse_cast_expression(&tokvec);

		struct UntypedExpression new_expr = unary_op_untyped(expr, kind);

		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_PLUS_PLUS ||
	           tokvec[0].kind == OP_MINUS_MINUS) {
		enum TokenKind opkind = tokvec[0].kind;
		++tokvec;

		struct UntypedExpression expr = parse_unary_expression(&tokvec);

		struct UntypedExpression new_expr = unary_op_untyped(expr, opkind);
		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == RES_SIZEOF && tokvec[1].kind == LEFT_PAREN &&
	           can_start_a_type(tokvec + 2)) {
		tokvec += 2;
		struct Type type = parse_type_name(&tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "closing parenthesis of sizeof(typename)");

		struct UntypedExpression expr;
		expr.category = SIZEOF_TYPE;
		expr.operand_of_sizeof = type;
		*ptr_tokvec = tokvec;
		return expr;
	} else {

		struct UntypedExpression expr = parse_postfix_expression(&tokvec);
		*ptr_tokvec = tokvec;
		return expr;
	}
}

static struct UntypedExpression dot(struct UntypedExpression expr,
                                    const char *name);

static struct UntypedExpression
parse_postfix_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;

		tokvec += 2;

		struct Vector arguments = init_vector();

		if (tokvec[0].kind == RIGHT_PAREN) {
			tokvec++;
		} else {
			struct UntypedExpression e = parse_assignment_expression(&tokvec);
			struct UntypedExpression *ptr_e =
			    calloc(1, sizeof(struct UntypedExpression));
			*ptr_e = e;
			push_vector(&arguments, ptr_e);

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;

				struct UntypedExpression e =
				    parse_assignment_expression(&tokvec);
				struct UntypedExpression *ptr_e =
				    calloc(1, sizeof(struct UntypedExpression));
				*ptr_e = e;
				push_vector(&arguments, ptr_e);
			}

			*ptr_tokvec = tokvec;

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing parenthesis of function call");

			*ptr_tokvec = tokvec;
		}
		*ptr_tokvec = tokvec;

		expr.category = FUNCCALL;
		expr.arg_exprs_vec = arguments;
		expr.var_name = ident_str;
	} else {
		expr = parse_primary_expression(&tokvec);
	}

	while (1) {
		if (tokvec[0].kind == LEFT_BRACKET) {
			++tokvec;
			struct UntypedExpression expr2 = parse_expression(&tokvec);
			expect_and_consume(&tokvec, RIGHT_BRACKET, "right bracket ]");

			expr = unary_op_untyped(binary_op_untyped(expr, expr2, OP_PLUS),
			                        OP_ASTERISK);
		} else if (tokvec[0].kind == OP_PLUS_PLUS ||
		           tokvec[0].kind == OP_MINUS_MINUS) {
			enum TokenKind opkind = tokvec[0].kind;
			tokvec++;

			struct UntypedExpression *ptr_expr1 =
			    calloc(1, sizeof(struct UntypedExpression));
			*ptr_expr1 = expr;

			struct UntypedExpression new_expr;
			new_expr.operator= opkind;
			new_expr.category = POSTFIX_EXPR;
			new_expr.ptr1 = ptr_expr1;
			new_expr.ptr2 = 0;
			new_expr.ptr3 = 0;

			expr = new_expr;
		} else if (tokvec[0].kind == DOT) {
			++tokvec;

			expect_and_consume(&tokvec, IDENT_OR_RESERVED,
			                   "identifier after a dot operator");
			const char *name = tokvec[-1].ident_str;

			expr = dot(expr, name);
		} else if (tokvec[0].kind == ARROW) {
			++tokvec;

			expect_and_consume(&tokvec, IDENT_OR_RESERVED,
			                   "identifier after an arrow operator");
			const char *name = tokvec[-1].ident_str;

			expr = dot(unary_op_untyped(expr, OP_ASTERISK), name);
		} else {
			break;
		}
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression dot(struct UntypedExpression expr,
                                    const char *name)
{
	struct UntypedExpression *ptr_expr1 =
	    calloc(1, sizeof(struct UntypedExpression));
	*ptr_expr1 = expr;

	struct UntypedExpression new_expr;
	new_expr.category = DOT_EXPR;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = 0;
	new_expr.ptr3 = 0;
	new_expr.ident_after_dot = name;
	return new_expr;
}

static struct UntypedExpression
parse_primary_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_tokvec;

		struct UntypedExpression expr;
		expr.int_value = tokvec[0].int_value;
		expr.category = INT_LITERAL_;

		return expr;

	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_tokvec;

		struct UntypedExpression expr;
		expr.category = VAR;
		expr.var_name = tokvec[0].ident_str;
		return expr;

	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		struct UntypedExpression expr = parse_expression(&tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right paren");

		*ptr_tokvec = tokvec;
		return expr;
	} else if (tokvec[0].kind == LIT_STRING) {
		struct Type *ptr_char = calloc(1, sizeof(struct Type));
		*ptr_char = CHAR_TYPE();

		struct UntypedExpression expr;
		expr.category = STRING_LITERAL_;
		expr.literal_string = tokvec[0].literal_str;

		++*ptr_tokvec;
		return expr;
	}

	error_unexpected_token(
	    tokvec, "the beginning of parse_typecheck_primary_expression");
}

static struct UntypedExpression
parse_assignment_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	const struct Token *tokvec2 = tokvec;
	struct UntypedExpression expr = parse_unary_expression(&tokvec2);

	/* parse failed */
	if (!isAssign(tokvec2[0].kind)) {
		struct UntypedExpression expr_ = parse_conditional_expression(&tokvec);
		*ptr_tokvec = tokvec;
		return expr_;
	}

	tokvec = tokvec2;

	assert(isAssign(tokvec[0].kind));
	enum TokenKind opkind = tokvec[0].kind;
	++tokvec;

	struct UntypedExpression expr2 = parse_assignment_expression(&tokvec);

	*ptr_tokvec = tokvec;
	return binary_op_untyped(expr, expr2, opkind);
}

static struct UntypedExpression
parse_conditional_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_logical_OR_expression(&tokvec);
	if (tokvec[0].kind == QUESTION) {

		++tokvec;
		*ptr_tokvec = tokvec;
		struct UntypedExpression true_branch = parse_expression(&tokvec);

		expect_and_consume(&tokvec, COLON, "colon of the conditional operator");

		*ptr_tokvec = tokvec;
		struct UntypedExpression false_branch =
		    parse_conditional_expression(&tokvec);

		*ptr_tokvec = tokvec;

		struct UntypedExpression *ptr_expr1 =
		    calloc(1, sizeof(struct UntypedExpression));
		struct UntypedExpression *ptr_expr2 =
		    calloc(1, sizeof(struct UntypedExpression));
		struct UntypedExpression *ptr_expr3 =
		    calloc(1, sizeof(struct UntypedExpression));
		*ptr_expr1 = expr;
		*ptr_expr2 = true_branch;
		*ptr_expr3 = false_branch;

		struct UntypedExpression new_expr;
		new_expr.category = CONDITIONAL;
		new_expr.ptr1 = ptr_expr1;
		new_expr.ptr2 = ptr_expr2;
		new_expr.ptr3 = ptr_expr3;

		return new_expr;
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct UntypedExpression parse_expression(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_assignment_expression(&tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_COMMA) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_assignment_expression(&tokvec);

		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}
