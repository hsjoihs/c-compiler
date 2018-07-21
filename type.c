#include "header.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size_of(struct Type type)
{
	switch (type.type) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
	}
}

int is_equal(struct Type t1, struct Type t2)
{
	if (t1.type == INT_ && t2.type == INT_) {
		return 1;
	}

	if (t1.type == PTR_ && t2.type == PTR_) {
		return is_equal(*t1.pointer_of, *t2.pointer_of);
	}

	return 0;
}

void debug_print_type(struct Type type)
{
	switch (type.type) {
		case PTR_:
			debug_print_type(*type.pointer_of);
			fprintf(stderr, "*");
			return;
		case INT_:
			fprintf(stderr, "int");
			return;
	}
}
