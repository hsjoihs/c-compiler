#include "statement.h"
#if 1
#include "printerstate.h"
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

enum ToplevelCategory {
	TOPLEVEL_VAR_DEFINITION,
	TOPLEVEL_FUNCTION_DEFINITION,
	TOPLEVEL_FUNCTION_DECLARATION
};

struct Toplevel {
	enum ToplevelCategory category;
	const char *declarator_name;
	struct Type declarator_type;
	struct {
		struct Statement sta;
		struct Vector offsets_and_types;
		struct Type ret_type;
		int capacity;
	} func;
};

void print_string_pool(struct Vector pool);
void parse_final(const struct Token **ptr_tokvec);
void generate(const struct Vector vec);
struct Vector parse(const struct Token *tokvec);
