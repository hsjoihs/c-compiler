#include "parse_statement.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const char *get_reg_name_from_arg_pos_4byte(int counter)
{
	switch (counter) {
		case 0:
			return "edi";
		case 1:
			return "esi";
		case 2:
			return "edx";
		case 3:
			return "ecx";
		case 4:
			return "r8d";
		case 5:
			return "r9d";
		default:
			assert("cannot happen" && 0);
	}
}

const char *get_reg_name_from_arg_pos_8byte(int counter)
{
	switch (counter) {
		case 0:
			return "rdi";
		case 1:
			return "rsi";
		case 2:
			return "rdx";
		case 3:
			return "rcx";
		case 4:
			return "r8";
		case 5:
			return "r9";
		default:
			assert("cannot happen" && 0);
	}
}

int get_new_label_name(struct PrinterState *ptr_prs)
{
	return ++(ptr_prs->final_label_name);
}

static void print_statement(struct ParserState *ptr_ps,
                            struct PrinterState *ptr_prs, struct Statement sta)
{
	switch (sta.category) {
		case DECLARATION_STATEMENT: {
			/* do nothing */
			return;
		}
		case BREAK_STATEMENT: {
			if (ptr_prs->break_label_name == -1) {
				fprintf(stderr, "invalid `break`; no loop, no switch\n");
				exit(EXIT_FAILURE);
			} else {
				gen_jump(ptr_prs->break_label_name, "break");
			}
			return;
		}
		case EXPRESSION_STATEMENT: {
			print_expression(ptr_prs, sta.expr1);
			gen_discard();
			return;
		}
		case RETURN_STATEMENT: {

			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);
			/* the first occurrence of return within a function */
			if (ptr_prs->return_label_name == -1) {
				int ret_label = get_new_label_name(ptr_prs);
				ptr_prs->return_label_name = ret_label;
				gen_jump(ret_label, "return");
			} else {
				gen_jump(ptr_prs->return_label_name, "return");
			}

			struct Statement s;
			s.category = RETURN_STATEMENT;
			s.expr1 = expr;
			return;
		}
		case CONTINUE_STATEMENT: {
			if (ptr_prs->continue_label_name == -1) {
				fprintf(stderr, "invalid `continue`; no loop\n");
				exit(EXIT_FAILURE);
			} else {
				gen_jump(ptr_prs->continue_label_name, "continue");
			}
			return;
		}
		case COMPOUND_STATEMENT: {

			struct Vector vec = sta.statement_vector;

			for (int counter = 0; counter != vec.length; ++counter) {
				const struct Statement *ptr_ith = vec.vector[counter];
				print_statement(ptr_ps, ptr_prs, *ptr_ith);
			}

			return;
		}
		case IF_ELSE_STATEMENT: {
			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);

			gen_if_else_part1(label1, label2);

			struct Statement inner_s =
			    *(struct Statement *)sta.statement_vector.vector[0];
			print_statement(ptr_ps, ptr_prs, inner_s);

			gen_if_else_part2(label1, label2);

			struct Statement inner_s2 =
			    *(struct Statement *)sta.statement_vector.vector[1];
			print_statement(ptr_ps, ptr_prs, inner_s2);
			gen_if_else_part3(label1, label2);

