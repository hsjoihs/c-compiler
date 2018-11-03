#include "header.h"
#include "std.h"
#include "std_io.h"

/***************************************
 * pure parsers with respect to types. *
 ***************************************/

static void parse_declarator(const struct Token **ptr_tokvec,
                             const char **ptr_to_ident_str,
                             struct Vector /*<struct TypeNode>*/ *ptr_vec);

static void skip_consts_or_noreturns(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	while (tokvec[0].kind == RES_CONST || tokvec[0].kind == RES_NORETURN) {
		++tokvec;
	}
	*ptr_tokvec = tokvec;
	return;
}

#define TypeNode Type
/* struct TypeNode denotes that the link list is incomplete and should not be
                                 brought outside */

static struct Type from_type3_to_type(const void **type3)
{
	struct Type type;
	const struct TypeNode *ptr_elem = type3[0];
	struct TypeNode elem = *ptr_elem;
	type = elem;
	switch (elem.type_category) {
	case INT_:
	case CHAR_:
	case STRUCT_:
	case VOID_:
	case ENUM_:
		return type;

	case PTR_:
	case ARRAY:
	case FN: {
		struct Type *ptr_to_current_type = calloc(1, sizeof(struct Type));
		*ptr_to_current_type = from_type3_to_type(type3 + 1);

		type.derived_from = ptr_to_current_type;
		return type;
	}
	}
	assert("unmatched case" && 0);
}

/*
parameter-declaration:
  declaration-specifiers declarator
  declaration-specifiers abstract-declarator_opt
*/
struct TypeAndIdent *
parse_parameter_declaration(const struct Token **ptr_tokvec)
{
	const char *ident_str = 0; /* null when abstract */
	struct TypeAndIdent *ptr_param_info =
	    calloc(1, sizeof(struct TypeAndIdent));

	struct Type type;
	{
		struct Type *ptr_base_type = parse_type_specifier(ptr_tokvec);
		struct Vector /*<struct TypeNode>*/ *ptr_vec = init_vector_();
		if ((*ptr_tokvec)[0].kind == OP_COMMA ||
		    (*ptr_tokvec)[0].kind == RIGHT_PAREN) {
			/* comma or right paren signifies the immediate end of
			 * parameter_declaration */
		} else {
			parse_declarator(ptr_tokvec, &ident_str, ptr_vec);
			if ((0)) {
				unsupported("abstract-declarator inside parameter declaration");
			}
		}
		push_vector(ptr_vec, ptr_base_type);
		type = from_type3_to_type(ptr_vec->vector);
	}

	if (type.type_category == FN) {
		/* shall be adjusted to `pointer to func`, according to the spec */
		ptr_param_info->type = ptr_to_type(&type);
	} else {
		/* convert to pointer */
		if (type.type_category == ARRAY) {
			type.type_category = PTR_;
		}
		ptr_param_info->type = type;
	}
	ptr_param_info->ident_str = ident_str;
	return ptr_param_info;
}

struct Type *
try_parse_type_specifier_and_semicolon(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec3 = *ptr_tokvec;
	struct Type *ptr_type = parse_type_specifier(&tokvec3);
	if (tokvec3[0].kind != SEMICOLON) {
		return 0;
	}
	++tokvec3;
	*ptr_tokvec = tokvec3;
	return ptr_type;
}

