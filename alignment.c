#include "header.h"
#include "std.h"

struct SizeAndAlignment get_size_alignment_offsets_for_union(
    const struct SizeAndAlignment *inner_type_vec, int **ptr_offset_vec,
    int length)
{
	struct SizeAndAlignment res;
	res.alignment = 1;

	/* union's alignment is the maximum of the members' alignments */
	for (int i = 0; i < length; i++) {
		if (inner_type_vec[i].alignment > res.alignment) {
			res.alignment = inner_type_vec[i].alignment;
		}
	}

	int max_size_of_member = 1;

	/* union's size is AT LEAST the maximum of the members' sizes */
	for (int i = 0; i < length; i++) {
		if (inner_type_vec[i].size > max_size_of_member) {
			max_size_of_member = inner_type_vec[i].size;
		}
	}

	if (max_size_of_member % res.alignment) {
		max_size_of_member +=
		    res.alignment - (max_size_of_member % res.alignment);
	}

	res.size = max_size_of_member;

	/* calloc; all is zero */
	int *offset_vec = calloc(length, sizeof(int));
	*ptr_offset_vec = offset_vec;
	return res;
}

struct SizeAndAlignment get_size_alignment_offsets_for_struct_not_union(
    const struct SizeAndAlignment *inner_type_vec, int **ptr_offset_vec,
    int length)
{
	struct SizeAndAlignment res;
	res.alignment = 1;

	/* struct's alignment is the maximum of the members' alignments */
	for (int i = 0; i < length; i++) {
		if (inner_type_vec[i].alignment > res.alignment) {
			res.alignment = inner_type_vec[i].alignment;
		}
	}

	int offset = 0;
	int *offset_vec = calloc(length, sizeof(int));

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
