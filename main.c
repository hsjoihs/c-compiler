#include "global_definition.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const **argv)
{

	int size = 1;
	int capacity = 50000;
	char *str = calloc(capacity, sizeof(char));
	char buffer[1024];

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

		ps.func_info_map = init_map();
		ps.global_vars_type_map = init_map();

		struct Vector vec = init_vector();
		while (1) {
			if (tokvec[0].kind == END) {
				parse_final(&tokvec);
				break;
			} else {
				struct Toplevel def = parse_toplevel_definition(&ps, &tokvec);
				struct Toplevel *ptr = calloc(1, sizeof(struct Toplevel));
				*ptr = def;
				push_vector(&vec, ptr);
			}
		}

		struct PrinterState prs;
		prs.final_label_name = 1;
		prs.return_label_name = GARBAGE_INT;
		prs.string_constant_pool = init_vector();
		prs.pool_largest_id = 0;

		for (int i = 0; i < vec.length; i++) {
			const struct Toplevel *ptr = vec.vector[i];

			print_toplevel_definition(&prs, *ptr);
		}

		print_string_pool(prs.string_constant_pool);
	}
	return 0;
}