			return;
		}
		case IF_STATEMENT: {

			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);

			gen_if_else_part1(label1, label2);

			struct Statement inner_s = *sta.inner_statement;
			print_statement(ptr_ps, ptr_prs, inner_s);

			gen_if_else_part2(label1, label2);
			gen_if_else_part3(label1, label2);

			return;
		}
		case DO_WHILE_STATEMENT: {
			int stashed_break_label = ptr_prs->break_label_name;
			int stashed_continue_label = ptr_prs->continue_label_name;
			int label1 = get_new_label_name(ptr_prs);
			int break_label = get_new_label_name(ptr_prs);
			int cont_label = get_new_label_name(ptr_prs);
			ptr_prs->break_label_name = break_label;
			ptr_prs->continue_label_name = cont_label;

			gen_label(label1);
			struct Statement inner_s = *sta.inner_statement;
			print_statement(ptr_ps, ptr_prs, inner_s);

			gen_label(cont_label);

			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);

			gen_do_while_final(label1, break_label);

			ptr_prs->break_label_name = stashed_break_label;
			ptr_prs->continue_label_name = stashed_continue_label;

			return;
		}
		case WHILE_STATEMENT: {
			struct Expression expr = sta.expr1;

			struct Statement inner_s = *sta.inner_statement;

			int stashed_break_label = ptr_prs->break_label_name;
			int stashed_continue_label = ptr_prs->continue_label_name;

			int label1 = get_new_label_name(ptr_prs);
			int break_label = get_new_label_name(ptr_prs);
			int cont_label = get_new_label_name(ptr_prs);
			ptr_prs->break_label_name = break_label;
			ptr_prs->continue_label_name = cont_label;

			gen_label(label1);

			print_expression(ptr_prs, expr);

			gen_while_part2(label1, break_label);

			print_statement(ptr_ps, ptr_prs, inner_s);

			gen_label(cont_label);
			gen_for_part4(label1, break_label);

			ptr_prs->break_label_name = stashed_break_label;
			ptr_prs->continue_label_name = stashed_continue_label;

			return;
		}
		case FOR_STATEMENT: {
			int stashed_break_label = ptr_prs->break_label_name;
			int stashed_continue_label = ptr_prs->continue_label_name;
			int label1 = get_new_label_name(ptr_prs);
			int break_label = get_new_label_name(ptr_prs);
			int cont_label = get_new_label_name(ptr_prs);
			ptr_prs->break_label_name = break_label;
			ptr_prs->continue_label_name = cont_label;

			struct Expression expr1 = sta.expr1;
			struct Expression expr2 = sta.expr2;
			struct Expression expr3 = sta.expr3;

			print_expression(ptr_prs, expr1); /* expression1 */
			gen_discard();
			gen_label(label1);
			print_expression(ptr_prs, expr2); /* expression2 */
			gen_while_part2(label1, break_label);
			struct Statement inner_s = *sta.inner_statement;
			print_statement(ptr_ps, ptr_prs, inner_s);
			gen_label(cont_label);
			print_expression(ptr_prs, expr3);
			gen_discard();
			gen_for_part4(label1, break_label);

			ptr_prs->break_label_name = stashed_break_label;
			ptr_prs->continue_label_name = stashed_continue_label;

			return;
		}
	}
	fprintf(stderr, "fooooooooooo\n");
	exit(EXIT_FAILURE);
}

void parse_final(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	expect_and_consume(&tokvec, END, "the end of file");
	return;
}

enum DefinitionCategory {
	TOPLEVEL_VAR_DEFINITION,
	TOPLEVEL_FUNCTION_DEFINITION,
	TOPLEVEL_FUNCTION_DECLARATION
};

struct Definition {
	enum DefinitionCategory category;
	const char *declarator_name;
	struct Type declarator_type;
	struct {
		struct Statement sta;
		struct Vector offsets_and_types;
		struct Type ret_type;
	} func;
};

static struct Definition *
try_parse_toplevel_var_definition(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec)
{
	const char *declarator_name;
	const struct Token *tokvec2 = *ptr_tokvec;
	struct Type declarator_type = parse_declarator(&tokvec2, &declarator_name);
	if (declarator_type.type_category == FN) {
		return 0; /* does not consume token on failure */
	} else {
		expect_and_consume(&tokvec2, SEMICOLON,
		                   "semicolon at the end of variable definition");
		struct Map globalmap = ptr_ps->global_vars_type_map;

		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = declarator_type;
		insert(&globalmap, declarator_name, ptr_type);
		ptr_ps->global_vars_type_map = globalmap;

		*ptr_tokvec = tokvec2;

		struct Definition d;
		d.category = TOPLEVEL_VAR_DEFINITION;
		d.declarator_name = declarator_name;
		d.declarator_type = declarator_type;

		struct Definition *ptr = calloc(1, sizeof(struct Definition));
		*ptr = d;
		return ptr;
	}
}

void print_toplevel_definition(struct ParserState *ptr_ps,
                               struct PrinterState *ptr_prs,
                               struct Definition def);

struct Definition parse_toplevel_definition(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec);

