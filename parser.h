#include "header.h"

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

struct Expression parse_typecheck_expression(const struct ParserState *ptr_ps,
                                             const struct Token **ptr_tokvec);

struct Statement parse_statement(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
struct Statement parse_compound_statement(struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec);

/*
 * Adjusts the newest_offset and add a local variable to the scope.
 * Returns the offset of the newly added variable.
 */
int add_local_var_to_scope(struct ParserState *ptr_ps,
                           const struct Type vartype, const char *str);
