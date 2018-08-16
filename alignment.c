#include "header.h"
#include <stdio.h>
#include <stdlib.h>

struct SizeAndAlignment
get_size_alignment_offsets(const struct SizeAndAlignment *inner_type_vec,
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
