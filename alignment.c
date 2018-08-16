#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

struct SizeAndAlignment {
	int size;
	int alignment;
};

struct SizeAndAlignment foo(const struct SizeAndAlignment *inner_type_vec,
                            int **ptr_offset_vec, int length)
{
	int *offset_vec = *ptr_offset_vec;
	struct SizeAndAlignment res;
	res.alignment = 1;
	for (int i = 0; i < length; i++) {
		if (inner_type_vec[i].alignment > res.alignment) {
			res.alignment = inner_type_vec[i].alignment;
		}
	}

	int offset = 0;
	offset_vec = calloc(length, sizeof(int));

	for (int i = 0; i < length; i++) {
		if (offset % inner_type_vec[i].alignment) {
			offset += inner_type_vec[i].alignment -
			          (offset % inner_type_vec[i].alignment);
		}
		offset_vec[i] = offset;
		offset += inner_type_vec[i].size;
	}

	if (offset % res.alignment) {
		offset += res.alignment - (offset % res.alignment);
	}

	res.size = offset;
	*ptr_offset_vec = offset_vec;
	return res;
}

int main()
{
	struct SizeAndAlignment *inner_type_vec =
	    calloc(4, sizeof(struct SizeAndAlignment));
	int *offset_vec;
	inner_type_vec[0].size = 4;
	inner_type_vec[0].alignment = 4;

	inner_type_vec[1].size = 1;
	inner_type_vec[1].alignment = 1;

	inner_type_vec[2].size = 1;
	inner_type_vec[2].alignment = 1;

	inner_type_vec[3].size = 4;
	inner_type_vec[3].alignment = 4;

	struct SizeAndAlignment ans = foo(inner_type_vec, &offset_vec, 4);
	printf("size: %d, alignment: %d\n", ans.size, ans.alignment);
	for (int i = 0; i < 4; i++) {
		printf("member #%d has offset: %d\n", i, offset_vec[i]);
	}
	printf("\n");

	inner_type_vec[0].size = 2;
	inner_type_vec[0].alignment = 2;

	inner_type_vec[1].size = 3;
	inner_type_vec[1].alignment = 1;

	ans = foo(inner_type_vec, &offset_vec, 2);
	printf("size: %d, alignment: %d\n", ans.size, ans.alignment);
	for (int i = 0; i < 2; i++) {
		printf("member #%d has offset: %d\n", i, offset_vec[i]);
	}
	printf("\n");

	inner_type_vec[0].size = 10;
	inner_type_vec[0].alignment = 1;

	inner_type_vec[1].size = 1;
	inner_type_vec[1].alignment = 1;

	ans = foo(inner_type_vec, &offset_vec, 2);
	printf("size: %d, alignment: %d\n", ans.size, ans.alignment);
	for (int i = 0; i < 2; i++) {
		printf("member #%d has offset: %d\n", i, offset_vec[i]);
	}
	printf("\n");

	inner_type_vec[0].size = 8;
	inner_type_vec[0].alignment = 8;

	inner_type_vec[1].size = 8;
	inner_type_vec[1].alignment = 8;

	inner_type_vec[2].size = 1;
	inner_type_vec[2].alignment = 1;

	ans = foo(inner_type_vec, &offset_vec, 3);
	printf("size: %d, alignment: %d\n", ans.size, ans.alignment);
	for (int i = 0; i < 3; i++) {
		printf("member #%d has offset: %d\n", i, offset_vec[i]);
	}
	printf("\n");

	return 0;
}
