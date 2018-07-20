#include "header.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size_of(struct Type type)
{
	switch (type.type) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
	}
}
