#include "file_io.h"
#include "header.h"
#include "std.h"
#include "std_io.h"

static void replace_recursively(struct Map2 *def_map, struct Map2 *used_map,
                                const struct Token *ref_src,
                                struct Token *ptr_dst);

struct Tokvec preprocess(const char *str, struct Map2 *def_map)
{
	const struct Tokvec t = read_all_tokens(str);
	const struct Token *src = t.v;

	int total_token_num = t.tok_num;

	struct Token *dst = calloc(total_token_num, sizeof(struct Token));

	int j = 0;
	int k = 0;
	enum PreprocessorState s = LINE_HAS_JUST_STARTED;
	while (1) {
		if (s == LINE_HAS_JUST_STARTED && src[k].kind == HASH) {
			k++;

			while (src[k].kind == SPACE) {
				k++;
			}

			if (src[k].kind == NEWLINE) { /* empty directive */
				dst[j] = src[k];
				s = LINE_HAS_JUST_STARTED;
				j++;
				k++;
				continue;
			} else if (src[k].kind == END) { /* empty directive */
				dst[j] = src[k];
				break;
			} else if (src[k].kind != IDENT_OR_RESERVED) {
				fprintf(stderr, "Expected preprocessor directive, but got `");
				print_token_at(&src[k]);
				fprintf(stderr, "` as the token after `#`.");
				exit(EXIT_FAILURE);
			}

			if (strcmp(src[k].ident_str, "define") == 0) {
				k++; /* `define` */

				while (src[k].kind == SPACE) {
					k++;
				}

				if (src[k].kind != IDENT_OR_RESERVED) {
					fprintf(stderr, "Expected macro name, but got `");
					print_token_at(&src[k]);
					fprintf(stderr, "` as the token after `#define`.");
					exit(EXIT_FAILURE);
				}

				const char *macro_name = src[k].ident_str;
				k++;

				if (src[k].kind == LEFT_PAREN) {
					unsupported("function-style macro");
				}

				while (src[k].kind == SPACE) {
					k++;
				}

				struct Token *ptr_token = calloc(1, sizeof(struct Token));

				/* empty replacement */
				if (src[k].kind == NEWLINE) {
					k++;
					s = LINE_HAS_JUST_STARTED;
					ptr_token->kind = SPACE;
					insert(def_map, macro_name, ptr_token);
					continue;
				}
				if (src[k].kind == END) {
					ptr_token->kind = SPACE;
					insert(def_map, macro_name, ptr_token);

					dst[j] = src[k];
					break;
				}

				if (src[k + 1].kind != NEWLINE && src[k + 1].kind != END) {
					unsupported(
					    "`#define` directive that expands to multiple tokens");
				}

				/* one-token replacement */
				*ptr_token = src[k];
				insert(def_map, macro_name, ptr_token);
				k++;

				if (src[k].kind == NEWLINE) {
					k++;
					s = LINE_HAS_JUST_STARTED;
					continue;
				}
				if (src[k].kind == END) {
					dst[j] = src[k];
					break;
				}
			} else if (strcmp(src[k].ident_str, "include") == 0) {
				k++; /* `include` */

				while (src[k].kind == SPACE) {
					k++;
				}

				if (src[k].kind == OP_LT) {
					unsupported("`#include <...>`");
				}

				if (src[k].kind != LIT_STRING) {
					fprintf(stderr, "Expected include path, but got `");
					print_token_at(&src[k]);
					fprintf(stderr, "` as the token after `#include`.");
					exit(EXIT_FAILURE);
				}

				struct __FILE *fp = fopen(src[k].literal_str, "r");

				k++; /* consume the filepath token */

				if (!fp) {
					fprintf(stderr,
					        "failed to open file `%s` to be `#include`d.\n",
					        src[k].literal_str);
					exit(EXIT_FAILURE);
				}

				const char *imported = read_from_file(fp);
				const struct Tokvec new_vec = preprocess(imported, def_map);
				/* def_map will be modified, and that's how all the macros will
				 * be obtained here */

				total_token_num +=
				    new_vec.tok_num - 2; /* BEGINNING and END gone */

				dst = realloc(
				    dst, total_token_num *
				             sizeof(struct Token)); /* realloc never fails */

				int l = 1;
				while (1) {
					if (new_vec.v[l].kind == END) {
						break;
					}
					dst[j] = new_vec.v[l];
					j++;
					l++;
				}

				while (src[k].kind == SPACE) {
					k++;
				}

				if (src[k].kind == NEWLINE) {
					k++;
					s = LINE_HAS_JUST_STARTED;
					continue;
				}
				if (src[k].kind == END) {
					dst[j] = src[k];
					break;
				}

				fprintf(stderr, "Unexpected token  `");
				print_token_at(&src[k]);
				fprintf(stderr, "` as the token after `#include (filepath)`.");
				exit(EXIT_FAILURE);

			} else if (strcmp(src[k].ident_str, "ifdef") == 0 ||
			           strcmp(src[k].ident_str, "ifndef") == 0) {
				int is_ifdef = strcmp(src[k].ident_str, "ifdef") == 0;
				k++; /* `define` */

				while (src[k].kind == SPACE) {
					k++;
				}

				if (src[k].kind != IDENT_OR_RESERVED) {
					fprintf(stderr, "Expected macro name, but got `");
					print_token_at(&src[k]);
					fprintf(stderr, "` as the token after `#if%sdef`.",
					        is_ifdef ? "" : "n");
					exit(EXIT_FAILURE);
				}

				const char *macro_name = src[k].ident_str;
				k++;
				unsupported("#ifdef/#ifndef");
			} else {
				unsupported("unknown directive");
			}
		}

		if (src[k].kind == NEWLINE || src[k].kind == BEGINNING) {
			s = LINE_HAS_JUST_STARTED;
		} else if (src[k].kind == SPACE) {
			/* keep the state as is */
		} else {
			s = NOTHING_SPECIAL;
		}

		struct Map2 *used_map = init_map();
		replace_recursively(def_map, used_map, &src[k], &dst[j]);

		if (dst[j].kind == END) {
			break;
		}

		j++;
		k++;
	}

	struct Tokvec u;
	u.tok_num = j + 1;
	u.v = dst;

	return u;
}

static void replace_recursively(struct Map2 *def_map, struct Map2 *used_map,
                                const struct Token *ref_src,
                                struct Token *ptr_dst)
{
	if (ref_src->kind == IDENT_OR_RESERVED &&
	    isElem(def_map, ref_src->ident_str) &&
	    !isElem(used_map, ref_src->ident_str)) {
		struct Token *replace_with = lookup(def_map, ref_src->ident_str);

		int u;
		insert(used_map, ref_src->ident_str,
		       &u /* this can be any arbitrary non-null pointer*/);

		replace_recursively(def_map, used_map, replace_with, ptr_dst);

	} else {
		*ptr_dst = *ref_src;
	}
}
