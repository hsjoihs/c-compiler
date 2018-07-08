#include "lexer.h"
#include <assert.h>
#include <stdio.h>

void print_token(struct Token tok)
{
	if(tok.kind == OP_PLUS) {
		fprintf(stderr,"+");
	} else if(tok.kind == OP_MINUS) {
		fprintf(stderr,"-");
	} else if(tok.kind == OP_ASTERISK) {
		fprintf(stderr,"*");
	} else if(tok.kind == LEFT_PAREN) {
		fprintf(stderr,"(");
	} else if(tok.kind == RIGHT_PAREN) {
		fprintf(stderr,")");
	} else if(tok.kind == END) {
		fprintf(stderr,"DUMMY: END");
	} else if(tok.kind == LIT_DEC_INTEGER) {
		fprintf(stderr,"%d", tok.int_value);
	} else {
		assert("TOKEN KIND UNHANDLED" && 0);
	}
}

struct Token get_token(const char** ptr_to_str)
{
	const char* str = *ptr_to_str;
	struct Token t;
	t.int_value = GARBAGE_INT;

	if(*str == 0) { /* '\0' is 0 in C */
		t.kind = END;
		return t;
	}

	if(*str == '+') {
		t.kind = OP_PLUS;
		++*ptr_to_str;
		return t;
	} else if(*str == '-') {
		t.kind = OP_MINUS;
		++*ptr_to_str;
		return t;
	} else if(*str == '*') {
		t.kind = OP_ASTERISK;
		++*ptr_to_str;
		return t;
	} else if(*str == '(') {
		t.kind = LEFT_PAREN;
		++*ptr_to_str;
		return t;
	} else if(*str == ')') {
		t.kind = RIGHT_PAREN;
		++*ptr_to_str;
		return t;
	}

	if(!(*str >= '0' && *str <= '9')) {
		assert("Expected a numeral, but found something else" && 0);
	}

	t.kind = LIT_DEC_INTEGER;
	t.int_value = 0;
	do{
		if(*str >= '0' && *str <= '9'){ /* portable, since it is guaranteed that '0' - '9' are consecutive */
			t.int_value *= 10;
			t.int_value += *str - '0'; /* portable */
			++str;
		} else {
			*ptr_to_str = str;
			return t;
		}
	} while(1);
}