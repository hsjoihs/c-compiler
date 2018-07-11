#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_token(struct Token tok)
{
	switch (tok.kind) {
		case OP_PLUS: fprintf(stderr,"+"); break;
		case OP_MINUS: fprintf(stderr,"-"); break;
		case OP_ASTERISK: fprintf(stderr,"*"); break;
		case LEFT_PAREN: fprintf(stderr,"("); break;
		case RIGHT_PAREN: fprintf(stderr,")"); break;
		case END: fprintf(stderr,"DUMMY: END"); break;
		case OP_SLASH: fprintf(stderr,"/"); break;
		case OP_PERCENT: fprintf(stderr,"%%"); break;
		case OP_COMMA: fprintf(stderr,","); break;
		case OP_LT: fprintf(stderr,"<"); break;
		case OP_LT_EQ: fprintf(stderr,"<="); break;
		case OP_LSHIFT: fprintf(stderr,"<<"); break;
		case OP_GT: fprintf(stderr,">"); break;
		case OP_GT_EQ: fprintf(stderr,">="); break;
		case OP_RSHIFT: fprintf(stderr,">>"); break;
		case OP_AND: fprintf(stderr,"&"); break;
		case OP_OR: fprintf(stderr,"|"); break;
		case EMPTY: fprintf(stderr,"(whitespace)"); break;
		case OP_EQ_EQ: fprintf(stderr,"=="); break;
		case OP_NOT_EQ: fprintf(stderr,"!="); break;
		case OP_NOT: fprintf(stderr,"!"); break;
		case OP_TILDA: fprintf(stderr,"~"); break;
		case LIT_DEC_INTEGER: fprintf(stderr,"%d", tok.int_value); break;
	}
}

struct Token get_token(const char** ptr_to_str)
{
	const char* str = *ptr_to_str;
	struct Token t;
	t.int_value = GARBAGE_INT;

	if (*str == 0) { /* '\0' is 0 in C */
		t.kind = END;
		return t;
	}

	if (*str == ' ' 
	 || *str == '\t'
	 || *str == '\n'
	 || *str == '\v'
	 || *str == '\f'
	 || *str == '\r') {
		t.kind = EMPTY;
		++*ptr_to_str;
		return t;
	} 

	if (*str == '+') {
		t.kind = OP_PLUS;
		++*ptr_to_str;
		return t;
	} else if (*str == '-') {
		t.kind = OP_MINUS;
		++*ptr_to_str;
		return t;
	} else if (*str == '*') {
		t.kind = OP_ASTERISK;
		++*ptr_to_str;
		return t;
	} else if (*str == '(') {
		t.kind = LEFT_PAREN;
		++*ptr_to_str;
		return t;
	} else if (*str == ')') {
		t.kind = RIGHT_PAREN;
		++*ptr_to_str;
		return t;
	} else if (*str == '/') {
		t.kind = OP_SLASH;
		++*ptr_to_str;
		return t;
	} else if (*str == '%') {
		t.kind = OP_PERCENT;
		++*ptr_to_str;
		return t;
	} else if (*str == ',') {
		t.kind = OP_COMMA;
		++*ptr_to_str;
		return t;
	} else if (*str == '<') {
		switch (str[1]) {
			case '<':
				t.kind = OP_LSHIFT;
				*ptr_to_str += 2;
				return t;
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
				t.kind = OP_RSHIFT;
				*ptr_to_str += 2;
				return t;
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
			/*case '&':
				t.kind = OP_AND_AND;
				*ptr_to_str += 2;
				return t;
			case '=':
				t.kind = OP_AND_EQ;
				*ptr_to_str += 2;
				return t;*/
			default:
				t.kind = OP_AND;
				++*ptr_to_str;
				return t;
		}
	} else if (*str == '|') {
		switch (str[1]) {
			/*case '|':
				t.kind = OP_OR_OR;
				*ptr_to_str += 2;
				return t;
			case '=':
				t.kind = OP_OR_EQ;
				*ptr_to_str += 2;
				return t;*/
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
				assert("= unimplemented!!!" && 0);

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
	}

	if (!(*str >= '0' && *str <= '9')) {
		fprintf(stderr, 
			"Expected a numeral, but found something else: '%c' %d", 
			*str, 
			(int)*str
		);
		abort();
	}

	t.kind = LIT_DEC_INTEGER;
	t.int_value = 0;
	do {
		if (*str >= '0' && *str <= '9'){ /* portable, since it is guaranteed that '0' - '9' are consecutive */
			t.int_value *= 10;
			t.int_value += *str - '0'; /* portable */
			++str;
		} else {
			*ptr_to_str = str;
			return t;
		}
	} while (1);
}