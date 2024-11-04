#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"
#include "allocator.h"

string str_init(size_t len, Allocator *alloc)
{
	string str = NULL;
	StringHeader *header = NULL;
	size_t header_size = sizeof(StringHeader);
	void *ptr = alloc->alloc(header_size + len + 1, alloc->ctx);
	if (!ptr)
		return NULL;
	str = (char *)ptr + header_size;
	header = STRING_HEADER(str);
	header->alloc = alloc;
	header->len = len;
	memset(str, 0, len);
	str[len] = '\0';
	return str;
}

string str_clone_from_buf(const void *src, size_t len, Allocator *alloc)
{
	if (!src)
		return NULL;

	string str = NULL;
	if (src && len) {
		str = str_init(len, alloc);
		if (str) {
			memcpy(str, src, len);
		}
	}
	return str;

}

string str_make(const char *cstr, Allocator *alloc)
{
	return str_clone_from_buf(cstr, strlen(cstr), alloc);
}

string str_merge(const string lhs, const string rhs, Allocator *alloc)
{
	string str = NULL;
	size_t total_len = str_len(lhs) + str_len(rhs);
	if (lhs && rhs) {
		str = str_init(total_len, alloc);
		memcpy(str, lhs, str_len(lhs));	
		memcpy(str + str_len(lhs), rhs, str_len(rhs));	
	}
	return str;
}

string str_clone(const string str, Allocator *alloc)
{
	return str_clone_from_buf(str, str_len(str), alloc);
}

void str_free(string str)
{
	if (!str)
		return;

	StringHeader *header = STRING_HEADER(str);
	if (header->alloc->type == GENERAL_PURPOSE_ALLOCATOR)
		header->alloc->free(header, header->alloc->ctx);
	else 
		printf("WARNING: Attempting to use 'str_free' on a non-GPA allocated string\n");
}

size_t str_len(const string str)
{
	return STRING_HEADER(str)->len;
}

void str_array_init(StringArray *array, size_t cap, Allocator *allocator)
{
	cap = (cap > 0) ? cap : 1;
	array->allocator = allocator;
	array->nodes = array->allocator->alloc(sizeof(uintptr_t) * cap, array->allocator->ctx);
	array->len = 0;
	array->cap = cap;

	if(array->allocator->type == ARENA_ALLOCATOR)
		printf("WARNING: using arena allocator in StringArray may fail if overfilled\n");
	return;
}

void str_array_append(string str, StringArray *arr)
{
	if (arr->len >= arr->cap) {
		if (arr->len == arr->cap) printf("same\n");
		size_t new_size = arr->cap * 2;
		if (arr->allocator->type == ARENA_ALLOCATOR) {
			arr->nodes = arena_resize(arr->nodes, arr->cap * sizeof(uintptr_t), new_size * sizeof(uintptr_t), arr->allocator->ctx);
			if (arr->nodes == NULL) {
				printf("ERROR: failed to append string: arena out of memory\n"); // TODO: create a proper error handling 
				return;
			}
		}
		if (arr->allocator->type == GENERAL_PURPOSE_ALLOCATOR)
			arr->nodes = arr->allocator->realloc(arr->nodes, new_size * sizeof(uintptr_t), arr->allocator->ctx); // TODO: handle realloc failure
		arr->cap = new_size;
	}
	arr->nodes[arr->len] = (uintptr_t)str_clone(str, arr->allocator);
	arr->len++;
}
