#include "file_io.h"
#include "header.h"
#include "std.h"
#include "std_io.h"

static void replace_recursively(struct Map2 *def_map, struct Map2 *used_map,
                                const struct Token *ref_src,
                                struct Token *ptr_dst);

static void consume_spaces(const struct Token **ptr_src)
{
	while ((*ptr_src)[0].kind == SPACE) {
		(*ptr_src)++;
	}
}

static void consume_the_rest_of_line(const struct Token **ptr_src)
{
	while ((*ptr_src)[0].kind != NEWLINE) {
		(*ptr_src)++;
	}
}

static void skip_till_corresponding_endif(const struct Token **ptr_src)
{
	const struct Token *src = *ptr_src;
	enum PreprocessorState s = LINE_HAS_JUST_STARTED;
	int ifdef_depth = 1;
	while (1) {
		if (s == LINE_HAS_JUST_STARTED && src[0].kind == HASH) {
			src++;

			consume_spaces(&src);

			if (src[0].kind == NEWLINE) { /* empty directive */
				s = LINE_HAS_JUST_STARTED;
				src++;
				continue;
			}

			expect_and_consume(
			    &src, IDENT_OR_RESERVED,
			    "identifier after `#` for preprocessor directive");

			if (strcmp(src[-1].ident_str, "ifdef") == 0 ||
			    strcmp(src[-1].ident_str, "ifndef") == 0) {
				ifdef_depth++;
				consume_the_rest_of_line(&src);
			} else if (strcmp(src[-1].ident_str, "endif") == 0) {

				consume_spaces(&src);

				if (src[0].kind == END) {
					if (ifdef_depth == 1) {
						*ptr_src = src;
						return;
					} else {
						fprintf(stderr, "insufficient `#endif`.\n");
						exit(EXIT_FAILURE);
					}
				}

				if (src[0].kind != NEWLINE) {
					error_unexpected_token(src, "newline after `#endif`");
				}

				if (ifdef_depth == 1) {
					*ptr_src = src;
					return;
				}
				ifdef_depth--;
			} else {
				consume_the_rest_of_line(&src);
			}
			src++;
			s = LINE_HAS_JUST_STARTED;
			continue;
		}

		if (src[0].kind == NEWLINE || src[0].kind == BEGINNING) {
			s = LINE_HAS_JUST_STARTED;
		} else if (src[0].kind == SPACE) {
			/* keep the state as is */
		} else {
			s = NOTHING_SPECIAL;
		}

		if (src[0].kind == END) {
			fprintf(stderr, "insufficient `#endif`.\n");
			exit(EXIT_FAILURE);
		}
		src++;
	}
}

static int foo(struct Token *dst, const struct Token **ptr_src, int *ptr_j,
               enum PreprocessorState *ptr_s, struct Map2 *def_map);

/* lexer inserts a NEWLINE before END; hence, END can mostly be ignored */
struct Tokvec preprocess(const char *str, struct Map2 *def_map)
{
	const struct Tokvec t = read_all_tokens(str);
	const struct Token *src = t.v;

	int total_token_num = t.tok_num;
	int ifdef_depth = 0;

	struct Token *dst = calloc(total_token_num, sizeof(struct Token));

