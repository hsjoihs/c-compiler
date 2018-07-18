#include "intmap.h"
#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _charptANDint {
	const char* ptr;
	int value;
};

/* it overrides; it does not overwrite. */
void insert(struct int_map *map_ptr, const char *key, int value)
{
	struct _charptANDint a;
	a.ptr = key;
	a.value = value;

	if (map_ptr->_allocated_length < map_ptr->_length + 1) {

		map_ptr->_internal =
		    realloc(map_ptr->_internal, map_ptr->_allocated_length * 2 *
		                                    sizeof(struct _charptANDint));

		map_ptr->_allocated_length *= 2;
	}

	map_ptr->_internal[map_ptr->_length] = a;
	++(map_ptr->_length);
}

/* returns garbage (0xCCCCCCCC == GARBAGE_INT) if not found. */
int lookup(const struct int_map map, const char *key)
{
	for (int i = (map._length) - 1; i >= 0; --i) {
		if (strcmp(map._internal[i].ptr, key) == 0) {
			return map._internal[i].value;
		}
	}
	return GARBAGE_INT;
}

int elem(const struct int_map map, const char *key)
{
	return lookup(map, key) != GARBAGE_INT;
}

void deletion(struct int_map *map_ptr, const char *key)
{
	insert(map_ptr, key, GARBAGE_INT);
}

struct int_map init_int_map(void)
{
	struct int_map res;
	res._length = 0;
	res._allocated_length = 256;
	res._internal = calloc(res._allocated_length, sizeof(struct _charptANDint));
	return res;
}
