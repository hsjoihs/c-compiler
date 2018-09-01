#include "header.h"

struct LocalVarInfo {
	struct Type type;
	int offset;
};

struct ScopeChain {
	struct Map2 * /*<LocalVarInfo>*/ var_table;
	struct ScopeChain *outer;
};

struct EnumeratorAndValue {
	const char *ident;
	int value;
};

struct AnalyzerState {
	struct ScopeChain scope_chain;
	struct Map2 * /*<Type>*/ global_vars_type_map;
	struct Map2 * /*<Type>*/ func_info_map;
	struct Type func_ret_type;
	int newest_offset;
	struct Map2 * /*<StructInternalCompleteInfo>*/ global_struct_tag_map;
	struct Map2 * /*<struct Vector<EnumeratorAndValue>>*/ global_enum_tag_map;
	struct Vector /*<EnumeratorAndValue>*/ global_enumerator_list;
};

struct Expr typecheck_expression(const struct AnalyzerState *ptr_ps,
                                 struct UntypedExpr uexpr);

struct Statement parse_statement(struct AnalyzerState *ptr_ps,
                                 const struct Token **ptr_tokvec);
struct Statement parse_compound_statement(struct AnalyzerState *ptr_ps,
                                          const struct Token **ptr_tokvec);

/*
 * Adjusts the newest_offset and add a local variable to the scope.
 * Returns the offset of the newly added variable.
 */
int add_local_var_to_scope(struct AnalyzerState *ptr_ps,
                           const struct Type *ref_vartype, const char *str);

int size_of(const struct AnalyzerState *ptr_ps, const struct Type *ref_type);
int align_of(const struct AnalyzerState *ptr_ps, const struct Type *ref_type);

void expect_type(const struct AnalyzerState *ptr_ps, struct Type actual_type,
                 struct Type expected_type, const char *message);

int typecheck_constant_integral_expression(struct AnalyzerState *ptr_ps,
                                           struct UntypedExpr uexpr,
                                           const char *context);

enum SystemVAbiClass { INTEGER_CLASS, MEMORY_CLASS };
enum SystemVAbiClass system_v_abi_class_of(const struct AnalyzerState *ptr_ps,
                                           const struct Type *ref_type);

void expect_scalar(struct Type type, const char *context);
