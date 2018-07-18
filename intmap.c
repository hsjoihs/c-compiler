#include "intmap.h"
#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vector_charptANDint init_vector_charptANDint(int initial_length)
{
	struct vector_charptANDint res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(struct charptANDint));
	return res;
}

void extend_vector_charptANDint(struct vector_charptANDint *ptr)
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

void push_vector_charptANDint(struct vector_charptANDint *ptr,
                              struct charptANDint tok)
{
	extend_vector_charptANDint(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

struct charptANDint pop_vector_charptANDint(struct vector_charptANDint *ptr)
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
	push_vector_charptANDint(map_ptr, a);
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

struct int_map init_int_map(void) { return init_vector_charptANDint(0); }
