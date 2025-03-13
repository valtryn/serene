#ifndef DS_H
#define DS_H

#include "allocator.h"
#include <stdlib.h>

/*+----------------------------- DYNAMIC ARRAY ------------------------+*/

typedef struct Array Array;

struct Array {
	Allocator *allocator;
	void      *buffer;
	size_t     length;
	size_t     capacity;
	size_t     element_size;
};

void array_init(Array *array, size_t capacity, size_t element_size, Allocator *allocator);
void *array_at(Array *array, size_t index);
void array_append(Array *array, void *data);
void array_free(Array *array);
void array_remove_unordered(Array *array, size_t index);
#endif // DS_H