void parseprint_toplevel_definition(struct ParserState *ptr_ps,
                                    struct PrinterState *ptr_prs,
                                    const struct Token **ptr_tokvec)
{
	struct Definition def = parse_toplevel_definition(ptr_ps, ptr_tokvec);
	if (def.category == TOPLEVEL_VAR_DEFINITION) {
		gen_global_declaration(def.declarator_name,
		                       size_of(def.declarator_type));
		return;
	} else if (def.category == TOPLEVEL_FUNCTION_DEFINITION) {
		print_toplevel_definition(ptr_ps, ptr_prs, def);
	}
}

/* implicitly assumes that it is a function */
struct Definition parse_toplevel_definition(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec)
{
	struct Definition *ptr_d =
	    try_parse_toplevel_var_definition(ptr_ps, ptr_tokvec);
	if (ptr_d) {
		return *ptr_d;
	}
	const char *declarator_name;
	const struct Token *tokvec2 = *ptr_tokvec;
	struct Type declarator_type = parse_declarator(&tokvec2, &declarator_name);

	struct ParamInfos param_infos = declarator_type.param_infos;
	struct Type ret_type = *declarator_type.derived_from;

	ptr_ps->scope_chain.outer = 0; /* most outer scope */
	ptr_ps->scope_chain.var_table = init_map();
	ptr_ps->newest_offset = 0;
	ptr_ps->func_ret_type = ret_type;

	struct FuncInfo *ptr_func_info = calloc(1, sizeof(struct FuncInfo));
	ptr_func_info->ret_type = ret_type;
	insert(&ptr_ps->func_info_map, declarator_name, ptr_func_info);

	if (!param_infos.param_vec &&
	    tokvec2[0].kind == SEMICOLON) { /* function prototype */
		++tokvec2;
		/* do nothing, since the return value is already in the retmap
		 */
		*ptr_tokvec = tokvec2;

		struct Definition def;
		def.category = TOPLEVEL_FUNCTION_DECLARATION;
		return def;
	}

	struct Vector offsets_and_types = init_vector();

	if (param_infos.param_vec) { /* parameter is not empty */
		for (int counter = 0; param_infos.param_vec[counter]; ++counter) {

			struct ParamInfo param_info = *(param_infos.param_vec[counter]);
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

	struct Definition def;
	def.category = TOPLEVEL_FUNCTION_DEFINITION;
	def.declarator_name = declarator_name;
	def.func.sta = sta;
	def.func.offsets_and_types = offsets_and_types;
	def.func.ret_type = ret_type;
	return def;
}

void print_toplevel_definition(struct ParserState *ptr_ps,
                               struct PrinterState *ptr_prs,
                               struct Definition def)
{
	struct Statement sta = def.func.sta;
	struct Vector offsets_and_types = def.func.offsets_and_types;
	struct Type ret_type = def.func.ret_type;
	const char *declarator_name = def.declarator_name;

	ptr_prs->return_label_name = -1;   /* -1 means invalid */
	ptr_prs->break_label_name = -1;    /* -1 means invalid */
	ptr_prs->continue_label_name = -1; /* -1 means invalid */
	int label1;
	int label2;
	label1 = get_new_label_name(ptr_prs);
	label2 = get_new_label_name(ptr_prs);
	gen_prologue(0, declarator_name);
	gen_after_prologue(label1, label2);
	for (int counter = 0; counter < offsets_and_types.length; ++counter) {
		struct LocalVarInfo info =
		    *(const struct LocalVarInfo *)offsets_and_types.vector[counter];

		int offset = info.offset;
		struct Type type = info.type;
		switch (size_of(type)) {
			case 1:
				gen_write_register_to_local_1byte(
				    /* yes, the register is 4byte */
				    get_reg_name_from_arg_pos_4byte(counter), offset);
			case 4:
				gen_write_register_to_local_4byte(
				    get_reg_name_from_arg_pos_4byte(counter), offset);
				break;
			case 8:
				gen_write_register_to_local_8byte(
				    get_reg_name_from_arg_pos_8byte(counter), offset);
				break;
			default:
				unsupported("Unsupported width in function parameter");
		}
	}
	print_statement(ptr_ps, ptr_prs, sta);

	gen_before_epilogue(label1, label2, -(ptr_ps->newest_offset));
	gen_epilogue_nbyte(size_of(ret_type), ptr_prs->return_label_name);
}

void print_string_pool(struct Vector pool)
{
	for (int i = 0; i < pool.length; ++i) {
		const char *str = pool.vector[i];
		gen_str(i, str);
	}
}
