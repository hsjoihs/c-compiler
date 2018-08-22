#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/***************************************
 * pure parsers with respect to types. *
 ***************************************/
typedef struct Type TypeNode; /* the link list is incomplete and should not be
                                 brought outside */

static struct Type from_type3_to_type(const void **type3)
{
	struct Type type;
	TypeNode elem = *(const TypeNode *)type3[0];
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

struct TypeAndIdent *parse_param(const struct Token **ptr_tokvec)
{
	const char *ident_str;
	struct TypeAndIdent *ptr_param_info =
	    calloc(1, sizeof(struct TypeAndIdent));

	struct Type type = parse_declaration(ptr_tokvec, &ident_str);
	if (type.type_category == FN) {
		/* shall be adjusted to `pointer to func`, according to the spec */
		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = type;
		ptr_param_info->type = ptr_of_type_to_ptr_to_type(ptr_type);
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
	if (tokvec3[0].kind == SEMICOLON) {
		++tokvec3;
		*ptr_tokvec = tokvec3;
		return ptr_type;
	} else {
		return 0;
	}
}

struct Type *parse_type_specifier(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	enum TokenKind tok = tokvec[0].kind;
	TypeNode *ptr = calloc(1, sizeof(TypeNode));

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
		ptr->struct_tag = ident;
		if (tokvec[0].kind != LEFT_BRACE) {
			ptr->struct_info.ptr_types_and_idents = 0; /* crucial; no info */
			*ptr_tokvec = tokvec;
			return ptr;
		}
		++tokvec;
		ptr->struct_info.ptr_types_and_idents = init_vector_();

		while (1) {
			if (tokvec[0].kind == RIGHT_BRACE) {
				++tokvec;
				break;
			}
			const char *ident_str;
			struct Type t = parse_declaration(&tokvec, &ident_str);
			expect_and_consume(&tokvec, SEMICOLON,
			                   "semicolon after the declarator inside struct");
			struct TypeAndIdent *ptr_t_and_i =
			    calloc(1, sizeof(struct TypeAndIdent));
			ptr_t_and_i->type = t;
			ptr_t_and_i->ident_str = ident_str;

			push_vector(ptr->struct_info.ptr_types_and_idents, ptr_t_and_i);
		}
	} else if (tok == RES_ENUM) {
		++tokvec;
		expect_and_consume(&tokvec, IDENT_OR_RESERVED,
		                   "identifier after `enum`");
		const char *ident = tokvec[-1].ident_str;
		ptr->type_category = ENUM_;
		ptr->enum_tag = ident;
		if (tokvec[0].kind != LEFT_BRACE) {
			ptr->enum_info.ptr_enumerators = 0; /* crucial; no info */
			*ptr_tokvec = tokvec;
			return ptr;
		}
		++tokvec;

		ptr->enum_info.ptr_enumerators = init_vector_();

		do { /* at least one enumerator is needed */
			expect_and_consume(&tokvec, IDENT_OR_RESERVED,
			                   "identifier as a declaration of an enumerator");
			const char *ident_str = tokvec[-1].ident_str;
			push_vector(ptr->enum_info.ptr_enumerators, ident_str);

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

	*ptr_tokvec = tokvec;
	return ptr;
}

struct Type parse_declaration_or_type_name(const struct Token **ptr_tokvec,
                                           const char **ptr_to_ident_str);

struct Type parse_declaration(const struct Token **ptr_tokvec,
                              const char **ptr_to_ident_str)
{
	assert(ptr_to_ident_str);
	return parse_declaration_or_type_name(ptr_tokvec, ptr_to_ident_str);
}

struct Type parse_type_name(const struct Token **ptr_tokvec)
{
	return parse_declaration_or_type_name(ptr_tokvec, 0);
}

static void
parse_declarator_or_abstract_declarator(const struct Token **ptr_tokvec,
                                        const char **ptr_to_ident_str,
                                        struct Vector /*<TypeNode>*/ *ptr_vec);

static void parse_direct_declarator(const struct Token **ptr_tokvec,
                                    const char **ptr_to_ident_str,
                                    struct Vector /*<TypeNode>*/ *ptr_vec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Vector vec = *ptr_vec;
	if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		parse_declarator_or_abstract_declarator(&tokvec, ptr_to_ident_str,
		                                        &vec);
		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "closing parenthesis inside direct declarator");
		*ptr_tokvec = tokvec;
	} else if (ptr_to_ident_str) {
		if (tokvec[0].kind == IDENT_OR_RESERVED) {
			*ptr_to_ident_str = tokvec[0].ident_str;
			++tokvec;
		} else {
			error_unexpected_token(tokvec, "an identifier in the declarator");
		}
	}

