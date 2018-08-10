#include "global_definition.h"
#include "print_x86_64.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int get_new_label_name(struct PrinterState *ptr_prs)
{
	return ++(ptr_prs->final_label_name);
}

static void print_statement(struct PrinterState *ptr_prs, struct Statement sta)
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
				print_statement(ptr_prs, *ptr_ith);
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
			print_statement(ptr_prs, inner_s);

			gen_if_else_part2(label1, label2);

			struct Statement inner_s2 =
			    *(struct Statement *)sta.statement_vector.vector[1];
			print_statement(ptr_prs, inner_s2);
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
			print_statement(ptr_prs, inner_s);

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
			print_statement(ptr_prs, inner_s);

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

			print_statement(ptr_prs, inner_s);

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
			print_statement(ptr_prs, inner_s);
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

void print_toplevel_definition(struct PrinterState *ptr_prs,
                               struct Toplevel def)
{
	if (def.category == TOPLEVEL_VAR_DEFINITION) {
		gen_global_declaration(def.declarator_name,
		                       size_of(def.declarator_type));
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
	print_statement(ptr_prs, sta);

	gen_epilogue_nbyte(size_of(ret_type), ptr_prs->return_label_name);
}

void print_string_pool(struct Vector pool)
{
	for (int i = 0; i < pool.length; ++i) {
		const char *str = pool.vector[i];
		gen_str(i, str);
	}
}
