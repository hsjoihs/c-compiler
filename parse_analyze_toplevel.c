#include "toplevel.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int size_of(const struct AnalyzerState *ptr_ps, struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
			return type.array_length * size_of(ptr_ps, *type.derived_from);
		case FN:
			fprintf(stderr, "function type does not have size\n");
			exit(EXIT_FAILURE);
		case STRUCT_:
			if ((0)) { /* is local struct */
				unsupported("local struct");
			}
			const char *tag = type.struct_tag;
			const struct StructInternalCompleteInfo *ptr_info =
			    lookup(ptr_ps->global_struct_tag_map, tag);
			if (!ptr_info) {
				fprintf(
				    stderr,
				    "tried to take the size of incomplete type `struct %s`\n",
				    type.struct_tag);
				exit(EXIT_FAILURE);
			}
			return ptr_info->s_and_a.size;
		case VOID_:
			fprintf(stderr, "size of type `void` is never known\n");
			exit(EXIT_FAILURE);
		case ENUM_:
			return 4;
	}
}

int align_of(const struct AnalyzerState *ptr_ps, struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
			return align_of(ptr_ps, *type.derived_from);
		case FN:
			fprintf(stderr, "function type does not have size or alignment\n");
			exit(EXIT_FAILURE);
		case STRUCT_:
			if ((0)) { /* is local struct */
				unsupported("local struct");
			}
			const char *tag = type.struct_tag;
			const struct StructInternalCompleteInfo *ptr_info =
			    lookup(ptr_ps->global_struct_tag_map, tag);
			if (!ptr_info) {
				fprintf(stderr,
				        "tried to find the alignment of incomplete type "
				        "`struct %s`\n",
				        type.struct_tag);
				exit(EXIT_FAILURE);
			}
			return ptr_info->s_and_a.alignment;
		case VOID_:
			fprintf(stderr, "cannot get alignment of `void`\n");
			exit(EXIT_FAILURE);
		case ENUM_:
			return 4;
	}
}

static void record_global_struct_declaration(struct AnalyzerState *ptr_ps,
                                             struct Type struct_type)
{
	assert(struct_type.type_category == STRUCT_);
	struct StructInternalInfo info = struct_type.struct_info;
	if (!info.ptr_types_and_idents) { /* null; incomplete type */
		return;
	}
	struct Vector /* <TypeAndIdent> */ types_and_idents =
	    *info.ptr_types_and_idents;

	struct SizeAndAlignment *inner_type_vec =
	    calloc(types_and_idents.length, sizeof(struct SizeAndAlignment));

	int i = 0;
	for (; i < types_and_idents.length; i++) {
		const struct TypeAndIdent *ptr_vec_i = types_and_idents.vector[i];
		inner_type_vec[i].size = size_of(ptr_ps, ptr_vec_i->type);
		inner_type_vec[i].alignment = align_of(ptr_ps, ptr_vec_i->type);
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

static void record_global_enum_declaration(struct AnalyzerState *ptr_ps,
                                           struct Type type)
{
	assert(type.type_category == ENUM_);

	struct Enumerators info = type.enum_info;
	if (!info.ptr_enumerators) { /* null; cannot record */
		return;
	}
	struct Vector /* <const char> */ idents = *info.ptr_enumerators;

	struct Vector /*<EnumeratorAndValue>*/ *ptr_e_and_v_vec = init_vector_();
	for (int i = 0; i < idents.length; i++) {
		const char *vec_i = idents.vector[i];
		struct EnumeratorAndValue *ptr_e_and_v =
		    calloc(1, sizeof(struct EnumeratorAndValue));
		ptr_e_and_v->ident = vec_i;
		ptr_e_and_v->value = i;
		push_vector(ptr_e_and_v_vec, ptr_e_and_v);
	}

	concat_vector(&ptr_ps->global_enumerator_list, *ptr_e_and_v_vec);

	insert(&ptr_ps->global_enum_tag_map, type.enum_tag, ptr_e_and_v_vec);
}

static void
record_if_global_struct_or_enum_declaration(struct AnalyzerState *ptr_ps,
                                            struct Type type)
{
	switch (type.type_category) {
		case ARRAY:
		case FN:
		case PTR_:
			record_if_global_struct_or_enum_declaration(ptr_ps,
			                                            *type.derived_from);
			return;
		case VOID_:
		case INT_:
		case CHAR_:
			return;
		case STRUCT_:
			record_global_struct_declaration(ptr_ps, type);
			return;
		case ENUM_:
			record_global_enum_declaration(ptr_ps, type);
			return;
	}
}

static struct Toplevel
parse_toplevel_definition(struct AnalyzerState *ptr_ps,
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

		record_if_global_struct_or_enum_declaration(ptr_ps, d.declarator_type);
		d.declarator_name = 0;
		return d;
	}

	const struct Token *tokvec2 = *ptr_tokvec;

	const char *declarator_name;
	struct Type declarator_type = parse_declaration(&tokvec2, &declarator_name);
	record_if_global_struct_or_enum_declaration(ptr_ps, declarator_type);

	if (declarator_type.type_category != FN) {

		expect_and_consume(&tokvec2, SEMICOLON,
		                   "semicolon at the end of variable definition");

		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = declarator_type;
		insert(&ptr_ps->global_vars_type_map, declarator_name, ptr_type);

		*ptr_tokvec = tokvec2;

		struct Toplevel d;
		d.category = TOPLEVEL_VAR_DEFINITION;
		d.declarator_name = declarator_name;
		d.declarator_type = declarator_type;
		d.size_of_declarator_type = size_of(ptr_ps, declarator_type);
		return d;
	}

	struct Vector /*<TypeAndIdent>*/ param_infos = declarator_type.param_infos;
	int is_param_infos_valid = declarator_type.is_param_infos_valid;
	struct Type *derived_from = declarator_type.derived_from;
	struct Type ret_type = *derived_from;

	ptr_ps->scope_chain.outer = 0; /* most outer scope */
	ptr_ps->scope_chain.var_table = init_map();
	ptr_ps->newest_offset = 0;
	ptr_ps->func_ret_type = ret_type;

	struct Type *ptr_func_info = calloc(1, sizeof(struct Type));
	ptr_func_info->derived_from = derived_from;
	insert(&ptr_ps->func_info_map, declarator_name, ptr_func_info);

	if (!is_param_infos_valid &&
	    tokvec2[0].kind == SEMICOLON) { /* function prototype */
		++tokvec2;
		/* do nothing, since the return value is already in the retmap
		 */
		*ptr_tokvec = tokvec2;

		struct Toplevel def;
		def.category = TOPLEVEL_FUNCTION_DECLARATION;
		return def;
	}

	struct Vector /*<LocalVarInfo>*/ offsets_and_types = init_vector();

	if (is_param_infos_valid) { /* parameter is not empty */
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

struct Vector /*<Toplevel>*/ parse(const struct Token *tokvec)
{
	struct AnalyzerState ps;
	ps.func_info_map = init_map();
	ps.global_vars_type_map = init_map();
	ps.global_struct_tag_map = init_map();
	ps.global_enum_tag_map = init_map();
	ps.global_enumerator_list = init_vector();

	struct Vector /*<Toplevel>*/ vec = init_vector();
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
