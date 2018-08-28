#include "analyzer.h"
#include "printerstate.h"

struct ToplevelFuncInfo {
	struct Statement sta;
	struct Vector /*<LocalVarInfo>*/ offsets_and_types;
	struct Type ret_type;
	int capacity;
};

enum ToplevelCategory {
	TOPLEVEL_VAR_DEFINITION = 1,
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

void generate(const struct Vector /*<Toplevel>*/ vec);
struct Vector /*<Toplevel>*/ parse(const struct Token *tokvec);
