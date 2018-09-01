#include "header.h"
#include "std.h"
#include "std_io.h"

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

int size_of_basic(const struct Type *ref_type, const char *msg)
{
	switch (ref_type->type_category) {
		case INT_:
		case ENUM_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
		case FN:
		case STRUCT_:
			fprintf(stderr,
			        "array, function or struct type is not a basic type.\n");
			fprintf(stderr, "context: %s\n", msg);
			exit(EXIT_FAILURE);
		case VOID_:
			fprintf(stderr, "size of `void` is never known\n");
			fprintf(stderr, "context: %s\n", msg);
			exit(EXIT_FAILURE);
	}
}

void debug_print_type(const struct Type *ref_type)
{
	struct Type type = *ref_type;
	switch (type.type_category) {
		case PTR_:
			fprintf(stderr, "pointer to ");
			debug_print_type(type.derived_from);
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
			fprintf(stderr, "struct %s", type.s.struct_tag);
			return;
		case ENUM_:
			fprintf(stderr, "enum %s", type.e.enum_tag);
			return;
		case ARRAY:
			fprintf(stderr, "array (length %d) of ", type.array_length);
			debug_print_type(type.derived_from);
			return;
		case FN:
			fprintf(stderr, "function (");
			if (!type.is_param_infos_valid) {
				fprintf(stderr, "param: no info");
			} else {
				if (type.param_infos.length == 0) {
					fprintf(stderr, "no params");
				} else if (type.param_infos.length < 2) {
					const struct TypeAndIdent *vec_0 =
					    type.param_infos.vector[0];
					fprintf(stderr, "%s: ",
					        vec_0->ident_str ? vec_0->ident_str : "@anon");
					debug_print_type(&vec_0->type);
				} else {
					fprintf(stderr, "params: \n");
					for (int i = 0; i < type.param_infos.length; i++) {
						const struct TypeAndIdent *ptr_paraminfo =
						    type.param_infos.vector[i];
						fprintf(stderr, "  %s: ",
						        ptr_paraminfo->ident_str
						            ? ptr_paraminfo->ident_str
						            : "@anon");
						debug_print_type(&ptr_paraminfo->type);
						fprintf(stderr, "\n");
					}
				}
			}
			fprintf(stderr, ") returning ");
			debug_print_type(type.derived_from);
	}
}

struct Type deref_type(const struct Type *ref_t)
{
	switch (ref_t->type_category) {
		case PTR_:
			return *ref_t->derived_from;

		default:
			fprintf(stderr, "Unmatched type: expected a pointer, but got `");
			debug_print_type(ref_t);
			fprintf(stderr, "`.\n");
			exit(EXIT_FAILURE);
	}
}

void if_array_convert_to_ptr_(struct Type *ptr_t)
{
	if (ptr_t->type_category == ARRAY) {
		ptr_t->type_category = PTR_;
	}
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
