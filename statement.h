#include "header.h"
#if 1
#include "parser.h"
#endif
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