struct Type *parse_type_specifier(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	skip_consts_or_noreturns(&tokvec);

	enum TokenKind tok = tokvec[0].kind;
	struct TypeNode *ptr = calloc(1, sizeof(struct TypeNode));

	if (tok == RES_CHAR) {
		ptr->type_category = CHAR_;
		++tokvec;
	} else if (tok == RES_INT) {
		ptr->type_category = INT_;
		++tokvec;
	} else if (tok == RES_VOID) {
		ptr->type_category = VOID_;
		++tokvec;
	} else if (tok == RES_STRUCT) {
		++tokvec;
		expect_and_consume(&tokvec, IDENT_OR_RESERVED,
		                   "identifier after `struct`");
		const char *ident = tokvec[-1].ident_str;

		ptr->type_category = STRUCT_;
		ptr->s.struct_tag = ident;
		if (tokvec[0].kind != LEFT_BRACE) {
			ptr->s.struct_info.ptr_types_and_idents = 0; /* crucial; no info */
			skip_consts_or_noreturns(&tokvec);
			*ptr_tokvec = tokvec;
			return ptr;
		}
		++tokvec;
		ptr->s.struct_info.ptr_types_and_idents = init_vector_();

		while (1) {
			if (tokvec[0].kind == RIGHT_BRACE) {
				++tokvec;
				break;
			}
			const char *ident_str;
			struct Type t = parse_struct_declaration(&tokvec, &ident_str);
			expect_and_consume(&tokvec, SEMICOLON,
			                   "semicolon after the declarator inside struct");
			struct TypeAndIdent *ptr_t_and_i =
			    calloc(1, sizeof(struct TypeAndIdent));
			ptr_t_and_i->type = t;
			ptr_t_and_i->ident_str = ident_str;

			push_vector(ptr->s.struct_info.ptr_types_and_idents, ptr_t_and_i);
		}
	} else if (tok == RES_ENUM) {
		++tokvec;
		expect_and_consume(&tokvec, IDENT_OR_RESERVED,
		                   "identifier after `enum`");
		const char *ident = tokvec[-1].ident_str;
		ptr->type_category = ENUM_;
		ptr->e.enum_tag = ident;
		if (tokvec[0].kind != LEFT_BRACE) {
			ptr->e.enum_info.ptr_enumerators = 0; /* crucial; no info */
			skip_consts_or_noreturns(&tokvec);
			*ptr_tokvec = tokvec;
			return ptr;
		}
		++tokvec;

		ptr->e.enum_info.ptr_enumerators = init_vector_();

		do { /* at least one enumerator is needed */
			expect_and_consume(&tokvec, IDENT_OR_RESERVED,
			                   "identifier as a declaration of an enumerator");
			const char *ident_str = tokvec[-1].ident_str;
			push_vector(ptr->e.enum_info.ptr_enumerators, ident_str);

			/* ending without comma */
			if (tokvec[0].kind == RIGHT_BRACE) {
				++tokvec;
				break;
			} else if (tokvec[0].kind == OP_COMMA) {
				++tokvec;
				if (tokvec[0].kind == RIGHT_BRACE) {
					++tokvec;
					break;
				}
			}
		} while (1);

	} else {
		error_unexpected_token(
		    tokvec, "type name `int`, `char`, `void`, `struct` or `enum`");
	}

	skip_consts_or_noreturns(&tokvec);
	*ptr_tokvec = tokvec;
	return ptr;
}

static void
parse_parameter_type_list(const struct Token **ptr_tokvec,
                          struct Vector /*<struct TypeNode>*/ *ptr_vec)
{
	assert((*ptr_tokvec)[-1].kind == LEFT_PAREN);
	const struct Token *tokvec = *ptr_tokvec;
	struct Vector vec = *ptr_vec;
	{

		if (tokvec[0].kind == RIGHT_PAREN) { /* NO INFO */
			struct TypeNode f;
			f.type_category = FN;
			f.param_infos_validity = INVALID;
			f.param_infos = init_vector();
			struct TypeNode *ptr = calloc(1, sizeof(struct TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);
		} else if (tokvec[0].kind == RES_VOID &&
		           tokvec[1].kind == RIGHT_PAREN) { /* EXPLICITLY EMPTY */
			tokvec += 1;
			struct TypeNode f;
			f.type_category = FN;
			f.param_infos_validity = VALID;
			f.param_infos = init_vector();
			struct TypeNode *ptr = calloc(1, sizeof(struct TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);
		} else if (can_start_a_type(tokvec)) {
			struct TypeNode f;
			f.type_category = FN;
			f.param_infos_validity = VALID;
			f.param_infos = init_vector();

			push_vector(&f.param_infos, parse_parameter_declaration(&tokvec));

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;

				if (tokvec[0].kind == TRIPLE_DOT) {
					f.param_infos_validity = VA_ARGS;
					++tokvec;
					break;
				} else {
					push_vector(&f.param_infos,
					            parse_parameter_declaration(&tokvec));
				}
			}

			struct TypeNode *ptr = calloc(1, sizeof(struct TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);
		} else {
			error_unexpected_token(tokvec,
			                       "while parsing function parameter list");
		}
	}
	*ptr_tokvec = tokvec;
	*ptr_vec = vec;
}

static void parse_dcl_postfixes(const struct Token **ptr_tokvec,
                                struct Vector /*<struct TypeNode>*/ *ptr_vec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Vector vec = *ptr_vec;
	while (1) {
		if (tokvec[0].kind == LEFT_BRACKET) {
			++tokvec;
			expect_and_consume(&tokvec, LIT_DEC_INTEGER,
			                   "an integer while parsing a declaration");
			int length = tokvec[-1].int_value;
			expect_and_consume(&tokvec, RIGHT_BRACKET,
			                   "closing ] while parsing a declaration");

			struct TypeNode a;
			a.type_category = ARRAY;
			a.array_length = length;
			struct TypeNode *ptr = calloc(1, sizeof(struct TypeNode));
			*ptr = a;
			push_vector(&vec, ptr);
			continue;
		}
		if (tokvec[0].kind == LEFT_PAREN &&
		    (can_start_a_type(&tokvec[1]) || tokvec[1].kind == RIGHT_PAREN)) {
			++tokvec;
			parse_parameter_type_list(&tokvec, &vec);
			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing ) while parsing functional type");
		}
		break;
	}

