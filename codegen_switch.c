#include "print_x86_64.h"
#include "std.h"
#include "std_io.h"
#include "toplevel.h"

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

void codegen_switch(struct PrinterState *ptr_prs,
                    const struct Statement *ptr_sta)
{
	const struct Statement sta = *ptr_sta;
	int stashed_break_label = ptr_prs->break_label_name;
	int stashed_is_inside_switch = ptr_prs->is_inside_switch;
	struct Vector /*<Label>*/ stashed_case_default_vec =
	    ptr_prs->case_default_vec;

	int break_label = get_new_label_name(ptr_prs);
	ptr_prs->break_label_name = break_label;
	ptr_prs->is_inside_switch = 1;
	struct Vector /*<SourceLabel>*/ vec = collect_labels(sta.inner_statement);

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
	if (size_of_basic(&expr.details.type, "expression of switch") == 1) {
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
