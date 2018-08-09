
struct Map {
	int _length;
	int _alloc;
	struct _mapchip *_vec;
};

void insert(struct Map *map_ptr, const char *key, void *value);
void *lookup(const struct Map map, const char *key);
struct Map init_map(void);
int isElem(const struct Map map, const char *key);
void *getIth(const struct Map m, int i, const char **ptr_to_str);
