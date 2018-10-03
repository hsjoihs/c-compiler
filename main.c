#include "std.h"
#include "std_io.h"
#include "toplevel.h"

int main(int argc, char const **argv)
{
	struct __FILE *fp;

	int is_lexer_debug = 0;
	if (argc == 1) {
		fp = stdin;
	} else if (argc == 2 && strcmp(argv[1], "--lexer-debug") == 0) {
		fp = stdin;
		is_lexer_debug = 1;
	} else {
		const char *filename = strcmp(argv[1], "--lexer-debug") == 0
		                           ? (is_lexer_debug = 1, argv[2])
		                           : argv[1];
		fp = fopen(filename, "r");
		if (!fp) {
			fprintf(stderr, "failed to open file `%s`.\n", filename);
			exit(EXIT_FAILURE);
		}
		if (argc == 3 && strcmp(argv[2], "--lexer-debug") == 0) {
			is_lexer_debug = 1;
		}
	}

	int size = 1;
	int capacity = 50000;
	char *str = calloc(capacity, sizeof(char));
	char buffer[1024];

	while (fgets(buffer, 1024, fp)) {
		size += strlen(buffer);
		if (size > capacity) {
			capacity *= 2;
			str = realloc(str, capacity);
		}
		strcat(str, buffer);
	}

	const struct Token *tokvec =
	    concat_str_literals(remove_spaces_and_newlines(read_all_tokens(str)));

	if (is_lexer_debug) {
		for (int i = 0;;) {
			if (tokvec[i].kind == END) {
				break;
			}
			print_token_at(tokvec + i);
			fprintf(stderr, "\n");
		}
		return 0;
	}

	++tokvec; /* skip the dummy token BEGINNING */

	const struct Vector /*<Toplevel>*/ vec = parse(tokvec);

	generate(&vec);

	return 0;
}
