#include "parser.h"
#include "printerstate.h"

void print_string_pool(struct Vector pool);
void generate(const struct Vector vec);
struct Vector parse(const struct Token *tokvec);
