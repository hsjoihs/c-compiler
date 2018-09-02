#include "print_x86_64.h"
#include "std.h"
#include "std_io.h"
#include "toplevel.h"

static void print_toplevel_definition(struct PrinterState *ptr_prs,
                                      const struct Toplevel *ptr_def);

int get_new_label_name(struct PrinterState *ptr_prs)
{
	return ++(ptr_prs->final_label_name);
}

static struct Vector /*<SourceLabel>*/
collect_labels(const struct Statement *ptr_sta)
{
	struct Statement sta = *ptr_sta;
	struct Vector /*<SourceLabel>*/ ans = init_vector();
	concat_vector(&ans, &sta.labels);
	switch (sta.category) {
		case RETURN_STATEMENT:
		case BREAK_STATEMENT:
		case CONTINUE_STATEMENT:
		case EXPRESSION_STATEMENT:
		case DECLARATION_STATEMENT:
			/* nothing */
			break;
		case SWITCH_STATEMENT:
			/* do nothing; don't peek labels */
			break;

		case IF_STATEMENT:
		case FOR_STATEMENT:
		case WHILE_STATEMENT:
		case DO_WHILE_STATEMENT: {
			const struct Vector /*<SourceLabel>*/ inner_vec =
			    collect_labels(sta.inner_statement);
			concat_vector(&ans, &inner_vec);
			break;
		}

		case COMPOUND_STATEMENT:
		case IF_ELSE_STATEMENT: {
			struct Vector /*<Statement>*/ statement_vec = sta.statement_vector;

			for (int counter = 0; counter != statement_vec.length; ++counter) {
				const struct Statement *ptr_ith = statement_vec.vector[counter];
				const struct Vector /*<SourceLabel>*/ inner_vec =
				    collect_labels(ptr_ith);
				concat_vector(&ans, &inner_vec);
			}
			break;
		}
	}
	return ans;
}

struct SourceLabelAndAssemblyLabel {
	int assembly_label;
	struct SourceLabel source_label;
};

