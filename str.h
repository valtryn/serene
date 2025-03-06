#ifndef _STR_H
#define _STR_H

#include <stdlib.h>

#include "allocator.h"

#define STRING_HEADER(str) ((StringHeader *)(uintptr_t)(str) - 1)

typedef struct StringHeader StringHeader;
struct StringHeader {
	Allocator *alloc;
	size_t     len;
	// TODO: maybe add cap
	/* size_t     cap; */
};
typedef char* string;

void str_debug(string str);
string str_make(const char *cstr,  Allocator *allocator);
string str_clone(const string str, Allocator *allocator);
string str_clone_from_buf(const void *src, size_t len, Allocator *allocator);
string str_merge(const string lhs, const string rhs, Allocator *allocator);
size_t str_len(const string str);

#endif // _STR_H
