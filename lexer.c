#include "file_io.h"
#include "header.h"
#include "std.h"
#include "std_io.h"
#include "global_flags.h"

static struct Token *concat_str_literals(const struct Tokvec *ref_v);
static struct Token get_token(const char **ptr_to_str);
static struct Tokvec remove_spaces_and_newlines(const struct Tokvec *ref_t);

struct Token *
read_and_preprocess(const char *str,
                    struct Vector /*<struct Token>*/ *ref_cmdline_macros)
{
	struct Map2 /*<struct Vector<struct Token>*/ *def_map = init_map();
	for (int i = 0; i < ref_cmdline_macros->length; i++) {
		struct Vector *p = init_vector_();

		struct Token *ptr_token = calloc(1, sizeof(struct Token));
		ptr_token->kind = LIT_DEC_INTEGER;
		ptr_token->int_value = 1;

		push_vector(p, ptr_token);

		insert(def_map, ref_cmdline_macros->vector[i], p);
	}
	struct Tokvec vec1 = preprocess(str, def_map);

	const struct Tokvec vec2 = remove_spaces_and_newlines(&vec1);

	return concat_str_literals(&vec2);
}

char *unescape(const char *str)
{
	char *ans = calloc(strlen(str) + 1, sizeof(char));
	int j = 0;
	for (int i = 0; str[i];) {
		if (str[i] != 92) {
			ans[j] = str[i];
			i++;
			j++;
		} else {
			switch (str[i + 1]) {
			case 92:
				ans[j] = 92;
				break;
			case 't':
				ans[j] = 9;
				break;
			case 'n':
				ans[j] = 10;
				break;
			case 'v':
				ans[j] = 11;
				break;
			case 'f':
				ans[j] = 12;
				break;
			case 'r':
				ans[j] = 13;
				break;
			case '"':
				ans[j] = '"';
				break;
			case 'e': {
				if (global_flag_pedantic) {
					fprintf(stderr, "escape sequence `\\e` is not in ISO C\n");
					exit(EXIT_FAILURE);
				} else {
					ans[j] = 0x1b;
					break;
				}
			}
			case 39:
				ans[j] = 39;
				break;
			default:
				fprintf(stderr, "unsupported: escape sequence `\\%c`\n",
				        str[i + 1]);
				exit(EXIT_FAILURE);
			}
			i += 2;
			j++;
		}
	}
	/* calloc means automatically null terminated */
	return ans;
}

char *escape(const char *str)
{
	char *ans =
	    calloc(strlen(str) * 4 /* length of "\\013" */ + 1, sizeof(char));
	int j = 0;
	for (int i = 0; str[i]; i++) {
		switch (str[i]) {
		case 92:
			ans[j] = 92;
			ans[j + 1] = 92;
			j += 2;
			break;
		case 9:
			ans[j] = 92;
			ans[j + 1] = 't';
			j += 2;
			break;
		case 10:
			ans[j] = 92;
			ans[j + 1] = 'n';
			j += 2;
			break;
		case 11: /* somehow \v fails */
			ans[j] = 92;
			ans[j + 1] = '0';
			ans[j + 2] = '1';
			ans[j + 3] = '3';
			j += 4;
			break;
		case 12:
			ans[j] = 92;
			ans[j + 1] = 'f';
			j += 2;
			break;
		case 13:
			ans[j] = 92;
			ans[j + 1] = 'r';
			j += 2;
			break;
		case '"':
			ans[j] = 92;
			ans[j + 1] = '"';
			j += 2;
			break;
		case 0x1b:
			ans[j] = 92;
			ans[j + 1] = '0';
			ans[j + 2] = '3';
			ans[j + 3] = '3';
			j += 4;
			break;
		/* single quote need not be re-escaped */
		default:
			ans[j] = str[i];
			j++;
			break;
		}
	}

	/* calloc means automatically null terminated */
	return ans;
}

static int hex_digit(char c);

void print_token(const struct Token *ptr_tok, const char *next_token_begins)
{
	if (ptr_tok->kind == END) {
		fprintf(stderr, "DUMMY: END");
		return;
	}
	if (ptr_tok->kind == BEGINNING) {
		fprintf(stderr, "DUMMY: BEGINNING");
		return;
	}
	for (int i = 0; i < next_token_begins - ptr_tok->token_begins_here; i++) {
		char c = ptr_tok->token_begins_here[i];
		if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
		    c == '\r') {
			break;
		}
		fprintf(stderr, "%c", c);
	}
}

void print_token_at(const struct Token *tokvec)
{
	print_token(&tokvec[0], tokvec[1].token_begins_here);
}

static struct Token get_token_raw(const char **ptr_to_str)
{
	const char *str = *ptr_to_str;
	struct Token t;
	t.ident_str = 0;
	t.literal_str = 0;
	t.token_begins_here = str;

