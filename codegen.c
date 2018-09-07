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

static int is_label_compatible(const struct SourceLabel *ptr_label1,
                               const struct SourceLabel *ptr_label2)
{
	if (ptr_label1->category == DEFAULT_LABEL &&
	    ptr_label2->category == DEFAULT_LABEL) {
		return 1;
	} else if (ptr_label1->category == CASE_LABEL &&
	           ptr_label2->category == CASE_LABEL &&
	           ptr_label1->case_int == ptr_label2->case_int) {
		return 1;
	}
	return 0;
}

void print_expression_or_addr_of_struct(struct PrinterState *ptr_prs,
                                        const struct Expr *ref_expr,
                                        const char *msg)
{
	if (ref_expr->details.type.type_category == STRUCT_) {
		print_address_of_lvalue_or_struct(ptr_prs, ref_expr, msg);
	} else {
		print_expression(ptr_prs, ref_expr);
	}
}

void print_statement(struct PrinterState *ptr_prs,
                     const struct Statement *ref_sta)
{
	if (ref_sta->category != DECLARATION_STATEMENT) {
		for (int j = 0; j < ref_sta->labels.length; j++) {
			const struct SourceLabel *ptr_label = ref_sta->labels.vector[j];

			if (ptr_label->category == IDENT_LABEL) {
				continue;
			}

			if (!ptr_prs->is_inside_switch) {
				simple_error(
				    "`default` or `case` was detected, but is not inside "
				    "`switch`.\n");
			}
			for (int k = 0; k < ptr_prs->case_default_vec.length; k++) {
				const struct SourceLabelAndAssemblyLabel *ptr_ll =
				    ptr_prs->case_default_vec.vector[k];
				if (is_label_compatible(&ptr_ll->source_label, ptr_label)) {
					gen_label(ptr_ll->assembly_label);
				}
			}
		}
	}
	switch (ref_sta->category) {
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
		print_expression(ptr_prs, &ref_sta->expr1);
		gen_discard();
		return;
	}
	case RETURN_STATEMENT: {

		print_expression_or_addr_of_struct(ptr_prs, &ref_sta->expr1,
		                                   "returning a struct");

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
		struct Vector /*<Statement>*/ vec = ref_sta->statement_vector;

		for (int counter = 0; counter != vec.length; ++counter) {
			const struct Statement *ptr_ith = vec.vector[counter];
			print_statement(ptr_prs, ptr_ith);
		}
		return;
	}
	case IF_ELSE_STATEMENT: {
		int label1 = get_new_label_name(ptr_prs);
		int label2 = get_new_label_name(ptr_prs);
		const struct Expr expr = ref_sta->expr1;
		print_expression(ptr_prs, &expr);

		gen_if_zero_jmp_nbyte(
		    size_of_basic(&expr.details.type, "condition of `if`"), label1, 0);
		gen_discard();

		const struct Statement *ptr_inner_s =
		    ref_sta->statement_vector.vector[0];
		print_statement(ptr_prs, ptr_inner_s);

		gen_jump(label2, "if statement");
		gen_label(label1);
		gen_discard();

		const struct Statement *ptr_inner_s2 =
		    ref_sta->statement_vector.vector[1];
		print_statement(ptr_prs, ptr_inner_s2);
		gen_label(label2);

		return;
	}
	case SWITCH_STATEMENT: {
		codegen_switch(ptr_prs, ref_sta);
		return;
	}
	case IF_STATEMENT: {

		int label1 = get_new_label_name(ptr_prs);
		int label2 = get_new_label_name(ptr_prs);
		const struct Expr expr = ref_sta->expr1;
		print_expression(ptr_prs, &expr);

		gen_if_zero_jmp_nbyte(
		    size_of_basic(&expr.details.type, "condition of `if`"), label1, 0);
		gen_discard();

		print_statement(ptr_prs, ref_sta->inner_statement);

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
		print_statement(ptr_prs, ref_sta->inner_statement);

		gen_label(cont_label);

		const struct Expr expr = ref_sta->expr1;
		print_expression(ptr_prs, &expr);

		gen_discard();
		gen_if_nonzero_jmp_nbyte(
		    size_of_basic(&expr.details.type, "condition of do-while"), label1,
		    -8);
		gen_label(break_label);

		ptr_prs->break_label_name = stashed_break_label;
		ptr_prs->continue_label_name = stashed_continue_label;

		return;
	}

	case WHILE_STATEMENT: {
		const struct Expr expr = ref_sta->expr1;

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

		print_statement(ptr_prs, ref_sta->inner_statement);

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

		print_expression(ptr_prs, &ref_sta->expr1); /* expression1 */
		gen_discard();
		gen_label(label1);
		print_expression(ptr_prs, &ref_sta->expr2); /* expression2 */

		gen_discard();
		gen_if_zero_jmp_nbyte(
		    size_of_basic(&ref_sta->expr2.details.type, "condition of `for`"),
		    break_label, -8);

		print_statement(ptr_prs, ref_sta->inner_statement);
		gen_label(cont_label);
		print_expression(ptr_prs, &ref_sta->expr3);
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
                                      const struct Toplevel *ref_def)
{
	if (ref_def->category == TOPLEVEL_VAR_DEFINITION) {
		if (ref_def->declarator_name && !ref_def->is_extern_global_var) {
			gen_global_declaration(ref_def->declarator_name,
			                       ref_def->size_of_declarator_type);
		}
		return;
	}
	if (ref_def->category == TOPLEVEL_FUNCTION_DECLARATION) {
		return;
	}

	assert(ref_def->category == TOPLEVEL_FUNCTION_DEFINITION);

	const struct Statement sta = ref_def->func.sta;
	struct Vector /*<LocalVarInfo>*/ offsets_and_types =
	    ref_def->func.offsets_and_types;
	struct Type ret_type = ref_def->func.ret_type;
	const char *declarator_name = ref_def->declarator_name;

	ptr_prs->return_label_name = -1;   /* -1 means invalid */
	ptr_prs->break_label_name = -1;    /* -1 means invalid */
	ptr_prs->continue_label_name = -1; /* -1 means invalid */
	int label1;
	int label2;
	label1 = get_new_label_name(ptr_prs);
	label2 = get_new_label_name(ptr_prs);
	if (ref_def->func.is_static_function) {
		gen_prologue_static(ref_def->func.capacity, declarator_name);
	} else {
		gen_prologue(ref_def->func.capacity, declarator_name);
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
		return;
	}

	if (ptr_prs->return_label_name == -1) {
		simple_error("warning: the return type is not void, but "
		             "`return` is not found");
	}
	if (ret_type.type_category == STRUCT_) {
		enum SystemVAbiClass abi_class = ref_def->func.abi_class;
		int ret_struct_size = ref_def->func.ret_struct_size;

		if (abi_class == INTEGER_CLASS) {
			gen_epilogue_returning_small_struct(ret_struct_size,
			                                    ptr_prs->return_label_name);
		} else {
			gen_label(ptr_prs->return_label_name);
			gen_push_from_local_nbyte(8, ref_def->func.hidden_var_offset);
			gen_swap();
			gen_copy_struct_and_discard(ret_struct_size);
			printf("  movl $123, %%eax\nleave\nret\n");
		}
	} else {
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
