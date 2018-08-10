#include "header.h"
#if 1
#include "parser.h"
#endif

struct PrinterState {
	int final_label_name;
	int return_label_name;   /* the label at the end of the function */
	int break_label_name;    /* the label at the end of the current loop */
	int continue_label_name; /* the label at the beginning of the current loop
	                          */
	struct Vector string_constant_pool;
	int pool_largest_id;
};

void parse_final(const struct Token **ptr_tokvec);

int get_new_label_name(struct PrinterState *ptr_prs);
void print_expression(struct PrinterState *ptr_prs, struct Expression expr);