	while (1) {
		if (tokvec[0].kind == LEFT_BRACKET) {
			++tokvec;
			expect_and_consume(&tokvec, LIT_DEC_INTEGER,
			                   "an integer while parsing a declaration");
			int length = tokvec[-1].int_value;
			expect_and_consume(&tokvec, RIGHT_BRACKET,
			                   "closing ] while parsing a declaration");

			TypeNode a;
			a.type_category = ARRAY;
			a.array_length = length;
			TypeNode *ptr = calloc(1, sizeof(TypeNode));
			*ptr = a;
			push_vector(&vec, ptr);
			continue;
		}
		if (tokvec[0].kind != LEFT_PAREN) {
			break;
		}

		++tokvec;
		if (tokvec[0].kind == RIGHT_PAREN) { /* NO INFO */
			++tokvec;
			TypeNode f;
			f.type_category = FN;
			f.is_param_infos_valid = 0;
			f.param_infos = init_vector();
			TypeNode *ptr = calloc(1, sizeof(TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);
		} else if (tokvec[0].kind == RES_VOID &&
		           tokvec[1].kind == RIGHT_PAREN) { /* EXPLICITLY EMPTY */
			tokvec += 2;
			TypeNode f;
			f.type_category = FN;
			f.is_param_infos_valid = 1;
			f.param_infos = init_vector();
			TypeNode *ptr = calloc(1, sizeof(TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);
		} else if (can_start_a_type(tokvec)) {
			TypeNode f;
			f.type_category = FN;
			f.is_param_infos_valid = 1;
			f.param_infos = init_vector();

			push_vector(&f.param_infos, parse_param(&tokvec));

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;

				push_vector(&f.param_infos, parse_param(&tokvec));
			}

			TypeNode *ptr = calloc(1, sizeof(TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing ) while parsing functional type");
		}
	}

	*ptr_tokvec = tokvec;
	*ptr_vec = vec;
}

static void
parse_declarator_or_abstract_declarator(const struct Token **ptr_tokvec,
                                        const char **ptr_to_ident_str,
                                        struct Vector /*<TypeNode>*/ *ptr_vec)
{
	const struct Token *tokvec = *ptr_tokvec;
	int asterisk_num = 0;
	for (; tokvec[0].kind == OP_ASTERISK; ++tokvec) {
		asterisk_num++;
	}

	parse_direct_declarator(&tokvec, ptr_to_ident_str, ptr_vec);

	while (asterisk_num-- > 0) {
		TypeNode *ptr = calloc(1, sizeof(TypeNode));
		ptr->type_category = PTR_;
		push_vector(ptr_vec, ptr);
	}

	*ptr_tokvec = tokvec;
}

/*
    when ptr_to_ident_str is a valid pointer: `int a`, `int *a`
    when ptr_to_ident_str is NULL: `int `, `int *`
*/
struct Type parse_declaration_or_type_name(const struct Token **ptr_tokvec,
                                           const char **ptr_to_ident_str)
{
	struct Type *ptr_base_type = parse_type_specifier(ptr_tokvec);
	struct Vector /*<TypeNode>*/ *ptr_vec = init_vector_();
	parse_declarator_or_abstract_declarator(ptr_tokvec, ptr_to_ident_str,
	                                        ptr_vec);
	push_vector(ptr_vec, ptr_base_type);
	return from_type3_to_type(ptr_vec->vector);
}

int can_start_a_type(const struct Token *tokvec)
{
	return tokvec[0].kind == RES_INT || tokvec[0].kind == RES_CHAR ||
	       tokvec[0].kind == RES_STRUCT || tokvec[0].kind == RES_VOID ||
	       tokvec[0].kind == RES_ENUM;
}