	int j = 0;
	enum PreprocessorState s = LINE_HAS_JUST_STARTED;
	while (1) {
		if (s != LINE_HAS_JUST_STARTED || src[0].kind != HASH) {
			if (foo(dst, &src, &j, &s, def_map)) {
				break;
			}
			continue;
		}

		src++; /* HASH */

		consume_spaces(&src);

		if (src[0].kind == NEWLINE) { /* empty directive */
			dst[j] = *src;
			s = LINE_HAS_JUST_STARTED;
			j++;
			src++;
			continue;
		}

		expect_and_consume(&src, IDENT_OR_RESERVED,
		                   "identifier after `#` for preprocessor directive");

		const char *directive = src[-1].ident_str;

			consume_spaces(&src);
		if (strcmp(directive, "define") == 0) {

			expect_and_consume(&src, IDENT_OR_RESERVED,
			                   "macro name after `#define`");
			const char *macro_name = src[-1].ident_str;

			/* only when the identifier is IMMEDIATELY followed by
			 * LEFT_PAREN */
			if (src[0].kind == LEFT_PAREN) {
				unsupported("function-style macro");
			}

			/* that's why this must be here, not earlier */
			consume_spaces(&src);

			struct Token *ptr_token = calloc(1, sizeof(struct Token));

			/* empty replacement */
			if (src[0].kind == NEWLINE) {
				src++;
				s = LINE_HAS_JUST_STARTED;
				ptr_token->kind = SPACE;
				insert(def_map, macro_name, ptr_token);
				continue;
			}

			if (src[1].kind != NEWLINE) {
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
		} else if (strcmp(directive, "include") == 0) {

			if (src[0].kind == OP_LT) {
				unsupported("`#include <...>`");
			}

			expect_and_consume(&src, LIT_STRING, "path after `#include`");

			struct __FILE *fp = fopen(src[-1].literal_str, "r");

			if (!fp) {
				fprintf(stderr, "failed to open file `%s` to be `#include`d.\n",
				        src[-1].literal_str);
				exit(EXIT_FAILURE);
			}

			const char *imported = read_from_file(fp);
			const struct Tokvec new_vec = preprocess(imported, def_map);
			/* def_map will be modified, and that's how all the macros will
			 * be obtained here */

			total_token_num += new_vec.tok_num - 2; /* BEGINNING and END gone */

			dst = realloc(dst,
			              total_token_num *
			                  sizeof(struct Token)); /* realloc never fails */

			int l = 1; /* skip BEGINNING */
			while (1) {
				if (new_vec.v[l].kind == END) {
					break;
				}
				dst[j] = new_vec.v[l];
				j++;
				l++;
			}

			consume_spaces(&src);

			if (src[0].kind == NEWLINE) {
				src++;
				s = LINE_HAS_JUST_STARTED;
				continue;
			}

			error_unexpected_token(
			    src, "newline or end of file after `#include (filepath)`");

		} else if (strcmp(directive, "ifdef") == 0 ||
		           strcmp(directive, "ifndef") == 0) {
			int is_ifdef = strcmp(directive, "ifdef") == 0;

			expect_and_consume(&src, IDENT_OR_RESERVED,
			                   is_ifdef ? "identifier after `#ifdef`"
			                            : "identifier after `#ifndef`");

			const char *macro_name = src[-1].ident_str;
			consume_spaces(&src);

			expect_and_consume(&src, NEWLINE,
			                   "newline after `#if%sdef (macro_name)`");
			src--; /* ad hoc */

			s = LINE_HAS_JUST_STARTED;

			if (is_ifdef == isElem(def_map, macro_name)) { /* true branch */
				ifdef_depth++;
			} else { /* false branch */
				skip_till_corresponding_endif(&src);
				continue;
			}

		} else if (strcmp(directive, "endif") ==
		           0) { /* passes only when the #if(n)?def condition was
			               true. If false, it will be handled in
			               #if(n)?def.*/
			if (ifdef_depth < 1) {
				fprintf(stderr,
				        "mismatch of `#endif` directive was detected.\n");
				exit(EXIT_FAILURE);
			}

			ifdef_depth--;
			expect_and_consume(&src, NEWLINE, "newline after `#endif`");
			s = LINE_HAS_JUST_STARTED;
			continue;
		} else {
			unsupported("unknown directive");
		}

		if (foo(dst, &src, &j, &s, def_map)) {
			break;
		}
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

static int foo(struct Token *dst, const struct Token **ptr_src, int *ptr_j,
               enum PreprocessorState *ptr_s, struct Map2 *def_map)
{
	const struct Token *src = *ptr_src;
	int j = *ptr_j;

	if (src[0].kind == NEWLINE || src[0].kind == BEGINNING) {
		*ptr_s = LINE_HAS_JUST_STARTED;
	} else if (src[0].kind == SPACE) {
		/* keep the state as is */
	} else {
		*ptr_s = NOTHING_SPECIAL;
	}

	struct Map2 *used_map = init_map();
	replace_recursively(def_map, used_map, src, &dst[j]);

	if (dst[j].kind == END) {
		*ptr_src = src;
		*ptr_j = j;
		return 1; /* break */
	}

	j++;
	src++;
	*ptr_src = src;
	*ptr_j = j;
	return 0;
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