	if (*str == 0) {
		t.kind = END;
		return t;
	}

	if (*str == ' ' || *str == '\t' || *str == '\v' || *str == '\f' ||
	    *str == '\r') {
		++*ptr_to_str;
		t.kind = SPACE;
		return t;
	}

	if (*str == '\n') {
		++*ptr_to_str;
		t.kind = NEWLINE;
		return t;
	}

	if (*str == '#') {
		++*ptr_to_str;
		t.kind = HASH;
		return t;
	}

	if (*str == '/' && str[1] == '*') {
		str += 2;
		while (*str != '*' || str[1] != '/') {
			++str;
		}
		str += 2;
		*ptr_to_str = str;
		return get_token(ptr_to_str);
	}

	if (*str == '/' && str[1] == '/') {
		str += 2;
		while (*str != '\n' && *str != 0) {
			str++;
		}
		*ptr_to_str = str;
		return get_token(ptr_to_str);
	}

	if (*str == 34) {
		int i = 0;
		++str;
		while (1) {
			if (str[i] == 92 && str[i + 1] == 92) {
				i += 2;
				continue;
			}
			if (str[i] == 92 && str[i + 1] == 34) {
				i += 2;
				continue;
			}
			if (str[i] == 34) {
				break;
			}
			i++;
		}
		int length = i;
		char *new_str = strndup(str, length);
		t.kind = LIT_STRING;
		t.literal_str = unescape(new_str);

		*ptr_to_str = str + length + 1;
		return t;
	}

	if (*str == 39) {
		int i = 0;
		++str;
		while (1) {
			if (str[i] == 92 && str[i + 1] == 92) {
				i += 2;
				continue;
			}
			if (str[i] == 92 && str[i + 1] == 39) {
				i += 2;
				continue;
			}
			if (str[i] == 39) {
				break;
			}
			i++;
		}
		int length = i;
		char *new_str = strndup(str, length);
		const char *unescaped = unescape(new_str);

		if (strlen(unescaped) > 1) {
			unsupported("more than one character in character literal");
		}

		t.kind = LIT_DEC_INTEGER;
		t.int_value = unescaped[0];
		*ptr_to_str = str + length + 1;
		return t;
	}

