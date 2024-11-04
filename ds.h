#ifndef DS_H
#define DS_H
#include <stddef.h>

#include "allocator.h"

#define ARRAY_HEADER(arr) ((ArrayHeader *)(void *)(arr) - 1)
#define ARRAY_BASE(arr) (ARRAY_HEADER((arr)))
#define ARRAY_PTR(ptr) ((void*)((unsigned char *)(ptr) + sizeof(ArrayHeader)))

typedef struct ArrayHeader ArrayHeader;
struct ArrayHeader {
	Allocator *allocator;
	size_t len;
	size_t cap;
	size_t item_size;
};

/*typedef unsigned char* Array;*/
void* array_init(size_t item_size, size_t initial_capacity, Allocator *alloc);
#define array_append(a, v) ( \
		(a) = array_inc(a), \
		(a)[ARRAY_HEADER(a)->len] = (v), \
		&(a)[ARRAY_HEADER(a)->len++])
size_t array_length(void *array);
size_t array_capacity(void *array);
// Not implemented
void* array_shrink_to_fit(void *array, size_t new_size);
void* array_resize(void *array, size_t new_size);
void dbg_metadata(void *arr);
#endif /* DS_H */
