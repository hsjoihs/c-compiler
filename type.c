#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_strictly_equal(struct Type t1, struct Type t2);

struct Type INT_TYPE() {
	struct Type t;
	t.type_category = INT_;
	return t;
}

struct Type
CHAR_TYPE() {
	struct Type t;
	t.type_category = CHAR_;
	return t;
}

int size_of_basic(struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
		case FN:
		case STRUCT_:
			fprintf(stderr,
			        "array, function or struct type is not a basic type\n");
			exit(EXIT_FAILURE);
		case VOID_:
			fprintf(stderr,
			        "size of `void` is never known\n");
			exit(EXIT_FAILURE);
	}
}

void debug_print_type(struct Type type)
{
	switch (type.type_category) {
		case PTR_:
			fprintf(stderr, "pointer to ");
			debug_print_type(*type.derived_from);
			return;
		case VOID_:
			fprintf(stderr, "void");
			return;
		case INT_:
			fprintf(stderr, "int");
			return;
		case CHAR_:
			fprintf(stderr, "char");
			return;
		case STRUCT_:
			fprintf(stderr, "struct %s", type.struct_tag);
			return;
		case ARRAY:
			fprintf(stderr, "array (length %d) of ", type.array_length);
			debug_print_type(*type.derived_from);
			return;
		case FN:
			fprintf(stderr, "function (");
			if (!type.param_infos.vector) {
				fprintf(stderr, "param: no info");
			} else {
				if (type.param_infos.length < 2) {
					const struct TypeAndIdent *vec_0 =
					    (const struct TypeAndIdent *)type.param_infos.vector[0];
					fprintf(stderr, "%s: ", vec_0->ident_str);
					debug_print_type(vec_0->type);
				} else {
					fprintf(stderr, "params: \n");
					for (int i = 0; i < type.param_infos.length; i++) {
						const struct TypeAndIdent *ptr_paraminfo =
						    (const struct TypeAndIdent *)
						        type.param_infos.vector[i];
						fprintf(stderr, "  %s: ", ptr_paraminfo->ident_str);
						debug_print_type(ptr_paraminfo->type);
						fprintf(stderr, "\n");
					}
				}
			}
			fprintf(stderr, ") returning ");
			debug_print_type(*type.derived_from);
	}
}

struct Type deref_type(struct Type t)
{
	switch (t.type_category) {
		case PTR_:
			return *t.derived_from;

		default:
			fprintf(stderr, "Unmatched type: expected a pointer, but got `");
			debug_print_type(t);
			fprintf(stderr, "`.\n");
			exit(EXIT_FAILURE);
	}
}

int is_pointer(struct Type t) { return t.type_category == PTR_; }

int is_array(struct Type t) { return t.type_category == ARRAY; }

struct Type if_array_convert_to_ptr(struct Type t)
{
	if (is_array(t)) {
		t.type_category = PTR_;
	}
	return t;
}

struct Type ptr_of_type_to_ptr_to_type(struct Type *ptr_type)
{
	struct Type type;
	type.type_category = PTR_;
	type.derived_from = ptr_type;
	return type;
}

struct Type ptr_of_type_to_arr_of_type(struct Type *ptr_type, int length)
{
	struct Type type;
	type.type_category = ARRAY;
	type.derived_from = ptr_type;
	type.array_length = length;
	return type;
}
