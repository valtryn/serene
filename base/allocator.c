#define _GNU_SOURCE
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "allocator.h"
void allocator_deinit(Allocator *allocator)
{
	switch (allocator->type) {
		case GENERAL_PURPOSE_ALLOCATOR:
					break;
		case ARENA_ALLOCATOR: {
					Arena *alloc = (Arena*)allocator->ctx;
					free(alloc->buffer);
					free(alloc->relative_offsets);
					free(alloc);
					break;
		}
		case TRACKING_ALLOCATOR: {
					TrackingAllocator *alloc = (TrackingAllocator*)allocator->ctx;
					// NOTE: here we free all the tracked allocations
					for (size_t i = 0; i < alloc->len; i++) {
					        if (alloc->offsets[i] != 0)
					       	 free((void*)alloc->offsets[i]);
					}
					free(alloc->offsets);
					free(alloc);
					break;
		}
	}
}

//
// GENERAL PURPOSE ALLOCATOR
//
int general_allocator_init(Allocator *allocator)
{
	allocator->type    = GENERAL_PURPOSE_ALLOCATOR;
	allocator->alloc   = gpa_alloc;
	allocator->realloc = gpa_realloc;
	allocator->free    = gpa_free;
	return 0;
}

void *gpa_alloc(size_t size, void *ctx)
{
       (void)ctx;
       return malloc(size);
}

void *gpa_realloc(void *ptr, size_t size, void *ctx)
{
	(void)ctx;
	return realloc(ptr, size);

}

void gpa_free(void *ptr, void *ctx)
{
	(void)ctx;
	free(ptr);
}

//
// TRACKING ALLOCATOR
//
int tracking_allocator_init(Allocator *allocator)
{
	int ret = 0;
	TrackingAllocator *ta = malloc(sizeof(TrackingAllocator));
	if (!ta)
		goto end;
	ta->len       = 0;
	ta->cap       = 1024;
	ta->offsets = malloc(sizeof(uintptr_t) * DEFAULT_ALLOC_SIZE);
	if (!ta->offsets) {
		free(ta);
		goto end;
	}
	memset(ta->offsets, 0, sizeof(uintptr_t) * DEFAULT_ALLOC_SIZE);

	allocator->type    = TRACKING_ALLOCATOR;
	allocator->alloc   = ta_alloc;
	allocator->realloc = ta_realloc;
	allocator->free    = ta_free;
	allocator->ctx     = ta;

end:
	return ret;
}

size_t ptr_position(void *ptr, TrackingAllocator *alloc)
{
	size_t ret = SIZE_MAX;
	if (ptr && alloc) {
		for (size_t i = 0; i < alloc->len; i++) {
			if (alloc->offsets[i] == (uintptr_t)ptr) {
				ret = i;
				break;
			}
		}
	}
	return ret;
}

void *ta_alloc(size_t size, void *ctx)
{
	void *ret = NULL;
	if (ctx) {
		TrackingAllocator *tracking_allocator = (TrackingAllocator *)ctx;
		if (tracking_allocator->len == tracking_allocator->cap) {
			size_t new_size = tracking_allocator->cap * 2;
			void *new_container = realloc(tracking_allocator->offsets, new_size * sizeof(uintptr_t));
			if (!new_container)
				goto end;
			tracking_allocator->cap = new_size;
			tracking_allocator->offsets = new_container;
		}
		void *ptr = malloc(size);
		if (ptr) {
			tracking_allocator->offsets[tracking_allocator->len] = (uintptr_t)ptr;
			tracking_allocator->len++;
			ret = ptr;
		}
	}
end:
	return ret;
}

void *ta_realloc(void *ptr, size_t size, void *ctx)
{
	void *ret = NULL;
	if (ptr && ctx) {
		TrackingAllocator *tracking_allocator = (TrackingAllocator *)ctx;
		size_t pos = ptr_position(ptr, tracking_allocator);
		if (pos != SIZE_MAX) {
			void *new_ptr = realloc(ptr, size);
			if (new_ptr) {
				tracking_allocator->offsets[pos] = (uintptr_t)new_ptr;
				ret = new_ptr;
			}
		}
	}
	return ret;
}


void ta_free(void *ptr, void *ctx)
{
	if (ptr && ctx) {
		TrackingAllocator *tracking_allocator = (TrackingAllocator *)ctx;
		size_t pos = ptr_position(ptr, tracking_allocator);
		if (pos != SIZE_MAX) {
			free(ptr);
			tracking_allocator->offsets[pos] = 0;
			ptr = NULL;
		}
	}
}

//
// ARENA ALLOCATOR
// 
// REFERENCES:
//	- https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002
// 	- https://www.gingerbill.org/code/memory-allocation-strategies/part002.c
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

// NOTE: should just make this a macro
uintptr_t arena_abs_offset(Arena *arena, uintptr_t relative_offset)
{
	return relative_offset + (uintptr_t)arena->buffer;
}

