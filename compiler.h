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
	struct Type func_ret_type;
};

int isAssign(enum TokenKind opkind);
struct LocalVarInfo resolve_name_locally(struct LocalVarTableList t,
                                         const char *str);
int is_local_var(struct LocalVarTableList t, const char *str);
