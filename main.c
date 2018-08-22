#include "toplevel.h"
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
		return 0;
	}

	const struct Token *tokvec = read_all_tokens(str);

	++tokvec; /* skip the dummy token BEGINNING */

	struct Vector /*<Toplevel>*/ vec = parse(tokvec);

	generate(vec);

	return 0;
}
