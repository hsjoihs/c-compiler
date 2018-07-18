struct int_map {
	int _length;
	int _allocated_length;
	struct _charptANDint* _internal;
};

void insert(struct int_map *map_ptr, const char *key, void *value);
void *lookup(const struct int_map map, const char *key);
void deletion(struct int_map *map_ptr, const char *key);
struct int_map init_int_map(void);
int isElem(const struct int_map map, const char *key);