size_t arena_get_block_size(uintptr_t ptr, Arena *arena)
{
	size_t ret = 0;
	if (!ptr || !arena) goto out;

	uintptr_t mem = (uintptr_t)ptr;
	
	if (mem < (uintptr_t)arena->buffer || mem >= (uintptr_t)arena->buffer + arena->cap) {
		goto out;
	}

	for (size_t i = 0; i < arena->offset_len - 1; i++) {
		if (mem == arena_abs_offset(arena, arena->relative_offsets[i])) {
			ret = arena_abs_offset(arena, arena->relative_offsets[i + 1]) - arena_abs_offset(arena, arena->relative_offsets[i]);
			goto out;
		}
	}

	if (arena->offset_len > 0 && mem == arena_abs_offset(arena, arena->relative_offsets[arena->offset_len - 1])) {
		ret = arena->curr_offset - arena_abs_offset(arena, (arena->relative_offsets[arena->offset_len - 1]) - (uintptr_t)arena->buffer);
	}
out:
	return ret;
}

int arena_allocator_init(Allocator *allocator, size_t size)
{
	int ret = 0;
	Arena *arena = malloc(sizeof(Arena));
	if (!arena) {
		ret = -1;
		goto OUT;
	};
	arena->buffer  = malloc(size);
	if (!arena->buffer) {
		ret = -1;
		free(arena);
		goto OUT;
	};
	arena->relative_offsets= malloc(sizeof(uintptr_t) * DEFAULT_ALLOC_SIZE);
	if (!arena->relative_offsets) {
		ret = -1;
		free(arena->buffer);
		free(arena);
		goto OUT;
	};
	arena->cap         = size;
	arena->offset_cap  = DEFAULT_ALLOC_SIZE;
	arena->offset_len  = 0;
	arena->curr_offset = 0;
	arena->prev_offset = 0;

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&arena->mutex, &attr);

	allocator->ctx      = arena;
	allocator->type     = ARENA_ALLOCATOR;
	allocator->alloc    = arena_alloc;
	allocator->realloc  = arena_realloc;
	allocator->free     = arena_free;
	allocator->free_all = arena_free_all;
OUT:
	return ret;
}

void *arena_alloc(size_t size, void *ctx)
{
	void *ret = NULL;
	if (!ctx) goto OUT;

	Arena *arena = (Arena*)ctx;
	pthread_mutex_lock(&arena->mutex);
	uintptr_t curr_ptr = (uintptr_t)arena->buffer + (uintptr_t)arena->curr_offset;
	uintptr_t offset   = align_forward(curr_ptr, DEFAULT_ALIGNMENT);
	offset -= (uintptr_t)arena->buffer;
	if (offset + size <= arena->cap) {
		void *ptr = &arena->buffer[offset];
		arena->prev_offset = offset;
		arena->curr_offset = offset+size;
		if (arena->offset_len >= arena->offset_cap) {
			size_t new_cap = arena->offset_cap * 2;
			size_t new_size = new_cap * sizeof(uintptr_t);
			uintptr_t *new_offsets = realloc(arena->relative_offsets, new_size);
			if (!new_offsets) goto OUT; // Handle failure
			arena->relative_offsets = new_offsets;
			arena->offset_cap = new_cap;
		}
		arena->relative_offsets[arena->offset_len] = offset;
		arena->offset_len++;
		ret = ptr;
	} else {
		printf("Memory is out of bounds or out of memory.\n");
	}
	pthread_mutex_unlock(&arena->mutex);
OUT:
	return ret;
}


void *arena_realloc(void *ptr, size_t size, void *ctx)
{
	void *ret = NULL;
	if (!ctx) goto out;

	if (!ptr || size == 0) {
		ret = arena_alloc(size, ctx);
		goto out;
	}

	Arena *arena = (Arena*)ctx;
	pthread_mutex_lock(&arena->mutex);
	uintptr_t old_mem = (uintptr_t)ptr;
	uintptr_t buf = (uintptr_t)arena->buffer;
	size_t old_size = arena_get_block_size(old_mem, arena);
	if (old_mem < buf || old_mem >= buf + arena->cap) {
		pthread_mutex_unlock(&arena->mutex);
		goto out;
	}
	// optimization
	if (old_mem == (uintptr_t)arena->buffer + arena->prev_offset) {
		arena->curr_offset = arena->prev_offset + size;
		if (size > old_size)
			memset(&arena->buffer[arena->curr_offset], 0, size - old_size);
		ret = ptr;
		pthread_mutex_unlock(&arena->mutex);
		printf("optimized\n");
		goto out;
	}

	void *new_mem = arena_alloc(size, arena);
	if (!new_mem) {
		pthread_mutex_unlock(&arena->mutex);
		goto out;
	}
	size_t copy_size = old_size < size ? old_size : size;
	memmove(new_mem, ptr, copy_size);
	ret = new_mem;
	pthread_mutex_unlock(&arena->mutex);
out:
	return ret;
}

void arena_free(void *ptr, void *ctx)
{
	(void)ptr;
	(void)ctx;
}

void arena_free_all(void *ctx)
{
	Arena *arena = (Arena*)ctx;
	arena->prev_offset = 0;
	arena->curr_offset = 0;
	arena->offset_len = 0;

}

