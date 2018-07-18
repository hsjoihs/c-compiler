#include "intmap.h"
#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* it overrides; it does not overwrite. */
void insert(struct int_map *map_ptr, const char *key, int value)
{
	struct charptANDint a;
	a.ptr = key;
	a.value = value;

	if (map_ptr->_allocated_length < map_ptr->length + 1) {

		map_ptr->vector = realloc(map_ptr->vector, map_ptr->_allocated_length * 2 *
		                                       sizeof(struct charptANDint));

		if (!(map_ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type charptANDint");
			abort();
		}

		map_ptr->_allocated_length *= 2;
	}

	map_ptr->vector[map_ptr->length] = a;
	++(map_ptr->length);
}

/* returns garbage (0xCCCCCCCC == GARBAGE_INT) if not found. */
int lookup(const struct int_map map, const char *key)
{
	for (int i = (map.length) - 1; i >= 0; --i) {
		if (strcmp(map.vector[i].ptr, key) == 0) {
			return map.vector[i].value;
		}
	}
	return GARBAGE_INT;
}

void deletion(struct int_map *map_ptr, const char *key)
{
	insert(map_ptr, key, GARBAGE_INT);
}

struct int_map init_int_map(void)
{
	struct int_map res;
	res.length = 0;
	res._allocated_length = 256;
	res.vector = calloc(res._allocated_length, sizeof(struct charptANDint));
	return res;
}
