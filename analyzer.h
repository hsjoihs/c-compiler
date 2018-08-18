#include "header.h"

struct LocalVarInfo {
	struct Type type;
	int offset;
};

struct ScopeChain {
	struct Map var_table;
	struct ScopeChain *outer;
};

struct AnalyzerState {
	struct ScopeChain scope_chain;
	struct Map global_vars_type_map;
	struct Map func_info_map;
	struct Type func_ret_type;
	int newest_offset;
	struct Map /*<StructInternalCompleteInfo>*/ global_struct_tag_map;
};

struct Expression typecheck_expression(const struct AnalyzerState *ptr_ps,
                                       struct UntypedExpression uexpr);

struct Statement parse_statement(struct AnalyzerState *ptr_ps,
                                 const struct Token **ptr_tokvec);
struct Statement parse_compound_statement(struct AnalyzerState *ptr_ps,
                                          const struct Token **ptr_tokvec);

/*
 * Adjusts the newest_offset and add a local variable to the scope.
 * Returns the offset of the newly added variable.
 */
int add_local_var_to_scope(struct AnalyzerState *ptr_ps,
                           const struct Type vartype, const char *str);

int size_of(const struct AnalyzerState *ptr_ps, struct Type type);
int align_of(const struct AnalyzerState *ptr_ps, struct Type type);

void expect_type(const struct AnalyzerState *ptr_ps, struct Type actual_type,
                 struct Type expected_type, const char *message);
