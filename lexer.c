#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int from_hex(char c);

void read_all_tokens_debug(const char *str)
{
	struct Token tok;
	tok.kind = BEGINNING;
	tok.int_value = GARBAGE_INT;
	tok.ident_str = 0;
	tok.literal_str = 0;

	print_token(tok);
	fprintf(stderr, "\n");

	do {
		tok = get_token(&str);
		print_token(tok);
		fprintf(stderr, "\n");
		if (tok.kind == END) {
			break;
		}
	} while (1);
}

void print_token(struct Token tok)
{
	switch (tok.kind) {
		case OP_PLUS:
			fprintf(stderr, "+");
			break;
		case OP_MINUS:
			fprintf(stderr, "-");
			break;
		case OP_ASTERISK:
			fprintf(stderr, "*");
			break;
		case LEFT_PAREN:
			fprintf(stderr, "(");
			break;
		case RIGHT_PAREN:
			fprintf(stderr, ")");
			break;
		case END:
			fprintf(stderr, "DUMMY: END");
			break;
		case BEGINNING:
			fprintf(stderr, "DUMMY: BEGINNING");
			break;
		case OP_SLASH:
			fprintf(stderr, "/");
			break;
		case OP_PERCENT:
			fprintf(stderr, "%%");
			break;
		case OP_COMMA:
			fprintf(stderr, ",");
			break;
		case OP_LT:
			fprintf(stderr, "<");
			break;
		case OP_HAT:
			fprintf(stderr, "^");
			break;
		case OP_LT_EQ:
			fprintf(stderr, "<=");
			break;
		case OP_LSHIFT:
			fprintf(stderr, "<<");
			break;
		case OP_GT:
			fprintf(stderr, ">");
			break;
		case OP_GT_EQ:
			fprintf(stderr, ">=");
			break;
		case OP_RSHIFT:
			fprintf(stderr, ">>");
			break;
		case OP_AND:
			fprintf(stderr, "&");
			break;
		case OP_OR:
			fprintf(stderr, "|");
			break;
		case OP_EQ_EQ:
			fprintf(stderr, "==");
			break;
		case OP_NOT_EQ:
			fprintf(stderr, "!=");
			break;
		case OP_EQ:
			fprintf(stderr, "=");
			break;
		case OP_NOT:
			fprintf(stderr, "!");
			break;
		case OP_TILDA:
			fprintf(stderr, "~");
			break;
		case SEMICOLON:
			fprintf(stderr, ";");
			break;
		case COLON:
			fprintf(stderr, ":");
			break;
		case LEFT_BRACE:
			fprintf(stderr, "{");
			break;
		case RIGHT_BRACE:
			fprintf(stderr, "}");
			break;
		case QUESTION:
			fprintf(stderr, "?");
			break;
		case OP_AND_AND:
			fprintf(stderr, "&&");
			break;
		case OP_OR_OR:
			fprintf(stderr, "||");
			break;
		case OP_PLUS_EQ:
			fprintf(stderr, "+=");
			break;
		case OP_MINUS_EQ:
			fprintf(stderr, "-=");
			break;
		case OP_ASTERISK_EQ:
			fprintf(stderr, "*=");
			break;
		case OP_SLASH_EQ:
			fprintf(stderr, "/=");
			break;
		case OP_PERCENT_EQ:
			fprintf(stderr, "%%=");
			break;
		case OP_LSHIFT_EQ:
			fprintf(stderr, "<<=");
			break;
		case OP_RSHIFT_EQ:
			fprintf(stderr, ">>=");
			break;
		case OP_AND_EQ:
			fprintf(stderr, "&=");
			break;
		case OP_HAT_EQ:
			fprintf(stderr, "^=");
			break;
		case OP_OR_EQ:
			fprintf(stderr, "|=");
			break;
		case OP_PLUS_PLUS:
			fprintf(stderr, "++");
			break;
		case OP_MINUS_MINUS:
			fprintf(stderr, "--");
			break;
		case IDENT_OR_RESERVED:
			fprintf(stderr, "%s", tok.ident_str);
			break;
		case LIT_DEC_INTEGER:
			fprintf(stderr, "%d", tok.int_value);
			break;
		case RES_RETURN:
			fprintf(stderr, "return");
			break;
		case RES_IF:
			fprintf(stderr, "if");
			break;
		case RES_ELSE:
			fprintf(stderr, "else");
			break;
		case RES_DO:
			fprintf(stderr, "do");
			break;
		case RES_WHILE:
			fprintf(stderr, "while");
			break;
		case RES_BREAK:
			fprintf(stderr, "break");
			break;
		case RES_CONTINUE:
			fprintf(stderr, "continue");
			break;
		case RES_FOR:
			fprintf(stderr, "for");
			break;
		case RES_INT:
			fprintf(stderr, "int");
			break;
		case LEFT_BRACKET:
			fprintf(stderr, "[");
			break;
		case RIGHT_BRACKET:
			fprintf(stderr, "]");
			break;
		case RES_CHAR:
			fprintf(stderr, "char");
			break;
		case LIT_STRING:
			fprintf(stderr, "\"%s\"", tok.literal_str);
			break;
	}
}

