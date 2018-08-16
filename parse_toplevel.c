#include "toplevel.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int size_of(struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
			return type.array_length * size_of(*type.derived_from);
		case FN:
			fprintf(stderr, "function type does not have size\n");
			exit(EXIT_FAILURE);
		case STRUCT_:
			unsupported("size of struct");
	}
}

int align_of(struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
			return align_of(*type.derived_from);
		case FN:
			fprintf(stderr, "function type does not have size or alignment\n");
			exit(EXIT_FAILURE);
		case STRUCT_:
			unsupported("width of struct");
	}
}

static void record_global_struct_declaration(struct ParserState *ptr_ps,
                                             struct Type struct_type)
{
	assert(struct_type.type_category == STRUCT_);
	struct StructInternalInfo info = struct_type.struct_info;
	struct Vector /* <TypeAndIdent> */ types_and_idents = info.types_and_idents;

	struct SizeAndAlignment *inner_type_vec =
	    calloc(types_and_idents.length, sizeof(struct SizeAndAlignment));

	int i = 0;
	for (; i < types_and_idents.length; i++) {
		const struct TypeAndIdent *ptr_vec_i = types_and_idents.vector[i];
		inner_type_vec[i].size = size_of(ptr_vec_i->type);
		inner_type_vec[i].alignment = align_of(ptr_vec_i->type);
	}

	int *offset_vec;
	struct SizeAndAlignment outer_s_and_a =
	    get_size_alignment_offsets(inner_type_vec, &offset_vec, i);

	struct StructInternalCompleteInfo *ptr_complete =
	    calloc(1, sizeof(struct StructInternalCompleteInfo));
	ptr_complete->info = info;
	ptr_complete->offset_vec = offset_vec;
	ptr_complete->s_and_a = outer_s_and_a;

	insert(&ptr_ps->global_struct_tag_map, struct_type.struct_tag,
	       ptr_complete);
}

static void record_if_global_struct_declaration(struct ParserState *ptr_ps,
                                                struct Type type)
{
	switch (type.type_category) {
		case ARRAY:
		case FN:
		case PTR_:
			record_if_global_struct_declaration(ptr_ps, *type.derived_from);
			return;
		case INT_:
		case CHAR_:
			return;
		case STRUCT_:
			record_global_struct_declaration(ptr_ps, type);
			return;
	}
}

static struct Toplevel
parse_toplevel_definition(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	struct Type *optional_ptr_type =
	    try_parse_type_specifier_and_semicolon(ptr_tokvec);

	if (optional_ptr_type) {
		struct Toplevel d;
		d.category = TOPLEVEL_VAR_DEFINITION;
		d.declarator_type = *optional_ptr_type;
		/* d.size_of_declarator_type need not be set; it is not a real variable
		 */

		record_if_global_struct_declaration(ptr_ps, d.declarator_type);
		d.declarator_name = 0;
		return d;
	}

	const struct Token *tokvec2 = *ptr_tokvec;

	const char *declarator_name;
	struct Type declarator_type = parse_declarator(&tokvec2, &declarator_name);
	record_if_global_struct_declaration(ptr_ps, declarator_type);

	if (declarator_type.type_category != FN) {

		expect_and_consume(&tokvec2, SEMICOLON,
		                   "semicolon at the end of variable definition");
		struct Map globalmap = ptr_ps->global_vars_type_map;

		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = declarator_type;
		insert(&globalmap, declarator_name, ptr_type);
		ptr_ps->global_vars_type_map = globalmap;

		*ptr_tokvec = tokvec2;

		struct Toplevel d;
		d.category = TOPLEVEL_VAR_DEFINITION;
		d.declarator_name = declarator_name;
		d.declarator_type = declarator_type;
		d.size_of_declarator_type = size_of(declarator_type);
		return d;
	}

	struct Vector param_infos = declarator_type.param_infos;
	struct Type ret_type = *declarator_type.derived_from;

	ptr_ps->scope_chain.outer = 0; /* most outer scope */
	ptr_ps->scope_chain.var_table = init_map();
	ptr_ps->newest_offset = 0;
	ptr_ps->func_ret_type = ret_type;

	struct FuncInfo *ptr_func_info = calloc(1, sizeof(struct FuncInfo));
	ptr_func_info->ret_type = ret_type;
	insert(&ptr_ps->func_info_map, declarator_name, ptr_func_info);

	if (!param_infos.vector &&
	    tokvec2[0].kind == SEMICOLON) { /* function prototype */
		++tokvec2;
		/* do nothing, since the return value is already in the retmap
		 */
		*ptr_tokvec = tokvec2;

		struct Toplevel def;
		def.category = TOPLEVEL_FUNCTION_DECLARATION;
		return def;
	}

	struct Vector offsets_and_types = init_vector();

	if (param_infos.vector) { /* parameter is not empty */
		for (int counter = 0; counter < param_infos.length; ++counter) {

			struct TypeAndIdent param_info =
			    *(const struct TypeAndIdent *)(param_infos.vector[counter]);
			const char *ident_str;

			struct Type type = param_info.type;
			ident_str = param_info.ident_str;

			if (counter > 5) {
				unsupported("6-or-more parameters");
			}

			int offset = add_local_var_to_scope(ptr_ps, type, ident_str);

			struct LocalVarInfo info;
			info.offset = offset;
			info.type = type;

			struct LocalVarInfo *ptr1 = calloc(1, sizeof(struct LocalVarInfo));
			*ptr1 = info;
			push_vector(&offsets_and_types, ptr1);
		}
	}

	struct Statement sta = parse_compound_statement(ptr_ps, &tokvec2);
	*ptr_tokvec = tokvec2;
	/* parse finished */

	struct Toplevel def;
	def.category = TOPLEVEL_FUNCTION_DEFINITION;
	def.declarator_name = declarator_name;
	def.func.sta = sta;
	def.func.offsets_and_types = offsets_and_types;
	def.func.ret_type = ret_type;
	def.func.capacity = -ptr_ps->newest_offset;
	return def;
}

struct Vector parse(const struct Token *tokvec)
{
	struct ParserState ps;
	ps.func_info_map = init_map();
	ps.global_vars_type_map = init_map();
	ps.global_struct_tag_map = init_map();

	struct Vector vec = init_vector();
	while (1) {
		if (tokvec[0].kind == END) {
			expect_and_consume(&tokvec, END, "the end of file");
			break;
		} else {
			struct Toplevel def = parse_toplevel_definition(&ps, &tokvec);
			struct Toplevel *ptr = calloc(1, sizeof(struct Toplevel));
			*ptr = def;
			push_vector(&vec, ptr);
		}
	}
	return vec;
}
