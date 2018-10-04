#include "analyzer.h"
#include "printerstate.h"

struct ToplevelFuncInfo {
	struct Statement sta;
	struct Vector /*<LocalVarInfo>*/ offsets_and_types;
	struct Type ret_type;
	int capacity;
	int is_static_function;

	/* valid only when ret_type is a struct */
	enum SystemVAbiClass abi_class;
	int ret_struct_size;

	/* valid only when ret_type is a struct of MEMORY_CLASS */
	int hidden_var_offset;
};

enum ToplevelCategory {
	TOPLEVEL_VAR_DEFINITION,
	TOPLEVEL_FUNCTION_DEFINITION,
	TOPLEVEL_FUNCTION_DECLARATION,
};

struct Toplevel {
	enum ToplevelCategory category;
	const char *declarator_name;
	/* NULL when TOPLEVEL_VAR_DEFINITION does not have an identifier */

	struct Type declarator_type;
	/* used when it is TOPLEVEL_VAR_DEFINITION or TOPLEVEL_TYPE_DECLARATION */

	struct ToplevelFuncInfo func;

	int size_of_declarator_type;
	int is_extern_global_var;
};

void generate(const struct Vector /*<Toplevel>*/ *ref_vec);
struct Vector /*<Toplevel>*/ parse(const struct Token *tokvec);

struct SourceLabelAndAssemblyLabel {
	int assembly_label;
	struct SourceLabel source_label;
};
void print_statement(struct PrinterState *ptr_prs,
                     const struct Statement *ptr_sta);
void codegen_switch(struct PrinterState *ptr_prs,
                    const struct Statement *ptr_sta);

void main2(const struct Token *tokvec);
