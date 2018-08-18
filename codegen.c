#include "print_x86_64.h"
#include "toplevel.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void print_toplevel_definition(struct PrinterState *ptr_prs,
                                      const struct Toplevel def);

int get_new_label_name(struct PrinterState *ptr_prs)
{
	return ++(ptr_prs->final_label_name);
}

static void print_statement(struct PrinterState *ptr_prs,
                            const struct Statement sta)
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
			print_expression(ptr_prs, sta.expr1);

			/* the first occurrence of return within a function */
			if (ptr_prs->return_label_name == -1) {
				int ret_label = get_new_label_name(ptr_prs);
				ptr_prs->return_label_name = ret_label;
				gen_jump(ret_label, "return");
			} else {
				gen_jump(ptr_prs->return_label_name, "return");
			}

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
				print_statement(ptr_prs, *ptr_ith);
			}

			return;
		}
		case IF_ELSE_STATEMENT: {
			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);

			gen_if_zero_jmp_nbyte(size_of_basic(expr.details.type), label1, 0);
			gen_discard();

			struct Statement inner_s =
			    *(const struct Statement *)sta.statement_vector.vector[0];
			print_statement(ptr_prs, inner_s);

			gen_jump(label2, "if statement");
			gen_label(label1);
			gen_discard();

			struct Statement inner_s2 =
			    *(const struct Statement *)sta.statement_vector.vector[1];
			print_statement(ptr_prs, inner_s2);
			gen_label(label2);

			return;
		}
		case SWITCH_STATEMENT: {
			int break_label = get_new_label_name(ptr_prs);
			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);
			int default_label = -1;

			if (!0) { /* default_label is not found */
				default_label = break_label;
			}
			gen_discard();

			for (; 0;) { /* for each case */
				int constant1 = 87950;
				int label1 = 678432;
				gen_if_neg8_matches_jmp_4byte(constant1, label1);
			}
			gen_jump(default_label, "switch-default");

			print_statement(ptr_prs, *sta.inner_statement);
			gen_label(break_label);
			return;
		}
		case IF_STATEMENT: {

			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);

			gen_if_zero_jmp_nbyte(size_of_basic(expr.details.type), label1, 0);
			gen_discard();

			struct Statement inner_s = *sta.inner_statement;
			print_statement(ptr_prs, inner_s);

			gen_jump(label2, "if statement");
			gen_label(label1);
			gen_discard();

			gen_label(label2);

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
			print_statement(ptr_prs, inner_s);

			gen_label(cont_label);

			struct Expression expr = sta.expr1;
			print_expression(ptr_prs, expr);

			gen_discard();
			gen_if_nonzero_jmp_nbyte(size_of_basic(expr.details.type), label1,
			                         -8);
			gen_label(break_label);

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

			gen_discard();
			gen_if_zero_jmp_nbyte(size_of_basic(expr.details.type), break_label,
			                      -8);

			print_statement(ptr_prs, inner_s);

			gen_label(cont_label);
			gen_jump(label1, "for(part4)");
			gen_label(break_label);

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

			print_expression(ptr_prs, sta.expr1); /* expression1 */
			gen_discard();
			gen_label(label1);
			print_expression(ptr_prs, sta.expr2); /* expression2 */

			gen_discard();
			gen_if_zero_jmp_nbyte(size_of_basic(sta.expr2.details.type),
			                      break_label, -8);

			struct Statement inner_s = *sta.inner_statement;
			print_statement(ptr_prs, inner_s);
			gen_label(cont_label);
			print_expression(ptr_prs, sta.expr3);
			gen_discard();
			gen_jump(label1, "for(part4)");
			gen_label(break_label);

			ptr_prs->break_label_name = stashed_break_label;
			ptr_prs->continue_label_name = stashed_continue_label;

			return;
		}
	}
	fprintf(stderr, "fooooooooooo\n");
	exit(EXIT_FAILURE);
}

static void print_toplevel_definition(struct PrinterState *ptr_prs,
                                      const struct Toplevel def)
{
	if (def.category == TOPLEVEL_VAR_DEFINITION) {
		if (def.declarator_name) {
			gen_global_declaration(def.declarator_name,
			                       def.size_of_declarator_type);
		}
		return;
	}
	if (def.category == TOPLEVEL_FUNCTION_DECLARATION) {
		return;
	}

	assert(def.category == TOPLEVEL_FUNCTION_DEFINITION);

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
	gen_prologue(def.func.capacity, declarator_name);
	for (int counter = 0; counter < offsets_and_types.length; ++counter) {
		const struct LocalVarInfo *ptr_info = offsets_and_types.vector[counter];

		int offset = ptr_info->offset;
		struct Type type = ptr_info->type;
		switch (size_of_basic(type)) {
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
	print_statement(ptr_prs, sta);

	if (ret_type.type_category == VOID_) {
		gen_label(ptr_prs->return_label_name);
		printf("  movl $123, %%eax\nleave\nret\n");
	} else {
		gen_epilogue_nbyte(size_of_basic(ret_type), ptr_prs->return_label_name);
	}
}

void generate(const struct Vector vec)
{
	struct PrinterState prs;
	prs.final_label_name = 1;
	prs.return_label_name = -1;
	prs.string_constant_pool = init_vector();
	prs.pool_largest_id = 0;

	for (int i = 0; i < vec.length; i++) {
		const struct Toplevel *ptr = vec.vector[i];

		print_toplevel_definition(&prs, *ptr);
	}

	for (int i = 0; i < prs.string_constant_pool.length; ++i) {
		const char *str = prs.string_constant_pool.vector[i];
		gen_str(i, str);
	}
}
