struct charptANDint {
	const char* ptr;
	int value;
};

struct int_map {
	int _length;
	int _allocated_length;
	struct charptANDint* _internal;
};

#define GARBAGE_INT 0xCCCCCCCC

void insert(struct int_map *map_ptr, const char *key, int value);
int lookup(const struct int_map map, const char *key);
void deletion(struct int_map *map_ptr, const char *key);
struct int_map init_int_map(void);
int elem(const struct int_map map, const char *key);
