#include "header.h"
#include "std.h"
#include "std_io.h"

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
	char *ans = calloc(strlen(str) * 4 + 1, sizeof(char));
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
			default:
				ans[j] = str[i];
				j++;
				break;
		}
	}

	/* calloc means automatically null terminated */
	return ans;
}

void read_all_tokens_debug(const char *str)
{
	struct Token tok;

	do {
		tok = get_token(&str);
		if (tok.kind == END) {
			break;
		}
		print_token(tok, str);
		fprintf(stderr, "\n");
	} while (1);
}

void print_token(struct Token tok, const char *next_token_begins)
{
	if (tok.kind == END) {
		fprintf(stderr, "DUMMY: END");
		return;
	}
	if (tok.kind == BEGINNING) {
		fprintf(stderr, "DUMMY: BEGINNING");
		return;
	}
	for (int i = 0; i < next_token_begins - tok.token_begins_here; i++) {
		char c = tok.token_begins_here[i];
		if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
		    c == '\r') {
			break;
		}
		fprintf(stderr, "%c", c);
	}
}

static struct Token get_token_raw(const char **ptr_to_str)
{
	const char *str = *ptr_to_str;
	struct Token t;
	t.ident_str = 0;
	t.literal_str = 0;
	t.token_begins_here = str;

	if (*str == 0) { /* '\0' is 0 in C */
		t.kind = END;
		return t;
	}

	if (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\v' ||
	    *str == '\f' || *str == '\r') {
		++*ptr_to_str;
		return get_token(ptr_to_str);
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

	if (*str == '"') {
		int i = 0;
		++str;
		while (1) {
			if (str[i] == '\\' && str[i + 1] == '\\') {
				i += 2;
				continue;
			}
			if (str[i] == '\\' && str[i + 1] == '"') {
				unsupported("escape sequence of double quote");
			}
			if (str[i] == '"') {
				break;
			}
			i++;
		}
		int length = i;
		char *new_str = calloc(length + 1, sizeof(char));
		for (int j = 0; j < length; j++) {
			new_str[j] = str[j];
		}
		new_str[length] = 0;
		t.kind = LIT_STRING;
		t.literal_str = unescape(new_str);

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
		t.kind = DOT;
		++*ptr_to_str;
		return t;
	}

	if (*str == '0') {
		t.kind = LIT_DEC_INTEGER;
		t.int_value = 0;

		++str;

		*ptr_to_str = str;
		return t;
	}

	if (*str >= '1' && *str <= '9') {
		t.kind = LIT_DEC_INTEGER;
		t.int_value = 0;
		do {
			if (*str >= '0' &&
			    *str <= '9') { /* portable, since it is guaranteed that
				                  '0' - '9' are consecutive */
				t.int_value *= 10;
				t.int_value += *str - '0'; /* portable */
				++str;
			} else {
				*ptr_to_str = str;
				return t;
			}
		} while (1);
	}

	if (strchr("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", *str) !=
	    0) {
		t.kind = IDENT_OR_RESERVED;
		int i = 1;

		for (;; ++i) {
			if (strchr("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
			           "0123456789",
			           str[i]) == 0) {
				break;
			}
		}
		/*
		    identifier: str[0] ~ str[i-1]
		*/
		char *new_str = calloc(i + 1, sizeof(char));
		if (!new_str) {
			fprintf(stderr, "memory ran out\n");
			exit(EXIT_FAILURE);
		}

		for (int j = 0; j < i; j++) {
			new_str[j] = str[j];
		}
		new_str[i] = 0;

		t.ident_str = new_str;

		*ptr_to_str = str + i;
		return t;
	}

	fprintf(stderr, "Found unexpected character: '%c' (%d)\n", *str, (int)*str);
	exit(EXIT_FAILURE);
}

struct Token get_token(const char **ptr_to_str)
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
	}
	return t;
}

static int count_all_tokens(const char *str);

struct Token *read_all_tokens(const char *str)
{
	struct Token tok;
	int tok_num;
	{
		const char *str2 = str;
		tok_num = count_all_tokens(str2);
	}

	struct Token *tokvec = calloc(tok_num, sizeof(struct Token));

	tok.kind = BEGINNING;
	tok.ident_str = 0;
	tok.token_begins_here = str;

	tokvec[0] = tok;

	for (int i = 1;; ++i) {
		tok = get_token(&str);
		tokvec[i] = tok;

		if (tok.kind == END) {
			break;
		}
	}

	struct Token *tokvec_new = calloc(tok_num, sizeof(struct Token));

	int j = 0;
	int k = 0;
	for (;; j++, k++) {
		tokvec_new[j] = tokvec[k];
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

		if (tokvec[j].kind == END) {
			break;
		}
	}
	return tokvec_new;
}

static int count_all_tokens(const char *str)
{
	struct Token tok;
	int count = 1;

	while (1) {
		tok = get_token(&str);
		++count;
		if (tok.kind == END) {
			break;
		}
	}
	return count;
}
