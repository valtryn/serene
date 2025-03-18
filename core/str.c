#include <stdio.h>
#include <string.h>

#include "str.h"
#include "allocator.h"

string str_init(size_t len, Allocator *allocator)
{
	StringHeader *hdr = NULL;
	string str = NULL;

	if (!allocator)
		goto out;

	void *ptr = allocator->alloc(sizeof(StringHeader) + len + 1, allocator->ctx);
	if (!ptr)
		goto out;

	str = (char*)ptr + sizeof(StringHeader);
	memset(str, 0, len + 1);

	hdr = STRING_HEADER(str);
	hdr->len = len;
	hdr->alloc = allocator;
out:
	return str;
}

string str_clone_from_buf(const void *src, size_t len, Allocator *allocator)
{
	string str = NULL;
	if (!src || len == 0 || !allocator)
		goto out;

	str = str_init(len, allocator);
	memcpy(str, src, len);
out:
	return str;
}

string str_make(const char *cstr, Allocator *allocator)
{
	return str_clone_from_buf(cstr, strlen(cstr), allocator);
}

string str_clone(const string str, Allocator *allocator)
{
	string clone = NULL;
	if (!str || !allocator) 
		goto out;

	clone = str_init(STRING_HEADER(str)->len, allocator);
	if (!clone)
		goto out;

	memcpy(clone, str, STRING_HEADER(str)->len);
out:
	return clone;
}

string str_merge(const string lhs, const string rhs, Allocator *allocator)
{
	string str = NULL;
	if (!lhs || !rhs || !allocator)
		goto out;

	size_t total_len = str_len(lhs) + str_len(rhs);
	str = str_init(total_len, allocator);
	if (!str)
		goto out;

	memcpy(str, lhs, str_len(lhs));
	memcpy(str + str_len(lhs), rhs, str_len(rhs));
out:
	return str;
}

size_t str_len(const string str)
{
	return (str) ? STRING_HEADER(str)->len : SIZE_MAX;
}

void str_debug(string str)
{
	if (!str) {
		printf("Invalid string\n");
		return;
	}
	StringHeader *hdr = STRING_HEADER(str);
	printf("data: %s\nlen: %zu\n", str, hdr->len);
}

//
// STRING ARRAY
//

void str_array_init(StringArray *array, size_t cap, Allocator *allocator)
{
	if (!array || cap == 0 || !allocator)
		return;

	array->nodes = allocator->alloc(sizeof(string) * cap, allocator->ctx);
	if (!array->nodes)
		return;

	array->allocator = allocator;
	array->len = 0;
	array->cap = cap;
	return;
}

void str_array_append(StringArray *array, const string str)
{
	if (!array || !str)
		return;

	if (array->len >= array->cap) {
		size_t new_cap = array->cap * 2;
		array->nodes = array->allocator->realloc(array->nodes, new_cap * sizeof(string), array->allocator->ctx);
		array->cap = new_cap;
	}

	array->nodes[array->len] = (uintptr_t)str_clone(str, array->allocator);
	array->len++;
	return;
}

string str_array_at(const StringArray *array, size_t index)
{
	return index < array->len ? (string)array->nodes[index] : NULL;
}

void str_arr_debug(StringArray *array)
{
	printf("cap: %zu | len: %zu\n", array->cap, array->len);
}

//
// STRING BUILDER
//
void str_builder_init(StringBuilder *builder, size_t initial_capacity, Allocator *allocator)
{
	if (!builder || initial_capacity == 0 || !allocator)
		return;

	builder->allocator = allocator;
	builder->buffer = allocator->alloc(sizeof(char) * initial_capacity, allocator->ctx);
	if (!builder->buffer)
		return;

	memset(builder->buffer, 0, initial_capacity);
	builder->cap = initial_capacity;
	builder->len = 0;
}

void str_builder_reset(StringBuilder *builder)
{
	builder->len = 0;
	return;
}


void str_builder_resize(StringBuilder *builder, size_t new_size)
{
	if (!builder)
		return;

	if (new_size > builder->cap) {
		builder->buffer = builder->allocator->realloc(builder->buffer, new_size, builder->allocator->ctx);
		if (!builder->buffer)
			return;

		builder->cap = new_size;
	}
	
	if (new_size <= builder->len)
		builder->len = new_size;

	return;
}

