#include "ds.h"
#include <stdio.h>
#include <string.h>

void array_init(Array *array, size_t element_size, size_t capacity, Allocator *allocator)
{
	array->allocator    = allocator;
	array->buffer       = allocator->alloc(element_size * capacity, allocator->ctx);
	array->length       = 0;
	array->capacity     = capacity;
	array->element_size = element_size;
}

void array_resize(Array *array, size_t new_size)
{
	printf("resized: %zu\n", new_size);
	if (new_size < array->length)
		array->length = new_size;

	if (new_size > array->capacity) {
		array->buffer = array->allocator->realloc(array->buffer, new_size * array->element_size, array->allocator->ctx);
		array->capacity = new_size;
	}
}

void array_append(Array *array, void *element)
{
	if (array->length >= array->capacity)
		array_resize(array, array->capacity * 2);
	unsigned char *curr_element = (unsigned char*)array->buffer + (array->length * array->element_size);
	memcpy(curr_element, element, array->element_size);
	array->length++;
}

void array_remove_unordered(Array *array, size_t index)
{
	// TODO: implement
	return;
}

void* array_at(Array *array, size_t index)
{
	return (unsigned char*)array->buffer + (index * array->element_size);
}
