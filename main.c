#include "file_io.h"
#include "std.h"
#include "std_io.h"
#include "toplevel.h"

int main(int argc, char const **argv)
{
	struct __FILE *fp;

	if (argc == 1) {
		fp = stdin;
	} else {
		const char *filename = argv[1];
		fp = fopen(filename, "r");
		if (!fp) {
			fprintf(stderr, "failed to open file `%s`.\n", filename);
			exit(EXIT_FAILURE);
		}
	}

	char *str = read_from_file(fp);

	const struct Token *tokvec = read_and_preprocess(str);

	++tokvec; /* skip the dummy token BEGINNING */

	const struct Vector /*<Toplevel>*/ vec = parse(tokvec);

	generate(&vec);

	return 0;
}
