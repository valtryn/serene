#ifndef STR_H

#include <stddef.h>
#include "allocator.h"

#define STR_H

#define STRING_HEADER(str) ((StringHeader *)(uintptr_t)(str) - 1)
#define STRING_BASE(str) (STRING_HEADER((str)))

typedef struct StringHeader StringHeader;
// TODO: implement slice
struct StringHeader {
	Allocator *alloc;
	size_t     len;
	// size_t cap; NOTE: maybe add a cap?
};
typedef char* string;

typedef struct StringArray StringArray;
struct StringArray {
	Allocator *allocator;
	uintptr_t *nodes;
	size_t     len;
	size_t     cap;
};

typedef struct StringBuilder StringBuilder;
struct StringBuilder {
	Allocator *allocator;
	char      *buffer;
	size_t     len;
	size_t     cap;
};

string str_make(const char *cstr,  Allocator *allocator);
string str_clone(const string str, Allocator *allocator);
string str_clone_from_buf(const void *src, size_t length, Allocator *allocator);
string str_merge(const string lhs, const string rhs, Allocator *allocator);
string str_join(const StringArray *array, const string separator, Allocator *allocator);
string str_concatenate(const StringArray *array, Allocator *allocator);
int    str_index(const string str, const string substr);
int    str_index_byte(const string str, const char byte);
void   str_free(string str);
size_t str_len(string str);
StringArray str_split(const string str, const string separator, Allocator *allocator) ;
//
// String Array
//
void   str_array_init(StringArray *array, size_t cap, Allocator *allocator);
void   str_array_append(StringArray *arr, string str);
void str_array_append_move(StringArray *arr, string str);
string str_array_at(const StringArray *array, size_t index);
//
// String Builder
//
void   str_builder_init(StringBuilder *builder, size_t initial_capacity, Allocator *allocator);
void   str_builder_reset(StringBuilder *builder);
void   str_builder_resize(StringBuilder *builder, size_t new_size);
string str_builder_to_string(StringBuilder *builder, Allocator *allocator);
void   str_write_string(StringBuilder *builder, string src); // TODO: maybe change str_builder prefix
void   str_write_byte(StringBuilder *builder, char c);
#endif /* STR_H */
