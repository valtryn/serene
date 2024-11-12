#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "allocator.h"
#include "stdlib.h"

void allocator_init(Allocator *allocator, enum AllocatorType type, size_t initial_size)
{
	switch (type) {
		case ARENA_ALLOCATOR: {
				    Arena *arena = malloc(sizeof(Arena));
				    void *buf = malloc(initial_size);
				    arena_init(arena, buf, initial_size);
				    allocator->alloc   = arena_alloc;
				    allocator->free    = arena_free;
				    allocator->realloc = arena_resize; // TODO: support realloc
				    allocator->ctx     = arena;
				    allocator->type    = type;
				    break;
			    }
		case GENERAL_PURPOSE_ALLOCATOR :   {
				    GenAlloc *ga = malloc(sizeof(GenAlloc));
				    gen_init(ga, initial_size); // TODO: not sure if we should use initial_size here
				    allocator->alloc   = gen_alloc;
				    allocator->free    = gen_free;
				    allocator->realloc = gen_realloc;
				    allocator->ctx     = ga;
				    allocator->type    = GENERAL_PURPOSE_ALLOCATOR;
				    break;
			    }
	}
	return;
}

void allocator_deinit(Allocator *allocator)
{
	switch (allocator->type) {
		case ARENA_ALLOCATOR: {
				    Arena *arena = (Arena *)allocator->ctx;
				    free(arena->buffer);
				    free(arena);
				    break;
			    }
		case GENERAL_PURPOSE_ALLOCATOR:    {
				    GenAlloc *ga = (GenAlloc *)allocator->ctx;
				    free(ga->ptr);
				    free(ga);
				    break;
			    }
	}
}

//
// General Purpose Allocator
//

void gen_init(GenAlloc *ga, size_t size)
{
	ga->len = 0;
	ga->cap = size;
	ga->ptr = malloc(sizeof(uintptr_t) * size);
}

void  gen_deinit(GenAlloc *ga)
{
	if (ga && ga->ptr)
		free(ga->ptr);
}

void *gen_alloc(size_t size, void *ctx)
{
	void *ptr = malloc(size);
	if (ctx && ptr) {
		GenAlloc *alloc = (GenAlloc *)ctx;
		if (alloc->len >= alloc->cap) {
			size_t new_size = alloc->cap * 2;
			alloc->ptr = realloc(alloc->ptr, new_size * sizeof(uintptr_t));
			alloc->cap = new_size;
		}
		alloc->ptr[alloc->len] = (uintptr_t)ptr;
		alloc->len++;
		return ptr;
	}
	return ptr;
}

void gen_free(void *ptr, void *ctx)
{
	if (ctx) {
		GenAlloc *alloc = (GenAlloc *)ctx;
		for (size_t i = 0; i < alloc->len; i++) {
			if ((uintptr_t)ptr == alloc->ptr[i]) {
				free((void *)alloc->ptr[i]);
				alloc->ptr[i] = 0;
				return;
			}
		}
	}
	free(ptr);
	return;
}

void *gen_realloc(void *ptr, size_t old_size, size_t size, void *ctx)
{
	(void)old_size;
	uintptr_t old_ptr = (uintptr_t)ptr;
	void *new_ptr = realloc(ptr, size);
	GenAlloc *alloc = (GenAlloc *)ctx;
	for (size_t i = 0; i < alloc->len; i++) {
		if (old_ptr == alloc->ptr[i]) {
			alloc->ptr[i] = (uintptr_t)new_ptr;
			break;
		}
	}
	return new_ptr;
}

void gen_free_all(Allocator *allocator)
{
	if (allocator->type != GENERAL_PURPOSE_ALLOCATOR) {
		printf("[ALLOCATOR] Mismatched allocator - failed to release memory\n"); // TODO: print the incorrectly used allocator
		return;
	}
	GenAlloc *alloc = NULL;
	if (allocator->ctx) {
		alloc = (GenAlloc *)allocator->ctx;
		for (size_t i = 0; i < alloc->len; i++) {
			if (alloc->ptr[i] != 0) {
				free((void *)alloc->ptr[i]);
			}
		}
		alloc->len = 0;
	}
}

//
// Arena Allocator
//
// Code taken from: 
// - https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002
// - https://www.gingerbill.org/code/memory-allocation-strategies/part002.c
//

bool is_power_of_two(uintptr_t x) {
	return (x & (x-1)) == 0;
}

uintptr_t align_forward(uintptr_t ptr, size_t align)
{
	uintptr_t p, a, modulo;

	assert(is_power_of_two(align));

	p = ptr;
	a = (uintptr_t)align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a-1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}

void* arena_alloc_align(Arena *arena, size_t size, size_t align)
{
	uintptr_t curr_ptr = (uintptr_t)arena->buffer + (uintptr_t)arena->curr_offset;
	uintptr_t offset = align_forward(curr_ptr, align);
	offset -= (uintptr_t)arena->buffer;
	if (offset+size <= arena->buf_len) {
		void *ptr = &arena->buffer[offset];
		arena->prev_offset = offset;
		arena->curr_offset = offset+size;
		memset(ptr, 0, size);
		return ptr;
	}
	// Return NULL if the arena is out of memory (or handle differently)
	/*printf("ERROR: arena out of memory!\n");*/
	return NULL;
}

void* arena_resize_align(Arena *arena, void *old_memory, size_t old_size, size_t new_size, size_t align)
{
	unsigned char *old_mem = (unsigned char *)old_memory;

	assert(is_power_of_two(align));

	if (old_mem == NULL || old_size == 0) {
		return arena_alloc_align(arena, new_size, align);
	} else if (arena->buffer <= old_mem && old_mem < arena->buffer + arena->buf_len) {
		if (arena->buffer + arena->prev_offset == old_mem) {
			arena->curr_offset = arena->prev_offset + new_size;
			if (new_size > old_size) {
				// Zero the new memory by default
				memset(&arena->buffer[arena->curr_offset], 0, new_size - old_size);
			}
			return old_memory;
		} else {
			void *new_memory = arena_alloc_align(arena, new_size, align);
			if (!new_memory) return NULL; // TODO: replace with proper error handling
			size_t copy_size = old_size < new_size ? old_size : new_size;
			// Copy across old memory to the new memory
			memmove(new_memory, old_memory, copy_size);
			return new_memory;
		}

	} else {
		assert(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
	}
}

void* arena_alloc(size_t size, void *ctx)
{
	return arena_alloc_align((Arena *)ctx, size, DEFAULT_ALIGNMENT);
}

void* arena_resize(void *old_memory, size_t old_size, size_t new_size, void *ctx)
{
	return arena_resize_align((Arena *)ctx, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}

void arena_free_all(void *ptr, void* ctx)
{
	(void)ptr;
	Arena *arena = (Arena *)ctx;
	memset(arena->buffer, 0, arena->buf_len); // TODO: decide if I should reset everything to zero
	arena->curr_offset = 0;
	arena->prev_offset = 0;
}

void arena_free(void *ptr, void* ctx)
{
	(void)ptr;
	(void)ctx;
}


void arena_init(Arena *arena, void *buffer, size_t size)
{
	arena->buffer = (unsigned char *)buffer;
	arena->buf_len = size;
	arena->prev_offset = 0;
	arena->curr_offset = 0;
}
