struct PrinterState {
	int final_label_name;
	int return_label_name;   /* the label at the end of the function */
	int break_label_name;    /* the label at the end of the current loop */
	int continue_label_name; /* the label at the beginning of the current loop
	                          */
	struct Vector /*<const char>*/ string_constant_pool;
	int pool_largest_id;
	int is_inside_switch;
	struct Vector /*<SourceLabelAndAssemblyLabel>*/ case_default_vec;
	struct Map2 /*<SourceLabelAndAssemblyLabel>*/
	    *source_label_to_assembly_label;

	int is_va;

	/* only valid when is_va is set */
	int integral_explicit_arg_num;
	int reg_save_area;
	int stack_chk_offset;
};

int get_new_label_name(struct PrinterState *ptr_prs);
void print_expression(struct PrinterState *ptr_prs,
                      const struct Expr *ref_expr);

void print_address_of_lvalue_or_struct(struct PrinterState *ptr_prs,
                                       const struct Expr *ref_expr,
                                       const char *msg);

void print_expression_or_addr_of_struct(struct PrinterState *ptr_prs,
                                        const struct Expr *ref_expr,
                                        const char *msg);
