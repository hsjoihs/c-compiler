#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const **argv)
{

	int size = 1;
	int capacity = 50000;
	char *str = calloc(capacity, sizeof(char));
	char *buffer = calloc(1024, sizeof(char));

	while (fgets(buffer, 1024, stdin)) {
		size += strlen(buffer);
		if (size > capacity) {
			capacity *= 2;
			str = realloc(str, capacity);
		}
		strcat(str, buffer);
	}

	if (argc == 2) {
		if (strcmp(argv[1], "--lexer-debug") == 0) {
			read_all_tokens_debug(str);
		}
	} else {
		const struct Token *tokvec = read_all_tokens(str);

		++tokvec; /* skip the dummy token BEGINNING */

		struct ParserState ps;
		struct PrinterState prs;
		prs.final_label_name = 1;
		prs.return_label_name = GARBAGE_INT;
		prs.string_constant_pool = init_vector();
		prs.pool_largest_id = 0;
		ps.func_info_map = init_map();
		ps.global_vars_type_map = init_map();

		while (1) {
			if (tokvec[0].kind == END) {
				parse_final(&tokvec);
				print_string_pool(prs.string_constant_pool);
				return 0;
			} else {
				parseprint_toplevel_definition(&ps, &prs, &tokvec);
			}
		}
	}
	return 0;
}
