#include "intmap.h"
#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extend_vector_charptANDint(struct int_map *ptr);
struct charptANDint pop_vector_charptANDint(struct int_map *ptr);

void extend_vector_charptANDint(struct int_map *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector = realloc(ptr->vector, ptr->_allocated_length * 2 *
		                                       sizeof(struct charptANDint));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type charptANDint");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}


struct charptANDint pop_vector_charptANDint(struct int_map *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `struct charptANDint`" &&
		       0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}

/* it overrides; it does not overwrite. */
void insert(struct int_map *map_ptr, const char *key, int value)
{
	struct charptANDint a;
	a.ptr = key;
	a.value = value;
	extend_vector_charptANDint(map_ptr);
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
