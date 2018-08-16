#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
		for (;; ++i) {
			if (str[i] == '\\') {
				unsupported("escape sequence");
			}
			if (str[i] == '"') {
				break;
			}
		}
		int length = i;
		char *new_str = calloc(length + 1, sizeof(char));
		for (int j = 0; j < length; j++) {
			new_str[j] = str[j];
		}
		new_str[length] = 0;
		t.kind = LIT_STRING;
		t.literal_str = new_str;

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
	    NULL) {
		t.kind = IDENT_OR_RESERVED;
		int i = 1;

		for (;; ++i) {
			if (strchr("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
			           "0123456789",
			           str[i]) == NULL) {
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
		new_str[i] = '\0';

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
	int i = 0;
	while (1) {
		tok = get_token(&str);
		++i;
		tokvec[i] = tok;
		if (tok.kind == END) {
			break;
		}
	}
	return tokvec;
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
