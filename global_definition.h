#include "header.h"
#if 1
#include "parser.h"
#endif
#if 1
#include "printerstate.h"
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void print_string_pool(struct Vector pool);
void parse_final(const struct Token **ptr_tokvec);
void generate(const struct Vector vec);
struct Vector parse(const struct Token *tokvec);
