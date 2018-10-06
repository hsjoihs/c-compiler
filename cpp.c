#include "file_io.h"
#include "header.h"
#include "std.h"
#include "std_io.h"

static void replace_recursively(struct Map2 *def_map, struct Map2 *used_map,
                                const struct Token *ref_src,
                                struct Token *ptr_dst);


void skip_till_corresponding_endif()
{
	unsupported("#ifdef/#ifndef false branch");
}

struct Tokvec preprocess(const char *str, struct Map2 *def_map)
{
	const struct Tokvec t = read_all_tokens(str);
	struct Token *src = t.v;

	int total_token_num = t.tok_num;
	int ifdef_depth = 0;

	struct Token *dst = calloc(total_token_num, sizeof(struct Token));

	int j = 0;
	enum PreprocessorState s = LINE_HAS_JUST_STARTED;
	while (1) {
		if (s == LINE_HAS_JUST_STARTED && src[0].kind == HASH) {
			src++;

			while (src[0].kind == SPACE) {
				src++;
			}

			if (src[0].kind == NEWLINE) { /* empty directive */
				dst[j] = *src;
				s = LINE_HAS_JUST_STARTED;
				j++;
				src++;
				continue;
			} else if (src[0].kind == END) { /* empty directive */
				dst[j] = *src;
				break;
			} else if (src[0].kind != IDENT_OR_RESERVED) {
				fprintf(stderr, "Expected preprocessor directive, but got `");
				print_token_at(src);
				fprintf(stderr, "` as the token after `#`.");
				exit(EXIT_FAILURE);
			}

			if (strcmp(src[0].ident_str, "define") == 0) {
				src++; /* `define` */

				while (src[0].kind == SPACE) {
					src++;
				}

				if (src[0].kind != IDENT_OR_RESERVED) {
					fprintf(stderr, "Expected macro name, but got `");
					print_token_at(src);
					fprintf(stderr, "` as the token after `#define`.");
					exit(EXIT_FAILURE);
				}

				const char *macro_name = src[0].ident_str;
				src++;

				if (src[0].kind == LEFT_PAREN) {
					unsupported("function-style macro");
				}

				while (src[0].kind == SPACE) {
					src++;
				}

				struct Token *ptr_token = calloc(1, sizeof(struct Token));

				/* empty replacement */
				if (src[0].kind == NEWLINE) {
					src++;
					s = LINE_HAS_JUST_STARTED;
					ptr_token->kind = SPACE;
					insert(def_map, macro_name, ptr_token);
					continue;
				}
				if (src[0].kind == END) {
					ptr_token->kind = SPACE;
					insert(def_map, macro_name, ptr_token);

					dst[j] = *src;
					break;
				}

				if (src[1].kind != NEWLINE && src[1].kind != END) {
					unsupported(
					    "`#define` directive that expands to multiple tokens");
				}

				/* one-token replacement */
				*ptr_token = *src;
				insert(def_map, macro_name, ptr_token);
				src++;

				if (src[0].kind == NEWLINE) {
					src++;
					s = LINE_HAS_JUST_STARTED;
					continue;
				}
				if (src[0].kind == END) {
					dst[j] = *src;
					break;
				}
			} else if (strcmp(src[0].ident_str, "include") == 0) {
				src++; /* `include` */

				while (src[0].kind == SPACE) {
					src++;
				}

				if (src[0].kind == OP_LT) {
					unsupported("`#include <...>`");
				}

				if (src[0].kind != LIT_STRING) {
					fprintf(stderr, "Expected include path, but got `");
					print_token_at(src);
					fprintf(stderr, "` as the token after `#include`.");
					exit(EXIT_FAILURE);
				}

				struct __FILE *fp = fopen(src[0].literal_str, "r");

				if (!fp) {
					fprintf(stderr,
					        "failed to open file `%s` to be `#include`d.\n",
					        src[0].literal_str);
					exit(EXIT_FAILURE);
				}

				src++; /* consume the filepath token */

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

				while (src[0].kind == SPACE) {
					src++;
				}

				if (src[0].kind == NEWLINE) {
					src++;
					s = LINE_HAS_JUST_STARTED;
					continue;
				}
				if (src[0].kind == END) {
					dst[j] = *src;
					break;
				}

				fprintf(stderr, "Unexpected token  `");
				print_token_at(src);
				fprintf(stderr, "` as the token after `#include (filepath)`.");
				exit(EXIT_FAILURE);

			} else if (strcmp(src[0].ident_str, "ifdef") == 0 ||
			           strcmp(src[0].ident_str, "ifndef") == 0) {
				int is_ifdef = strcmp(src[0].ident_str, "ifdef") == 0;
				src++; /* `if(n)?def` */

				while (src[0].kind == SPACE) {
					src++;
				}

				if (src[0].kind != IDENT_OR_RESERVED) {
					fprintf(stderr, "Expected macro name, but got `");
					print_token_at(src);
					fprintf(stderr, "` as the token after `#if%sdef`.",
					        is_ifdef ? "" : "n");
					exit(EXIT_FAILURE);
				}

				while (src[0].kind == SPACE) {
					src++;
				}

				const char *macro_name = (src++)[0].ident_str;

				if (src[0].kind == END) {
					fprintf(stderr,
					        "End of file was encountered immediately after an "
					        "`#if%sdef` directive",
					        is_ifdef ? "" : "n");
					exit(EXIT_FAILURE);
				}

				if (src[0].kind != NEWLINE) {
					fprintf(stderr, "Expected newline, but got `");
					print_token_at(&src[0]);
					fprintf(stderr,
					        "` as the token after `#if%sdef (macro_name)`.",
					        is_ifdef ? "" : "n");
					exit(EXIT_FAILURE);
				}

				src++;
				s = LINE_HAS_JUST_STARTED;

				if (is_ifdef == isElem(def_map, macro_name)) { /* true branch */
					ifdef_depth++;
				} else { /* false branch */
					skip_till_corresponding_endif();
				}

			} else if (strcmp(src[0].ident_str, "endif") ==
			           0) { /* passes only when the #if(n)?def condition was
				               true. If false, it will be handled in
				               #if(n)?def.*/
				src++;

				if (ifdef_depth < 1) {
					fprintf(stderr,
					        "mismatch of `#endif` directive was detected.\n");
					exit(EXIT_FAILURE);
				}

				ifdef_depth--;

				while (src[0].kind == SPACE) {
					src++;
				}

				if (src[0].kind == NEWLINE) {
					src++;
					s = LINE_HAS_JUST_STARTED;
					continue;
				}
				if (src[0].kind == END) {
					dst[j] = *src;
					break;
				}

				fprintf(stderr, "Expected newline, but got `");
				print_token_at(src);
				fprintf(stderr, "` as the token after `#endif`.");
				exit(EXIT_FAILURE);

			} else {
				unsupported("unknown directive");
			}
		}

		if (src[0].kind == NEWLINE || src[0].kind == BEGINNING) {
			s = LINE_HAS_JUST_STARTED;
		} else if (src[0].kind == SPACE) {
			/* keep the state as is */
		} else {
			s = NOTHING_SPECIAL;
		}

		struct Map2 *used_map = init_map();
		replace_recursively(def_map, used_map, src, &dst[j]);

		if (dst[j].kind == END) {
			break;
		}

		j++;
		src++;
	}

	if (ifdef_depth) {
		fprintf(stderr, "insufficient number of `#endif`s.\n");
		exit(EXIT_FAILURE);
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
