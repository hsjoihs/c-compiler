#include "file_io.h"
#include "std.h"
#include "std_io.h"
#include "toplevel.h"
#include "print_x86_64.h"

int mkstemp(char *template);
struct __FILE *fdopen(int desc, const char *mode);
char *strdup(const char *str1);

int main(int argc, char const **argv)
{
	struct __FILE *fp = stdin;

	struct Vector macros = init_vector();

	const char *file_name_dbg = "<stdin>";

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
			file_name_dbg = filename;
		}
	}

	char *str = read_from_file(fp);

	const struct Token *tokvec = read_and_preprocess(str, &macros);

	++tokvec; /* skip the dummy token BEGINNING */

	const struct Vector /*<Toplevel>*/ vec = parse(tokvec);

	char *stat_template = strdup("./ir/statXXXXXX");
	int stat_fd = mkstemp(stat_template);

	global_stat_log = fdopen(stat_fd, "w");
	fprintf(global_stat_log, "IR for %s: \n", file_name_dbg);
	generate(&vec);
	fclose(global_stat_log);

	return 0;
}
