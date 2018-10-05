#include "file_io.h"
#include "std.h"

char *read_from_file(struct __FILE *fp)
{
	int size = 1;
	int capacity = 50000;
	char *str = calloc(capacity, sizeof(char));
	char buffer[1024];

	while (fgets(buffer, 1024, fp)) {
		size += strlen(buffer);
		if (size > capacity) {
			capacity *= 2;
			str = realloc(str, capacity);
		}
		strcat(str, buffer);
	}

	return str;
}
