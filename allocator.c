#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"

void allocator_deinit(Allocator *allocator)
{
	switch (allocator->type) {
		case GENERAL_PURPOSE_ALLOCATOR:
			break;
		case ARENA_ALLOCATOR:
			break;
		case TRACKING_ALLOCATOR: {
						 TrackingAllocator *alloc = (TrackingAllocator*)allocator->ctx;
						 // NOTE: here we free all the tracked allocations
						 for (size_t i = 0; i < alloc->len; i++) {
							 if (alloc->container[i] != 0) {
								 free((void*)alloc->container[i]);
							 }
						 }
						 free(alloc->container);
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

void *gpa_realloc(void *ptr, size_t size, size_t old_size, void *ctx)
{
	(void)ctx;
	(void)old_size;
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
	size_t default_size = 1024;
	TrackingAllocator *ta = malloc(sizeof(TrackingAllocator));
	ta->len       = 0;
	ta->cap       = 1024;
	ta->container = malloc(sizeof(uintptr_t) * default_size);
	memset(ta->container, 0, sizeof(uintptr_t) *default_size);

	allocator->type    = TRACKING_ALLOCATOR;
	allocator->alloc   = ta_alloc;
	allocator->realloc = ta_realloc;
	allocator->free    = ta_free;
	allocator->ctx     = ta;

	return 0;
}

size_t ptr_position(void *ptr, TrackingAllocator *alloc)
{
	size_t ret = SIZE_MAX;
	if (ptr && alloc) {
		for (size_t i = 0; i < alloc->len; i++) {
			if (alloc->container[i] == (uintptr_t)ptr) {
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
			void *new_container = realloc(tracking_allocator->container, new_size * sizeof(uintptr_t));
			if (!new_container)
				goto end;
			tracking_allocator->cap = new_size;
			tracking_allocator->container = new_container;
		}
		void *ptr = malloc(size);
		if (ptr) {
			tracking_allocator->container[tracking_allocator->len] = (uintptr_t)ptr;
			tracking_allocator->len++;
			ret = ptr;
		}
	}
end:
	return ret;
}

void *ta_realloc(void *ptr, size_t size, size_t old_size, void *ctx)
{
	(void)old_size;
	void *ret = NULL;
	if (ptr && ctx) {
		TrackingAllocator *tracking_allocator = (TrackingAllocator *)ctx;
		size_t pos = ptr_position(ptr, tracking_allocator);
		if (pos != SIZE_MAX) {
			void *new_ptr = realloc(ptr, size);
			if (new_ptr) {
				tracking_allocator->container[pos] = (uintptr_t)new_ptr;
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
			tracking_allocator->container[pos] = 0;
			ptr = NULL;
		}
	}
}
