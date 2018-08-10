#include "parse_statement.h"
#if 1
#include "codegen.h"
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

enum DefinitionCategory {
	TOPLEVEL_VAR_DEFINITION,
	TOPLEVEL_FUNCTION_DEFINITION,
	TOPLEVEL_FUNCTION_DECLARATION
};

struct Definition {
	enum DefinitionCategory category;
	const char *declarator_name;
	struct Type declarator_type;
	struct {
		struct Statement sta;
		struct Vector offsets_and_types;
		struct Type ret_type;
	} func;
};

struct Definition parse_toplevel_definition(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec);
void print_toplevel_definition(struct ParserState *ptr_ps,
                               struct PrinterState *ptr_prs,
                               struct Definition def);

void print_string_pool(struct Vector pool);
void parse_final(const struct Token **ptr_tokvec);