static void print_statement(struct PrinterState *ptr_prs,
                            const struct Statement *ptr_sta)
{
	const struct Statement sta = *ptr_sta;
	if (sta.category != DECLARATION_STATEMENT) {
		for (int j = 0; j < sta.labels.length; j++) {
			const struct SourceLabel *ptr_label = sta.labels.vector[j];
			if (ptr_label->category == DEFAULT_LABEL) {
				if (!ptr_prs->is_inside_switch) {
					simple_error("`default` was detected, but is not inside "
					             "`switch`.\n");
				}
				for (int k = 0; k < ptr_prs->case_default_vec.length; k++) {
					const struct SourceLabelAndAssemblyLabel *ptr_ll =
					    ptr_prs->case_default_vec.vector[k];
					if (ptr_ll->source_label.category == DEFAULT_LABEL) {
						gen_label(ptr_ll->assembly_label);
					}
				}
			} else if (ptr_label->category == CASE_LABEL) {
				if (!ptr_prs->is_inside_switch) {
					simple_error(
					    "`case` was detected, but is not inside `switch`.\n");
				}
				for (int k = 0; k < ptr_prs->case_default_vec.length; k++) {
					const struct SourceLabelAndAssemblyLabel *ptr_ll =
					    ptr_prs->case_default_vec.vector[k];
					if (ptr_ll->source_label.category == CASE_LABEL &&
					    ptr_label->case_int == ptr_ll->source_label.case_int) {
						gen_label(ptr_ll->assembly_label);
					}
				}
			}
		}
	}
	switch (sta.category) {
		case DECLARATION_STATEMENT: {
			/* do nothing */
			return;
		}
		case BREAK_STATEMENT: {
			if (ptr_prs->break_label_name == -1) {
				simple_error("invalid `break`; no loop, no switch\n");

			} else {
				gen_jump(ptr_prs->break_label_name, "break");
			}
			return;
		}
		case EXPRESSION_STATEMENT: {
			print_expression(ptr_prs, &sta.expr1);
			gen_discard();
			return;
		}
		case RETURN_STATEMENT: {
			if (sta.expr1.details.type.type_category == STRUCT_) {
				print_address_of_lvalue(ptr_prs, &sta.expr1,
				                        "returning a struct");
			} else {
				print_expression(ptr_prs, &sta.expr1);
			}

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
				simple_error("invalid `continue`; no loop\n");

			} else {
				gen_jump(ptr_prs->continue_label_name, "continue");
			}
			return;
		}
		case COMPOUND_STATEMENT: {

			struct Vector /*<Statement>*/ vec = sta.statement_vector;

			for (int counter = 0; counter != vec.length; ++counter) {
				const struct Statement *ptr_ith = vec.vector[counter];
				print_statement(ptr_prs, ptr_ith);
			}

			return;
		}
		case IF_ELSE_STATEMENT: {
			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			const struct Expr expr = sta.expr1;
			print_expression(ptr_prs, &expr);

			gen_if_zero_jmp_nbyte(
			    size_of_basic(&expr.details.type, "condition of `if`"), label1,
			    0);
			gen_discard();

			const struct Statement *ptr_inner_s =
			    sta.statement_vector.vector[0];
			print_statement(ptr_prs, ptr_inner_s);

			gen_jump(label2, "if statement");
			gen_label(label1);
			gen_discard();

			const struct Statement *ptr_inner_s2 =
			    sta.statement_vector.vector[1];
			print_statement(ptr_prs, ptr_inner_s2);
			gen_label(label2);

			return;
		}
		case SWITCH_STATEMENT: {
			int stashed_break_label = ptr_prs->break_label_name;
			int stashed_is_inside_switch = ptr_prs->is_inside_switch;
			struct Vector /*<Label>*/ stashed_case_default_vec =
			    ptr_prs->case_default_vec;

			int break_label = get_new_label_name(ptr_prs);
			ptr_prs->break_label_name = break_label;
			ptr_prs->is_inside_switch = 1;
			struct Vector /*<SourceLabel>*/ vec =
			    collect_labels(sta.inner_statement);

			int default_label = -1;
			ptr_prs->case_default_vec = init_vector();
			for (int i = 0; i < vec.length; i++) {
				const struct SourceLabel *ptr_vec_i = vec.vector[i];
				if (ptr_vec_i->category == IDENT_LABEL) {
					continue;
				}

				int label = get_new_label_name(ptr_prs);
				if (ptr_vec_i->category == DEFAULT_LABEL) {
					if (default_label == -1) {
						default_label = label;
					} else {
						simple_error("duplicate `default` detected.\n");
					}
				}
				struct SourceLabelAndAssemblyLabel ll;
				ll.assembly_label = label;
				ll.source_label = *ptr_vec_i;

				struct SourceLabelAndAssemblyLabel *ptr_ll =
				    calloc(1, sizeof(struct SourceLabelAndAssemblyLabel));
				*ptr_ll = ll;
				push_vector(&ptr_prs->case_default_vec, ptr_ll);
			}

			const struct Expr expr = sta.expr1;
			print_expression(ptr_prs, &expr);
			if (size_of_basic(&expr.details.type, "expression of switch") ==
			    1) {
				gen_extend_to_4byte();
			}

			if (default_label == -1) { /* default_label is not found */
				default_label = break_label;
			}
			gen_discard();

			for (int i = 0; i < ptr_prs->case_default_vec.length;
			     i++) { /* for each case */
				const struct SourceLabelAndAssemblyLabel *ptr_ll =
				    ptr_prs->case_default_vec.vector[i];
				if (ptr_ll->source_label.category != CASE_LABEL) {
					continue;
				}
				int constant1 = ptr_ll->source_label.case_int;
				int label1 = ptr_ll->assembly_label;
				gen_if_neg8_matches_jmp_4byte(constant1, label1);
			}
			gen_jump(default_label, "switch-default");

			print_statement(ptr_prs, sta.inner_statement);
			gen_label(break_label);

			ptr_prs->break_label_name = stashed_break_label;
			ptr_prs->is_inside_switch = stashed_is_inside_switch;
			ptr_prs->case_default_vec = stashed_case_default_vec;
			return;
		}
		case IF_STATEMENT: {

			int label1 = get_new_label_name(ptr_prs);
			int label2 = get_new_label_name(ptr_prs);
			const struct Expr expr = sta.expr1;
			print_expression(ptr_prs, &expr);

			gen_if_zero_jmp_nbyte(
			    size_of_basic(&expr.details.type, "condition of `if`"), label1,
			    0);
			gen_discard();

			print_statement(ptr_prs, sta.inner_statement);

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
			print_statement(ptr_prs, sta.inner_statement);

			gen_label(cont_label);

			const struct Expr expr = sta.expr1;
			print_expression(ptr_prs, &expr);

			gen_discard();
			gen_if_nonzero_jmp_nbyte(
			    size_of_basic(&expr.details.type, "condition of do-while"),
			    label1, -8);
			gen_label(break_label);

			ptr_prs->break_label_name = stashed_break_label;
			ptr_prs->continue_label_name = stashed_continue_label;

			return;
		}

		case WHILE_STATEMENT: {
			const struct Expr expr = sta.expr1;

			int stashed_break_label = ptr_prs->break_label_name;
			int stashed_continue_label = ptr_prs->continue_label_name;

			int label1 = get_new_label_name(ptr_prs);
			int break_label = get_new_label_name(ptr_prs);
			int cont_label = get_new_label_name(ptr_prs);
			ptr_prs->break_label_name = break_label;
			ptr_prs->continue_label_name = cont_label;

			gen_label(label1);

			print_expression(ptr_prs, &expr);

			gen_discard();
			gen_if_zero_jmp_nbyte(
			    size_of_basic(&expr.details.type, "condition of `while`"),
			    break_label, -8);

			print_statement(ptr_prs, sta.inner_statement);

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

			print_expression(ptr_prs, &sta.expr1); /* expression1 */
			gen_discard();
			gen_label(label1);
			print_expression(ptr_prs, &sta.expr2); /* expression2 */

			gen_discard();
			gen_if_zero_jmp_nbyte(
			    size_of_basic(&sta.expr2.details.type, "condition of `for`"),
			    break_label, -8);

			print_statement(ptr_prs, sta.inner_statement);
			gen_label(cont_label);
			print_expression(ptr_prs, &sta.expr3);
			gen_discard();
			gen_jump(label1, "for(part4)");
			gen_label(break_label);

			ptr_prs->break_label_name = stashed_break_label;
			ptr_prs->continue_label_name = stashed_continue_label;

			return;
		}
	}

	assert0("nljsdgfs" && 0);
}

