#ifndef STR_H

#include <stddef.h>
#include "allocator.h"

#define STR_H

typedef char* string;
#define STRING_HEADER(str) ((StringHeader *)(uintptr_t)(str) - 1)
#define STRING_BASE(str) (STRING_HEADER((str)))

typedef struct StringHeader StringHeader;
struct StringHeader {
	Allocator *alloc;
	size_t len;
	// size_t cap; NOTE: maybe add a cap?
};

typedef struct StringArray StringArray;
struct StringArray {
	Allocator *allocator;
	uintptr_t *nodes;
	size_t len;
	size_t cap;
};


string str_make(const char *cstr, Allocator *alloc);
string str_clone_from_buf(const void *src, size_t length, Allocator *alloc);
string str_merge(const string lhs, const string rhs, Allocator *alloc);
string str_clone(const string str, Allocator *alloc);
string str_join(const StringArray *array, const string separator);
void   str_free(string str);
size_t str_len(string str);

//
// String Array
//
void str_array_init(StringArray *array, size_t cap, Allocator *allocator);
void str_array_append(string str, StringArray *arr);

#endif /* STR_H */
