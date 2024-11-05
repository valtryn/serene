#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "allocator.h"
#include "ds.h"
#include "str.h"

int main(void)
{
	Allocator arena = {0};
	allocator_init(&arena, GENERAL_PURPOSE_ALLOCATOR, megabytes(1));
	string str = str_make("Hello", &arena);
	string substr = str_make("H", &arena);
	int index = str_index(str, substr);
	printf("index: %d\n", index);
	gen_free_all(&arena);
	allocator_deinit(&arena);
}