	*ptr_tokvec = tokvec;
	*ptr_vec = vec;
}

static void
parse_direct_declarator(const struct Token **ptr_tokvec,
                        const char **ptr_to_ident_str,
                        struct Vector /*<struct TypeNode>*/ *ptr_vec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Vector vec = *ptr_vec;
	if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		parse_declarator(&tokvec, ptr_to_ident_str, &vec);
		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "closing parenthesis inside direct declarator");
		*ptr_tokvec = tokvec;
	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		*ptr_to_ident_str = tokvec[0].ident_str;
		++tokvec;
	} else {
		error_unexpected_token(tokvec, "an identifier in the declarator");
	}

	parse_dcl_postfixes(&tokvec, &vec);

	*ptr_tokvec = tokvec;
	*ptr_vec = vec;
}

static void parse_declarator(const struct Token **ptr_tokvec,
                             const char **ptr_to_ident_str,
                             struct Vector /*<struct TypeNode>*/ *ptr_vec)
{
	int asterisk_num = 0;
	for (; (*ptr_tokvec)[0].kind == OP_ASTERISK;
	     ++*ptr_tokvec, skip_consts_or_noreturns(ptr_tokvec)) {
		asterisk_num++;
	}

	parse_direct_declarator(ptr_tokvec, ptr_to_ident_str, ptr_vec);

	while (asterisk_num-- > 0) {
		struct TypeNode *ptr = calloc(1, sizeof(struct TypeNode));
		ptr->type_category = PTR_;
		push_vector(ptr_vec, ptr);
	}
}

struct UntypedExpr *
parse_init_declarator(const struct Token **ptr_tokvec,
                      const char **ptr_to_ident_str,
                      struct Vector /*<struct TypeNode>*/ *ptr_vec)
{
	struct UntypedExpr *ptr_expr = 0;
	parse_declarator(ptr_tokvec, ptr_to_ident_str, ptr_vec);
	if ((*ptr_tokvec)[0].kind == OP_EQ) {
		++*ptr_tokvec;
		ptr_expr = calloc(1, sizeof(struct UntypedExpr));
		*ptr_expr = parse_assignment_expression(ptr_tokvec);
	}
	return ptr_expr;
}

int can_start_a_type(const struct Token *tokvec)
{
	return tokvec[0].kind == RES_INT || tokvec[0].kind == RES_CHAR ||
	       tokvec[0].kind == RES_STRUCT || tokvec[0].kind == RES_VOID ||
	       tokvec[0].kind == RES_ENUM || tokvec[0].kind == RES_CONST ||
	       tokvec[0].kind == RES_NORETURN;
}

/* `int a`, `int *a` */
struct Type parse_struct_declaration(const struct Token **ptr_tokvec,
                                     const char **ptr_to_ident_str)
{
	struct Type *ptr_base_type = parse_type_specifier(ptr_tokvec);
	struct Vector /*<struct TypeNode>*/ *ptr_vec = init_vector_();
	parse_declarator(ptr_tokvec, ptr_to_ident_str, ptr_vec);
	push_vector(ptr_vec, ptr_base_type);
	return from_type3_to_type(ptr_vec->vector);
}

