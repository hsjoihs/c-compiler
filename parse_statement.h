#include "codegen.h"

enum StatementCategory {
	COMPOUND_STATEMENT = 1,
	IF_STATEMENT,
	IF_ELSE_STATEMENT,
	FOR_STATEMENT,
	WHILE_STATEMENT,
	DO_WHILE_STATEMENT,
	RETURN_STATEMENT,
	BREAK_STATEMENT,
	CONTINUE_STATEMENT,
	EXPRESSION_STATEMENT,
	DECLARATION_STATEMENT,
};

struct Statement {
	enum StatementCategory category;
	struct Vector statement_vector;
	struct Expression expr1;
	struct Expression expr2;
	struct Expression expr3;
	struct Statement *inner_statement;
	struct {
		struct Type type;
		const char *ident_str;
	} declaration;
};

struct Statement parse_statement(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
struct Statement parse_compound_statement(struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec);
void add_local_var_to_scope(struct ParserState *ptr_ps,
                            const struct Type vartype, const char *str);
