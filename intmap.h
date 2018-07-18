#include "header.h"

struct vector_charptANDint {
	int length;
	int _allocated_length;
	struct charptANDint* vector;
};
struct vector_charptANDint init_vector_charptANDint(int initial_length);
void extend_vector_charptANDint(struct vector_charptANDint* ptr);
void push_vector_charptANDint(struct vector_charptANDint* ptr, struct charptANDint tok);
struct charptANDint pop_vector_charptANDint(struct vector_charptANDint* ptr);


#define int_map vector_charptANDint
#define GARBAGE_INT 0xCCCCCCCC

void insert(struct int_map *map_ptr, const char *key, int value);
int lookup(const struct int_map map, const char *key);
void deletion(struct int_map *map_ptr, const char *key);
struct int_map init_int_map(void);
