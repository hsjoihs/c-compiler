struct Expression parse_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
struct Expression parse_unary_expression(struct ParserState *ptr_ps,
                                         const struct Token **ptr_tokvec);
struct Expression ident_as_lvalue(struct ParserState ps, const char *name);

struct LocalVarInfo {
	struct Type type;
	int offset;
};

struct LocalVarTableList {
	struct map var_table;
	struct LocalVarTableList *outer;
};

struct ParserState {
	struct LocalVarTableList scope_chain;
	struct map global_vars_type_map;
	struct map func_info_map;
	int newest_offset;
	int final_label_name;
	int return_label_name; /* the label at the end of the function */
	struct Type func_ret_type;
	int break_label_name;    /* the label at the end of the current loop */
	int continue_label_name; /* the label at the beginning of the current loop
	                          */
};

int isAssign(enum TokenKind opkind);
struct Type resolve_name_globally(struct map m, const char *str);
struct LocalVarInfo resolve_name_locally(struct LocalVarTableList t,
                                         const char *str);
int is_local_var(struct LocalVarTableList t, const char *str);
