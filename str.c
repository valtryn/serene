#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"
#include "allocator.h"

string str_init(size_t len, Allocator *allocator)
{
	string str = NULL;
	StringHeader *header = NULL;

	size_t header_size = sizeof(StringHeader);
	void *ptr = allocator->alloc(header_size + len + 1, allocator->ctx);
	if (!ptr)
		return NULL;
	str = (char *)ptr + header_size;
	header = STRING_HEADER(str);
	header->alloc = allocator;
	header->len = len;
	memset(str, 0, len);
	str[len] = '\0';
	return str;
}

void str_set_header(string str, size_t len, Allocator *allocator)
{
	StringHeader *hdr = STRING_HEADER(str);
	hdr->alloc = allocator;
	hdr->len   = len;
	// hdr->cap   = cap; TODO: uncomment when capacity is supported
}

string str_clone_from_buf(const void *src, size_t len, Allocator *allocator)
{
	if (!src)
		return NULL;

	string str = NULL;
	if (src) {
		str = str_init(len, allocator);
		if (str)
			memcpy(str, src, len);
	}
	return str;

}

string str_make(const char *cstr, Allocator *allocator)
{
	return str_clone_from_buf(cstr, strlen(cstr), allocator);
}

string str_merge(const string lhs, const string rhs, Allocator *allocator)
{
	string str = NULL;
	size_t total_len = str_len(lhs) + str_len(rhs);
	if (lhs && rhs) {
		str = str_init(total_len, allocator);
		memcpy(str, lhs, str_len(lhs));	
		memcpy(str + str_len(lhs), rhs, str_len(rhs));	
	}
	return str;
}

string str_clone(const string str, Allocator *allocator)
{
	return str_clone_from_buf(str, str_len(str), allocator);
}

string str_join(const StringArray *array, const string separator, Allocator *allocator)
{
	if (!array || !allocator) return NULL;
	StringBuilder builder = {0};
	size_t total_length   = 0;
	if (separator) total_length += (str_len(separator) * array->len) - str_len(separator);

	for (size_t i = 0; i < array->len; i++) total_length += str_len(str_array_at(array, i));
	str_builder_init(&builder, total_length, allocator);
	for (size_t i = 0; i < array->len; i++) {
		    str_write_string(&builder, str_array_at(array, i));
		    if (i < array->len - 1 && separator)
			    str_write_string(&builder, separator);
	}
	return str_builder_to_string(&builder, allocator);
}

string str_concatenate(const StringArray *array, Allocator *allocator)
{
	return str_join(array, NULL, allocator);
}

int str_index(const string str, const string substr)
{
	if (!str || !substr || str_len(str) < str_len(substr))
		return -1;

	if (str_len(substr) == 1)
		return str_index_byte(str, substr[0]);

	for (size_t i = 0; i <= str_len(str) - str_len(substr); i++) {
		if (str[i] == substr[0]) {
			if (memcmp(str + i, substr, str_len(substr)) == 0)
				return i;
		}

	}
	return -1;
}

int str_index_byte(const string str, const char byte)
{
	if (!str) return -1;
	for (size_t i = 0; i < str_len(str); i++) {
		if (str[i] == byte)
			return i;
	}
	return -1;
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

void str_array_append(StringArray *arr, string str)
{
	if (arr->len >= arr->cap) {
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

string str_array_at(const StringArray *array, size_t index)
{
	return index < array->len ? (string)array->nodes[index] : NULL;
}

void str_builder_init(StringBuilder *builder, size_t initial_capacity, Allocator *allocator)
{
	builder->allocator = allocator;
	builder->buffer    = allocator->alloc(initial_capacity, allocator->ctx);
	builder->len       = 0;
	builder->cap       = initial_capacity <= 0 ? 1 : initial_capacity;
	return;
}

void str_write_string(StringBuilder *builder, string src)
{
	if (!src) return;
	if (builder->len + str_len(src) > builder->cap) {
		size_t new_cap = (builder->cap * 2) + str_len(src);
		str_builder_resize(builder, new_cap);
	}
	memcpy(builder->buffer + builder->len, src, str_len(src));
	builder->len += str_len(src);
}

void str_builder_reset(StringBuilder *builder)
{
	builder->len = 0;
}

void str_builder_resize(StringBuilder *builder, size_t new_size)
{
	if (new_size > builder->cap) {
		// TODO: implement realloc function pointer for arena
		if (builder->allocator->type == GENERAL_PURPOSE_ALLOCATOR)
			builder->buffer = builder->allocator->realloc(builder->buffer, new_size, builder->allocator->ctx); // TODO: handle realloc failure
		if (builder->allocator->type == ARENA_ALLOCATOR)
			builder->buffer = arena_resize(builder->buffer, builder->cap, new_size, builder->allocator->ctx);
		builder->cap = new_size;
	} else 
		builder->len = new_size;

}

// TODO: not sure if we should request a new allocator or use the existing builder allocator
string str_builder_to_string(StringBuilder *builder, Allocator *allocator)
{
	string ret = str_init(builder->len, allocator);
	memcpy(ret, builder->buffer, builder->len);
	str_set_header(ret, builder->len, allocator);
	return ret;
}