	if (*str == '+') {
		switch (str[1]) {
		case '=':
			t.kind = OP_PLUS_EQ;
			*ptr_to_str += 2;
			return t;
		case '+':
			t.kind = OP_PLUS_PLUS;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_PLUS;
			++*ptr_to_str;
			return t;
		}

	} else if (*str == '-') {
		switch (str[1]) {
		case '=':
			t.kind = OP_MINUS_EQ;
			*ptr_to_str += 2;
			return t;
		case '-':
			t.kind = OP_MINUS_MINUS;
			*ptr_to_str += 2;
			return t;
		case '>':
			t.kind = ARROW;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_MINUS;
			++*ptr_to_str;
			return t;
		}

	} else if (*str == '*') {
		switch (str[1]) {
		case '=':
			t.kind = OP_ASTERISK_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_ASTERISK;
			++*ptr_to_str;
			return t;
		}

	} else if (*str == '(') {
		t.kind = LEFT_PAREN;
		++*ptr_to_str;
		return t;
	} else if (*str == ')') {
		t.kind = RIGHT_PAREN;
		++*ptr_to_str;
		return t;
	} else if (*str == '/') {
		switch (str[1]) {
		case '=':
			t.kind = OP_SLASH_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_SLASH;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == '%') {
		switch (str[1]) {
		case '=':
			t.kind = OP_PERCENT_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_PERCENT;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == ',') {
		t.kind = OP_COMMA;
		++*ptr_to_str;
		return t;
	} else if (*str == '^') {
		switch (str[1]) {
		case '=':
			t.kind = OP_HAT_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_HAT;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == ';') {
		t.kind = SEMICOLON;
		++*ptr_to_str;
		return t;
	} else if (*str == '?') {
		t.kind = QUESTION;
		++*ptr_to_str;
		return t;
	} else if (*str == ':') {
		t.kind = COLON;
		++*ptr_to_str;
		return t;
	} else if (*str == '{') {
		t.kind = LEFT_BRACE;
		++*ptr_to_str;
		return t;
	} else if (*str == '}') {
		t.kind = RIGHT_BRACE;
		++*ptr_to_str;
		return t;
	} else if (*str == '<') {
		switch (str[1]) {
		case '<':
			switch (str[2]) {
			case '=':
				t.kind = OP_LSHIFT_EQ;
				*ptr_to_str += 3;
				return t;
			default:
				t.kind = OP_LSHIFT;
				*ptr_to_str += 2;
				return t;
			}
		case '=':
			t.kind = OP_LT_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_LT;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == '>') {
		switch (str[1]) {
		case '>':
			switch (str[2]) {
			case '=':
				t.kind = OP_RSHIFT_EQ;
				*ptr_to_str += 3;
				return t;
			default:
				t.kind = OP_RSHIFT;
				*ptr_to_str += 2;
				return t;
			}
		case '=':
			t.kind = OP_GT_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_GT;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == '&') {
		switch (str[1]) {
		case '&':
			t.kind = OP_AND_AND;
			*ptr_to_str += 2;
			return t;
		case '=':
			t.kind = OP_AND_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_AND;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == '|') {
		switch (str[1]) {
		case '|':
			t.kind = OP_OR_OR;
			*ptr_to_str += 2;
			return t;
		case '=':
			t.kind = OP_OR_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_OR;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == '=') {
		switch (str[1]) {
		case '=':
			t.kind = OP_EQ_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_EQ;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == '!') {
		switch (str[1]) {
		case '=':
			t.kind = OP_NOT_EQ;
			*ptr_to_str += 2;
			return t;
		default:
			t.kind = OP_NOT;
			++*ptr_to_str;
			return t;
		}
	} else if (*str == '~') {
		t.kind = OP_TILDA;
		++*ptr_to_str;
		return t;
	} else if (*str == '[') {
		t.kind = LEFT_BRACKET;
		++*ptr_to_str;
		return t;
	} else if (*str == ']') {
		t.kind = RIGHT_BRACKET;
		++*ptr_to_str;
		return t;
	} else if (*str == '.') {
		if (str[1] == '.' && str[2] == '.') {
			t.kind = TRIPLE_DOT;
			*ptr_to_str += 3;
			return t;
		} else {
			t.kind = DOT;
			++*ptr_to_str;
			return t;
		}
	}

	if (*str == '0') {
		t.kind = LIT_DEC_INTEGER;
		t.int_value = 0;
		if (str[1] == 'x' || str[1] == 'X') {
			str += 2;
			for (; hex_digit(*str) != -1; ++str) {
				t.int_value *= 16;
				t.int_value += hex_digit(*str);
			}
			*ptr_to_str = str;
			return t;
		} else if (str[1] == 'b' || str[1] == 'B') {
			if (global_flag_pedantic) {
				fprintf(stderr, "binary constants are in ISO C23, but not in ISO C17");
				exit(EXIT_FAILURE);
			} else {
				str += 2;
				for (; *str >= '0' && *str <= '1'; ++str) {
					t.int_value *= 2;
					t.int_value += *str - '0';
				}
				*ptr_to_str = str;
				return t;
			}
		} else {
			++str;
			/* octal */
			for (; *str >= '0' && *str <= '7'; ++str) {
				t.int_value *= 8;
				t.int_value += *str - '0'; /* portable */
			}
			*ptr_to_str = str;
			return t;
		}
	}

	if (*str >= '1' && *str <= '9') {
		t.kind = LIT_DEC_INTEGER;
		t.int_value = 0;
		/* portable, since it is guaranteed that
		                          '0' - '9' are consecutive */
		for (; *str >= '0' && *str <= '9'; ++str) {
			t.int_value *= 10;
			t.int_value += *str - '0'; /* portable */
		}
		*ptr_to_str = str;
		return t;
	}

	if (strchr("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", *str) !=
	    0) {
		t.kind = IDENT_OR_RESERVED;
		int i = 1;

		for (;; ++i) {
			if (str[i] == 0) {
				break;
			}
			if (strchr("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
			           "0123456789",
			           str[i]) == 0) {
				break;
			}
		}
		/*
		    identifier: str[0] ~ str[i-1]
		*/
		t.ident_str = strndup(str, i);
		*ptr_to_str = str + i;
		return t;
	}

	int charcode = *str;
	fprintf(stderr, "Found unexpected character: '%c' (%d)\n", *str, charcode);
	exit(EXIT_FAILURE);
}

static struct Token get_token(const char **ptr_to_str)
{
	struct Token t = get_token_raw(ptr_to_str);
	if (t.kind != IDENT_OR_RESERVED) {
		return t;
	}
	const char *new_str = t.ident_str;
	if (strcmp(new_str, "return") == 0) {
		t.kind = RES_RETURN;
	} else if (strcmp(new_str, "if") == 0) {
		t.kind = RES_IF;
	} else if (strcmp(new_str, "else") == 0) {
		t.kind = RES_ELSE;
	} else if (strcmp(new_str, "do") == 0) {
		t.kind = RES_DO;
	} else if (strcmp(new_str, "while") == 0) {
		t.kind = RES_WHILE;
	} else if (strcmp(new_str, "break") == 0) {
		t.kind = RES_BREAK;
	} else if (strcmp(new_str, "continue") == 0) {
		t.kind = RES_CONTINUE;
	} else if (strcmp(new_str, "for") == 0) {
		t.kind = RES_FOR;
	} else if (strcmp(new_str, "int") == 0) {
		t.kind = RES_INT;
	} else if (strcmp(new_str, "char") == 0) {
		t.kind = RES_CHAR;
	} else if (strcmp(new_str, "struct") == 0) {
		t.kind = RES_STRUCT;
	} else if (strcmp(new_str, "sizeof") == 0) {
		t.kind = RES_SIZEOF;
	} else if (strcmp(new_str, "void") == 0) {
		t.kind = RES_VOID;
	} else if (strcmp(new_str, "switch") == 0) {
		t.kind = RES_SWITCH;
	} else if (strcmp(new_str, "case") == 0) {
		t.kind = RES_CASE;
	} else if (strcmp(new_str, "default") == 0) {
		t.kind = RES_DEFAULT;
	} else if (strcmp(new_str, "_Alignof") == 0) {
		t.kind = RES_ALIGNOF;
	} else if (strcmp(new_str, "enum") == 0) {
		t.kind = RES_ENUM;
	} else if (strcmp(new_str, "const") == 0) {
		t.kind = RES_CONST;
	} else if (strcmp(new_str, "_Noreturn") == 0) {
		t.kind = RES_NORETURN;
	} else if (strcmp(new_str, "extern") == 0) {
		t.kind = RES_EXTERN;
	} else if (strcmp(new_str, "static") == 0) {
		t.kind = RES_STATIC;
	} else if (strcmp(new_str, "goto") == 0) {
		t.kind = RES_GOTO;
	} else if (strcmp(new_str, "union") == 0) {
		t.kind = RES_UNION;
	}
	return t;
}

static int hex_digit(char c)
{
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return c - '0';

	/* works for EBCDIC, too! */
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return c - 'a' + 10;

	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return c - 'A' + 10;

	default:
		return -1;
	}
}

static int count_all_tokens(const char *str);

static struct Tokvec remove_spaces_and_newlines(const struct Tokvec *ref_t)
{
	const struct Token *tokvec = ref_t->v;

	struct Token *tokvec_new = calloc(ref_t->tok_num, sizeof(struct Token));

	int j = 0;

	for (int k = 0;; k++) {
		if (tokvec[k].kind == NEWLINE || tokvec[k].kind == SPACE) {
			continue;
		}

		tokvec_new[j] = tokvec[k];

		if (tokvec_new[j].kind == END) {
			break;
		}

		j++;
	}
	struct Tokvec t;
	t.v = tokvec_new;
	t.tok_num = j + 1;
	return t;
}

static struct Token *concat_str_literals(const struct Tokvec *ref_v)
{
	struct Token *tokvec = ref_v->v;

	struct Token *tokvec_new = calloc(ref_v->tok_num, sizeof(struct Token));

	int j = 0;
	int k = 0;
	for (;; j++, k++) {
		tokvec_new[j] = tokvec[k];
		/* token_begins_here is also copied here */
		if (tokvec_new[j].kind == LIT_STRING) {
			while (tokvec[k + 1].kind == LIT_STRING) {
				int total_len = strlen(tokvec_new[j].literal_str) +
				                strlen(tokvec[k + 1].literal_str);
				char *new_str = calloc(total_len + 1, sizeof(char));
				strcpy(new_str, tokvec_new[j].literal_str);
				strcat(new_str, tokvec[k + 1].literal_str);
				tokvec_new[j].literal_str = new_str;
				k++;
			}
		}

		if (tokvec_new[j].kind == END) {
			break;
		}
	}
	return tokvec_new;
}

/* before END comes always a NEWLINE */
struct Tokvec read_all_tokens(const char *str)
{
	struct Token tok;
	int tok_num;
	{
		const char *str2 = str;
		tok_num = count_all_tokens(str2);
	}

	/* 1 additional for extra NEWLINE */
	struct Token *tokvec = calloc(tok_num + 1, sizeof(struct Token));

	tok.kind = BEGINNING;
	tok.ident_str = 0;
	tok.token_begins_here = str;

	tokvec[0] = tok;

	for (int i = 1;; ++i) {
		tok = get_token(&str);
		tokvec[i] = tok;

		if (tok.kind == END) {
			tokvec[i + 1] = tok;
			tokvec[i].kind = NEWLINE;
			break;
		}
	}

	struct Tokvec t;
	t.tok_num = tok_num + 1;
	t.v = tokvec;
	return t;
}

static int count_all_tokens(const char *str)
{
	struct Token tok;
	int count = 1;
	do {
		tok = get_token(&str);
		++count;
	} while (tok.kind != END);
	return count;
}