static void print_toplevel_definition(struct PrinterState *ptr_prs,
                                      const struct Toplevel *ptr_def)
{
	struct Toplevel def = *ptr_def;
	if (def.category == TOPLEVEL_VAR_DEFINITION) {
		if (def.declarator_name && !def.is_extern_global_var) {
			gen_global_declaration(def.declarator_name,
			                       def.size_of_declarator_type);
		}
		return;
	}
	if (def.category == TOPLEVEL_FUNCTION_DECLARATION) {
		return;
	}

	assert(def.category == TOPLEVEL_FUNCTION_DEFINITION);

	const struct Statement sta = def.func.sta;
	struct Vector /*<LocalVarInfo>*/ offsets_and_types =
	    def.func.offsets_and_types;
	struct Type ret_type = def.func.ret_type;
	const char *declarator_name = def.declarator_name;

	ptr_prs->return_label_name = -1;   /* -1 means invalid */
	ptr_prs->break_label_name = -1;    /* -1 means invalid */
	ptr_prs->continue_label_name = -1; /* -1 means invalid */
	int label1;
	int label2;
	label1 = get_new_label_name(ptr_prs);
	label2 = get_new_label_name(ptr_prs);
	if (def.func.is_static_function) {
		gen_prologue_static(def.func.capacity, declarator_name);
	} else {
		gen_prologue(def.func.capacity, declarator_name);
	}
	for (int counter = 0; counter < offsets_and_types.length; ++counter) {
		const struct LocalVarInfo *ptr_info = offsets_and_types.vector[counter];

		int offset = ptr_info->offset;
		const struct Type type = ptr_info->type;
		switch (size_of_basic(&type, "argument to be passed to function")) {
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
	print_statement(ptr_prs, &sta);

	if (ret_type.type_category == VOID_) {
		if (ptr_prs->return_label_name != -1) {
			gen_label(ptr_prs->return_label_name);
		}
		printf("  movl $123, %%eax\nleave\nret\n");
	} else if (ret_type.type_category == STRUCT_) {
		enum SystemVAbiClass abi_class = def.func.abi_class;
		int ret_struct_size = def.func.ret_struct_size;
		if (abi_class == INTEGER_CLASS) {
			if (ptr_prs->return_label_name == -1) {
				simple_error("warning: the return type is not void, but "
				             "`return` is not found");
			}

			gen_epilogue_returning_small_struct(ret_struct_size,
			                                    ptr_prs->return_label_name);

		} else {
			unsupported("returning a MEMORY_CLASS struct");
		}
	} else {
		if (ptr_prs->return_label_name == -1) {
			simple_error("warning: the return type is not void, but "
			             "`return` is not found");
		}
		gen_epilogue_nbyte(size_of_basic(&ret_type, "return value"),
		                   ptr_prs->return_label_name);
	}
}

void generate(const struct Vector /*<Toplevel>*/ *ref_vec)
{
	struct PrinterState prs;
	prs.final_label_name = 1;
	prs.return_label_name = -1;
	prs.string_constant_pool = init_vector();
	prs.pool_largest_id = 0;
	prs.is_inside_switch = 0;

	for (int i = 0; i < ref_vec->length; i++) {
		const struct Toplevel *ptr = ref_vec->vector[i];

		print_toplevel_definition(&prs, ptr);
	}

	for (int i = 0; i < prs.string_constant_pool.length; ++i) {
		const char *str = prs.string_constant_pool.vector[i];
		gen_str(i, escape(str));
	}
}
