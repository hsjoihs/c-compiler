#include "intmap.h"
#include <assert.h>
#include <stdio.h>

int main()
{
	struct int_map map = init_int_map();
	assert(GARBAGE_INT == lookup(map, "ffjkl"));
	insert(&map, "abc", 3);
	assert(3 == lookup(map, "kabc"+1));
	assert(3 == lookup(map, "QQabc"+2));
	insert(&map, "abc", 12);
	assert(12 == lookup(map, "QQabc"+2));
	assert(12 == lookup(map, "kabc"+1));
	insert(&map, "pqrs"+1, 45);
	assert(45 == lookup(map, "QQqrs"+2));
	assert(45 == lookup(map, "kqrs"+1));
	deletion(&map, "abc");
	assert(GARBAGE_INT == lookup(map, "abc"));
	assert(GARBAGE_INT == lookup(map, "ffjkl"));
	assert(45 == lookup(map, "QQqrs"+2));
	assert(45 == lookup(map, "kqrs"+1));

	fprintf(stderr, "\033[32mPASS\033[m\n");
	return 0;
}