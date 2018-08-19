#include "analyzer.h"
#include "printerstate.h"

struct ToplevelFuncInfo {
	struct Statement sta;
	struct Vector /*<LocalVarInfo>*/ offsets_and_types;
	struct Type ret_type;
	int capacity;
};

struct Toplevel {
	enum ToplevelCategory category;
	const char *declarator_name;
	/* NULL when TOPLEVEL_VAR_DEFINITION does not have an identifier */

	struct Type declarator_type;
	/* used when it is TOPLEVEL_VAR_DEFINITION or TOPLEVEL_TYPE_DECLARATION */

	struct ToplevelFuncInfo func;

	int size_of_declarator_type;
};

void generate(const struct Vector vec);
struct Vector parse(const struct Token *tokvec);
