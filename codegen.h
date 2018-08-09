#include "header.h"
#include "vector.h"
#if 1
#include "parser.h"
#endif
#include "print_x86_64.h"

struct PrinterState {
	int final_label_name;
	int return_label_name;   /* the label at the end of the function */
	int break_label_name;    /* the label at the end of the current loop */
	int continue_label_name; /* the label at the beginning of the current loop
	                          */
	struct Vector string_constant_pool;
	int pool_largest_id;
};

void print_string_pool(struct Vector pool);
void parse_final(const struct Token **ptr_tokvec);
void parseprint_toplevel_definition(struct ParserState *ptr_ps,
                                    struct PrinterState *ptr_prs,
                                    const struct Token **ptr_tokvec);
