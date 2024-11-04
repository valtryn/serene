#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds.h"
#include "allocator.h"

void* array_init(size_t item_size, size_t initial_capacity, Allocator *allocator)
{
	void *ptr = NULL;
	ArrayHeader *hdr = NULL;
	size_t hdr_size = sizeof(ArrayHeader);
	ptr = allocator->alloc(hdr_size + (initial_capacity * item_size), allocator->ctx);
	if (ptr) {
		ptr = (unsigned char *)ptr + hdr_size;
		hdr = ARRAY_HEADER(ptr);
		hdr->allocator = allocator;
		hdr->cap = initial_capacity;
		hdr->len = 0;
		hdr->item_size = item_size;
		memset(ptr, 0, initial_capacity * item_size);
	}
	return ptr;
}

// NOTE: size is the added size not total
void* array_grow_gpa(void *array, size_t size)
{
	if (!array) return NULL;

	ArrayHeader *new_hdr = NULL;

	ArrayHeader *old_hdr = ARRAY_HEADER(array);
	size_t hdr_size = sizeof(ArrayHeader);
	size_t new_cap = old_hdr->cap + size;
	size_t new_size = hdr_size + (new_cap * old_hdr->item_size);

	void *ptr = old_hdr->allocator->alloc(new_size, old_hdr->allocator->ctx);
	if (!ptr) return NULL;
	ptr = (unsigned char *)ptr + hdr_size;
	new_hdr = ARRAY_HEADER(ptr);
	*new_hdr = *old_hdr;
	new_hdr->cap = old_hdr->cap + size;
	memcpy(ptr, array, old_hdr->len * old_hdr->item_size);
	memset( 
			(unsigned char *)ptr + old_hdr->len * old_hdr->item_size,
			0,
			((old_hdr->cap + size) - old_hdr->len) * old_hdr->item_size
	      );
	old_hdr->allocator->free(old_hdr, old_hdr->allocator->ctx);
	return ptr;
}

void* array_grow(void *array, size_t size)
{
	ArrayHeader *hdr = ARRAY_HEADER(array);
	if (hdr->allocator->type == GPA)
		return array_grow_gpa(array, size);
	if (hdr->allocator->type == ARENA) {
		size_t old_size = hdr->cap;
		size_t new_size = hdr->cap + size;
		void *new_ptr = arena_resize(ARRAY_BASE(array), old_size, new_size + sizeof(ArrayHeader), hdr->allocator->ctx);
		new_ptr = (unsigned char*)new_ptr + sizeof(ArrayHeader);
		ArrayHeader *new_hdr = ARRAY_HEADER(new_ptr);
		new_hdr->allocator = hdr->allocator;
		new_hdr->len = hdr->len;
		new_hdr->cap = new_size;
		new_hdr->item_size = hdr->item_size;
		memcpy(new_ptr, array, hdr->len);
		return new_ptr;
	}
	return NULL;
}

void* array_resize(void *array, size_t new_capacity)
{
	if (!array)
		return NULL;

	ArrayHeader *hdr = ARRAY_HEADER(array);

	if (new_capacity == hdr->cap)
		return array;

	// Shrink
	if (new_capacity < hdr->cap) {
		if (new_capacity < hdr->len)
			hdr->len = new_capacity;
		return array;
	}

	// Grow
	if (new_capacity > hdr->cap) {
		size_t growth = new_capacity - hdr->cap;
		return array_grow(array, growth);
	}

	return NULL;
}

void* array_inc(void *array)
{
	ArrayHeader *hdr = ARRAY_HEADER(array);
	if (hdr->len >= hdr->cap) {
		size_t new_size = (hdr->cap == 0) ? 1 : hdr->cap * 2;
		/*printf("size: %zu\n", new_size);*/
		void *ptr = array_resize(array, new_size * hdr->item_size);
		hdr = ARRAY_HEADER(ptr) + 1;
	} else {
		hdr += 1;
	}
	return hdr;
}

inline size_t array_length(void *array)
{
	return ARRAY_HEADER(array)->len;
}

inline size_t array_capacity(void *array)
{
	return ARRAY_HEADER(array)->cap;
}

void array_free(void *arr)
{
	if (!arr)
		return;
	ArrayHeader *hdr = ARRAY_HEADER(arr);
	if (hdr->allocator->type== GPA)
		hdr->allocator->free(hdr, hdr->allocator->ctx);
	else
		printf("WARNING: Attempting to use 'array_free' on a non-GPA allocated array\n");
}

void dbg_metadata(void *arr)
{
	printf("\n+-------------------------+\n");
	ArrayHeader *hdr = ARRAY_HEADER(arr);
	/*printf("allocator type: %s\n", (hdr->allocator->type == ARENA) ? "arena" : "gpa");*/
	printf("len: %zu\n", hdr->len);
	printf("cap: %zu\n", hdr->cap);
	printf("item_size: %zu\n", hdr->item_size);
	printf("+-------------------------+\n\n");
}

#include "str.h"

int main(void)
{
	Arena arena = {0};
	size_t size = megabytes(100);
	void *buf = malloc(size);
	arena_init(&arena, buf, size);
	Allocator allocator = allocator_init(GPA, NULL);
	String *arr = allocator.alloc(sizeof(String) * 1, allocator.ctx);
	for (size_t i = 0; i < 100; i++) arr[i] = str_make("Hello", &allocator);
	/*arr[0] = str_make("Hello", &allocator);*/
	dbg_metadata(arr);
	allocator.free(ARRAY_BASE(arr), allocator.ctx);
}