/* `int a;`, `int *a;`, `int a = 5;` */
struct Type parse_declaration(const struct Token **ptr_tokvec,
                              const char **ptr_to_ident_str,
                              struct UntypedExpr **ptr_ptr_uexpr)
{
	struct Type *ptr_base_type = parse_type_specifier(ptr_tokvec);
	struct Vector /*<struct TypeNode>*/ *ptr_vec = init_vector_();
	struct UntypedExpr *ptr_uexpr = parse_init_declarator(
	    ptr_tokvec, ptr_to_ident_str, ptr_vec); /* nullable */
	*ptr_ptr_uexpr = ptr_uexpr;
	push_vector(ptr_vec, ptr_base_type);
	expect_and_consume(
	    ptr_tokvec, SEMICOLON,
	    "semicolon at the end of variable definition/declaration");
	return from_type3_to_type(ptr_vec->vector);
}

struct Type parse_type_specifier_and_declarator(const struct Token **ptr_tokvec,
                                                const char **ptr_to_ident_str)
{
	struct Type *ptr_base_type = parse_type_specifier(ptr_tokvec);
	struct Vector /*<struct TypeNode>*/ *ptr_vec = init_vector_();
	parse_declarator(ptr_tokvec, ptr_to_ident_str, ptr_vec);
	push_vector(ptr_vec, ptr_base_type);
	return from_type3_to_type(ptr_vec->vector);
}

/*

type-name:
  type-specifier abstract-declarator_opt

abstract-declarator:
  pointer
  pointer_opt direct-abstract-declarator

direct-abstract-declarator:
  ( abstract-declarator )
  direct-abstract-declarator_opt [ constant-expression_opt ]
  direct-abstract-declarator_opt ( parameter-type-list_opt )

*/

static void
parse_abstract_declarator(const struct Token **ptr_tokvec,
                          struct Vector /*<struct TypeNode>*/ *ptr_vec);

static void
parse_direct_abstract_declarator(const struct Token **ptr_tokvec,
                                 struct Vector /*<struct TypeNode>*/ *ptr_vec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Vector vec = *ptr_vec;
	if (tokvec[0].kind == LEFT_PAREN && !can_start_a_type(&tokvec[1]) &&
	    tokvec[1].kind !=
	        RIGHT_PAREN) { /* an empty parenthesis must be of a function */
		++tokvec;
		parse_abstract_declarator(&tokvec, &vec);
		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "closing parenthesis inside direct declarator");
	}

	parse_dcl_postfixes(&tokvec, &vec);

	*ptr_tokvec = tokvec;
	*ptr_vec = vec;
}

static void
parse_abstract_declarator(const struct Token **ptr_tokvec,
                          struct Vector /*<struct TypeNode>*/ *ptr_vec)
{
	const struct Token *tokvec = *ptr_tokvec;

	if (tokvec[0].kind != OP_ASTERISK) {
		parse_direct_abstract_declarator(&tokvec, ptr_vec);
		*ptr_tokvec = tokvec;
		return;
	}

	int asterisk_num = 0;
	for (; tokvec[0].kind == OP_ASTERISK;
	     ++tokvec, skip_consts_or_noreturns(&tokvec)) {
		asterisk_num++;
	}

	/* optional */
	if (tokvec[0].kind == LEFT_PAREN || tokvec[0].kind == LEFT_BRACKET) {
		parse_direct_abstract_declarator(&tokvec, ptr_vec);
	}

	while (asterisk_num-- > 0) {
		struct TypeNode *ptr = calloc(1, sizeof(struct TypeNode));
		ptr->type_category = PTR_;
		push_vector(ptr_vec, ptr);
	}

	*ptr_tokvec = tokvec;
}

/* `int `, `int *` */
struct Type parse_type_name(const struct Token **ptr_tokvec)
{
	struct Type *ptr_base_type = parse_type_specifier(ptr_tokvec);
	struct Vector /*<struct TypeNode>*/ *ptr_vec = init_vector_();

	const struct Token *tokvec = *ptr_tokvec;

	/* optional */
	if (tokvec[0].kind == OP_ASTERISK || tokvec[0].kind == LEFT_PAREN ||
	    tokvec[0].kind == LEFT_BRACKET) {
		parse_abstract_declarator(&tokvec, ptr_vec);
	}
	push_vector(ptr_vec, ptr_base_type);

	*ptr_tokvec = tokvec;
	return from_type3_to_type(ptr_vec->vector);
}
