#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*#include "arena.h"*/
#include "allocator.h"
#include "ds.h"
#include "str.h"

int main(void)
{
	Allocator arena = {0};
	allocator_init(&arena, ARENA_ALLOCATOR, megabytes(100));
	string str = str_make("hello, world!", &arena);
	StringArray arr = {0};
	str_array_init(&arr, 100, &arena);
	for (size_t i = 0; i < 10000000; i++) {
		str_array_append(str, &arr);
	}
	printf("cap: %zu\n", arr.cap);
	printf("len: %zu\n", arr.len);
	allocator_deinit(&arena);
	/*printf("string: %s\n", str1);*/
}
