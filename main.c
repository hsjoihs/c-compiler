#include "file_io.h"
#include "std.h"
#include "std_io.h"
#include "toplevel.h"

int main(int argc, char const **argv)
{
	struct __FILE *fp = stdin;

	struct Vector macros = init_vector();

	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "-D", 2) == 0) {
			push_vector(&macros, argv[i] + 2);
		} else {
			const char *filename = argv[i];
			fp = fopen(filename, "r");
			if (!fp) {
				fprintf(stderr, "failed to open file `%s`.\n", filename);
				exit(EXIT_FAILURE);
			}
		}
	}

	char *str = read_from_file(fp);

	const struct Token *tokvec = read_and_preprocess(str, &macros);

	++tokvec; /* skip the dummy token BEGINNING */

	const struct Vector /*<Toplevel>*/ vec = parse(tokvec);

	generate(&vec);

	return 0;
}
