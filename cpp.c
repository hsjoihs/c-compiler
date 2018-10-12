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

static int replacement(struct Token *dst_initial, const struct Token **ptr_src,
                       int *ref_dst_offset, enum PreprocessorState *ptr_s,
                       struct Map2 *def_map);

/*
 * ptr_src:
 *    at the start, # followed by spaces are already consumed.
 *    when returning, NEWLINE is consumed if it exists (return 1 => continue)
 *    if nonexistent, it will return 0 and fallthru.
 */
static int handle_define(const struct Token **ptr_src, struct Map2 *def_map)
{
	expect_and_consume(ptr_src, IDENT_OR_RESERVED,
	                   "macro name after `#define`");
	const char *macro_name = (*ptr_src)[-1].ident_str;

	/* only when the identifier is IMMEDIATELY followed by
	 * LEFT_PAREN */
	if ((*ptr_src)[0].kind == LEFT_PAREN) {
		unsupported("function-style macro");
	}

	/* that's why this must be here, not earlier */
	consume_spaces(ptr_src);

	struct Token *ptr_token = calloc(1, sizeof(struct Token));

	/* empty replacement */
	if ((*ptr_src)[0].kind == NEWLINE) {
		(*ptr_src)++;
		ptr_token->kind = SPACE;
		insert(def_map, macro_name, ptr_token);
		return 1;
	}

	if ((*ptr_src)[1].kind != NEWLINE) {
		unsupported("`#define` directive that expands to multiple tokens");
	}

	/* one-token replacement */
	*ptr_token = **ptr_src;
	insert(def_map, macro_name, ptr_token);
	(*ptr_src)++;

	if ((*ptr_src)[0].kind == NEWLINE) {
		(*ptr_src)++;
		return 1;
	}
	return 0;
}

/*
 * ptr_src:
 *    at the start, # followed by spaces are already consumed.
 *    when returning, NEWLINE is consumed if it exists (return 1 => continue)
 *    if nonexistent, it will throw an error.
 */
static void handle_include(struct Token **ptr_dst, const struct Token **ptr_src,
                           int *ref_dst_offset, 
                           struct Map2 *def_map, int *ptr_total_token_num)
{
	const struct Token *src = *ptr_src;
	int dst_offset = *ref_dst_offset;
	struct Token *dst_initial = *ptr_dst;

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

	*ptr_total_token_num += new_vec.tok_num - 2; /* BEGINNING and END gone */

	dst_initial = realloc(dst_initial, *ptr_total_token_num *
	                       sizeof(struct Token)); /* realloc never fails */

	int l = 1; /* skip BEGINNING */
	while (1) {
		if (new_vec.v[l].kind == END) {
			break;
		}
		dst_initial[dst_offset] = new_vec.v[l];
		dst_offset++;
		l++;
	}

	consume_spaces(&src);

	if (src[0].kind == NEWLINE) {
		src++;
	} else {
		error_unexpected_token(
		    src, "newline or end of file after `#include (filepath)`");
	}

	*ref_dst_offset = dst_offset;
	*ptr_src = src;
	*ptr_dst = dst_initial;
}

static int handle_ifdef(int is_ifdef, const struct Token **ptr_src,
                        struct Map2 *def_map, 
                        int *ptr_ifdef_depth)
{
	expect_and_consume(ptr_src, IDENT_OR_RESERVED,
	                   is_ifdef ? "identifier after `#ifdef`"
	                            : "identifier after `#ifndef`");

	const char *macro_name = (*ptr_src)[-1].ident_str;
	consume_spaces(ptr_src);

	expect_and_consume(ptr_src, NEWLINE,
	                   is_ifdef ? "newline after `#ifdef (macro_name)`"
	                            : "newline after `#ifndef (macro_name)`");
	(*ptr_src)--; /* ad hoc */

	if (is_ifdef == isElem(def_map, macro_name)) { /* true branch */
		(*ptr_ifdef_depth)++;
	} else { /* false branch */
		skip_till_corresponding_endif(ptr_src);

		return 1;
	}
	return 0;
}

/* lexer inserts a NEWLINE before END; hence, END can mostly be ignored */
struct Tokvec preprocess(const char *str, struct Map2 *def_map)
{
	const struct Tokvec t = read_all_tokens(str);
	const struct Token *src = t.v;

	int total_token_num = t.tok_num;
	int ifdef_depth = 0;

	struct Token *dst_initial = calloc(total_token_num, sizeof(struct Token));

	int dst_offset = 0;
	enum PreprocessorState s = LINE_HAS_JUST_STARTED;
	while (1) {
		if (s != LINE_HAS_JUST_STARTED || src[0].kind != HASH) {
			if (replacement(dst_initial, &src, &dst_offset, &s, def_map)) {
				break;
			}
			continue;
		}

		assert(s == LINE_HAS_JUST_STARTED); /* because otherwise we aren't here */

		src++; /* HASH */

		consume_spaces(&src);

		if (src[0].kind == NEWLINE) { /* empty directive */
			src++;
			continue;
		}

		expect_and_consume(&src, IDENT_OR_RESERVED,
		                   "identifier after `#` for preprocessor directive");

		const char *directive = src[-1].ident_str;
		consume_spaces(&src);

		if (strcmp(directive, "define") == 0) {
			assert(s == LINE_HAS_JUST_STARTED);
			if (handle_define(&src, def_map)) {
				continue;
			}
		} else if (strcmp(directive, "include") == 0) {
			assert(s == LINE_HAS_JUST_STARTED);
			handle_include(&dst_initial, &src, &dst_offset, def_map, &total_token_num);
			continue;
		} else if (strcmp(directive, "ifdef") == 0 ||
		           strcmp(directive, "ifndef") == 0) {
			assert(s == LINE_HAS_JUST_STARTED);
			if (handle_ifdef(strcmp(directive, "ifdef") == 0, &src, def_map, 
			                 &ifdef_depth)) {
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

		if (replacement(dst_initial, &src, &dst_offset, &s, def_map)) {
			break;
		}
	}

	if (ifdef_depth) {
		fprintf(stderr, "insufficient number of `#endif`s.\n");
		exit(EXIT_FAILURE);
	}

	struct Tokvec u;
	u.tok_num = dst_offset + 1;
	u.v = dst_initial;

	return u;
}

static int replacement(struct Token *dst_initial, const struct Token **ptr_src,
                       int *ref_dst_offset, enum PreprocessorState *ptr_s,
                       struct Map2 *def_map)
{
	const struct Token *src = *ptr_src;
	int dst_offset = *ref_dst_offset;

	if (src[0].kind == NEWLINE || src[0].kind == BEGINNING) {
		*ptr_s = LINE_HAS_JUST_STARTED;
	} else if (src[0].kind == SPACE) {
		/* keep the state as is */
	} else {
		*ptr_s = NOTHING_SPECIAL;
	}

	struct Map2 *used_map = init_map();
	replace_recursively(def_map, used_map, src, &dst_initial[dst_offset]);

	if (dst_initial[dst_offset].kind == END) {
		*ptr_src = src;
		*ref_dst_offset = dst_offset;
		return 1; /* break */
	}

	dst_offset++;
	src++;
	*ptr_src = src;
	*ref_dst_offset = dst_offset;
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
