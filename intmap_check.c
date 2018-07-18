#include "map.h"
#include <assert.h>
#include <stdio.h>

int main()
{
	struct map map = init_int_map();
	assert(!isElem(map, "ffjkl"));
	insert(&map, "abc", (void *)3);
	assert((void *)3 == lookup(map, "kabc" + 1));
	assert((void *)3 == lookup(map, "QQabc" + 2));
	insert(&map, "abc", (void *)12);
	assert((void *)12 == lookup(map, "QQabc" + 2));
	assert((void *)12 == lookup(map, "kabc" + 1));
	insert(&map, "pqrs" + 1, (void *)45);
	assert((void *)45 == lookup(map, "QQqrs" + 2));
	assert((void *)45 == lookup(map, "kqrs" + 1));
	deletion(&map, "abc");
	assert(!isElem(map, "abc"));
	assert(!isElem(map, "ffjkl"));
	assert((void *)45 == lookup(map, "QQqrs" + 2));
	assert((void *)45 == lookup(map, "kqrs" + 1));

	fprintf(stderr, "\033[32mPASS\033[m\n");
	return 0;
}