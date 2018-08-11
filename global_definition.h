#include "header.h"
#include "parser.h"
#include "printerstate.h"

void print_string_pool(struct Vector pool);
void parse_final(const struct Token **ptr_tokvec);
void generate(const struct Vector vec);
struct Vector parse(const struct Token *tokvec);
