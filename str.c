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