struct Token get_token(const char **ptr_to_str)
{
	const char *str = *ptr_to_str;
	struct Token t;
	t.int_value = GARBAGE_INT;
	t.ident_str = 0;
	t.literal_str = 0;

	if (*str == 0) { /* '\0' is 0 in C */
		t.kind = END;
		return t;
	}

	if (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\v' ||
	    *str == '\f' || *str == '\r') {
		++*ptr_to_str;
		return get_token(ptr_to_str);
	}

	if (*str == '"') {
		int i = 0;
		++str;
		for (;; ++i) {
			if (str[i] == '\\') {
				unimplemented("escape sequence");
			}
			if (str[i] == '"') {
				break;
			}
		}
		int length = i;
		char *new_str = malloc(length + 1);
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
	}

	if (*str == '0') {
		t.kind = LIT_DEC_INTEGER;
		t.int_value = 0;
		if (str[1] == 'x' || str[1] == 'X') {
			str += 2;
			/* hexadecimal */
			do {
				if (from_hex(*str) != -1) {
					t.int_value *= 16;
					t.int_value += from_hex(*str);
					++str;
				} else {
					*ptr_to_str = str;
					return t;
				}
			} while (1);
		} else {
			++str;

			do {
				if (*str >= '0' &&
				    *str <= '7') { /* portable, since it is guaranteed
					                  that '0' - '9' are consecutive */
					t.int_value *= 8;
					t.int_value += *str - '0'; /* portable */
					++str;
				} else {
					*ptr_to_str = str;
					return t;
				}
			} while (1);
		}
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
		char *new_str =
		    malloc(i + 1); /* memory leak is much better than use after free */
		if (!new_str) {
			fprintf(stderr, "memory ran out\n");
			exit(EXIT_FAILURE);
		}

		for (int j = 0; j < i; j++) {
			new_str[j] = str[j];
		}
		new_str[i] = '\0';

		if (strcmp(new_str, "return") == 0) {
			t.kind = RES_RETURN;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "if") == 0) {
			t.kind = RES_IF;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "else") == 0) {
			t.kind = RES_ELSE;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "do") == 0) {
			t.kind = RES_DO;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "while") == 0) {
			t.kind = RES_WHILE;
			t.int_value = GARBAGE_INT;

		} else if (strcmp(new_str, "break") == 0) {
			t.kind = RES_BREAK;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "continue") == 0) {
			t.kind = RES_CONTINUE;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "for") == 0) {
			t.kind = RES_FOR;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "int") == 0) {
			t.kind = RES_INT;
			t.int_value = GARBAGE_INT;
		} else if (strcmp(new_str, "char") == 0) {
			t.kind = RES_CHAR;
			t.int_value = GARBAGE_INT;
		} else {
			t.ident_str = new_str;
		}
		*ptr_to_str = str + i;
		return t;
	}

	fprintf(stderr, "Found unexpected character: '%c' (%d)\n", *str, (int)*str);
	exit(EXIT_FAILURE);
}

int from_hex(char c)
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

struct Token *read_all_tokens_(const char *str)
{
	struct Token tok;
	int tok_num;
	{
		const char *str2 = str;
		tok_num = count_all_tokens(str2);
	}

	struct Token *tokvec = calloc(tok_num, sizeof(struct Token));

	tok.kind = BEGINNING;
	tok.int_value = GARBAGE_INT;
	tok.ident_str = 0;

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

int count_all_tokens(const char *str)
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
