#include "global_definition.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void parse_final(const struct Token **ptr_tokvec);

static void parse_final(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	expect_and_consume(&tokvec, END, "the end of file");
	return;
}

static struct Toplevel
parse_toplevel_definition(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	const char *declarator_name;
	const struct Token *tokvec2 = *ptr_tokvec;
	struct Type declarator_type = parse_declarator(&tokvec2, &declarator_name);

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
		return d;
	}

	struct ParamInfos param_infos = declarator_type.param_infos;
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

			struct ParamInfo param_info =
			    *(const struct ParamInfo *)(param_infos.vector[counter]);
			const char *ident_str;

			struct Type type = param_info.param_type;
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

	struct Vector vec = init_vector();
	while (1) {
		if (tokvec[0].kind == END) {
			parse_final(&tokvec);
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
