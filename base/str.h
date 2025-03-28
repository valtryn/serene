#ifndef _STR_H
#define _STR_H

#include <stdlib.h>

#include "allocator.h"

#define STRING_HEADER(str) ((StringHeader *)(uintptr_t)(str) - 1)

typedef char* string;

typedef struct StringHeader StringHeader;
struct StringHeader {
	Allocator *alloc;
	size_t     len;
	// TODO: maybe add cap
	/* size_t     cap; */
};

void str_debug(string str);
string str_make(const char *cstr,  Allocator *allocator);
string str_clone(const string str, Allocator *allocator);
string str_clone_from_buf(const void *src, size_t len, Allocator *allocator);
string str_merge(const string lhs, const string rhs, Allocator *allocator);
size_t str_len(const string str);

// TODO: implement
string str_reverse(const string str);


//
// STRING ARRAY
//
typedef struct StringArray StringArray;
struct StringArray {
	Allocator *allocator;
	uintptr_t *nodes;
	size_t     len;
	size_t     cap;
};

void str_array_init(StringArray *array, size_t cap, Allocator *allocator);
void str_array_append(StringArray *array, const string str);
/* void str_array_append_move(StringArray *array, string str); */
string str_array_at(const StringArray *array, size_t index);

// NOTE: for debug only
void str_arr_debug(StringArray *array);

//
// STRING BUILDER
//
typedef struct StringBuilder StringBuilder;
struct StringBuilder {
	Allocator *allocator;
	char      *buffer;
	size_t     len;
	size_t     cap;
};

void str_builder_init(StringBuilder *builder, size_t initial_capacity, Allocator *allocator);
void str_builder_reset(StringBuilder *builder);
void str_builder_resize(StringBuilder *builder, size_t new_size);
string str_builder_to_string(StringBuilder *builder, Allocator *allocator);
void str_builder_write_string(StringBuilder *builder, const string src);
void str_write_byte(StringBuilder *builder, char c);

#endif // _STR_H
