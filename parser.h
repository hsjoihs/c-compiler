struct Expression parse_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
struct Expression parse_unary_expression(struct ParserState *ptr_ps,
                                         const struct Token **ptr_tokvec);
struct Expression ident_as_lvalue(struct ParserState ps, const char *name);

struct LocalVarInfo {
	struct Type type;
	int offset;
};

struct ScopeChain {
	struct Map var_table;
	struct ScopeChain *outer;
};

struct ParserState {
	struct ScopeChain scope_chain;
	struct Map global_vars_type_map;
	struct Map func_info_map;
	struct Type func_ret_type;
	int newest_offset;
};
